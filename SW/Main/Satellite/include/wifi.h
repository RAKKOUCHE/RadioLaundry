/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite\include\wifi.h
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief 
* \remarks None
*/


#ifndef WIFI_H
#define WIFI_H

/*! Importation of librairies*/
#include <stdint.h>
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_log.h"

/*!
* \fn void InitWifi(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
*/
void InitWifi(void);

#endif