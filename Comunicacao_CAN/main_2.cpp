//
//    ESP1
//

#include <CAN.h>
#include <WiFi.h>
#include <PubSubClient.h>


#define TX_GPIO_NUM   5
#define RX_GPIO_NUM   4


const int pwmChannel = 0;
const int freq = 5000;  
const int resolution = 8;  
const int pwmPin = 15;  


float ldr_valor ;
float ldr_percentual;
float ajuste_pwm;
float luminusidade_percentual;
float brilho_led;



// WiFi and MQTT configuration
const char* ssid = "iPhone";           // WiFi SSID
const char* password = "laranjo56";    // WiFi Password
const char* mqtt_server = "172.20.10.5"; // MQTT Broker IP

WiFiClient espClient;
PubSubClient client(espClient);

TaskHandle_t mqttTaskHandle;
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;


void setup_wifi() { // conexao WiFi
  Serial.println();
  Serial.print("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    char clientId[24];
    snprintf(clientId, sizeof(clientId), "ESP32Master-%04X", random(0xffff));
    if (client.connect(clientId)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(3000);
    }
  }
   
}

void mqttTask(void* parameter) {
  
  client.setServer(mqtt_server, 1883);

  while (true) {
    if (!client.connected()) {
      reconnect();
    }

    client.loop();

    char var_pub[64]; // array de caracteres para publicar no mqtt
    // String para publicar no mqtt
    snprintf(var_pub, sizeof(var_pub), "%.2f", luminusidade_percentual);
    client.publish("sensor/dados/lum", var_pub);

    snprintf(var_pub, sizeof(var_pub), "%.2f", brilho_led);
    client.publish("sensor/dados/brilho", var_pub);

    
    snprintf(var_pub, sizeof(var_pub), "%.2f", ajuste_pwm);
    client.publish("sensor/dados/pwm", var_pub);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// TASK 1
void task1(void *pvParameters) {
  while (1) { // repete sempre a task
    // Tenta analisar o pacote
    int packetSize = CAN.parsePacket();

    if (packetSize) {
      int endereco_mensagem = CAN.packetId(); // ID da mensagem recebida
      if (endereco_mensagem == 0x12) {
        // Recebe os dados
        uint8_t recebido[4];
        for (int i = 0; i < 4; i++) {
          recebido[i] = CAN.read(); // Lê os 4 bytes do float
        }

        // Converte os bytes de volta para float
       memcpy(&ldr_valor, recebido, sizeof(ldr_valor));
        
        Serial.print(ldr_valor);
        Serial.println();
        
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 1 segundo
  }
}

void task2(void *pvParameters) {
while (1) {

    // Brilho atual em %
    luminusidade_percentual = (ldr_valor / 4095)*100;

    // Brilho do LED
    brilho_led = map(ajuste_pwm,0,255,100,0);

    // Ajuste PWM
    ajuste_pwm = map(ldr_valor,0,4095,0,255);
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void envio(void *pvParameters){
  while(1){

  //Serial.print("Enviando pacote ... ");
  uint8_t envio[4];  // Cria um vetor de 4 posições
  memcpy(envio, &ajuste_pwm, sizeof(ajuste_pwm)); // Copia o valor do LDR para o vetor payload
   
  

  CAN.beginPacket(0x10); // Envia o ID 0x10
  
  CAN.write(envio,4);  
  
  CAN.endPacket();

  
  
  vTaskDelay(100 / portTICK_PERIOD_MS); // Delay de 1 segundo
  }

}

// TASK 2
void task3(void *pvParameters) {
  while (1) { // repete sempre a task
    
    ledcWrite(pwmChannel, ajuste_pwm); // Envia o valor PWM
    delay(100);
  
}
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setup_wifi();  // Inicia o WiFi ANTES de criar as tasks

  // Configuração do CAN
  CAN.setPins(RX_GPIO_NUM, TX_GPIO_NUM);
  if (CAN.begin(500E3)) {
    Serial.println("Iniciou Comunicação CAN com SUCESSO");
  } else {
    Serial.println("Ocorreu algum erro ao iniciar a comunicação CAN");
    while (1);
  }

  pinMode(pwmPin, OUTPUT); // Define GPIO 0 como saída
    ledcSetup(pwmChannel, freq, resolution); // Configura canal PWM
    ledcAttachPin(pwmPin, pwmChannel); 

  // Criando as tasks
  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, &Task1, 1);
  xTaskCreatePinnedToCore(task2, "Task2", 4096, NULL, 1, &Task2, 1);
  xTaskCreatePinnedToCore(mqttTask, "MQTT Task", 6144, NULL, 1, &mqttTaskHandle, 1);
  xTaskCreatePinnedToCore(envio, "Task2", 4096, NULL, 1, &Task3, 1);
  xTaskCreatePinnedToCore(task3, "Task1", 4096, NULL, 1, &Task4, 1);
}


void loop() {
  
  vTaskDelete(NULL);
}
