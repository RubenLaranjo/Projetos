// Bibliotecas Utilizadas
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h> 

// Definição pinos AD9834
#define FSY   10
#define RST   8

// Definições pinos Encouder
#define ENCODER_CLK 2
#define ENCODER_DT  3
#define ENCODER_SW  4

// Definição pino Botão
#define UNIT_BUTTON 5

// Definição pino Buzzer
#define BUZZER_PIN 6

// Constantes para calcular a frequência
const double F_CLK = 75000000.0;
const double POW2_28 = 268435456.0;

// Limite inferior e superior de frequência
const double MIN_FREQ = 1.0; //1Hz
const double MAX_FREQ = 20000000.0; // 20 MHz

// Tipos de onda
enum WaveformType { SINE, SQUARE, TRIANGLE };

// Unidades de Frequencia
enum FrequencyUnit { HERTZ, KILOHERTZ, MEGAHERTZ };

// Setup Inicial
WaveformType currentWaveform = SINE;
FrequencyUnit currentUnit = HERTZ;
double currentFreq = 1.0;

// Inicialização LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inicialização do Encouder
RotaryEncoder encoder(ENCODER_CLK, ENCODER_DT, RotaryEncoder::LatchMode::TWO03); 

// Variáveis para debounce dos botões
unsigned long lastButtonPressTime = 0;
unsigned long lastUnitButtonPressTime = 0;
const long debounceDelay = 50;

void setup() {

  // Definição de saídas de Fsync e Reset para comunicação SPI
  pinMode(FSY, OUTPUT);
  pinMode(RST, OUTPUT);

  // Setup inicial de Fsync e Reset
  digitalWrite(FSY, HIGH);
  digitalWrite(RST, HIGH);

  // Definição do Encouder, Botão e Buzzer
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(UNIT_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  // Setup inicial do buzzer a low
  digitalWrite(BUZZER_PIN, LOW);

  // Inicialização comunicação serial 
  Serial.begin(9600);

  // Setup inicial Display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Gerador Funcoes");
  lcd.setCursor(0, 1);
  lcd.print("AD9834");
  delay(1500);
  lcd.clear();

  // Setup comunicação SPI
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE2)); 

  // Setup inicial AD9834 
  resetAD9834();
  setWaveform(currentWaveform); // Define a onda primeiro
  setFrequency(currentFreq);    // Envia a frequência inicial
  updateLCD();

}

void loop() {
  // inicialização variavel ultima posição do encouder 
  static long lastPosition = 0;

  // Atualiza o estado do encoder
  encoder.tick();

  //Condição para ser alterado o tipo de onda com botão do encouder 
  if (digitalRead(ENCODER_SW) == LOW && (millis() - lastButtonPressTime) > debounceDelay) {
    currentWaveform = static_cast<WaveformType>((currentWaveform + 1) % 3);
    setWaveform(currentWaveform);
    lastButtonPressTime = millis();
  }

  // Condição para alterar a unidade da frequência
  if (digitalRead(UNIT_BUTTON) == LOW && (millis() - lastUnitButtonPressTime) > debounceDelay) {
    if (currentUnit == HERTZ) currentUnit = KILOHERTZ;
    else if (currentUnit == KILOHERTZ) currentUnit = MEGAHERTZ;
    else currentUnit = HERTZ;
    updateLCD(); // Atualiza o LCD para mostrar o novo passo
    lastUnitButtonPressTime = millis();
  }

  // Condição para o encouder ajustar a frequência e setup do buzzer para quando alcaça limites definidos
  long newPosition = encoder.getPosition();
  if (newPosition != lastPosition) {
    double stepValue = 1.0;
    switch (currentUnit) {
      case KILOHERTZ: stepValue = 1000.0; break;
      case MEGAHERTZ: stepValue = 1000000.0; break;
    }
    if (newPosition > lastPosition) currentFreq += stepValue;
    else currentFreq -= stepValue;
    if (currentFreq < MIN_FREQ) { currentFreq = MIN_FREQ; beep(); }
    if (currentFreq > MAX_FREQ) { currentFreq = MAX_FREQ; beep(); }
    setFrequency(currentFreq);
    lastPosition = newPosition;
  }
}

// Função Reset AD9834
void resetAD9834() {
  // Envia registos para dar reset no ad9834
  writeRegister(0x2100);
  delay(10);
  // Envia registos para retirar o reset mantendo a escrita de 28 bits habilitada
  writeRegister(0x2000);
  delay(10);
}

// Função de seleção de tipo de onda 
void setWaveform(WaveformType type) {
  uint16_t controlRegValue = 0x2000; // Valor base com B28=1
  switch (type) {
    case SINE:     controlRegValue |= 0x0000; break;
    case SQUARE:   controlRegValue |= 0x0028; break;
    case TRIANGLE: controlRegValue |= 0x0002; break;
  }
  writeRegister(controlRegValue);
  updateLCD();
}

// Função de envio de frequência
void setFrequency(double freq) {
  uint32_t freqWord = (uint32_t)(((freq * POW2_28) / F_CLK) + 0.5);
  uint16_t lsb = (freqWord & 0x3FFF) | 0x4000; // Aponta para FREQ0
  uint16_t msb = ((freqWord >> 14) & 0x3FFF) | 0x4000; // Aponta para FREQ0
  writeRegister(lsb);
  writeRegister(msb);
  updateLCD();
}

// Função de envio de registos para ad9834
void writeRegister(uint16_t data) {
  digitalWrite(FSY, LOW);
  SPI.transfer(highByte(data));
  SPI.transfer(lowByte(data));
  digitalWrite(FSY, HIGH);
}

// Função de autalização de Display
void updateLCD() {
  lcd.clear();
  // Primeira linha: Frequência
  lcd.setCursor(0, 0);
  lcd.print("Freq:");
  if (currentFreq >= 1000000.0) {
    lcd.print(currentFreq / 1000000.0, 3);
    lcd.print("MHz");
  } else if (currentFreq >= 1000.0) {
    lcd.print(currentFreq / 1000.0, 3);
    lcd.print("kHz");
  } else {
    lcd.print(currentFreq, 0);
    lcd.print("Hz ");
  }
  // Segunda linha: Tipo de onda + Unidade
  lcd.setCursor(0, 1);
  switch (currentWaveform) {
    case SINE:     lcd.print("Seno"); break;
    case SQUARE:   lcd.print("Quadrada"); break;
    case TRIANGLE: lcd.print("Triang"); break;
  }

  lcd.setCursor(10, 1);
  lcd.print("Uni:");
  switch (currentUnit) {
    case HERTZ:     lcd.print("Hz"); break;
    case KILOHERTZ: lcd.print("kHz"); break;
    case MEGAHERTZ: lcd.print("MHz"); break;
  }
}

// Função "beep" do Buzzer
void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(50);
  digitalWrite(BUZZER_PIN, LOW);
}
