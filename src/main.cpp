#include <Arduino.h>

#include "Calibration/calibration.h"
#include "Flash/flash.h"

// Variável global para controlar o modo de operação
bool isCalibrationMode = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Sistema de Sensores de Qualidade do Ar ---");

  // Verifica o estado do dispositivo usando o módulo de flash
  DeviceState status = flash_check_status();

  switch (status) {
    case CALIBRATION_COMPLETE:
      Serial.println("Status: Calibração já concluída. Carregando dados...");
      flash_load_final_r0(sensores, NUM_SENSORES);
      isCalibrationMode = false;
      Serial.println("--- Sistema em modo de operação normal ---");
      break;

    case CALIBRATION_IN_PROGRESS:
      isCalibrationMode = true;
      calibration_setup(status);
      break;

    case CALIBRATION_NEEDED:
      isCalibrationMode = true;
      calibration_setup(status);
      break;
  }
}

void loop() {
  if (isCalibrationMode) {
    calibration_loop();
  } else {
    normal_operation_loop();
  }
}
