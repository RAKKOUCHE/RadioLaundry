
#include "main.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
// #include "esp_log.h"

#define LO 0
#define HI 1

#define POS_LED_1 GPIO_SEL_2

#define POS_LED_2 GPIO_SEL_5

#define POS_CTRL_MACHINE GPIO_SEL_25

#define POS_CTRL_MAIN GPIO_SEL_32

#define POS_EMPTY GPIO_SEL_33

#define POS_DIPSWITCH_2 GPIO_SEL_4

#define POS_BOOT GPIO_SEL_0

#define LED_1 GPIO_NUM_2

#define LED_2 GPIO_NUM_5

#define CTRL_MACHINE GPIO_NUM_25

#define CTRL_MAIN GPIO_NUM_32

#define EMPTY GPIO_NUM_33

#define DIPSWITCH_2 GPIO_NUM_4

#define BOOT GPIO_NUM_0

#define ESPNOW_WIFI_MODE WIFI_MODE_STA

#define CONFIG_ESPNOW_CHANNEL 0

#define HOST 1

#define DELAY_TASK_IO 100

typedef enum
{
    ESPNOW_IDLE,
    WIFIINIT,
    ESPNOWINIT,
    ESPNOWMSGSEND,
} ESPNOWTaskState_t;

typedef enum
{
    IOtASKIdle,
    IOTaskInit,
} IOTaskState_t;

//static const char *TAG_MAIN = "MAIN";
//static const char *TAG_FICHIER = "Fichier binaire";
//static const char *TAG_IO = "IO module";
//static const char *TAG_ESPNOW = "ESPNOW module";
//static const char *TAG_WIFI = "WIFI module";

static const char *filename = "/data/parameters";
static uint8_t macAddress[] = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
static const char *CONFIG_ESPNOW_LMK = "lmk*Waterloo1612";

static esp_vfs_spiffs_conf_t conf;
static esp_err_t err;
static FILE *filedata;
static esp_err_t err;

TaskHandle_t hTaskIO;
static IOTaskState_t IOTaskState;
static ESPNOWTaskState_t ESPNOWTaskState;
static TaskHandle_t hTaskESPNOW;
static esp_now_peer_info_t peer_info;
uint8_t msg_address_recipient;
uint8_t msg_len;
uint8_t msg_cmd;
uint8_t *msg_buffer;

typedef enum __attribute__((__packed__))
{
    SIMPLEPOLL = 0xFE, /*!< Vérifie la communication, le satellite répond par un ack.*/

    MODIFY_MACHINE_NUMBER = SIMPLEPOLL - 1,             /*!< Modifie le numéro de la machine que contrôle le satellite adressé.*/
    REQUEST_MACHINE_NUMBER = MODIFY_MACHINE_NUMBER - 1, /*!< Demande le numéro de la machine gérée par le satellite.*/

    MODIFY_MACHINE_RELAY_STATE = REQUEST_MACHINE_NUMBER - 1,      /*!< Active ou desactive le relais qui contrôle la machine.*/
    REQUEST_MACHINE_RELAY_STATE = MODIFY_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état du relais qui contrôle la machine.*/

    REQUEST_MACHINE_STATUS = REQUEST_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état d'occupation de la machine.*/

    MODIFY_DELAY_OVER_BUSY = REQUEST_MACHINE_STATUS - 1,  /*!< Modifie le temps de suroccupation du satellite.*/
    REQUEST_DELAY_OVER_BUSY = MODIFY_DELAY_OVER_BUSY - 1, /*!< Le satellite renvoie le temps suroccupation.*/

    MODIFY_BUSY_LEVEL = REQUEST_DELAY_OVER_BUSY - 1, /*!Modifie le niveau permettant de définir une occupation de la machine.*/
    REQUEST_BUSY_LEVEL = MODIFY_BUSY_LEVEL - 1,      /*!< Renvoi le niveau permettant de définir une occupation de la machine.*/

    REQUEST_ISMAINPRESENT = REQUEST_BUSY_LEVEL - 1, /*!< Retourne la présence du secteur.*/

    MODIFY_MAIN_POWER = REQUEST_ISMAINPRESENT - 1, /*!< Défini la position du relais de fourniture de courant à la machine.*/
    REQUEST_MAIN_POWER = MODIFY_MAIN_POWER - 1,    /*!< Retourne la position du relais de fourniture de courant à la machine.*/

    REQUEST_MACHINE_LEVEL = REQUEST_MAIN_POWER - 1, /*!< Retourne le niveau des produits dans la machine.*/
    REQUEST_FW_VERSION = REQUEST_MACHINE_LEVEL - 1, /*!<Retourne la version du satellite.*/

} Command_t;

static uint16_t wCRC16(void *byData, const uint8_t byLen)
{
    uint8_t *data = byData;
    uint8_t byIndex, byIndex2;
    uint16_t wLCRC = 0;
    for (byIndex = 0; byIndex < byLen; ++byIndex)
    {
        wLCRC ^= (data[byIndex] << 8);
        for (byIndex2 = 0; byIndex2 < 8; ++byIndex2)
        {
            (wLCRC & 0x8000) ? (wLCRC = ((wLCRC << 1) ^ 0x1021)) : (wLCRC <<= 1);
        }
    }
    return wLCRC;
}

static void formatSendMsg(uint8_t addressRecipient, Command_t command, uint8_t len, uint8_t *message)
{
    uint16_t LCRC;
    uint8_t *buffer;
    buffer = malloc(sizeof(addressRecipient) + sizeof(command) + sizeof(len) + len + sizeof(uint16_t));
    buffer[0] = addressRecipient;
    buffer[1] = len;
    buffer[2] = HOST;
    buffer[3] = command;
    memmove(&buffer[4], message, len);
    LCRC = wCRC16(buffer, len + 4);
    memmove(&buffer[len + 4], &LCRC, sizeof(uint16_t));
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
}

void toggleLed(gpio_num_t led)
{
    gpio_set_level(led, !gpio_get_level(led));
    printf("Led 1 : %s\n", gpio_get_level(led) ? "ON" : "OFF");
}

static void OnDataSend(const uint8_t *macAddr, esp_now_send_status_t status)
{
    //ESP_LOGI(TAG_ESPNOW, "result : %d\n", (uint8_t)status);
}

static void OnDataRcv(const uint8_t *macAddr, const uint8_t *data, int len)
{
    toggleLed(LED_1);
    //ESP_LOGI(TAG_ESPNOW, "result : ");
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02u ", data[i]);
    }
    //ESP_LOGI(TAG_ESPNOW, "\nMac address sender : %02X:%02X:%02X:%02X:%02X:%02X", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    printf("\n");
}

static void InitESPNOW(void)
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(OnDataSend));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(OnDataRcv));
    memset(&peer_info, 0, sizeof(peer_info));
    memcpy(peer_info.peer_addr, macAddress, ESP_NOW_ETH_ALEN);
    memcpy(&peer_info.lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
    peer_info.channel = CONFIG_ESPNOW_CHANNEL;
    peer_info.ifidx = ESP_IF_WIFI_STA;
    peer_info.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer_info));
    //ESP_LOGI(TAG_ESPNOW, "%s", "ESPNOW initialisé.\n");
}

void InitWifi(void)
{

    uint8_t macAddress[6];
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(3, 0));
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_MODE_STA, macAddress));
    //ESP_LOGI(TAG_WIFI, "mac adressse : %02X:%02X:%02X:%02X:%02X:%02X\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
}

void setESPNOWTaskState(ESPNOWTaskState_t state)
{
    ESPNOWTaskState = state;
}

void TASKESPNOW(void *vParameter)
{
    setESPNOWTaskState(WIFIINIT);
    while (1)
    {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        switch (ESPNOWTaskState)
        {
        case ESPNOW_IDLE: //Never call
        {
            //ESP_LOGD(TAG_ESPNOW, "%s", "Notification ESPNOW injustifié!");
            break;
        }
        case WIFIINIT:
        {
            setESPNOWTaskState(ESPNOWINIT);
            InitWifi();
            break;
        }
        case ESPNOWINIT:
        {
            setESPNOWTaskState(ESPNOW_IDLE);
            InitESPNOW();
            break;
        }
        case ESPNOWMSGSEND:
        {
            formatSendMsg(msg_address_recipient, msg_cmd, msg_len, msg_buffer);

            break;
        }
        default:
            //ESP_LOGD(TAG_ESPNOW, "%s %u", "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}

static void InitIO(void)
{
    ESP_ERROR_CHECK(gpio_set_direction(LED_1, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED_2, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MAIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MACHINE, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DIPSWITCH_2, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(BOOT, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(EMPTY, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_level(LED_1, LO));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, LO));
    ESP_ERROR_CHECK(gpio_set_level(CTRL_MACHINE, LO));
    ESP_ERROR_CHECK(gpio_set_level(CTRL_MAIN, LO));
    //ESP_LOGI(TAG_IO, "%s", "IOs iinitialisés.\n");
}

static void setIOState(IOTaskState_t state)
{
    IOTaskState = state;
}

void TaskIO(void *VParameter)
{
    setIOState(IOTaskInit);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        switch (IOTaskState)
        {
        case IOtASKIdle:
        {
            break;
        }
        case IOTaskInit:
        {
            setIOState(IOtASKIdle);
            InitIO();
            break;
        }
        default:
        {
            break;
        }
        }
        vTaskDelayUntil(&xLastWakeTime, DELAY_TASK_IO);
    }
}

static void checkFileParameters(void)
{
    filedata = fopen(filename, "wb+");
    if (!(fread(&MachineAddress, sizeof(MachineAddress), 1, filedata)))
    {
        //ESP_LOGI(TAG_FICHIER, "%s", "première utilisation");
        MachineAddress = 0XFF;
        fwrite(&MachineAddress, sizeof(MachineAddress), 1, filedata);
        fseek(filedata, 0, SEEK_SET);
    }
}

static bool setVFS_SPIFFS(void)
{
    conf.base_path = "/data";
    conf.partition_label = NULL;
    conf.max_files = 5;
    conf.format_if_mount_failed = true;
    ESP_ERROR_CHECK(err = esp_vfs_spiffs_register(&conf));
    if (err)
    {
        if (err == ESP_FAIL)
        {
            //ESP_LOGE(TAG_FICHIER, "Failed to mount or format filesystem\n");
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            //ESP_LOGE(TAG_FICHIER, "Failed to find SPIFFS partition\n");
        }
        else
        {
            //ESP_LOGE(TAG_FICHIER, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(err));
        }
    }
    return (err == 0);
}

static void InitParameters(void)
{
    if (setVFS_SPIFFS())
    {
        checkFileParameters();
    }
}

void createTasks(void)
{
    //Création la tâche des entrées sorties
    xTaskCreate(TaskIO, "Task IO", 2048, NULL, 1, &hTaskIO);
    //Création de la tâche ESPNOW
    xTaskCreate(TASKESPNOW, "Task ESPNOW", 2560, NULL, 1, &hTaskESPNOW);
}

static void InitApp(void)
{
    //Initialisation nécessaire de la flash
    InitFlash();
    //Initialisation des logs
    //InitESPLOG();
    //Lecture des paramètres
    InitParameters();
}

void app_main()
{
    //ESP_LOGI(TAG_MAIN, "%s", "Debut du programme");
    InitApp();
}