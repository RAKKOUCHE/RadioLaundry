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
#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"

/*!
* \def DEBUG
* Description Flag à retirer pour la version release
*/
#define DEBUG

uint8_t MachineAddress;

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