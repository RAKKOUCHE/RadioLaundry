#include "parameters.h"

static const char *TAG_TEST3 = "Test bfichier binaire";
static const char *filename = "/data/parameters";
static FILE *filedata;
static esp_err_t err;

static esp_vfs_spiffs_conf_t conf;

static bool setVFS_SPIFFS(void)
{
    conf.base_path = "/data";
    conf.partition_label = NULL;
    conf.max_files = 5;
    conf.format_if_mount_failed = true;
    ESP_ERROR_CHECK(err = esp_vfs_spiffs_register(&conf));
    if (err)
    {
        if (err == ESP_FAIL)
        {
            ESP_LOGE(TAG_TEST3, "Failed to mount or format filesystem\n");
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG_TEST3, "Failed to find SPIFFS partition\n");
        }
        else
        {
            ESP_LOGE(TAG_TEST3, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(err));
        }
    }
    return (err == 0);
}

static void checkFileParameters(void)
{
    filedata = fopen(filename, "wb+");
    if(!(fread(&MachineAddress, sizeof(MachineAddress), 1, filedata)))
    {
        ESP_LOGI(TAG_TEST3, "%s", "première utilisation");
        MachineAddress = 0XFF;
        fwrite(&MachineAddress, sizeof(MachineAddress), 1, filedata );
        fseek(filedata, 0, 	SEEK_SET);
    }
}

void InitParameters(void)
{
    if(setVFS_SPIFFS())
    {
        checkFileParameters();
    }
}