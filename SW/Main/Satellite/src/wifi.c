/*!
* \file d:\Projets\RadioLaundry\SW\Main\Satellite\src\wifi.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 16/02/2021
* \brief 
* \remarks None
*/

/*! Importation of librairies*/
#include "wifi.h"

/*!
* \def WIFI_TAG
* Description
*/
#define WIFI_TAG "WIFI module"

/*!
* \def ESPNOW_WIFI_MODE
* Description
*/
#define ESPNOW_WIFI_MODE WIFI_MODE_STA

/*!
* \fn void InitWifi(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  16/02/2021
* \brief 
* \remarks None
*/
void InitWifi(void)
{
    
    uint8_t macAddress[6];
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(3, 0));
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_MODE_STA, macAddress));
    ESP_LOGI(WIFI_TAG, "mac adressse : %02X:%02X:%02X:%02X:%02X:%02X\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
}