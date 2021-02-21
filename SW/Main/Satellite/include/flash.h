#ifndef FLASH_H
#define FLASH_H

#include <string.h>
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_err.h"

void InitFlash(void);

#endif
