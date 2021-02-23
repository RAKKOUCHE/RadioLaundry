/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board\include\main.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 23/02/2021
* \brief 
* \r
*/

#ifndef MAIN_H
#define MAIN_h

/*! Fichiers inclus*/
#include "esp_log.h"
#include "io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "flash.h"
#include "logs.h"
#include "parameters.h"
#include "espnow.h"

/*!
* \def DEBUG
* Description Flag à retirer pour la version release
*/
#define DEBUG

uint8_t MachineAddress;


#endif