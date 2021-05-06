/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\flash.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief 
* \remarks None
*/

/*! Fichiers inclus*/
#include "flash.h"

#define TAG_FLASH "\nFlash Module : "

/*!
* \fn void InitFlash(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief Initialisation de la flash comme indiqué par espressif
* \remarks None
*/
void InitFlash(void)
{
    esp_err_t ret;
    ESP_ERROR_CHECK(ret = nvs_flash_init());
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(ret = nvs_flash_init());
    }  
    printf("%s%s",TAG_FLASH,  "Fin de l'initialisation de la flash");
}