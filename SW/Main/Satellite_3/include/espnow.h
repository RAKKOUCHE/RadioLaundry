/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\espnow.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

#ifndef ESPNOW_H
#define ESPNOW_H

/*! Fichiers includes*/
/*! Fichiers inclus*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "io.h"
#include "esp_err.h"
#include "wifi.h"
#include "parameters.h"
#include "main.h"

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
    ESPNOWANSWER,
    //ESPNOWRECEIVE,
} ESPNOWTaskState_t;

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

    MODIFY_DELAY_ACTIVATION = REQUEST_MACHINE_LEVEL - 1,    /*!<Modify le délai d'activation du relai de commande.*/
    REQUEST_DELAY_ACTIVATION = MODIFY_DELAY_ACTIVATION - 1, /*!<Retourne le delai d'activation  du relais de commande.*/

    REQUEST_FW_VERSION = REQUEST_DELAY_ACTIVATION - 1, /*!<Retourne la version du satellite.*/

} Command_t;

/**
 * @brief 
 * 
 */
TaskHandle_t hTaskESPNOW;

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
void vTaskESPNOW(void *vParameter);

#endif