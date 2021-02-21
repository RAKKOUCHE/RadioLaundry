
#include "io.h"

#define LO 0
#define HI 1

#define DELAY_TASK_IO 100

#define IO_TAG "IO module"

typedef enum 
{
    IOtASKIdle,
    IOTaskInit,
}IOTaskState_t;

static IOTaskState_t IOTaskState;

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
    ESP_LOGI(IO_TAG,"%s", "IOs iinitialisés.\n");
}


static void setIOState(IOTaskState_t state)
{
    IOTaskState = state;
}

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
        default:
        {
            break;
        }
        }
        vTaskDelayUntil(&xLastWakeTime, DELAY_TASK_IO);
    }
}
