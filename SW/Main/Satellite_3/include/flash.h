/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\include\flash.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

#ifndef FLASH_H
#define FLASH_H

/*! Fichiers includes*/
#include <string.h>
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_err.h"

/*!
* \fn void initFlash(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
void initFlash(void);

#endif
