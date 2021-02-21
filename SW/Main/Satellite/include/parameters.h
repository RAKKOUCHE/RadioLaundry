#ifndef PARAMETERS_H
#define PARANETERS_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_log.h"

uint8_t MachineAddress;

void InitParameters(void);

#endif