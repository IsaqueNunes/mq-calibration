#ifndef CALIBRATION_MANAGER_H
#define CALIBRATION_MANAGER_H

#include <Arduino.h>

#include "types.h"

// Declaração das funções públicas
void calibration_setup(DeviceState status);
void calibration_loop();
void normal_operation_loop();  // Função para o modo de operação normal

// Declara o array de sensores como 'extern' para que o main.cpp possa acessá-lo
extern Sensor sensores[];
extern const int NUM_SENSORES;

#endif  // CALIBRATION_MANAGER_H
