/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\parameters.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief Fichier gérant les paramètres
* \remarks None
*/

/*! Fichiers inclus*/
/*! Fichier inclus*/
#include "parameters.h"

/*!
* \def TAG_PARAMETER
* Description
*/
#define TAG_PARAMETER "\nModule paramètres : "

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
    printf("%s%s",TAG_PARAMETER,  "Vérification SPIFFS");
    conf.base_path = "/data";
    conf.partition_label = NULL;
    conf.max_files = 2;
    conf.format_if_mount_failed = true;
    ESP_ERROR_CHECK(err = esp_vfs_spiffs_register(&conf));
    if (err)
    {
        if (err == ESP_FAIL)
        {
            printf("%sFailed to mount or format filesystem", TAG_PARAMETER );
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            printf("%sFailed to find SPIFFS partition", TAG_PARAMETER);
        }
        else
        {
            printf("%sFailed to initialize SPIFFS (%s)", TAG_PARAMETER,  esp_err_to_name(err));
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
* \return 
*/
static void checkFileParameters(void)
{
    struct stat s;
    printf("%s%s",TAG_PARAMETER,  "Lecture du numéro de la machine");
    if (stat(filename, &s) < 0)
    {
        printf("%s%s", TAG_PARAMETER, "Le fichier n'existe pas!");
        MachineAddress = 0XFF;
        fwrite(&MachineAddress, sizeof(MachineAddress), 1, (filedata = fopen(filename, "wb+")));
        fflush(filedata);
        fclose(filedata);
        filedata = NULL;
    }
    
    if (!fread(&MachineAddress, sizeof(MachineAddress), 1, filedata = fopen(filename, "rb+")))
    {
        printf("%s%s",TAG_PARAMETER,  "La lecture du numéro de la machine a échoué");
    }
    else
    {
        printf("%s%s%u", TAG_PARAMETER, "La lecture du numéro de la machine est : ", MachineAddress);
        fseek(filedata, 0, SEEK_SET);
        if (MachineAddress == 0xff)
        {
            MachineAddress = 11;
            fwrite(&MachineAddress, sizeof(MachineAddress), 1, filedata);
            fflush(filedata);
        }
    }
}

/*!
* \fn bool saveMachineNumber(const uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  24/02/2021
* \brief Enregistre l'adresse du module
* \remarks None
* \param address 
*/
bool saveMachineNumber(const uint8_t address)
{
    fseek(filedata, 0, SEEK_SET);
    fwrite(&address, sizeof(address), 1, filedata);
    fflush(filedata);
    fseek(filedata, 0, SEEK_SET);
    return (fread(&MachineAddress, sizeof(MachineAddress), 1,filedata) && (MachineAddress == address));
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
    printf("%s%s",TAG_PARAMETER,  "Initialisation et lecture des paramètres.");
    if (setVFS_SPIFFS())
    {
        checkFileParameters();
    }
}