#include <Arduino.h> 
// Definições de pinos
#define PIN_V_Set_Point 32  // Pino ADC para setpoint (potenciômetro)
#define PIN_V_Saida 33      // Pino ADC para medir saída do LM2596
#define PIN_PWM 25          // Pino PWM para FB do LM2596

// Definições do PWM
#define PWM_Freq 10000  // Frequência do PWM: 10 kHz
#define PWM_Canal 0     // Canal do PWM
#define PWM_Resol 8     // Resolução de 8 bits (valores de 0 a 255)

//divisor de tensao
#define R1 6780.0
#define R2 2140.0

#define MAX_TENSAO_SETPOINT 12.0
#define KP 10.0

int PWM_duty = 128;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  //configuracao PWM
  ledcSetup(PWM_Canal, PWM_Freq, PWM_Resol);
  ledcAttachPin(PIN_PWM, PWM_Canal);
}

float medida_tensao()
{
  int medicao_adc = analogRead(PIN_V_Saida);
  float tensao = (medicao_adc*3.3)/4095.0;
  float tensao_real = (tensao*((R1+R2)/R2));
  return tensao_real;
}

float medida_setpoint()
{
  int medicao_set = analogRead(PIN_V_Set_Point);
  float medicao = (medicao_set*3.3)/4095.0;
  float setpoint=medicao*(MAX_TENSAO_SETPOINT/3.3);
  return setpoint;
}


void loop() {

  float tensao = medida_tensao();
  float setpoint =medida_setpoint();
  float erro= setpoint-tensao;
  PWM_duty -= (int)(KP*erro);
  if(PWM_duty>255)  PWM_duty =255;
  if(PWM_duty<0)  PWM_duty=0;

  ledcWrite(PWM_Canal, PWM_duty);

  Serial.print("Tensão de Saída: ");
  Serial.print(tensao, 2);
  Serial.println("V | ");

  Serial.print("Tensão de SetPoint: ");
  Serial.print(setpoint, 2);
  Serial.println("V | ");

  Serial.print("Erro: ");
  Serial.print(erro, 2);
  Serial.println("V | ");

  Serial.print("PWM: ");
  Serial.println(PWM_duty);
  delay(100);
}