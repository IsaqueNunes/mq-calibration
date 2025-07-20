#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <Arduino.h>

#include "types.h"

// Declaração das funções públicas do módulo
DeviceState flash_check_status();

void flash_load_final_r0(Sensor sensors[], int num_sensors);

void flash_load_progress(
    Sensor sensors[],
    int num_sensors,
    unsigned long &start_time,
    unsigned long &sample_count
);

void flash_save_progress(
    Sensor sensors[],
    int num_sensors,
    unsigned long start_time,
    unsigned long sample_count
);

void flash_commit_final_results(Sensor sensors[], int num_sensors);

#endif  // FLASH_MANAGER_H
