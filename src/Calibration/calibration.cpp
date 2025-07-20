#include "calibration.h"

#include "Flash/flash.h"

// --- Definições dos Sensores e da Calibração ---
const unsigned long CALIBRATION_DURATION_MS = 48UL * 3600 * 1000;
const unsigned long SAVE_PROGRESS_INTERVAL_MS = 5UL * 60 * 1000;

#define VCC 5.0
#define ADC_RESOLUTION 4095.0
#define VREF 3.3

// Instanciação do array de sensores
Sensor sensores[] = {
    {"MQ-3",   "r0_mq3",   "sum_mq3",   34, 5.0, 0.0, 0.0},
    {"MQ-135", "r0_mq135", "sum_mq135", 35, 5.0, 0.0, 0.0},
    {"MQ-136", "r0_mq136", "sum_mq136", 32, 5.0, 0.0, 0.0},
    {"MQ-137", "r0_mq137", "sum_mq137", 33, 5.0, 0.0, 0.0}
};
const int NUM_SENSORES = sizeof(sensores) / sizeof(Sensor);

// Variáveis internas do módulo de calibração
static unsigned long calibrationStartTime = 0;
static unsigned long sampleCount = 0;
static unsigned long lastSaveTime = 0;

// Função privada para calcular o Rs
static float calcularRs(int pino, float rl_kohm) {
  int valor_adc = analogRead(pino);
  float vout = (valor_adc * VREF) / ADC_RESOLUTION;
  if (vout == 0) return 0;
  return ((VCC * rl_kohm) / vout) - rl_kohm;
}

void calibration_setup(DeviceState status) {
  if (status == CALIBRATION_IN_PROGRESS) {
    Serial.println("Progresso de calibração encontrado. Recuperando estado...");
    flash_load_progress(
        sensores, NUM_SENSORES, calibrationStartTime, sampleCount
    );
    unsigned long elapsedTime = millis() - calibrationStartTime;
    Serial.printf("  > Tempo já decorrido: %lu segundos\n", elapsedTime / 1000);
    Serial.printf("  > Amostras já coletadas: %lu\n", sampleCount);
  } else {  // CALIBRATION_NEEDED
    Serial.println("Nenhum progresso encontrado. Iniciando nova calibração...");
    calibrationStartTime = millis();
    sampleCount = 0;
    for (int i = 0; i < NUM_SENSORES; i++) {
      sensores[i].r0_sum = 0.0;
    }
    flash_save_progress(
        sensores, NUM_SENSORES, calibrationStartTime, sampleCount
    );
  }
  lastSaveTime = millis();
  Serial.printf(
      "A calibração irá durar um total de %lu horas.\n",
      CALIBRATION_DURATION_MS / (3600 * 1000)
  );
}

void calibration_loop() {
  unsigned long elapsedTime = millis() - calibrationStartTime;

  for (int i = 0; i < NUM_SENSORES; i++) {
    sensores[i].r0_sum += calcularRs(sensores[i].pino, sensores[i].rl_value);
  }
  sampleCount++;

  if (sampleCount % 10 == 0) {
    Serial.printf(
        "Calibrando... Tempo: %lu / %lu s | Amostras: %lu\n",
        elapsedTime / 1000,
        CALIBRATION_DURATION_MS / 1000,
        sampleCount
    );
  }

  if (millis() - lastSaveTime >= SAVE_PROGRESS_INTERVAL_MS) {
    flash_save_progress(
        sensores, NUM_SENSORES, calibrationStartTime, sampleCount
    );
    lastSaveTime = millis();
  }

  if (elapsedTime >= CALIBRATION_DURATION_MS) {
    Serial.println("\n--- CALIBRAÇÃO FINALIZADA ---");
    for (int i = 0; i < NUM_SENSORES; i++) {
      if (sampleCount > 0) {
        sensores[i].r0 = sensores[i].r0_sum / sampleCount;
      }
    }
    flash_commit_final_results(sensores, NUM_SENSORES);
    Serial.println(
        "Reiniciando em 5 segundos para entrar em modo de operação..."
    );
    delay(5000);
    ESP.restart();
  }
  delay(1000);
}

void normal_operation_loop() {
  Serial.print("Leitura (Rs/R0): ");
  for (int i = 0; i < NUM_SENSORES; i++) {
    if (sensores[i].r0 > 0) {
      float rs = calcularRs(sensores[i].pino, sensores[i].rl_value);
      float ratio = rs / sensores[i].r0;
      Serial.printf(" | %s: %.2f ", sensores[i].nome, ratio);
    }
  }
  Serial.println("|");
  delay(2000);
}
