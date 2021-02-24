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
* \def LO
* Description
*/
#define LO 0
/*!
* \def HI
* Description
*/
#define HI 1

/*!
* \def DELAY_TASK_IO
* Description
*/
#define DELAY_TASK_IO 100

/*!
* \def IO_TAG
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
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MAIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(CTRL_MACHINE, GPIO_MODE_OUTPUT));
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
    printf("%s %s", TAG_IO, " IOs iinitialisés.\n");
}

/*!
* \fn static void setIOState(IOTaskState_t state)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param state 
* \return 
*/
void setIOState(IOTaskState_t state)
{
    IOTaskState = state;
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
* \fn void TaskIO(void *VParameter)
* \author Rachid AKKOUCHE <rachid.akkouche@wanadoo.fr>
* \version 0.1
* \date  21/02/2021
* \brief 
* \remarks None
* \param VParameter 
*/
void TaskIO(void *VParameter)
{
    setIOState(IOTaskInit);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        switch (IOTaskState)
        {
        case IOtASKIdle:
        {
            break;
        }
        case IOTaskInit:
        {
            setIOState(IOtASKIdle);
            InitIO();
            break;
        }
        case IOLedFlash:
        {
            gpio_set_level(LED, !gpio_get_level(LED));
            printf("%s Led %u : %s\n",TAG_IO,  LED,  (gpio_get_level(LED) ? "ON" : "OFF"));
            break;
        }
        case IOLEDOff:
        {
            setIOState(IOtASKIdle);
            gpio_set_level(LED, LO);
            printf("%sLed %u : %s\n", TAG_IO, LED, "OFF");
            break;
        }
        case IOLEDOn:
        {
            setIOState(IOtASKIdle);
            gpio_set_level(LED, HI);
            printf("%sLed %u : %s\n", TAG_IO, LED, "ON");
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
