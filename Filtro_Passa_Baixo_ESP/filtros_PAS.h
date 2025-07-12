// Template para implementação de filtros
// Gabriel Pires - junho de 2023

#include <Arduino.h>

// FREQUÊNCIA DE AMOSTRAGEM
#define SAMP_FREQ 2000              // colocar a frequência de amostragem desejada em Hz
#define SAMP_PER (1000000/(SAMP_FREQ))      // calcula o periodo de amostragem


// Entrada/saída analogica
#define ADCPin 34                   // Entrada ADC
#define DAC1Pin 26                  // Saída ADC (DAC1)
#define ADC_offset 2048  + 250      // offset identificado para o microfone max4466
volatile int ADC_Value; // ERRO CORRIGIDO: Adicionado ';'
volatile int yfilt;

// Parametros filtros
#define NORDER 2                    // ajustar ordem
// int offset = round((NORDER + 1)/2); // REMOVIDO: Variável não utilizada para esta implementação IIR
char FILTRAGEM=1;                   // selecionado junto da filtragem

// Configura qual o plot de saida  controlado no ficheiro main
char plot_out;      // 0: entrada; 1: saida filtrada; 2: saida DAC (pino 25 ou 26); 3: transmissão Serie para uma aplicação externa usando pacotes

// Buffers associados aos filtros
volatile float buffer1[NORDER + 1];


// VALORES OBTIDOS DO MATLAB

// COEFICIENTES do filtro de ORDEM NORDER
const float Num[3] = {1.0, 2.0, 1.0};
// Denominadores (a), com Den[0] = 1.0
const float Den[3] = {1.0, -1.142980502539901133118860343529377132654,  0.412801598096188770981029847462195903063};


/* *******************************
 * Setup do buffer do filtro
 *
 *************************** */
// ERRO CORRIGIDO: O protótipo da função deve estar fora de qualquer definição de função.
// A definição de setupBuffer_filtro estava duplicada e mal posicionada.
void setupBuffer_filtro(void); // Protótipo da função

int filtro_IIR(int ADC_Value_in); // Protótipo da função (Renomeado de filtro_FIR para filtro_IIR, como discutido)


void setupBuffer_filtro(void) {
  for (int i = 0; i <= NORDER; i++) {
    buffer1[i] = 0.0; // Inicializa todos os elementos do buffer com zero (usando float para precisão)
  }
}

/*****************************************/
/* Filtro IIR  */ // ERRO CORRIGIDO: Comentário ajustado para refletir que é um filtro IIR
/*****************************************/
int filtro_IIR(int ADC_Value_in) {
  float y_filtered; // Variável para armazenar a saída intermediária (float para precisão)
  float gain = 0.067455273889071895587754568168747937307 ;


  for (int i = NORDER; i >= 1; i--) {
    buffer1[i] = buffer1[i - 1];
  }

  buffer1[0] = (gain * ADC_Value_in) - (Den[1] * buffer1[1]) - (Den[2] * buffer1[2]);
  y_filtered = (Num[0] * buffer1[0]) + (Num[1] * buffer1[1]) + (Num[2] * buffer1[2]);

  return (int)y_filtered;
}