#ifndef TYPES_H
#define TYPES_H

// Enum compartilhado para o estado do dispositivo
enum DeviceState {
  CALIBRATION_COMPLETE,
  CALIBRATION_IN_PROGRESS,
  CALIBRATION_NEEDED
};

// Struct compartilhada para os dados do sensor
struct Sensor {
  const char* nome;
  const char* r0_key;
  const char* r0_sum_key;
  int pino;
  float rl_value;
  float r0;
  double r0_sum;
};

#endif  // TYPES_H
