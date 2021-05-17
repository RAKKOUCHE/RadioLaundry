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
const char *filename = "/data/parameters";
const char *filerandom = "/data/random";

/**
*\brief
 */
FILE *filedata;
FILE *filerand;

/**
*\brief
 */
static esp_err_t err;

/**
 * @brief 
 * 
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
* \fn bool saveMachineConfig(MACHINECONFIG machineconfig.
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  04/05/2021
* \brief 
* \remarks None
* \param machineconfig.
* \return 
*/
bool saveMachineConfig(MACHINECONFIG machineConfig)
{
    volatile int nb;
    volatile bool isFlushed, relayState;
    rewind(filedata);
    if (((nb = fwrite(&machineConfig, sizeof(machineConfig), 1, filedata)) > 0) && (isFlushed = !fflush(filedata)) && (relayState = saveRelayMachineState(LO)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*!
* \fn static bool readStateRelayMachine(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  09/03/2021
* \brief 
* \remarks None
* \return 
*/
static bool readStateRelayMachine(void)
{
    fseek(filedata, ADDRESS_STATE_RELAY_MACHINE, SEEK_SET);
    return fread(&isRelayMachineActivated, sizeof(isRelayMachineActivated), 1, filedata);
}

/*!
* \fn static void initNewDevice()
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  04/05/2021
* \brief 
* \remarks None
* \return 
*/
static void initNewDevice()
{
    memset(machineConfig.buffer, 0, sizeof(machineConfig));
    machineConfig.config.address = NEWDEVICE;
    machineConfig.config.wCashPrice = DEFAULTCASHPRICE;
    machineConfig.config.wCashLessPrice = DEFAULTCLPRICE;
    machineConfig.config.wCashPriceHH = DEFAULTCASHPRICEHH;
    machineConfig.config.wCashLessPriceHH = DEFAULTCLPRICEHH;
    machineConfig.config.lPulseInMS = DEFAULTPULSE;
    machineConfig.config.delayOverBusy = DEFAULTOVERBUSY;
    machineConfig.config.lTimeToDisplay = DEFAULTTIMETODISPLAY;
    machineConfig.config.BusyLevel = DEFAULTBUSYLEVEL;

    machineConfig.config.BeginSelect.heures = DEFAULTDEBUTH;
    machineConfig.config.BeginSelect.minutes = DEFAULTDEBUTM;
    machineConfig.config.FinSelect.heures = DEFAULTFINH;
    machineConfig.config.FinSelect.minutes = DEFAULTFINM;

    machineConfig.config.BeginHH.heures = DEFAULTBEGINHHH;
    machineConfig.config.BeginHH.minutes = DEFAULTBEGINHHM;
    machineConfig.config.EndHH.heures = DEFAULTENDHHH;
    machineConfig.config.EndHH.minutes = DEFAULTENDHHM;
    machineConfig.config.isCumulEnable = false;
    machineConfig.config.isDisplayedTimeRemaining = false;
    machineConfig.config.isMachinePowered = true;
    machineConfig.config.isBillAccepted = true;
    saveMachineConfig(machineConfig);
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
    volatile int number;
    printf("%s%s", TAG_PARAMETER, "Lecture du numéro de la machine");
    if (stat(filename, &s) < 0)
    {
        printf("%s%s", TAG_PARAMETER, "Le fichier n'existe pas!");
        filedata = fopen(filename, "wb+");
        initNewDevice();
        fclose(filedata);
        filedata = NULL;
    }
    if (stat(filerandom, &s) < 0)
    {
        printf("%s%s", TAG_PARAMETER, "Le fichier n'existe pas!");
        filerand = fopen(filerandom, "wb+");
        //initNewDevice();
        fclose(filedata);
        filedata = NULL;
    }
    filerand = fopen(filerandom, "wb+");
    number = fread(&key, 1, 100, filerand);
    number++;

    if (!fread(&machineConfig, sizeof(machineConfig), 1, (filedata = fopen(filename, "rb+"))))
    {
        initNewDevice();
        printf("%s%s", TAG_PARAMETER, "La lecture de la configuration de la machine a échouée");
    }
    else
    {
        //initNewDevice();

        printf("%s%s%u", TAG_PARAMETER, "Le numéro de la machine est : ", machineConfig.config.address);
        rewind(filedata);

        if (machineConfig.config.address == NEWDEVICE)
        {
            initNewDevice();
        }
        printf("%s%s%u%s\n", TAG_PARAMETER, "Le délai de suroccupation est de : ", machineConfig.config.delayOverBusy, " secondes");
        printf("%s%s%u%s\n", TAG_PARAMETER, "Le délai d'activation du relay machine est de :: ", machineConfig.config.lPulseInMS, " millisecondes");
        readStateRelayMachine();
        setIOState(isRelayMachineActivated ? IORELAYMACHINEON : IORELAYMACHINEOFF);
        printf("%s%s%s\n", TAG_PARAMETER, "Le relais machine est ", gpio_get_level(CTRL_MACHINE) ? "activé" : "desactivé");

        //****************************
    }
}

// /*!
// * \fn uint32_t getDelayActivation(void)
// * \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
// * \version 0.1
// * \date  08/03/2021
// * \brief
// * \remarks None
// * \return
// */
// uint32_t getDelayActivation(void)
// {
//     return machineconfig.lPulseInMS;
// }

// /*!
// * \fn bool saveDelayOverBusy(const uint16_t delay)
// * \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
// * \version 0.1
// * \date  01/03/2021
// * \brief
// * \remarks None
// * \param delay
// * \return
// */
// bool saveDelayOverBusy(const uint16_t delay)
// {
//     printf("%s%s%u\n", TAG_PARAMETER, " Enregistrement du délai de suroccupation : ", delay);
//     return ((fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET) == 0) &&
//             (fwrite(&delay, sizeof(delay), 1, filedata) == 1) &&
//             (fflush(filedata) == 0) &&
//             (fseek(filedata, ADDRESS_OVER_BUSY, SEEK_SET) == 0) &&
//             (fread(&machineconfig.delayOverBusy, sizeof(machineconfig.delayOverBusy), 1, filedata) == 1) &&
//             (machineconfig.delayOverBusy == delay));
// }

// /*!
// * \fn bool saveDelayActivation(const uint32_t delay)
// * \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
// * \version 0.1
// * \date  01/03/2021
// * \brief
// * \remarks None
// * \param delay
// * \return
// */
// bool saveDelayActivation(const uint32_t delay)
// {
//     printf("%s%s%u\n", TAG_PARAMETER, " Enregistrement du délai d'occupation : ", delay);
//     return ((fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET) == 0) &&
//             (fwrite(&delay, sizeof(delay), 1, filedata) == 1) &&
//             (fflush(filedata) == 0) &&
//             (fseek(filedata, ADDRESS_DELAY_RELAY, SEEK_SET) == 0) &&
//             (fread(&machineconfig.lPulseInMS, sizeof(machineconfig.lPulseInMS), 1, filedata) == 1) &&
//             (machineconfig.lPulseInMS == delay));
// }

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
    return machineConfig.config.delayOverBusy;
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