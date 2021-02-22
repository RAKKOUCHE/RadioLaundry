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

/*!
* \def TAG_ESPNOW
* Description
*/
#define TAG_ESPNOW "ESPNOW module"

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

/**
* \brief
*/
uint8_t msg_len;

/**
* \brief
*/
uint8_t msg_state;

/**
* \brief
*/
uint8_t msg_buffer[32];

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
* \fn static bool isMsgOK(const uint8_t *data, int len)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param data 
* \param len 
* \return 
*/
static bool isMsgOK(const uint8_t *data, int len)
{
    uint16_t LCRC;
    if ((len > 5) && (data[0] == MachineAddress) && (data[2] == HOST))
    {
        LCRC = (data[len - 1] * 256) + data[len - 2];
        return LCRC == wCRC16(data, len - sizeof(LCRC));
    }
    return false;
}

/*!
* \fn static void vAck(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \return 
*/
static void vAck(void)
{
    setESPNOWTaskState(ESPNOWMSANSWER);
    msg_state = ACK;
    msg_len = 0;
    xTaskNotifyGive(hTaskESPNOW);
}

/*!
* \fn static void sendSerialNumber(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \return 
*/
static void sendSerialNumber(void)
{
    uint8_t lmacAddress[6];
    setESPNOWTaskState(ESPNOWMSANSWER);
    msg_state = ACK;
    msg_len = 3;
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_MODE_STA, lmacAddress));
    memmove(msg_buffer, &lmacAddress[3], 3);
    xTaskNotifyGive(hTaskESPNOW);
}

/*!
* \fn static void checkCommand(const uint8_t *data)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param data
* \return 
*/
static void checkCommand(const uint8_t *data)
{
    switch (data[3])
    {
    case SIMPLEPOLL:
    {
        vAck();
        break;
    }
    case REQUEST_MACHINE_NUMBER:
    {
        sendSerialNumber();
        break;
    }
    case MODIFY_MACHINE_NUMBER:
    {
        MachineAddress = data[4];
        vAck();
        saveMachineNumber();
    }
    default:
    {
        break;
    }
    }
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
    ESP_LOGI(TAG_ESPNOW, "result : %d\n", (uint8_t)status);
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
    if (isMsgOK(data, len))
    {
        setLED(LED_1);
        setIOState(IOLedFlash);
        checkCommand(data);
        ESP_LOGI(TAG_ESPNOW, "result : ");
        for (uint8_t i = 0; i < len; i++)
        {
            printf("%02u ", data[i]);
        }
        ESP_LOGI(TAG_ESPNOW, "\nMac address sender : %02X:%02X:%02X:%02X:%02X:%02X\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
        vTaskDelay(250);
        setLED(LED_1);
        setIOState(IOLEDOff);
    }
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
    ESP_LOGI(TAG_ESPNOW, "%s", "ESPNOW initialisé.\n");
}

/*!
* \fn static void formatSendAnswer(EPSStateBoard_t state, uint8_t len, uint8_t *message)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param len 
* \param message 
* \param state
* \return 
*/
static void formatSendAnswer(EPSStateBoard_t state, uint8_t len, uint8_t *message)
{
    uint16_t LCRC;
    uint8_t *buffer;
    buffer = malloc(sizeof(MachineAddress) + sizeof(state) + sizeof(len) + len + sizeof(uint16_t));
    buffer[0] = HOST;
    buffer[1] = len;
    buffer[2] = MachineAddress;
    buffer[3] = state;
    memmove(&buffer[4], message, len);
    LCRC = wCRC16(buffer, len + 4);
    memmove(&buffer[len + 4], &LCRC, sizeof(uint16_t));
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
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
            ESP_LOGD(TAG_ESPNOW, "%s", "Notification ESPNOW injustifié!");
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
        case ESPNOWMSANSWER:
        {
            formatSendAnswer(msg_state, msg_len, msg_buffer);
            break;
        }

        default:
            ESP_LOGD(TAG_ESPNOW, "%s %u", "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}
