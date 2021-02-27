/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\src\espnow.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

//TODO: supprimer la lecture des paramètres dans la release.

/*! Fichiers inclus*/
#include "espnow.h"

/*!
* \def TAG_ESPNOW
* \brief Etiquette du module ESPNOW
*/
#define TAG_ESPNOW "\nESPNOW Module : "

/*!
* \def CONFIG_ESPNOW_CHANNEL
* \brief Canal Wifi utilisé par le système
*/
#define CONFIG_ESPNOW_CHANNEL 0

/*!
* \def HOST
* \brief
*/
#define HOST 1

/*!
* \def CONFIG_ESPNOW_PMK
* \brief
*/
#define CONFIG_ESPNOW_PMK "pmk*Waterloo1612"

/*!
* \def CONFIG_ESPNOW_LMK
* \brief
*/
#define CONFIG_ESPNOW_LMK "lmk*Waterloo1612"

/*!
* \def TO_COMMAND
* \brief
*/
#define TO_COMMAND (1000 / portTICK_PERIOD_MS)

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
static uint8_t msg_address_recipient;

/**
* \brief
*/
static uint8_t msg_len;

/**
* \brief
*/
static uint8_t msg_cmd;

/**
* \brief
*/
static uint8_t msg_buffer[250];

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
* \fn static void CommandTO(const TimerHandle_t handle)
* \author Name <email@email.com>
* \version 0.1
* \date  23/02/2021
* \brief Fonction locale appelée lors du dépassement du délai autorisé pour une réponse.
* \remarks Positionne le flag indiquant que l'opération est terminéé
* \param[in] handle du timer
*/
static void CommandTO(const TimerHandle_t handle)
{
    isCommandFinished = true;
}

/*!
* \fn static uint16_t wCRC16(void *byData, const uint8_t byLen)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Fonction locale calculant le CRC16 d'un buffer
* \remarks None
* \param[in] byData buffer pour lequel on calcule le CRC16
* \param[in] byLen Nombre d'octets du buffer.
* \return Le CRC sur 16 bits du buffer
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
* \fn static void OnDataSend(const uint8_t *macAddr, const esp_now_send_status_t status)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Fonction call back locale permettant de savoir si le message a été envoyé
* \remarks Voir documentation ESP-NOW
* \param[in] macAddr Buffer contenant l'adresse mac de l'expédieur.
* \param[in] status Résultat de l'expédition.
* \return 
*/
static void OnDataSend(const uint8_t *macAddr, const esp_now_send_status_t status)
{
    printf("%s%s%d", TAG_ESPNOW, "Resultat : ", (uint8_t)status);
}

/*!
* \fn static void OnDataRcv(const uint8_t *macAddr, const uint8_t *data, int len)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Function call back locale activée quand le module ESP332 reçoit un message
* \remarks None
* \param[in] macAddr Adresse mac de l'expéditeur.
* \param[in] data Buffer contenant les données reçues.
* \param[in] len Nombre d'octets reçus.
* \return 
*/
static void OnDataRcv(const uint8_t *macAddr, const uint8_t *data, int len)
{
    printf("%s%s%02X:%02X:%02X:%02X:%02X:%02X : ", TAG_ESPNOW, "Données reçues de ", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02u ", data[i]);
    }
    memmove(msg_received, data, len);
    isCommandFinished = true;
    xTimerStop(hCommandTO, 1 * SECONDE);
    delay = 5;
    setLED(LED_1);
    setIOState(IOLedFlash);
}

/*!
* \fn static void InitESPNOW(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Fonction locale d'initialisation de l'API ESP32
* \remarks Cette  fonction doit être appelée avant toute autre opération sur ESPNOW.
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
* \fn static void formatSendMsg(const uint8_t addressRecipient, const Command_t command, uint8_t len, const uint8_t *message)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Fonction locale formatant le message et l'envoyant
* \remarks None
* \param[in] addressRecipient Numéro de la machine
* \param[in] command Header de la commande
* \param[in] len Nombre d'octets du paramètres
* \param[in] message Buffer contenant les paramètres
*/
static void formatSendMsg(const uint8_t addressRecipient, const Command_t command, uint8_t len, const uint8_t *message)
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
    printf("%s%s", TAG_ESPNOW, "Données envoyées : ");
    for (uint8_t i = 0; i < (len + 6); i++)
    {
        printf("%02u ", buffer[i]);
    }
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
    printf("%s%s%d%s", TAG_ESPNOW, "Commande ", command, " envoyée!");
    printf("\n");
}

/*!
* \fn static void setESPNOWTaskState(const ESPNOWTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Affecte un état à la variable ESPNOWTaskState
* \remarks None
* \param[in] state Etat à affecter à la tâche gérant les états du module.
*/
static void setESPNOWTaskState(const ESPNOWTaskState_t state)
{
    ESPNOWTaskState = state;
}

/*!
* \fn static void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const uint8_t *data)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Fonction locale Préparant le message pour l'envoi.
* \remarks None
* \param[in] address Adresse du module concerné.
* \param[in] header Commande a exécuter.
* \param[in] len Nombre de paramètres contenu dans le buffer.
* \param[in] data Buffer contenant les paramètres.
* \return 
*/
static void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const uint8_t *data)
{
    isCommandFinished = false;
    xTimerStart(hCommandTO, 1 * SECONDE);
    msg_address_recipient = address;
    msg_cmd = header;
    msg_len = len;
    if (len)
    {
        memmove(msg_buffer, data, len);
    }
    setESPNOWTaskState(ESPNOWMSGSEND);
    xTaskNotifyGive(hTaskESPNOW);
    while (!isCommandFinished)
    {
        vTaskDelay(1);
    }
}

/*!
* \fn bool ESPNOWPoll(const uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Interroge un module pour vérifier sa présence.
* \remarks None
* \param[in] address Adresse du module.
* \return true si le module répond.
*/
bool ESPNOWPoll(const uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Poll");
    prepareMessageToSend(address, SIMPLEPOLL, 0, NULL);
    printf("%s%s%s", TAG_ESPNOW, "Pool ", msg_received[0] == HOST ? "résussi" : "échoué");
    return msg_received[0] == HOST;
}

/*!
* \fn bool setESPNOWNewAddress(const uint8_t address, const uint8_t newAddress)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief Fixe une nouvelle adresse au module.
* \remarks None
* \param[in] address Adresse actuelle du module.
* \param[in] newAddress Adresse à affecter.
* \return true si le changement d'adresse à réussi.
*/
bool setESPNOWNewAddress(const uint8_t address, const uint8_t newAddress)
{
    printf("%sChange address", TAG_ESPNOW);
    prepareMessageToSend(address, MODIFY_MACHINE_NUMBER, 1, &newAddress);
    printf("%s%s%u", TAG_ESPNOW, "Nouvelle adresse : ", msg_received[2]);
    return msg_received[2] == newAddress;
}

/*!
* \fn uint32_t getESPNOWSerialNumber(const uint8_t address)
* \author Name <email@email.com>
* \version 0.1
* \date  23/02/2021
* \brief Récupère le numéro de série de la carte
* \remarks None
* \param[in] address Numéro de la machine sur laquelle la carte est installée.
* \return Numéro 
*/
uint32_t getESPNOWSerialNumber(const uint8_t address)
{
    printf("%sDemande le numéro de série", TAG_ESPNOW);
    memset(msg_received, 0, sizeof(msg_received));
    prepareMessageToSend(address, REQUEST_MACHINE_NUMBER, 0, NULL);
    printf("%s%s%u", TAG_ESPNOW, "Le numéro de série est : ", msg_received[6] + (msg_received[5] * 0X100) + (msg_received[4] * 0x10000));
    return (msg_received[6] + (msg_received[5] * 0X100) + (msg_received[4] * 0x10000));
}

/*!
* \fn bool ESPNOWSetStateMachineRelay(uint8_t address, bool isActive)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Active ou desactive le relais de machine
* \remarks None
* \param[in] address Adresse de la machine sur laquelle l'action se produira
* \param[in] isActive Flag indiquant si le relais sera activé ou relaché.
* \return true si l'action s'est effectuée correctement.
*/
bool ESPNOWSetStateMachineRelay(const uint8_t address, const bool isActive)
{
    uint8_t active = (uint8_t)isActive;
    printf("%s%s%s", TAG_ESPNOW, isActive ? "Active" : "Desactive", " le relais");
    prepareMessageToSend(address, MODIFY_MACHINE_RELAY_STATE, 1, &active);
    printf("%s%s%s", TAG_ESPNOW, "L'opération a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "réussie" : "échouée");
    return ((msg_received[0] == HOST) && (msg_received[3] == ACK)) ? isActive : false;
}

/*!
* \fn int getESPNOWStateMachineRelay(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  26/02/2021
* \brief Renvoi l'état du relais.
* \remarks None
* \param address 
* \return 
*/
int getESPNOWStateMachineRelay(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Demande l'état du relais de la machine");
    prepareMessageToSend(address, REQUEST_MACHINE_RELAY_STATE, 0, NULL);
    if ((msg_received[0] == HOST) && (msg_received[3] == ACK))
    {
        printf("%s%s%s", TAG_ESPNOW, "Le relais est : ", msg_received[4] ? "activé" : "repos");
        return msg_received[4] ;
    }
    else
    {
        printf("%s%s", TAG_ESPNOW, "Echec de la demande");
        return false;
    }
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
        case ESPNOWMSGSEND:
        {
            setESPNOWTaskState(ESPNOW_IDLE);
            formatSendMsg(msg_address_recipient, msg_cmd, msg_len, msg_buffer);
            break;
        }
        default:
            printf("%s%s %u", TAG_ESPNOW, "Notification ESPNOW injustifié : ", (uint8_t)ESPNOWTaskState);
            break;
        }
    }
}
