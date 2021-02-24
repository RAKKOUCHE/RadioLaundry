/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\espnow.c
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
static uint8_t msg_buffer[64];

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
* \return 
*/
static void formatSendAnswer(const board_Answer_t state, const uint8_t len, const uint8_t *message)
{
    uint16_t LCRC;
    uint8_t *buffer;
    buffer = malloc(sizeof(HOST) + sizeof(len) + sizeof(MachineAddress) + 1 + len + sizeof(uint16_t));
    buffer[0] = HOST;
    buffer[1] = len;
    buffer[2] = MachineAddress;
    buffer[3] = (uint8_t)state;
    memmove(&buffer[4], message, len);
    LCRC = wCRC16(buffer, len + 4);
    memmove(&buffer[len + 4], &LCRC, sizeof(uint16_t));
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
}

/*!
* \fn static void vACK(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \return 
*/
static void vACK(void)
{
    boardState = ACK,
    msg_len = 0;
    setESPNOWTaskState(ESPNOWMSGSEND);
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
* \return 
*/
static void checkheader(const uint8_t *data)
{
    switch (data[POS_HEADER])
    {
    case MODIFY_MACHINE_NUMBER:
    {
//        MachineAddress = 
    }
    case SIMPLEPOLL:
    {
        ESP_LOGI(TAG_ESPNOW, "SIMPLE POLL");
        vACK();
        break;
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
    return ((data[0] == MachineAddress) && (data[2] == HOST) && (LCRC == (data[len - 2] + (data[len - 1] * 0x100))));
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
    ESP_LOGI(TAG_ESPNOW, "%s", "Donnée reçues!");
    if (isMsgValid(data, len))
    {
        setLED(LED_1);
        setIOState(IOLedFlash);
        checkheader(data);
        vTaskDelay(500);
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
        case ESPNOWMSGSEND:
        {
            formatSendAnswer(boardState, msg_len, msg_buffer);
            break;
        }
        case ESPNOWRECEIVE:
        {
            // ESP_LOGD(TAG_ESPNOW, "%s", "ESPNOW_RECEIVE");
            // setESPNOWTaskState(ESPNOW_IDLE);
            break;
        }
        default:
            ESP_LOGD(TAG_ESPNOW, "%s %u", "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}
