/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\parameters.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier gérant les paramètres
* \remarks None
*/

/*! Fichiers inclus*/
#include "parameters.h"

/*!
* \def TAG_PARAMETER
* Description
*/
#define TAG_PARAMETER "Module paramètres"

/**
*\brief Nom du fichier contenant les paramètres
 */
static const char *filename = "/data/parameters";

/**
*\brief
 */
static FILE *filedata;
/**
*\brief
 */
static esp_err_t err;

/**
*\brief
 */
static esp_vfs_spiffs_conf_t conf;

/*!
* \fn static bool setVFS_SPIFFS(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \return 
*/
static bool setVFS_SPIFFS(void)
{
    ESP_LOGI(TAG_PARAMETER, "%s", "Vérification SPIFFS\n");
    conf.base_path = "/data";
    conf.partition_label = NULL;
    conf.max_files = 2;
    conf.format_if_mount_failed = true;
    ESP_ERROR_CHECK(err = esp_vfs_spiffs_register(&conf));
    if (err)
    {
        if (err == ESP_FAIL)
        {
            ESP_LOGE(TAG_PARAMETER, "Failed to mount or format filesystem\n");
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG_PARAMETER, "Failed to find SPIFFS partition\n");
        }
        else
        {
            ESP_LOGE(TAG_PARAMETER, "Failed to initialize SPIFFS (%s)\n", esp_err_to_name(err));
        }
    }
    return (err == 0);
}

/*!
* \fn static void checkFileParameters(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief Vérification de la présence du fichier des paramètres et lecture du nuuméro de la machine.
* \details Vérifie si le fichier existe, sinon crée le fichier avec un un numéro de machine 255
* indiquant que la carte est vierge. Dans le cas où le fichier existe, lecture du numéro de machine.
* \remarks Si le fichier des paramètres n'existe pas, il s'agit de la première utilisation de la carte.
*/
static void checkFileParameters(void)
{
    struct stat s;
    ESP_LOGI(TAG_PARAMETER, "%s", "Lecture du numéro de la machine\n");
    if (stat(filename, &s) < 0)
    {
        ESP_LOGD(TAG_PARAMETER, "%s", "Le fichier n'existe pas!\n");
        MachineAddress = 0XFF;
        fwrite(&MachineAddress, sizeof(MachineAddress), 1, (filedata = fopen(filename, "wb+")));
        fflush(filedata);
        fclose(filedata);
        filedata = NULL;
    }
    
    if (!fread(&MachineAddress, sizeof(MachineAddress), 1, filedata = fopen(filename, "rb+")))
    {
        ESP_LOGD(TAG_PARAMETER, "%s", "La lecture du numéro de la machine a échoué\n");
    }
    else
    {
        ESP_LOGI(TAG_PARAMETER, "%s%u\n", "La lecture du numéro de la machine est : ", MachineAddress);
        fseek(filedata, 0, SEEK_SET);
#ifdef DEBUG
        if (MachineAddress == 0xff)
        {
            MachineAddress = 0x01;
            fwrite(&MachineAddress, sizeof(MachineAddress), 1, filedata);
            fflush(filedata);
        }
#endif
    }
    fclose(filedata);
}

/*!
* \fn void InitParameters(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
void InitParameters(void)
{
    ESP_LOGI(TAG_PARAMETER, "%s", "Initialisation et lecture des paramètres.");
    if (setVFS_SPIFFS())
    {
        checkFileParameters();
    }
}