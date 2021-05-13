/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\espnow.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

/*! Fichiers inclus*/
#include "espnow.h"

const char version[8] = "V0.1";

/*!
* \def TAG_ESPNOW
* Description
*/
#define TAG_ESPNOW "\nESPNOW module : "

/*!
* \def CONFIG_ESPNOW_CHANNEL
* Description
*/
#define CONFIG_ESPNOW_CHANNEL 0

/*!
* \def HOST
* Description
*/
#define HOST 1

/*!
* \def POS_HEADER
* Description
*/
#define POS_HEADER 3

/*!
* \def POS_PARAMETERS
* Description
*/
#define POS_PARAMETERS 4

/*!
* \def CONFIG_ESPNOW_PMK
* Description
*/
#define CONFIG_ESPNOW_PMK "pmk*Waterloo1612"

/*!
* \def CONFIG_ESPNOW_LMK
* Description
*/
#define CONFIG_ESPNOW_LMK "lmk*Waterloo1612"

/*!
* \fn typedef enum __attribute__((__packed__))
* \author Name <email@email.com>
* \version 0.1
* \date  24/02/2021
* \brief 
* \remarks None
* \return 
*/
typedef enum __attribute__((__packed__))
{
    ACK = 0,        /*!< Instruction reçu correctement.*/
    NAK = 5,        /*!< Instruction erronée.*/
    BOARD_BUSY = 6, /*!< Impossible de répondre pour cause d'occupation.*/
} board_Answer_t;

/**
* \brief
*/
static uint8_t msg_len;

/**
* \brief
*/
static uint8_t msg_buffer[128];

/**
 * @brief 
 */
static board_Answer_t boardState;

/**
* \brief
*/
static ESPNOWTaskState_t ESPNOWTaskState;

/**
* \brief
*/
static esp_now_peer_info_t peer_info;

/**
* \brief
*/
static uint8_t macAddress[] = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};

/*!
* \fn static uint16_t wCRC16(const void *byData, const uint8_t byLen)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param byData 
* \param byLen 
* \return 
*/
static uint16_t wCRC16(const void *byData, const uint8_t byLen)
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

/*!
* \fn static void formatSendAnswer(const board_Answer_t state, const uint8_t len, const uint8_t *message)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param state 
* \param len 
* \param message 
*/
static void formatSendAnswer(const board_Answer_t state, const uint8_t len, const uint8_t *message)
{
    uint8_t i;
    uint16_t LCRC;
    uint8_t *buffer;
    uint8_t bufferSize = 1 + sizeof(len) + sizeof(machineConfig.config.address) + 1 + len + sizeof(uint16_t);
    buffer = malloc(bufferSize);
    memset(buffer, 0, bufferSize);
    buffer[0] = HOST;
    buffer[1] = len;
    buffer[2] = machineConfig.config.address;
    buffer[3] = (uint8_t)state;
    memmove(&buffer[4], message, len);
    LCRC = wCRC16(buffer, len + 4);
    memmove(&buffer[len + 4], &LCRC, sizeof(uint16_t));
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    printf("%s%s%d%s", TAG_ESPNOW, "Module ", machineConfig.config.address, " envoie message :");
    for (i = 0; i < bufferSize; i++)
    {
        printf("%02u ", buffer[i]);
    }
    printf("\n");
    free(buffer);
}

/*!
* \fn static void vNACK(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
static void vNACK(void)
{
    boardState = NAK;
    msg_len = 0;
    setESPNOWTaskState(ESPNOWANSWER);
    xTaskNotifyGive(hTaskESPNOW);
}

/*!
* \fn static void vACK(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
static void vACK(void)
{
    boardState = ACK;
    msg_len = 0;
    setESPNOWTaskState(ESPNOWANSWER);
    xTaskNotifyGive(hTaskESPNOW);
}

/*!
* \fn static void checkheader(const uint8_t *data)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param data
*/
static void checkheader(const uint8_t *data)
{
    switch (data[POS_HEADER])
    {
    case SIMPLEPOLL:
    {
        printf("%s%s\n", TAG_ESPNOW, "SIMPLE POLL");
        vACK();
        break;
    }
    case MODIFY_MACHINE_NUMBER:
    {
        machineConfig.config.address = data[4];
        printf("%s%s%s%s%u", TAG_ESPNOW, "L'enregistrement du numéro de la machine a ", saveMachineConfig(machineConfig) ? "résussi" : "échoué:", " Le nouveau numéro est : ", machineConfig.config.address);
        vACK();
        break;
    }
    case REQUEST_SERIAL_NUMBER:
    {
        uint8_t macAddress[6];
        ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_MODE_STA, macAddress));
        boardState = ACK;
        msg_len = 3;
        memmove(msg_buffer, &macAddress[3], msg_len);
        printf("%s%s%u", TAG_ESPNOW, "Le numéro de série de la carte est : ", msg_buffer[2] + (msg_buffer[1] * 0X100) + (msg_buffer[0] * 0x10000));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case MODIFY_MACHINE_RELAY_STATE:
    {
        printf("%s%s", TAG_ESPNOW, "Active ou desactive le relais d'activation");
        setIOState(data[4] ? IORELAYMACHINEON : IORELAYMACHINEOFF);
        while (getIOState() != IOTASKIDLE)
        {
            vTaskDelay(1);
        };
        if (gpio_get_level(CTRL_MACHINE) == data[4])
        {
            printf("%s%s%s", TAG_ESPNOW, "Relais machine ", data[4] ? "activé" : "desactivé");
            vACK();
        }
        else
        {
            printf("%s%s", TAG_ESPNOW, "Echec d'activation du relais machine.");
            vNACK();
        }
        break;
    }
    case REQUEST_MACHINE_RELAY_STATE:
    {
        printf("%s%s", TAG_ESPNOW, "L'état du relais d'activation");
        boardState = ACK;
        msg_len = 1;
        msg_buffer[0] = (uint8_t)gpio_get_level(CTRL_MACHINE);
        printf("%s%s%s", TAG_ESPNOW, "Le relais machine est ", gpio_get_level(CTRL_MACHINE) ? "ON" : "OFF");
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case REQUEST_MACHINE_BUSY_LEVEL:
    {
        uint16_t result = getADCValue();
        printf("%s%s", TAG_ESPNOW, "Lecture de la valeur de l'état d'occupation.");
        boardState = ACK;
        msg_len = sizeof(result);
        memmove(msg_buffer, &result, sizeof(result));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        printf("%s%s%u", TAG_ESPNOW, "La valeur d'occupation est : ", result);
        break;
    }
    case MODIFY_DELAY_OVER_BUSY:
    {
        printf("%s%s", TAG_ESPNOW, "Enregistrement du délais de suroccupation");
        machineConfig.config.delayOverBusy = data[4] + (data[5] * 0x100);
        saveMachineConfig(machineConfig);
        printf("%s%s%s : %u", TAG_ESPNOW, "L'enregistrement du délais de suroccupation a ", (machineConfig.config.delayOverBusy) ? "résussi" : "échoué", data[4] + (data[5] * 0x100));
        vACK();
        break;
    }
    case REQUEST_DELAY_OVER_BUSY:
    {
        printf("%s%s", TAG_ESPNOW, "Lecture du délai de suroccupation");
        printf("%s%s%u", TAG_ESPNOW, "Le délai d'overbusy est de :", machineConfig.config.delayOverBusy);
        boardState = ACK;
        msg_len = sizeof(machineConfig.config.delayOverBusy);
        memmove(msg_buffer, &machineConfig.config.delayOverBusy, sizeof(machineConfig.config.delayOverBusy));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case MODIFY_MAIN_RELAY:
    {
        printf("%s%s", TAG_ESPNOW, "Active ou desactive le relais secteur\n");
        setIOState(data[4] ? IORELAYMAINON : IORELAYMAINOFF);
        while (getIOState() != IOTASKIDLE)
        {
            vTaskDelay(1);
        };
        if (gpio_get_level(CTRL_MACHINE) == data[4])
        {
            printf("%s%s%s.", TAG_ESPNOW, "Relais secteur ", data[4] ? "activé" : "desactivé");
            vACK();
        }
        else
        {
            printf("%s%s", TAG_ESPNOW, "Echec d'activation du relais secteur.");
            vNACK();
        }
        break;
    }
    case REQUEST_MAIN_RELAY:
    {
        printf("%s%s", TAG_ESPNOW, "L'état du relais secteur.");
        boardState = ACK;
        msg_len = 1;
        msg_buffer[0] = (uint8_t)gpio_get_level(CTRL_MAIN);
        printf("%s%s%s", TAG_ESPNOW, "Le relais secteur est ", gpio_get_level(CTRL_MAIN) ? "ON" : "OFF");
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case REQUEST_MACHINE_LEVEL:
    {
        printf("%s%s", TAG_ESPNOW, "L'état du niveau des produits.");
        boardState = ACK;
        msg_len = 1;
        msg_buffer[0] = !(uint8_t)gpio_get_level(EMPTY);
        printf("%s%s%s%s", TAG_ESPNOW, "Le distributeur ", gpio_get_level(EMPTY) ? "n'est pas " : "est ", "vide");
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case MODIFY_DELAY_ACTIVATION:
    {
        printf("%s%s", TAG_ESPNOW, "Enregistrement du délais d'activation de la machine.'");
        machineConfig.config.lPulseInMS = data[4] + (data[5] * 0x100) + (data[6] * 0x10000) + (data[7] * 0x1000000);
        printf("%s%s%s : %u", TAG_ESPNOW, "L'enregistrement du délais d'activation de la machine a ", saveMachineConfig(machineConfig) ? "résussi" : "échoué", machineConfig.config.lPulseInMS);
        vACK();
        break;
    }
    case REQUEST_DELAY_ACTIVATION:
    {
        uint32_t result = machineConfig.config.lPulseInMS;
        printf("%s%s", TAG_ESPNOW, "Lecture du délai d'activation");
        printf("%s%s%u", TAG_ESPNOW, "Le délai d'activation est de :", result);
        boardState = ACK;
        msg_len = sizeof(result);
        memmove(msg_buffer, &result, sizeof(result));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case REQUEST_REST_ACTIVATION:
    {
        uint32_t result;
        result = xTimerIsTimerActive(hTORelayMachine) ? xTimerGetExpiryTime(hTORelayMachine) - xTaskGetTickCount() : 0;
        printf("%s%s", TAG_ESPNOW, "Lecture du délai d'activation restant");
        printf("%s%s%u", TAG_ESPNOW, "Le délai d'activation restant est de :", result);
        boardState = ACK;
        msg_len = sizeof(result);
        memmove(msg_buffer, &result, sizeof(result));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case REQUEST_FW_VERSION:
    {
        printf("%s%s%s", TAG_ESPNOW, "lecture de la versio du FW : ", version);
        boardState = ACK;
        msg_len = sizeof(version);
        memmove(msg_buffer, version, sizeof(version));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);

        break;
    }
    case REQUEST_CONFIG:
    {
        printf("%s%s", TAG_ESPNOW, "Lecture de la configuration.");
        boardState = ACK;
        msg_len = sizeof(MACHINECONFIG);
        memmove(msg_buffer, &machineConfig, sizeof(MACHINECONFIG));
        setESPNOWTaskState(ESPNOWANSWER);
        xTaskNotifyGive(hTaskESPNOW);
        break;
    }
    case MODIFY_CONFIG:
    {
        printf("%s%s", TAG_ESPNOW, "Enregistrement de la configuration du module.");
        boardState = ACK;
        memmove(machineConfig.buffer, &data[4], data[1]);
        printf("%s%s%u%s", TAG_ESPNOW, "L'enregistrement de la configuration de la machine : ", machineConfig.config.address, saveMachineConfig(machineConfig) ? " a résussi" : " a échoué ");
        vACK();
    }
    default:
    {
        break;
    }
    }
}

/*!
* \fn static bool isMsgValid(const uint8_t *data, uint8_t len)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021

* \brief 
* \remarks None
* \param data
* \param len
* \return  true si le message est destiné à cette carte.
*/
static bool isMsgValid(const uint8_t *data, uint8_t len)
{
    uint16_t LCRC = wCRC16(data, len - 2);
    return ((data[0] == machineConfig.config.address) && (data[2] == HOST) && (LCRC == (data[len - 2] + (data[len - 1] * 0x100))));
}

/*!
* \fn static void OnDataSend(const uint8_t *macAddr, esp_now_send_status_t status)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param macAddr 
* \param status 
*/
static void OnDataSend(const uint8_t *macAddr, esp_now_send_status_t status)
{
    printf("%s%s%d\n", TAG_ESPNOW, " result : ", (uint8_t)status);
}

/*!
* \fn static void OnDataRcv(const uint8_t *macAddr, const uint8_t *data, int len)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param macAddr 
* \param data 
* \param len 
* \return 
*/
static void OnDataRcv(const uint8_t *macAddr, const uint8_t *data, int len)
{
    printf("\n%s%s%02X:%02X:%02X:%02X:%02X:%02X : - ", TAG_ESPNOW, "Données reçues de ", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    printf("\n");
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02u ", data[i]);
    }
    printf("%s%u\n", "Commande ", data[3]);
    if (isMsgValid(data, len))
    {
        checkheader(data);

        //Lance le clignotement
        setLED(LED_1);
        setIOState(IOLEDFLASH);
        delayBlink = 5;
        //---------------
    }
    else
    {
        printf("%s", "Message invalide\n");
    }
}

/*!
* \fn static void InitESPNOW(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
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
    printf("%s%s", TAG_ESPNOW, "ESPNOW initialisé.\n");
}

/*!
* \fn void setESPNOWTaskState(ESPNOWTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param state 
*/
void setESPNOWTaskState(ESPNOWTaskState_t state)
{
    ESPNOWTaskState = state;
}

/*!
* \fn void vTaskESPNOW(void *vParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param vParameter 
*/
void vTaskESPNOW(void *vParameter)
{
    setESPNOWTaskState(WIFIINIT);
    while (1)
    {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        switch (ESPNOWTaskState)
        {
        case ESPNOW_IDLE: //Never call
        {
            printf("%s%s", TAG_ESPNOW, "Notification ESPNOW injustifié!");
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
        case ESPNOWANSWER:
        {
            formatSendAnswer(boardState, msg_len, msg_buffer);
            break;
        }
        default:
            printf("%s%s %u", TAG_ESPNOW, "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}
