/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Main_board_ESP32\include\main.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 23/02/2021
* \brief 
* \r
*/

#ifndef MAIN_H
#define MAIN_h

/*! Fichiers inclus*/
#include "io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "flash.h"
#include "espnow.h"
#include "adc.h"
#include "esp_spiffs.h"

/*!
* \def DEBUG
* \brief Flag à retirer pour la version release
*/
#define DEBUG

/**
 * @brief Numéro de la machine utilisée pour les test
 * 
 */
uint8_t MachineAddress = 11;

/**
 * @brief delay de clignotement de la led
 * 
 */
uint8_t delay = 5;

/**
 * @brief 
 * 
 */
uint8_t key[1024];

#endif