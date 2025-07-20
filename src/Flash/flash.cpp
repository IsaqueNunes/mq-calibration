#include "flash.h"

#include <Preferences.h>

Preferences preferences;
const char *PREFERENCES_NAMESPACE = "mq-calib";

DeviceState flash_check_status() {
  preferences.begin(PREFERENCES_NAMESPACE, true);  // true = Read-only
  bool completed = preferences.getBool("calib_done", false);
  unsigned long start_time = preferences.getULong("startTime", 0);
  preferences.end();

  if (completed) {
    return CALIBRATION_COMPLETE;
  } else if (start_time > 0) {
    return CALIBRATION_IN_PROGRESS;
  } else {
    return CALIBRATION_NEEDED;
  }
}

void flash_load_final_r0(Sensor sensors[], int num_sensors) {
  preferences.begin(PREFERENCES_NAMESPACE, true);  // Read-only
  for (int i = 0; i < num_sensors; i++) {
    sensors[i].r0 = preferences.getFloat(sensors[i].r0_key, 0.0);
  }
  preferences.end();
}

void flash_load_progress(
    Sensor sensors[],
    int num_sensors,
    unsigned long &start_time,
    unsigned long &sample_count
) {
  preferences.begin(PREFERENCES_NAMESPACE, true);  // Read-only
  start_time = preferences.getULong("startTime", 0);
  sample_count = preferences.getULong("sampleCount", 0);
  for (int i = 0; i < num_sensors; i++) {
    sensors[i].r0_sum = preferences.getDouble(sensors[i].r0_sum_key, 0.0);
  }
  preferences.end();
}

void flash_save_progress(
    Sensor sensors[],
    int num_sensors,
    unsigned long start_time,
    unsigned long sample_count
) {
  preferences.begin(PREFERENCES_NAMESPACE, false);  // Read-write
  preferences.putULong("startTime", start_time);
  preferences.putULong("sampleCount", sample_count);
  for (int i = 0; i < num_sensors; i++) {
    preferences.putDouble(sensors[i].r0_sum_key, sensors[i].r0_sum);
  }
  preferences.end();
  Serial.printf(
      "\n[Progresso salvo na memória flash. Tempo decorrido: %lu segundos]\n",
      (millis() - start_time) / 1000
  );
}

void flash_commit_final_results(Sensor sensors[], int num_sensors) {
  preferences.begin(PREFERENCES_NAMESPACE, false);  // Read-write
  for (int i = 0; i < num_sensors; i++) {
    preferences.putFloat(sensors[i].r0_key, sensors[i].r0);
  }
  preferences.putBool("calib_done", true);
  preferences.end();
  Serial.println("Valores finais de R0 salvos permanentemente.");
}
