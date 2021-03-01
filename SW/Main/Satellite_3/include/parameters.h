/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\parameters.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier entête du fichier de gestion des paramètres.
* \remarks None
*/

#ifndef PARAMETERS_H
#define PARANETERS_H

/*! Fichiers inclus*/
/*! Fichier inclus*/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"

/*!
* \def ADDRESS_POS
* Description
*/
#define ADDRESS_POS 0
/*!
* \def ADDRESS_OVER_BUSY
* Description
*/
#define ADDRESS_OVER_BUSY ADDRESS_POS + sizeof(uint8_t)
/*!
* \def ADDRESS_DELAY_RELAY
* Description
*/
#define ADDRESS_DELAY_RELAY ADDRESS_OVER_BUSY + sizeof(uint32_t)

/*!
* \def DEBUG
* Description Flag à retirer pour la version release
*/
#define DEBUG

/**
 * @brief 
 * 
 */
uint8_t MachineAddress;

/*!
* \fn bool saveMachineNumber(const uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  24/02/2021
* \brief 
* \remarks None
* \param address 
* \return 
*/

bool saveMachineNumber(const uint8_t address);
/*!
* \fn bool saveDelayOverBusy(const uint16_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \param delay 
* \return 
*/
bool saveDelayOverBusy(const uint16_t delay);

/*!
* \fn uint16_t getDelayOverBusy(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \return 
*/
uint16_t getDelayOverBusy(void);

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