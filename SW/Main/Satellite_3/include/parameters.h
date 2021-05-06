/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\parameters.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier entête du fichier de gestion des paramètres.
* \remarks None
*/

#ifndef PARAMETERS_H
#define PARAMETERS_H

/*! Fichiers inclus*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "io.h"

/*!
* \def DEBUG
* Description Flag à retirer pour la version release
*/
#define DEBUG

/*!
* \def NEWDEVICE
* Description
*/
#define NEWDEVICE 0XFF

/*!
* \def DEFAULTDELAYOVERBUSY
* Description
*/
#define DEFAULTDELAYOVERBUSY 30

/*!
* \def DEFAULTDELAYACTIVION
* Description
*/
#define DEFAULTDELAYACTIVION 1000

/*!
* \def DEFAULTCASHPRICE
* Description
*/
#define DEFAULTCASHPRICE 400

/*!
* \def DEFAULTCLPRICE
* Description
*/
#define DEFAULTCLPRICE 400

/*!
* \def DEFAULTCASHPRICEHH
* Description
*/
#define DEFAULTCASHPRICEHH 400

/*!
* \def DEFAULTCLPRICEHH
* Description
*/
#define DEFAULTCLPRICEHH 400

/*!
* \def DEFAULTPULSE
* Description
*/
#define DEFAULTPULSE 1000

/*!
* \def DEFAULTOVERBUSY
* Description
*/
#define DEFAULTOVERBUSY 60

/*!
* \def DEFAULTTIMETODISPLAY
* Description
*/
#define DEFAULTTIMETODISPLAY 0

/*!
* \def DEFAULTBUSYLEVEL
* Description
*/
#define DEFAULTBUSYLEVEL 50

/*!
* \def DEFAULTDEBUTH
* Description
*/
#define DEFAULTDEBUTH 8

/*!
* \def DEFAULTDEBUTM
* Description
*/
#define DEFAULTDEBUTM 0

/*!
* \def DEFAULTFINH
* Description
*/
#define DEFAULTFINH 20

/*!
* \def DEFAULTFINM
* Description
*/
#define DEFAULTFINM 0

/*!
* \def DEFAULTBEGINHHH
* Description
*/
#define DEFAULTBEGINHHH 0

/*!
* \def DEFAULTBEGINHHM
* Description
*/
#define DEFAULTBEGINHHM 0

/*!
* \def DEFAULTENDHHH
* Description
*/
#define DEFAULTENDHHH 0

/*!
* \def DEFAULTENDHHM
* Description
*/
#define DEFAULTENDHHM 0

typedef struct
{
    uint8_t heures;
    uint8_t minutes;
} HORAIRES;

typedef struct
{
    uint8_t address;               /*!< Adresse du satellite qui correspond aussi au numéro de la machine pilotée par ce satellite.*/
    uint16_t wCashPrice;           /*!< Prix du service en espèce.*/
    uint16_t wCashLessPrice;       /*!< Prix du service payé avec le système cashless.*/
    uint16_t wCashPriceHH;         /*!< Prix du service en heures creuses.*/
    uint16_t wCashLessPriceHH;     /*!< prix du service payé avec le système cashless en heure creuse.*/
    uint32_t lPulseInMS;           /*!< Durée de l'impulsion pour activé la machine en ms.*/
    uint32_t delayOverBusy;        /*!< Durée de la suroccupation en ms.*/
    uint32_t lTimeToDisplay;       /*! Temps à afficher pour le client sans décompte.*/
    uint32_t BusyLevel;            /*!< Niveau de detection de l'occupation*/
    HORAIRES BeginSelect;          /*!< Heure de début de sélection autorisé.*/
    HORAIRES FinSelect;            /*!< Heure de  fin de sélection autorisé.*/
    HORAIRES BeginHH;              /*!< Heure de début de l'Happy Hour.*/
    HORAIRES EndHH;                /*!< Heure de fin de l'Happy Hour.*/
    bool isCumulEnable;            /*!< Autorise ou non le cumul du temps d'activation*/
    bool isDisplayedTimeRemaining; /*! Affiche le temps restant.*/
    bool isMachinePowered;         /*!< Indique si le relais d'alimentation principale doit être alimenté.*/
    bool isBillAccepted;           /*!<Indique si les billets sont acceptés pour cette machine.*/
} CONFIG;

/**
 * @brief 
 * 
 */
typedef union
{
    CONFIG config;
    uint8_t buffer[sizeof(CONFIG)]; /*!<Tableau d'octets contenant la structure.*/
} MACHINECONFIG;

MACHINECONFIG machineConfig;

/**
 * @brief 
 * 
 */
uint8_t isRelayMachineActivated;

/*!
* \def ADDRESS_MACHINE_CONFIG
* Description
*/
#define ADDRESS_MACHINE_CONFIG 0

/**
 * @brief 
 * 
 */
/*!
* \def ADDRESS_STATE_RELAY_MACHINE
* Description
*/
#define ADDRESS_STATE_RELAY_MACHINE ADDRESS_MACHINE_CONFIG + sizeof(MACHINECONFIG)

/*!
* \fn bool saveMachineConfig(MACHINECONFIG machineconfig.config.config.
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  04/05/2021
* \brief 
* \remarks None
* \param machineconfig.
* \return 
*/
bool saveMachineConfig(MACHINECONFIG machineConfig);

// /*!
// * \fn bool saveDelayActivation(const uint32_t delay)
// * \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
// * \version 0.1
// * \date  01/03/2021
// * \brief
// * \remarks None
// * \param delay
// * \return
// */
// bool saveDelayActivation(const uint32_t delay);

// /*!
// * \fn uint16_t getDelayOverBusy(void)
// * \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
// * \version 0.1
// * \date  01/03/2021
// * \brief
// * \remarks None
// * \return
// */
// uint16_t getDelayOverBusy(void);

/*!
* \fn bool saveRelayMachineState(const uint8_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  09/03/2021
* \brief 
* \remarks None
* \param state 
* \return 
*/
bool saveRelayMachineState(const uint8_t state);

/*!
* \fn void initParameters(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
void initParameters(void);

#endif