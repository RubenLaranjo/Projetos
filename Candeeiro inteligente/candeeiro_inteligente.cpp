#include <Arduino.h>

// Definição dos pinos
const int fb_PIN = 26;    // Pino de saída PWM conectado ao pino FB do LM2596
const int ldr_PIN = 32;   // Pino analógico conectado ao LDR para medir a luminosidade

// Configuração do PWM
const int pwm_canal = 0;    // Canal PWM utilizado pelo ESP32 (0-15)
const int pwm_freq = 5000;  // Frequência do PWM
const int pwm_resol = 8;    // Resolução do PWM em bits (8 bits = 0 a 255)

// Controle Proporcional
float Kp = 3.0;                     // Constante proporcional para ajuste do PWM
const float brilho_desejado = 0.6;  // Luminosidade pretendida (60% ou 0.6)

// Intervalo de controlo
unsigned long ultima_atualizacao = 0; // Armazena o tempo da última actualização
const int intervalo = 5;              // Intervalo de actualização em milissegundos (5ms)

// Limites de mapeamento
const int ldr_min = 0;    // Valor mínimo do LDR (ambiente totalmente escuro)
const int ldr_max = 4095; // Valor máximo do LDR (ambiente totalmente iluminado)
const int pwm_min = 0;    // Duty cycle mínimo (LED com brilho máximo)
const int pwm_max = 255;  // Duty cycle máximo (LED apagado)

void setup() {
  Serial.begin(115200); 

  ledcSetup(pwm_canal, pwm_freq, pwm_resol);
  
  // Associa o canal PWM ao pino de saída (fb_PIN)
  ledcAttachPin(fb_PIN, pwm_canal);
}

void loop() {
  // Executa o controlo apenas no intervalo especificado (5ms)
  if (millis() - ultima_atualizacao >= intervalo) {
    ultima_atualizacao = millis(); 

    // Lê o valor do LDR (0 a 4095)
    int ldr_valor = analogRead(ldr_PIN);

    // Converte a leitura do LDR para um valor percentual (0.0 a 1.0)
    float ldr_percentual = (float)(ldr_max - ldr_valor) / (ldr_max - ldr_min);

    // Calcula o erro entre a luminosidade pretendida e a luminosidade medida
    float erro = brilho_desejado - ldr_percentual;

    // Calcula o ajuste proporcional para o PWM
    int ajuste_pwm = (int)(Kp * erro * pwm_max); 
    ajuste_pwm = constrain(ajuste_pwm, pwm_min, pwm_max); 

    // Aplica do pwm ajustado
    ledcWrite(pwm_canal, ajuste_pwm);

    // Mostra os valores calculados no monitor serial
    Serial.print("Luminosidade actual (%): ");
    Serial.print(ldr_percentual * 100, 2); 
    Serial.print("\tErro: ");
    Serial.print(erro, 2);                 
    Serial.print("\tPWM: ");
    Serial.println(ajuste_pwm);            
  }
}

