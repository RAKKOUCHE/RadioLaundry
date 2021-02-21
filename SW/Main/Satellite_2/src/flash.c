#include "flash.h"

static const char *TAG_FLASH = "Module FLASH";

void InitFlash(void)
{
    esp_err_t ret;
    ESP_LOGI(TAG_FLASH, "%s", "Début de l'initialisation du module FLASH\n");
    ESP_ERROR_CHECK(ret = nvs_flash_init());
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(ret = nvs_flash_init());
    }  
}