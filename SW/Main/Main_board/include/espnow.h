/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main_board\include\espnow.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

#ifndef ESPNOW_H
#define ESPNOW_H

/*! Fichiers inclus*/
#include <string.h>
#include <stdio.h>
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "io.h"
#include "esp_err.h"

#define DEBUG

/**
 * @brief 
 * 
 */
#define SECONDE (1000 / portTICK_PERIOD_MS)

/**
 * @brief 
 * 
 */
typedef enum
{
    ESPNOW_IDLE,
    WIFIINIT,
    ESPNOWINIT,
    ESPNOWMSGSEND,
    ESPNOWPOOL,
} ESPNOWTaskState_t;

// /**
//  * @brief
//  *
//  */
// typedef enum __attribute__((__packed__))
// {
//     SIMPLEPOLL = 0xFE, /*!< Vérifie la communication, le satellite répond par un ack.*/

//     MODIFY_MACHINE_NUMBER = SIMPLEPOLL - 1,            /*!< Modifie le numéro de la machine que contrôle le satellite adressé.*/
//     REQUEST_SERIAL_NUMBER = MODIFY_MACHINE_NUMBER - 1, /*!< Demande le numéro de la machine gérée par le satellite.*/

//     MODIFY_MACHINE_RELAY_STATE = REQUEST_SERIAL_NUMBER - 1,       /*!< Active ou desactive le relais qui contrôle la machine.*/
//     REQUEST_MACHINE_RELAY_STATE = MODIFY_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état du relais qui contrôle la machine.*/

//     REQUEST_MACHINE_STATUS = REQUEST_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie la valeur de l'état d'occupation de la machine.*/

//     MODIFY_DELAY_OVER_BUSY = REQUEST_MACHINE_STATUS - 1,  /*!< Modifie le temps de suroccupation du satellite.*/
//     REQUEST_DELAY_OVER_BUSY = MODIFY_DELAY_OVER_BUSY - 1, /*!< Le satellite renvoie le temps suroccupation.*/

//     MODIFY_BUSY_LEVEL = REQUEST_DELAY_OVER_BUSY - 1, /*!Modifie le niveau permettant de définir une occupation de la machine.*/
//     REQUEST_BUSY_LEVEL = MODIFY_BUSY_LEVEL - 1,      /*!< Renvoi le niveau permettant de définir une occupation de la machine.*/

//     REQUEST_ISMAINPRESENT = REQUEST_BUSY_LEVEL - 1, /*!< Retourne la présence du secteur.*/

//     MODIFY_MAIN_POWER = REQUEST_ISMAINPRESENT - 1, /*!< Défini la position du relais de fourniture de courant à la machine.*/
//     REQUEST_MAIN_POWER = MODIFY_MAIN_POWER - 1,    /*!< Retourne la position du relais de fourniture de courant à la machine.*/

//     REQUEST_MACHINE_LEVEL = REQUEST_MAIN_POWER - 1, /*!< Retourne le niveau des produits dans la machine.*/
//     REQUEST_FW_VERSION = REQUEST_MACHINE_LEVEL - 1, /*!<Retourne la version du satellite.*/

//     MODIFY_DELAY_ACTIVATION = REQUEST_FW_VERSION - 1,       /*!<Defini le temps d'activation du relay d'activation de la machine.*/
//     REQUEST_DELAY_ACTIVATION = MODIFY_DELAY_ACTIVATION - 1, /*!<Retourne le temps d'activation du relay d'activation de la machine.*/

// } Command_t;

/**
 * @brief 
 * 
 */
typedef enum __attribute__((__packed__))
{
    SIMPLEPOLL = 0xFE, /*!< Vérifie la communication, le satellite répond par un ack.*/

    MODIFY_MACHINE_NUMBER = SIMPLEPOLL - 1,            /*!< Modifie le numéro de la machine que contrôle le satellite adressé.*/
    REQUEST_SERIAL_NUMBER = MODIFY_MACHINE_NUMBER - 1, /*!< Demande le numéro de la machine gérée par le satellite.*/

    MODIFY_MACHINE_RELAY_STATE = REQUEST_SERIAL_NUMBER - 1,       /*!< Active ou desactive le relais qui contrôle la machine.*/
    REQUEST_MACHINE_RELAY_STATE = MODIFY_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état du relais qui contrôle la machine.*/

    REQUEST_MACHINE_BUSY_LEVEL = REQUEST_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre la valeur d'occupation de la machine.*/

    MODIFY_DELAY_OVER_BUSY = REQUEST_MACHINE_BUSY_LEVEL - 1, /*!< Modifie le temps de suroccupation du satellite.*/
    REQUEST_DELAY_OVER_BUSY = MODIFY_DELAY_OVER_BUSY - 1,    /*!< Le satellite renvoie le temps suroccupation.*/

    MODIFY_BUSY_LEVEL = REQUEST_DELAY_OVER_BUSY - 1, /*!Modifie le niveau permettant de définir une occupation de la machine.*/
    REQUEST_BUSY_LEVEL = MODIFY_BUSY_LEVEL - 1,      /*!< Renvoi le niveau permettant de définir une occupation de la machine.*/

    //REQUEST_ISMAINPRESENT = REQUEST_BUSY_LEVEL - 1, /*!< Retourne la présence du secteur.*/
    //MODIFY_MAIN_RELAY = REQUEST_ISMAINPRESENT - 1, /*!< Défini la position du relais de fourniture de courant à la machine.*/

    MODIFY_MAIN_RELAY = REQUEST_BUSY_LEVEL - 1, /*!< Défini la position du relais de fourniture de courant à la machine.*/
    REQUEST_MAIN_RELAY = MODIFY_MAIN_RELAY - 1, /*!< Retourne la position du relais de fourniture de courant à la machine.*/

    REQUEST_MACHINE_LEVEL = REQUEST_MAIN_RELAY - 1, /*!< Retourne le niveau des produits dans la machine.*/

    MODIFY_DELAY_ACTIVATION = REQUEST_MACHINE_LEVEL - 1,    /*!<Modify le délai d'activation du relais de commande.*/
    REQUEST_DELAY_ACTIVATION = MODIFY_DELAY_ACTIVATION - 1, /*!<Retourne le delai d'activation  du relais de commande.*/
    REQUEST_REST_ACTIVATION = REQUEST_DELAY_ACTIVATION - 1, /*!<Retourne le délai restant pour l'activation du relais de commande.*/

    REQUEST_FW_VERSION = REQUEST_REST_ACTIVATION - 1, /*!<Retourne la version du satellite.*/
} Command_t;

/**
 * @brief 
 */
TaskHandle_t hTaskESPNOW;

/**
 * @brief 
 */
uint8_t msg_received[32];

/*!
* \fn void TASKESPNOW(void *vParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param[in] vParameter 
*/
void TASKESPNOW(void *vParameter);

/*!
* \fn void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const uint8_t *data)
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
void prepareMessageToSend(const uint8_t address, const Command_t header, const uint8_t len, const void *data);

/*!
* \fn void ESPNOWPoll(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param[in] address 
* \return true si le pool réussi.
*/
bool ESPNOWPoll(uint8_t address);

/*!
* \fn bool setESPNOWNewAddress(uint8_t address, uint8_t newAddress)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param[in] address 
* \param[in] newAddress 
* \return 
*/
bool setESPNOWNewAddress(uint8_t address, uint8_t newAddress);

/*!
* \fn uint32_t getESPNOWSerialNumber(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param[in] address Numéro de la machine pour laquelle la requête est effectuée.
* \return Le numéro de série de la carte.
*/
uint32_t getESPNOWSerialNumber(uint8_t address);

/*!
* \fn bool setESPNOWMachineRelay(uint8_t address, bool isActive)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param[in] state 
* \return 
*/
bool setESPNOWMachineRelay(uint8_t address, bool isActive);

/*!
* \fn int getESPNOWStateMachineRelay(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param[in] address Numéro de la machine pour laquelle la requête est effectuée.
* \return Le numéro de série de la carte.
*/
int getESPNOWStateMachineRelay(uint8_t address);

/*!
* \fn bool setDelayOverBusy(uint8_t address, uint16_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param[in] state 
* \return 
*/
bool setDelayOverBusy(uint8_t address, uint16_t delay);

/*!
* \fn bool isModifiedDelayActivation(uint8_t address, uint8_t)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param[in] state 
* \return 
*/
bool isModifiedDelayActivation(uint8_t address, uint8_t);

/*!
* \fn uint16_t getDelayOverBusy(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param[in] address Numéro de la machine pour laquelle la requête est effectuée.
* \return Le délai d'overbusy
*/
uint16_t getDelayOverBusy(uint8_t address);

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
bool setESPNOWMainRelay(const uint8_t address, const bool isActive);

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
int getESPNOWStateMainRelay(uint8_t address);

/*!
* \fn ibool isMachineEmpty(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  26/02/2021
* \brief Renvoi le niveau de la machine.
* \remarks None
* \param address 
* \return True si la machine est vide.
*/
bool isMachineEmpty(uint8_t address);

/*!
* \fn bool setDelayActivationRelayMachine(uint8_t address, uint32_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  05/03/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/
bool setDelayActivationRelayMachine(uint8_t address, uint32_t delay);

/*!
* \fn uint32_t getDelayActivationRelayMachine(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  11/03/2021
* \brief 
* \remarks None
* \param address 
* \param delay 
* \return 
*/
uint32_t getDelayActivationRelayMachine(uint8_t address);

/*!
* \fn uint32_t getDelayActivationRelayMachine(uint8_t address, uint32_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  11/03/2021
* \brief 
* \remarks None
* \param address 
* \param delay 
* \return 
*/
uint32_t getRestActivationRelayMachine(uint8_t address);

#endif