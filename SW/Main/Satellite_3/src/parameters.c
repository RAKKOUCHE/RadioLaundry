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

/**
* \def NEWDEVICE
 * 
 */
#define NEWDEVICE 0XFF

/*!
* \def DEFAUTDELAYOVERBUSY
* Description
*/
#define DEFAUTDELAYOVERBUSY 30

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
    printf("%s%s", TAG_PARAMETER, "Vérification SPIFFS");
    conf.base_path = "/data";
    conf.partition_label = NULL;
    conf.max_files = 2;
    conf.format_if_mount_failed = true;
    ESP_ERROR_CHECK(err = esp_vfs_spiffs_register(&conf));
    if (err)
    {
        if (err == ESP_FAIL)
        {
            printf("%s%s", TAG_PARAMETER, "Failed to mount or format filesystem");
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            printf("%s%s", TAG_PARAMETER, "Failed to find SPIFFS partition");
        }
        else
        {
            printf("%s%s(%s)", TAG_PARAMETER, "Failed to initialize SPIFFS ", esp_err_to_name(err));
        }
    }
    else
    {
        printf("%s%s", TAG_PARAMETER, "Système fichiers monté.");
    }
    return (err == 0);
}

/*!
* \fn static void InitNewDevice(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  08/03/2021
* \brief 
* \remarks None
* \return 
*/
static void InitNewDevice(void)
{
    // delayOverBusy = DEFAUTDELAYOVERBUSY;
    // delayActivation = DEFAULDELAYACTIVION;
    // fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET);
    // fwrite(&delayOverBusy, sizeof(delayOverBusy), 1, filedata);
    // fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET);
    // fwrite(&delayActivation, sizeof(delayActivation), 1, filedata);
    saveDelayOverBusy((delayOverBusy = DEFAUTDELAYOVERBUSY));
    saveDelayActivation(delayActivation = DEFAUTDELAYACTIVION);
    saveRelayMachineState(LO);
}

/*!
* \fn static uint32_t readDelayOverBusy(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  08/03/2021
* \brief 
* \remarks None
* \return 
*/
static uint32_t readDelayOverBusy(void)
{
    fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET);
    return fread(&delayOverBusy, sizeof(delayOverBusy), 1, filedata);
}

/*!
* \fn static uint32_t readDelayActivation(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  08/03/2021
* \brief 
* \remarks None
* \return 
*/
static uint32_t readDelayActivation(void)
{
    fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET);
    return fread(&delayActivation, sizeof(delayActivation), 1, filedata);
}

/*!
* \fn static uint8_t readStateRelayMachine(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  09/03/2021
* \brief 
* \remarks None
* \return 
*/
static uint8_t readStateRelayMachine(void)
{
    fseek(filedata, ADDRESS_STATE_RELAY_MACHINE, SEEK_SET);
    return fread(&isRelayMachineActivated, sizeof(isRelayMachineActivated), 1, filedata);
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

    printf("%s%s", TAG_PARAMETER, "Lecture du numéro de la machine");
    if (stat(filename, &s) < 0)
    {
        printf("%s%s", TAG_PARAMETER, "Le fichier n'existe pas!");
        MachineAddress = NEWDEVICE;
        fwrite(&MachineAddress, sizeof(MachineAddress), 1, (filedata = fopen(filename, "wb+")));
        fflush(filedata);
        fclose(filedata);
        filedata = NULL;
    }

    if (!fread(&MachineAddress, sizeof(MachineAddress), 1, filedata = fopen(filename, "rb+")))
    {
        printf("%s%s", TAG_PARAMETER, "La lecture du numéro de la machine a échoué");
    }
    else
    {

        printf("%s%s%u", TAG_PARAMETER, "Le numéro de la machine est : ", MachineAddress);
        rewind(filedata);
        //saveMachineNumber(MachineAddress = 11);

        if (MachineAddress == NEWDEVICE)
        {
            InitNewDevice();
        }
        readDelayOverBusy();
        printf("%s%s%u%s\n", TAG_PARAMETER, "Le délai de suroccupation est de : ", getDelayOverBusy(), " secondes");
        readDelayActivation();
        printf("%s%s%u%s\n", TAG_PARAMETER, "Le délai d'activation du relay machine est de :: ", getDelayActivation(), " millisecondes");
        readStateRelayMachine();
        setIOState(isRelayMachineActivated ? IORELAYMACHINEON : IORELAYMACHINEOFF);
        printf("%s%s%s", TAG_PARAMETER, "Le relais machine est ", gpio_get_level(CTRL_MACHINE) ? "activé" : "desactivé");
        printf("\n");
        //****************************
    }
}

/*!
* \fn uint32_t getDelayActivation(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  08/03/2021
* \brief 
* \remarks None
* \return 
*/
uint32_t getDelayActivation(void)
{
    return delayActivation;
}

/*!
* \fn bool saveMachineNumber(const uint8_t address)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  24/02/2021
* \brief Enregistre l'adresse du module
* \remarks None
* \param address 
* \return 
*/
bool saveMachineNumber(const uint8_t address)
{
    rewind(filedata);
    fwrite(&address, sizeof(address), 1, filedata);
    fflush(filedata);
    fseek(filedata, 0, SEEK_SET);
    return (fread(&MachineAddress, sizeof(MachineAddress), 1, filedata) && (MachineAddress == address));
}

/*!
* \fn bool saveDelayOverBusy(const uint16_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \param delay 
* \return 
*/
bool saveDelayOverBusy(const uint16_t delay)
{
    return ((fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET) == 0) &&
            (fwrite(&delay, sizeof(delay), 1, filedata) == 1) &&
            (fflush(filedata) == 0) &&
            (fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET) == 0) &&
            (fread(&delayOverBusy, sizeof(delayOverBusy), 1, filedata) == 1) &&
            (delayOverBusy == delay));
}

/*!
* \fn bool saveDelayActivation(const uint32_t delay)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \param delay 
* \return 
*/
bool saveDelayActivation(const uint32_t delay)
{
    return ((fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET) == 0) &&
            (fwrite(&delay, sizeof(delay), 1, filedata) == 1) &&
            (fflush(filedata) == 0) &&
            (fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET) == 0) &&
            (fread(&delayActivation, sizeof(delayActivation), 1, filedata) == 1) &&
            (delayActivation == delay));
}

/*!
* \fn bool saveRelayMachineState(const uint8_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  09/03/2021
* \brief 
* \remarks None
* \param state 
* \return 
*/
bool saveRelayMachineState(const uint8_t state)
{
    gpio_set_level(CTRL_MACHINE, (isRelayMachineActivated = state));
    printf("%s%s%s", TAG_PARAMETER, "Relais machine est ", state == HI ? "activé" : "desactivé");
    return ((fseek(filedata, ADDRESS_STATE_RELAY_MACHINE, SEEK_SET) == 0) &&
            (fwrite(&isRelayMachineActivated, sizeof(isRelayMachineActivated), 1, filedata) == 1) &&
            (fflush(filedata) == 0) &&
            (fseek(filedata, ADDRESS_STATE_RELAY_MACHINE, SEEK_SET) == 0) &&
            (fread(&isRelayMachineActivated, sizeof(isRelayMachineActivated), 1, filedata) == 1) &&
            (isRelayMachineActivated == state));
}

/*!
* \fn uint16_t getDelayOverBusy(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  01/03/2021
* \brief 
* \remarks None
* \return 
*/
uint16_t getDelayOverBusy(void)
{
    return delayOverBusy;
}

/*!
* \fn void initParameters(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
*/
void initParameters(void)
{
    printf("%s%s", TAG_PARAMETER, "Initialisation et lecture des paramètres.");
    if (setVFS_SPIFFS())
    {
        checkFileParameters();
    }
}