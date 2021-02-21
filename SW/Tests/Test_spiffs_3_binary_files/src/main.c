#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

#define DEBUG

static const char *TAG_TEST3 = "Test bfichier binaire";
const char *filename = "/data/parameters";

FILE *filedata;

uint8_t MachineAddress;

int read_or_write;

void app_main()
{
    esp_err_t ret;
    ESP_LOGI(TAG_TEST3, "%s", "Début\n");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/data",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};
#ifdef DEBUG
    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG_TEST3, "Failed to mount or format filesystem\n");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG_TEST3, "Failed to find SPIFFS partition\n");
        }
        else
        {
            ESP_LOGE(TAG_TEST3, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return;
    }
#else
    ESP_ERROR_CHECK(ret = esp_vfs_spiffs_register(&conf));
#endif

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_TEST3, "Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG_TEST3, "Partition size: total: %d, used: %d\n", total, used);
    }

    struct stat buffer;
    if (stat(filename, &buffer))
    {
        ESP_LOGI(TAG_TEST3, "%s", "Création du fichier\n");
        if ((filedata = fopen(filename, "wb")))
        {
            MachineAddress = 0XFF;
            read_or_write = fwrite(&MachineAddress, sizeof(MachineAddress), 1, filedata);
            fflush(filedata);
            ESP_ERROR_CHECK(fclose(filedata));
        }
        else
        {
            ESP_LOGE(TAG_TEST3, "%s", "Echec de la création du fichier des datas\n");
        }
    }

    MachineAddress = 0;
    filedata = NULL;
    //Ouvre ou crée le fichier des données
    if ((filedata = fopen(filename, "wb+")))
    {
        if (!(read_or_write = fread(&MachineAddress, sizeof(MachineAddress), 1, filedata)))
        {
            ESP_LOGE(TAG_TEST3, "%s", "La lecture des données a échouée");
        }
    }
    else
    {
        ESP_LOGE(TAG_TEST3, "%s", "Echec de lecture du fichier des paramètres");
    }

    fclose(filedata);
    while (1)
    {
        vTaskDelay(1);
    }
}