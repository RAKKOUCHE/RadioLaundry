/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board_ESP32\include\parameters.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 05/05/2021
* \brief 
* \remarks None
*/

#ifndef PARAMETERS_H
#define PARAMETERS_H

/*! Fichiers inclus*/
#include <stdio.h>

/**
 * @brief 
 * 
 */
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
#endif