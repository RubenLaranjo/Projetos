//
//    ESP1
//

#include <CAN.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TX_GPIO_NUM   5
#define RX_GPIO_NUM   4
#define ldr_PIN 2


float ldr_valor;
float pwm;

TaskHandle_t envioHandle;
TaskHandle_t Task1;
TaskHandle_t Task2;

void envio(void *pvParameters){
  while(1){


  uint8_t envio[4];  // Cria um vetor de 4 posições
  memcpy(envio, &ldr_valor, sizeof(ldr_valor)); // Copia o valor do LDR para o vetor payload
   


  CAN.beginPacket(0x12); // Envia o ID 0x12
  
  CAN.write(envio,4);  
  
  CAN.endPacket();

  vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 1 segundo
  }

}

// TASK 1
void task1(void *pvParameters) {
  while (1) { // repete sempre a task
    // Tenta analisar o pacote
    int packetSize = CAN.parsePacket();

    if (packetSize) {
      int endereco_mensagem = CAN.packetId(); // ID da mensagem recebida
      if (endereco_mensagem == 0x10) {
        // Recebe os dados
        uint8_t recebido[4];
        for (int i = 0; i < 4; i++) {
          recebido[i] = CAN.read(); // Lê os 4 bytes do float
        }

        // Converte os bytes de volta para float
        memcpy(&pwm, recebido, sizeof(pwm));
        
        Serial.print(pwm);
        Serial.println();
       
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 1 segundo
  }
}

// TASK 2
void task2(void *pvParameters) {
  while (1) { // repete sempre a task
    
    ldr_valor=analogRead(ldr_PIN);
    
    
    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 1 segundo
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Espera que a comunicação serial

  // Pins do CAN TX e RX
  CAN.setPins(RX_GPIO_NUM, TX_GPIO_NUM);

  // Verifica se a comunicação CAN está funcional
  if (CAN.begin(500E3)) {
    Serial.println("Iniciou Comunicação CAN com SUCESSO");  
  } else {
    Serial.println("Ocorreu algum erro ao iniciar a comunicação CAN");
    while (1);
  }
  xTaskCreatePinnedToCore(envio, "envio", 2058, NULL, 1, &envioHandle, 1);
  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, &Task1, 1);
  xTaskCreatePinnedToCore(task2, "Task1", 4096, NULL, 1, &Task2, 1);
}



void loop() {


  vTaskDelete(NULL);
 

}
