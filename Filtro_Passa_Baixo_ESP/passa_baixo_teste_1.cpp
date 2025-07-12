
// FILTRO PASSA_BAIXO SEGUNDA ORDEM COM FREQUENCIA DE CORTE A 200 HZ


#include <Arduino.h>
#include "filtros_PAS.h"
#include <soc/timer_group_struct.h>
#include <soc/timer_group_reg.h>

#define NUMCHANNELS 1
#define HEADERLEN 3
#define PACKETLEN (NUMCHANNELS * 2 + HEADERLEN)


//temporizador e semaforo para sincronizar ocorrência de interrupção
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

//Buffer para transmissão de dados série para Matlab
volatile unsigned char TXBuf[PACKETLEN];  //The transmission packet
volatile unsigned char TXIndex;           //Next byte to write in the transmission packet.
unsigned char byte1, byte2;

//variáveis de debugging
volatile uint32_t isrCounter = 0;  //variáveis de debugging
volatile uint32_t lastIsrAt = 0;   //variáveis de debugging
volatile int tst = -10000;         //variáveis de debugging
//int count=0;    //variável de debugging


// gpires: rotina de serviço à interrupção (ISR) ----
// Esta rotina tem de ter o atributo IRAM_ATTR para que o código seja colocado no memória IRAM
void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  //lastIsrAt = micros(); //millis(); //para debugging de tempo

  //leitura atual do ADC - amostra atual -------------
  ADC_Value = analogRead(ADCPin) - ADC_offset;  //retira offset do sinal ajustado para a placa de microfone Max4466

  TIMERG0.wdt_feed = 1; // ERRO CORRIGIDO: Troca TIMG0.wdt_feed.reg_val por TIMERG0.wdt_feed (conforme erro do compilador)
  //--------------------------------------------------
  // Semáforo que Sinaliza o main loop que a interrupção ocorreu
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void setup() {
  Serial.begin(115200); // Ajustar Baudrate conforme necessidades de bitrate (depende da frequência de amostragem selecionada)
                        // 115200 bps deverão ser suficientes para fs=2000 Hz
  delay(1000);         // Espera que a ligação seja estabelecida

  setupBuffer_filtro();
 //Estrutura do quadro em que os pacotes são enviados  - necessário para sincronismo na receção
  TXBuf[0] = 0xa5;     //Sync 0
  TXBuf[1] = 0x5a;     //Sync 1
  TXBuf[2] = 0;        //Packet counter
  TXBuf[3] = 0x00;     //CH1 High Byte
  TXBuf[4] = 0x00;     //CH1 Low Byte
  //TXBuf[2 * NUMCHANNELS + HEADERLEN] =  0x01; // Switches state


  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more info).

  // prescaler de 80, logo 80 000 000 /80 =1000000 ,ou seja o TICK é igual a 1 uS; o true significa que incrementa (false decrementa)
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);  //associa a rotina ISR onTimer()

  //define de quanto em quanto tempo é desencadeada a interrupção, neste caso 1 000 000 us = 1s (o TICK foi definido com um 1us atrás)
    timerAlarmWrite(timer, SAMP_PER, true);
  //Gpires:  testado para 1s (1000000), 100ms (100000), 10ms (10000), 1ms (1000), 0.1 us (100 - CONFIRMAR se funciona)

  //ativa a interrupção de tempo  (alarm)
  timerAlarmEnable(timer);
}

void loop() {
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    uint32_t isrCount = 0, isrTime = 0;
   //para debugging de tempo
   // isrCount = isrCounter;
   // isrTime = lastIsrAt;

   //OPCÕES ---------------------------------------------------------
      plot_out= 1;   // 0: saida igual a entrada; 1: saida filtrada ou não filtrada; 2: saida DAC (pino 25 ou 26); 3: transmissão Serie para uma aplicação externa usando pacotes
      FILTRAGEM=1;   // Aplicação de filtragem/processamento_sinal 1- com ou 0- sem   filtragem
   //FIM OPCOES ---------------------------------------------------------

      //Filtragem -----------------------------------------
      if (FILTRAGEM) {
          yfilt=filtro_IIR(ADC_Value); // ERRO CORRIGIDO: Chamada para filtro_IIR, não filtro_FIR
        }
      else{
        yfilt=ADC_Value;                       //sem filtragem
      }
      //---------------------------------------------------

    // Print de informação de debugging
    //Serial.print("onTimer no. ");Serial.print(isrCount);Serial.print(" at ");Serial.println(isrTime);Serial.println(" ms");

    //PLOT ----------------
    if (plot_out == 1){               //sinal filtrado ou não filtrado (depende de FILTRAGEM)
        Serial.println(yfilt);
    }


    if (plot_out == 0) {              //sinal diretamente do ADC
        Serial.println(ADC_Value);
        }

    if (plot_out == 2){       //a resolução do DAC é de apenas 8 bits logo tem de se escalar yfilt (12 bit)
       float saida = map(yfilt, -2048, 2047, 0, 255);
       float saida_dac_float = constrain(saida, 0, 255); // ERRO CORRIGIDO: `saida_dac_float` não declarada
       dacWrite(DAC1Pin, (uint8_t)saida_dac_float);  // ERRO CORRIGIDO: Cast explícito para uint8_t para dacWrite
      }

    if (plot_out == 3){       //envia para aplicação externa, Matlab ou outra ao bitrate ajustado em Serial.begin()
        //Serial.write(65);         //debugging
        //count++;                  //debugging
        //Serial.println(count);    //debugging
        //yfilt = -507;             //debugging
        Serial.write(byte1);Serial.write(byte2);


        byte1 = ((unsigned char)(yfilt & 0x00FF));
        byte2 = ((unsigned char)((yfilt >> 8) & 0x00FF)); // ERRO CORRIGIDO: Adicionado & 0x00FF para garantir apenas 8 bits
        TXBuf[HEADERLEN*1]   =  byte1;           // Byte menos significativo
        TXBuf[HEADERLEN*1+1] =  byte2;           // Byte mais significativo

     //Envia pacote com dados para Matlab
       for(TXIndex=0;TXIndex<PACKETLEN;TXIndex++){ // ERRO CORRIGIDO: Loop deve ir até PACKETLEN (5), não apenas 5 magic number
          Serial.write(TXBuf[TXIndex]);
        }
        TXBuf[2]++;     // Incrementa o nr do pacote
      }
  }
}