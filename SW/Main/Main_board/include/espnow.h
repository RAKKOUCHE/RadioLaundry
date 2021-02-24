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
}ESPNOWTaskState_t;

/**
 * @brief 
 * 
 */
typedef enum __attribute__ ((__packed__))
{
    SIMPLEPOLL = 0xFE, /*!< Vérifie la communication, le satellite répond par un ack.*/

    MODIFY_MACHINE_NUMBER = SIMPLEPOLL - 1, /*!< Modifie le numéro de la machine que contrôle le satellite adressé.*/
    REQUEST_MACHINE_NUMBER = MODIFY_MACHINE_NUMBER - 1, /*!< Demande le numéro de la machine gérée par le satellite.*/

    MODIFY_MACHINE_RELAY_STATE = REQUEST_MACHINE_NUMBER - 1, /*!< Active ou desactive le relais qui contrôle la machine.*/
    REQUEST_MACHINE_RELAY_STATE = MODIFY_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état du relais qui contrôle la machine.*/

    REQUEST_MACHINE_STATUS = REQUEST_MACHINE_RELAY_STATE - 1, /*!< Le satellite renvoie en paramètre l'état d'occupation de la machine.*/

    MODIFY_DELAY_OVER_BUSY = REQUEST_MACHINE_STATUS - 1, /*!< Modifie le temps de suroccupation du satellite.*/
    REQUEST_DELAY_OVER_BUSY = MODIFY_DELAY_OVER_BUSY - 1, /*!< Le satellite renvoie le temps suroccupation.*/

    MODIFY_BUSY_LEVEL = REQUEST_DELAY_OVER_BUSY - 1, /*!Modifie le niveau permettant de définir une occupation de la machine.*/
    REQUEST_BUSY_LEVEL = MODIFY_BUSY_LEVEL - 1, /*!< Renvoi le niveau permettant de définir une occupation de la machine.*/

    REQUEST_ISMAINPRESENT = REQUEST_BUSY_LEVEL - 1, /*!< Retourne la présence du secteur.*/

    MODIFY_MAIN_POWER = REQUEST_ISMAINPRESENT - 1, /*!< Défini la position du relais de fourniture de courant à la machine.*/
    REQUEST_MAIN_POWER = MODIFY_MAIN_POWER - 1, /*!< Retourne la position du relais de fourniture de courant à la machine.*/

    REQUEST_MACHINE_LEVEL = REQUEST_MAIN_POWER - 1, /*!< Retourne le niveau des produits dans la machine.*/
    REQUEST_FW_VERSION = REQUEST_MACHINE_LEVEL - 1, /*!<Retourne la version du satellite.*/

    MODIFY_DELAY_ACTIVATION = REQUEST_FW_VERSION - 1, /*!<Defini le temps d'activation du relay d'activation de la machine.*/
    REQUEST_DELAY_ACTIVATION = MODIFY_DELAY_ACTIVATION - 1, /*!<Retourne le temps d'activation du relay d'activation de la machine.*/

}Command_t;

/**
 * @brief 
 */
TaskHandle_t hTaskESPNOW;

/**
 * @brief 
 */
uint8_t msg_received[32];

/*!
* \fn void setESPNOWTaskState(ESPNOWTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param state 
*/
void setESPNOWTaskState(ESPNOWTaskState_t state);

/*!
* \fn void TASKESPNOW(void *vParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param vParameter 
*/
void TASKESPNOW(void *vParameter);

/*!
* \fn void ESPNOWPoll(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
* \param address 
* \return true si le pool réussi.
*/
bool ESPNOWPoll(uint8_t address);

/*!
* \fn bool ESPNOWSetNewAddress(uint8_t address, uint8_t newAddress)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param address 
* \param newAddress 
* \return 
*/
bool ESPNOWSetNewAddress(uint8_t address, uint8_t newAddress);

/*!
* \fn uint32_t ESPNOWGetSerialNumber(uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  22/02/2021
* \brief 
* \remarks None
* \param address Numéro de la machine pour laquelle la requête est effectuée.
* \return Le numéro de série de la carte.
*/
uint32_t ESPNOWGetSerialNumber(uint8_t address);

bool isModifiedDelayActivation(uint8_t address, uint8_t);

#endif