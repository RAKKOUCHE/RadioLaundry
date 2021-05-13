/*!
* \file d:\Projets\RADIOLAUNDRY\SW\Main\Satellite_3\src\io.c
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date 21/02/2021
* \brief 
* \remarks None
*/

/*! Fichiers inclus*/
#include "io.h"

/*!
* \def DELAY_TASK_IO
* Description
*/
#define DELAY_TASK_IO pdMS_TO_TICKS(100)

/*!
* \def TAG_IO
* Description
*/
#define TAG_IO "\nIO module : "

/**
*\brief
*/
static IOTaskState_t IOTaskState;

/**
*\briefe
*/
static gpio_num_t LED;

/*!
* \fn static void InitIO(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \return 
*/
static void InitIO(void)
{
    ESP_ERROR_CHECK(gpio_set_direction(LED_1, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED_2, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MAIN, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MACHINE, GPIO_MODE_INPUT_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DIPSWITCH_2, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(BOOT, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(EMPTY, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_3, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_level(LED_1, LO));
    ESP_ERROR_CHECK(gpio_set_level(LED_2, LO));
    ESP_ERROR_CHECK(gpio_set_level(CTRL_MACHINE, LO));
    ESP_ERROR_CHECK(gpio_set_level(CTRL_MAIN, LO));
    printf("%s%s", TAG_IO, "IOs initialisées\n");
}

/*!
* \fn static vTORelay(xTimerHandle xTimer)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  09/03/2021
* \brief 
* \remarks None
* \param xTimer 
* \return 
*/
void vTORelay(xTimerHandle xTimer)
{
    printf("%s%s", TAG_IO, "Le relais d'activation est desactivé parce-que le délai est atteint.");
    gpio_set_level(CTRL_MACHINE, LO); //Action à effetuer immédaitement.
    setIOState(IORELAYMACHINEOFF);
}

/*!
* \fn void setIOState(IOTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param state 
*/
void setIOState(IOTaskState_t state)
{
    IOTaskState = state;
}

/*!
* \fn IOTaskState_t getIOState(void)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  25/02/2021
* \brief Renvoi l'état de la tâche IO
* \remarks None
* \return L'etat de la tâche IO
*/
IOTaskState_t getIOState(void)
{
    return IOTaskState;
}

/*!
* \fn void setLED(const gpio_num_t led)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param[in] led 
*/
void setLED(const gpio_num_t led)
{
    LED = led;
}

/*!
* \fn void vTaskIO(void *VParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param VParameter 
*/
void vTaskIO(void *VParameter)
{
    uint8_t delay_flash = 4;
    setIOState(IOTASKINIT);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        printf("%s%s%u\n", TAG_IO, "Machine ", machineConfig.config.address);
        switch (IOTaskState)
        {
        case IOTASKIDLE:
        {
            break;
        }
        case IOTASKINIT:
        {
            setIOState(IOTASKIDLE);
            InitIO();
            break;
        }
        case IOLEDFLASH:
        {
            if ((LED == LED_2) && --delay_flash)
            {
                break;
            }

            delay_flash = 10;
            gpio_set_level(LED, !gpio_get_level(LED));
            printf("%s%s%d%s", TAG_IO, "Led ", LED, (gpio_get_level(LED) ? " ON" : " OFF"));

            break;
        }
        case IOLEDOFF:
        {
            setIOState(IOTASKIDLE);
            gpio_set_level(LED, LO);
            printf("%s%s%d%s", TAG_IO, "Led ", LED, " : OFF\n");
            break;
        }
        case IOLEDON:
        {
            setIOState(IOTASKIDLE);
            gpio_set_level(LED, HI);
            printf("%s%s%d%s", TAG_IO, "Led ", LED, " : ON");
            break;
        }
        case IORELAYMACHINEON:
        {

            // gpio_set_level(CTRL_MACHINE, HI);
            printf("%s%s", TAG_IO, "Relais machine activé");
            xTimerChangePeriod(hTORelayMachine, machineConfig.config.lPulseInMS, 1 * SECONDE);

            saveRelayMachineState(HI);
            setIOState(IOTASKIDLE);
            break;
        }
        case IORELAYMACHINEOFF:
        {
            // gpio_set_level(CTRL_MACHINE, LO);
            printf("%s%s", TAG_IO, "Relais machine desactivé");
            saveRelayMachineState(LO);
            setIOState(IOTASKIDLE);
            break;
        }
        case IORELAYMAINON:
        {
            gpio_set_level(CTRL_MAIN, HI);
            printf("%s%s", TAG_IO, "Relais secteur activé");
            setIOState(IOTASKIDLE);
            xTimerStop(hTORelayMachine, 1 * SECONDE);
            break;
        }
        case IORELAYMAINOFF:
        {
            gpio_set_level(CTRL_MAIN, LO);
            printf("%s%s", TAG_IO, "Relais secteur desactivé");
            setIOState(IOTASKIDLE);
            break;
        }
        default:
        {
            break;
        }
        }
        vTaskDelayUntil(&xLastWakeTime, DELAY_TASK_IO);
    }
}
