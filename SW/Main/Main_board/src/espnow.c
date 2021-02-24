/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\src\espnow.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

/*! Fichiers inclus*/
/*! Importation of librairies*/
#include "espnow.h"

/*!
* \def TAG_ESPNOW
* Description
*/
#define TAG_ESPNOW "\nESPNOW Module : "

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
* \def TO_COMMAND
* Description
*/
#define TO_COMMAND (2000 / portTICK_PERIOD_MS)

/**
* \brief
*/
uint8_t msg_address_recipient;

/**
* \brief
*/
uint8_t msg_len;

/**
* \brief
*/
uint8_t msg_cmd;

/**
* \brief
*/
uint8_t msg_buffer[250];

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
static bool isCommandFinished;

/**
 * @brief 
 * 
 */
static TimerHandle_t hCommandTO;

/**
* \brief
*/
static uint8_t macAddress[] = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};

/*!
* \fn static void CommandTO(TimerHandle_t handle)
* \author Name <email@email.com>
* \version 0.1
* \date  23/02/2021
* \brief 
* \remarks None
* \param handle 
* \return 
*/
static void CommandTO(TimerHandle_t handle)
{
    isCommandFinished = true;
}

/*!
* \fn static uint16_t wCRC16(void *byData, const uint8_t byLen)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param byData 
* \param byLen 
* \return 
*/
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

/*!
* \fn static void OnDataSend(const uint8_t *macAddr, esp_now_send_status_t status)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param macAddr 
* \param status 
* \return 
*/
static void OnDataSend(const uint8_t *macAddr, esp_now_send_status_t status)
{
    isCommandFinished = false;
    xTimerStart(hCommandTO, 1 * SECONDE);
    printf("%sresult : %d\n", TAG_ESPNOW, (uint8_t)status);
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
    setLED(LED_1);
    setIOState(IOLedFlash);
    printf("%s%s", TAG_ESPNOW, "result : ");
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02u ", data[i]);
    }
    memmove(msg_received, data, len);
    printf("%sMac address sender : %02X:%02X:%02X:%02X:%02X:%02X\n", TAG_ESPNOW, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    isCommandFinished = true;
    xTimerStop(hCommandTO, 1 * SECONDE);
    vTaskDelay(350);
    setLED(LED_1);
    setIOState(IOLEDOff);
}

/*!
* \fn static void InitESPNOW(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \return 
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
    hCommandTO = xTimerCreate("TO command", TO_COMMAND, pdFALSE, 0, CommandTO);
    printf("%s%s", TAG_ESPNOW, "ESPNOW initialisé.");
}

/*!
* \fn static void formatSendMsg(uint8_t addressRecipient, Command_t command, uint8_t len, uint8_t *message)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param addressRecipient 
* \param command 
* \param len 
* \param message 
* \return 
*/
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
    memset(msg_received, 0, sizeof(msg_received));
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
    printf("%s%s%u%s\n", TAG_ESPNOW,  "Commande ", command, " envoyée!");
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
* \fn bool ESPNOWPoll(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
bool ESPNOWPoll(uint8_t address)
{
    msg_address_recipient = address;
    msg_cmd = SIMPLEPOLL;
    msg_len = 0;
    printf("%sPoll", TAG_ESPNOW);
    xTaskNotifyGive(hTaskESPNOW);
    while (!isCommandFinished)
    {
    };
    printf("%s%s%s", TAG_ESPNOW, "Pool ", msg_received[0] == HOST ? "résussi" : "échoué");
    return msg_received[0] == HOST;
}

/*!
* \fn bool ESPNOWSetNewAddress(uint8_t address, uint8_t newAddress )
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param address 
* \param newAddress 
* \return 
*/
bool ESPNOWSetNewAddress(uint8_t address, uint8_t newAddress)
{
    msg_address_recipient = address;
    msg_cmd = MODIFY_MACHINE_NUMBER;
    msg_len = 1;
    msg_buffer[0] = newAddress;
    printf("%sChange address", TAG_ESPNOW);
    xTaskNotifyGive(hTaskESPNOW);
    while (!isCommandFinished)
        ;
    printf("%s%s%u\n", TAG_ESPNOW, "Nouvelle adresse : ", msg_received[2]);
    return msg_received[2] == newAddress;
}

/*!
* \fn uint32_t ESPNOWGetSerialNumber(uint8_t address)
* \author Name <email@email.com>
* \version 0.1
* \date  23/02/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
uint32_t ESPNOWGetSerialNumber(uint8_t address)
{
    msg_address_recipient = address;
    msg_cmd = REQUEST_MACHINE_NUMBER;
    msg_len = 0;
    printf("%sChange address", TAG_ESPNOW);
    xTaskNotifyGive(hTaskESPNOW);
    while (!isCommandFinished)
        ;
    return (msg_received[2] * 0X10000) + (msg_received[1] * 0X10000) + msg_received[0];
}

/*!
* \fn void TASKESPNOW(void *vParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param vParameter 
*/
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
            printf("%s%s",TAG_ESPNOW,  "Notification ESPNOW injustifié!");
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
            printf("%s%s %u",TAG_ESPNOW,  "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}
