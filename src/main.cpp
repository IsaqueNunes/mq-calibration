#include <Arduino.h>

// --- Configurações dos Sensores ---

// Pinos ADC onde os sensores estão conectados
#define MQ3_PIN 34
#define MQ135_PIN 35
#define MQ136_PIN 33
#define MQ137_PIN 32

// Tensão de alimentação dos módulos (geralmente 5V)
#define VCC 5.0

// Valor do resistor de carga (RL) em KOhms
#define RL_MQ3 5.0
#define RL_MQ135 5.0
#define RL_MQ136 5.0
#define RL_MQ137 5.0

// Parâmetros do ADC do ESP32
#define ADC_RESOLUTION 4095.0  // Resolução de 12 bits (0-4095)
#define VREF 3.3               // Tensão de referência interna do ADC do ESP32

struct Sensor {
  const char* nome;
  int pino;
  float rl_value;
  float r0;
};

Sensor sensores[] = {
    {"MQ-3",   MQ3_PIN,   RL_MQ3,   0.0},
    {"MQ-135", MQ135_PIN, RL_MQ135, 0.0},
    {"MQ-136", MQ136_PIN, RL_MQ136, 0.0},
    {"MQ-137", MQ137_PIN, RL_MQ137, 0.0}
};

const int NUM_SENSORES = sizeof(sensores) / sizeof(Sensor);

/**
 * @brief Calcula a resistência do sensor (Rs) em KOhms.
 *
 * @param pino Pino analógico conectado ao sensor.
 * @param rl_kohm Valor do resistor de carga (RL) em KOhms.
 * @return float A resistência calculada do sensor (Rs) em KOhms.
 */
float calcularRs(int pino, float rl_kohm) {
  // Lê o valor bruto do ADC
  int valor_adc = analogRead(pino);

  // Converte o valor do ADC para tensão (Vout)
  // O ADC do ESP32 lê tensões de até 3.3V
  float vout = (valor_adc * VREF) / ADC_RESOLUTION;

  // Evita divisão por zero se a leitura for 0
  if (vout == 0) {
    return 0;  // Ou um valor que indique erro
  }

  // Calcula Rs usando a fórmula do divisor de tensão
  // Rs = ( (VCC * RL) / Vout ) - RL
  float rs = ((VCC * rl_kohm) / vout) - rl_kohm;

  return rs;
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // Aguarda a inicialização do Serial

  Serial.println("--- INICIANDO CALIBRAÇÃO DOS SENSORES MQ ---");
  Serial.println(
      "Monitore os valores de Rs. Eles devem se estabilizar após o período de "
      "burn-in."
  );
  Serial.println(
      "Anote os valores estáveis de Rs para cada sensor. Eles serão os seus "
      "valores de R0."
  );
  Serial.println("-------------------------------------------------");
  Serial.println(
      "Tempo (s) | MQ-3 (Rs kOhm) | MQ-135 (Rs kOhm) | MQ-136 (Rs kOhm) | "
      "MQ-137 (Rs kOhm)"
  );
  Serial.println("-------------------------------------------------");
}

void loop() {
  // Imprime o cabeçalho a cada 20 leituras para facilitar a visualização
  if (millis() % (20000) < 1000) {
    Serial.println(
        "Tempo (s) | MQ-3 (Rs kOhm) | MQ-135 (Rs kOhm) | MQ-136 (Rs kOhm) | "
        "MQ-137 (Rs kOhm)"
    );
  }

  Serial.print(millis() / 1000);
  Serial.print("s\t| ");

  // Itera por todos os sensores, calcula e imprime o Rs de cada um
  for (int i = 0; i < NUM_SENSORES; i++) {
    float rs_value = calcularRs(sensores[i].pino, sensores[i].rl_value);
    Serial.print(rs_value, 4);
    Serial.print("\t\t | ");
  }
  Serial.println();

  delay(1000);
}
