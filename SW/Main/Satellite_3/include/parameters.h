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
* \fn void InitParameters(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
void InitParameters(void);

#endif