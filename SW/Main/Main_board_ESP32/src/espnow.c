/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\src\espnow.c
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
#define TO_COMMAND (3 * SECONDE)

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
* \return 
*/
static void CommandTO(const TimerHandle_t handle)
{
    xSemaphoreGive(hSemaphoreESPNOW);
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
    printf("%s%s%02X:%02X:%02X:%02X:%02X:%02X : ", TAG_ESPNOW, "Données reçues du module ", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    for (uint8_t i = 0; i < len; i++)
    {
        printf("%02u ", data[i]);
    }
    printf("\n");
    memmove(msg_received, data, len);
    xTimerStop(hCommandTO, 1 * SECONDE);
    setLED(LED_1);
    setIOState(IOLedFlash);
    xSemaphoreGive(hSemaphoreESPNOW);
}

/*!
* \fn static void InitESPNOW(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Fonction locale d'initialisation de l'API ESP32
* \remarks Cette  fonction doit être appelée avant toute autre opération sur ESPNOW.
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
    hSemaphoreESPNOW = xSemaphoreCreateBinary();
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
* \return 
*/
static void formatSendMsg(const uint8_t addressRecipient, const Command_t command, uint8_t len, const uint8_t *message)
{
    uint16_t LCRC;
    uint8_t *buffer;
    buffer = malloc(sizeof(addressRecipient) + sizeof(command) + sizeof(len) + len + sizeof(uint16_t));
    buffer[0] = addressRecipient;
    buffer[2] = HOST;
    buffer[3] = command;
    if ((buffer[1] = len))
    {
        memmove(&buffer[4], message, len);
    }
    LCRC = wCRC16(buffer, len + 4);
    memmove(&buffer[len + 4], &LCRC, sizeof(uint16_t));
    printf("%s%s", TAG_ESPNOW, "Données envoyées : ");
    for (uint8_t i = 0; i < (len + 6); i++)
    {
        printf("%02u ", buffer[i]);
    }
    ESP_ERROR_CHECK(esp_now_send(macAddress, buffer, len + 6));
    free(buffer);
    printf("%s%d", "- Commande : ", command);
}

/*!
* \fn static void setESPNOWTaskState(const ESPNOWTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Affecte un état à la variable ESPNOWTaskState
* \remarks None
* \param[in] state Etat à affecter à la tâche gérant les états du module.
* \return 
*/
static void setESPNOWTaskState(const ESPNOWTaskState_t state)
{
    ESPNOWTaskState = state;
}

/*!
* \fn void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const void *data)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Fonction locale Préparant le message pour l'envoi.
* \remarks None
* \param[in] address Adresse du module concerné.
* \param[in] header Commande a exécuter.
* \param[in] len Nombre de paramètres contenu dans le buffer.
* \param[in] data Buffer contenant les paramètres.
*/
void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const void *data)
{
    xTimerStart(hCommandTO, 10 * SECONDE);
    xSemaphoreGive(hSemaphoreESPNOW);
    xSemaphoreTake(hSemaphoreESPNOW, portMAX_DELAY);
    memset(msg_received, 0, sizeof(msg_received));
    msg_address_recipient = address;
    msg_cmd = header;
    if ((msg_len = len))
    {
        memmove(msg_buffer, data, len);
    }
    setESPNOWTaskState(ESPNOWMSGSEND);
    xTaskNotifyGive(hTaskESPNOW);
    xSemaphoreTake(hSemaphoreESPNOW, portMAX_DELAY);
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
    printf("%s%s%s", TAG_ESPNOW, "Le pool a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "résussi" : "échoué");
    return msg_received[0] == HOST && (msg_received[3] == ACK);
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
    printf("%s%s", TAG_ESPNOW, "Demande le numéro de série");
    memset(msg_received, 0, sizeof(msg_received));
    prepareMessageToSend(address, REQUEST_SERIAL_NUMBER, 0, NULL);
    printf("%s%s%u", TAG_ESPNOW, "Le numéro de série est : ", msg_received[6] + (msg_received[5] * 0X100) + (msg_received[4] * 0x10000));
    return (msg_received[6] + (msg_received[5] * 0X100) + (msg_received[4] * 0x10000));
}

/*!
* \fn bool setESPNOWMachineRelay(const uint8_t address, const bool isActive)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Active ou desactive le relais de machine
* \remarks None
* \param[in] address Adresse de la machine sur laquelle l'action se produira
* \param[in] isActive Flag indiquant si le relais sera activé ou relaché.
* \return true si l'action s'est effectuée correctement.
*/
bool setESPNOWMachineRelay(const uint8_t address, const bool isActive)
{
    uint8_t active = (uint8_t)isActive;
    printf("%s%s%s", TAG_ESPNOW, isActive ? "Active" : "Desactive", " le relais machine");
    prepareMessageToSend(address, MODIFY_MACHINE_RELAY_STATE, 1, &active);
    printf("%s%s%s", TAG_ESPNOW, "L'opération a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "réussie" : "échouée");
    return ((msg_received[0] == HOST) && (msg_received[3] == ACK)) ? isActive : false;
}

/*!
* \fn int getESPNOWStateMachineRelay(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  26/02/2021
* \brief Renvoi l'état du relais machine
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
        printf("%s%s%s", TAG_ESPNOW, "Le relais est ", msg_received[4] ? "activé" : "au repos");
        return msg_received[4];
    }
    else
    {
        printf("%s%s", TAG_ESPNOW, "Echec de la demande");
        return false;
    }
}

/*!
* \fn bool setDelayOverBusy(uint8_t address, uint16_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \param address 
* \param delay 
* \return 
*/
bool setDelayOverBusy(uint8_t address, uint16_t delay)
{
    printf("%s%s%u", TAG_ESPNOW, "Enregistrement du délai overbusy : ", delay);
    prepareMessageToSend(address, MODIFY_DELAY_OVER_BUSY, sizeof(delay), &delay);
    printf("%s%s%s", TAG_ESPNOW, "L'opération a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "réussie" : "échouée");
    return (msg_received[0] == HOST) && (msg_received[3] == ACK);
}

/*!
* \fn uint16_t getDelayOverBusy(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
uint16_t getDelayOverBusy(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Lecture du délai overbusy");
    prepareMessageToSend(address, REQUEST_DELAY_OVER_BUSY, 0, NULL);
    printf("%s%s%u", TAG_ESPNOW, "Le délai d'overbusy est de : ", msg_received[4] + (msg_received[5] * 0X100));
    return (msg_received[4] + (msg_received[5] * 0X100));
}

/*!
* \fn bool setESPNOWMainRelay(const uint8_t address, const bool isActive)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Active ou desactive le relais du secteur
* \remarks None
* \param[in] address Adresse de la machine sur laquelle l'action se produira
* \param[in] isActive Flag indiquant si le relais sera activé ou relaché.
* \return true si l'action s'est effectuée correctement.
*/
bool setESPNOWMainRelay(const uint8_t address, const bool isActive)
{
    uint8_t active = (uint8_t)isActive;
    printf("%s%s%s", TAG_ESPNOW, isActive ? "Active" : "Desactive", " le relais secteur");
    prepareMessageToSend(address, MODIFY_MAIN_RELAY, 1, &active);
    printf("%s%s%s", TAG_ESPNOW, "L'opération a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "réussie" : "échouée");
    return ((msg_received[0] == HOST) && (msg_received[3] == ACK)) ? isActive : false;
}

/*!
* \fn int getESPNOWStateMainRelay(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  26/02/2021
* \brief Renvoi l'état du relais secteur.
* \remarks None
* \param address 
* \return 
*/
int getESPNOWStateMainRelay(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Demande l'état du relais secteur");
    prepareMessageToSend(address, REQUEST_MAIN_RELAY, 0, NULL);
    if ((msg_received[0] == HOST) && (msg_received[3] == ACK))
    {
        printf("%s%s%s", TAG_ESPNOW, "Le relais est ", msg_received[4] ? "activé" : "au repos");
        return msg_received[4];
    }
    else
    {
        printf("%s%s", TAG_ESPNOW, "Echec de la demande");
        return false;
    }
}

/*!
* \fn bool isMachineEmpty(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  05/03/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
bool isMachineEmpty(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Demande le niveau de la machine");
    prepareMessageToSend(address, REQUEST_MACHINE_LEVEL, 0, NULL);
    if ((msg_received[0] == HOST) && (msg_received[3] == ACK))
    {
        printf("%s%s%s%s", TAG_ESPNOW, " La machine ", msg_received[4] ? "est " : "n'est pas ", "vide");
        return msg_received[4];
    }
    else
    {
        printf("%s%s", TAG_ESPNOW, "Echec de la demande");
        return false;
    }
}

/*!
* \fn bool setDelayActivationRelayMachine(uint8_t address, uint32_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  05/03/2021
* \brief 
* \remarks None
* \param address 
* \param delay
* \return 
*/
bool setDelayActivationRelayMachine(uint8_t address, uint32_t delay)
{
    printf("%s%s%u", TAG_ESPNOW, "Enregistrement du délai activation du relais machine : ", delay);
    prepareMessageToSend(address, MODIFY_DELAY_ACTIVATION, sizeof(delay), &delay);
    printf("%s%s%s", TAG_ESPNOW, "L'opération a ", (msg_received[0] == HOST) && (msg_received[3] == ACK) ? "réussie" : "échouée");
    return (msg_received[0] == HOST) && (msg_received[3] == ACK);
}

/*!
* \fn uint32_t getDelayActivationRelayMachine(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  11/03/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
uint32_t getDelayActivationRelayMachine(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Lecture du délai d'activation du relais machine");
    prepareMessageToSend(address, REQUEST_DELAY_ACTIVATION, 0, NULL);
    printf("%s%s%u", TAG_ESPNOW, "Le délai d'activation du re;ai : ", msg_received[4] + (msg_received[5] * 0X100) + (msg_received[6] * 0X100000) + (msg_received[7] * 0X1000000));
    return (msg_received[4] + (msg_received[5] * 0X100) + (msg_received[6] * 0X100000) + (msg_received[7] * 0X1000000));
}

/*!
* \fn uint32_t getDelayActivationRelayMachine(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  11/03/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
uint32_t getRestActivationRelayMachine(uint8_t address)
{
    printf("%s%s", TAG_ESPNOW, "Lecture du délai restant d'activation du relais machine");
    prepareMessageToSend(address, REQUEST_REST_ACTIVATION, 0, NULL);
    printf("%s%s%u", TAG_ESPNOW, "Le délai restant d'activation est : ", msg_received[4] + (msg_received[5] * 0X100) + (msg_received[6] * 0X100000) + (msg_received[7] * 0X1000000));
    return (msg_received[4] + (msg_received[5] * 0X100) + (msg_received[6] * 0X100000) + (msg_received[7] * 0X1000000));
}

bool getConfig(uint8_t address)
{
    printf("%s%s%u\n", TAG_ESPNOW, "Lecture de la configuration du module ", address);
    prepareMessageToSend(address, REQUEST_CONFIG, 0, NULL);
    if (msg_received[4] == address)
    {
        memmove(machineConfig.buffer, &msg_received[4], sizeof(machineConfig));
        return true;
    }
    return false;
}

bool setConfig(uint8_t address, MACHINECONFIG machineConfig)
{
    printf("%s%s%u\n", TAG_ESPNOW, "Enregistrement de la configuration du module ", machineConfig.config.address);
    prepareMessageToSend(address, MODIFY_CONFIG, sizeof(machineConfig), machineConfig.buffer);
    return (msg_received[0] == HOST) && (msg_received[3] == ACK);
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
