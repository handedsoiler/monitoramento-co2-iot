#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// ======================================================
// OBJETOS
// ======================================================

SensirionI2cScd4x sensor;

SoftwareSerial espSerial(2, 3);  // RX, TX

LiquidCrystal_I2C lcd(0x27, 16, 2);


// ======================================================
// SENSOR
// ======================================================

char errorMessage[64];

int16_t error;

uint16_t co2 = 0;

float temperatura = 0;
float umidade = 0;


// ======================================================
// PINOS
// ======================================================

const byte PIN_LED_VERMELHO = 8;
const byte PIN_LED_VERDE    = 9;
const byte PIN_BUZZER       = 7;

const int LIMIAR_CO2 = 600;


// ======================================================
// INTERNET
// ======================================================

bool internetOK = false;


// ======================================================
// RECEPCAO ESP
// ======================================================

// Só precisamos de mensagens pequenas
char msgESP[12];

byte posESP = 0;


// ======================================================
// TEMPO
// ======================================================

unsigned long ultimaLeitura = 0;

const unsigned long INTERVALO = 15000;


// ======================================================
// LIMPA LCD
// ======================================================

void limparLCD() {

  lcd.clear();
}


// ======================================================
// MOSTRA TENTATIVA DE REDE
// ======================================================

void mostrarRede(char numero) {

  internetOK = false;

  limparLCD();

  lcd.setCursor(0, 0);
  lcd.print(F("Conectando WiFi"));

  lcd.setCursor(0, 1);
  lcd.print(F("Rede "));
  lcd.print(numero);
  lcd.print(F("/5..."));
}


// ======================================================
// PROCESSA MENSAGEM DA ESP
// ======================================================

void processarESP() {

  // ==================================================
  // R1 a R5
  // ==================================================

  if (
    msgESP[0] == 'R' &&
    msgESP[1] >= '1' &&
    msgESP[1] <= '5'
  ) {

    mostrarRede(msgESP[1]);

    return;
  }


  // ==================================================
  // WIFI FALHOU
  // ==================================================

  if (
    msgESP[0] == 'W' &&
    msgESP[1] == 'F'
  ) {

    internetOK = false;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("WiFi falhou"));

    lcd.setCursor(0, 1);
    lcd.print(F("Proxima rede"));

    return;
  }


  // ==================================================
  // TESTANDO INTERNET
  // ==================================================

  if (
    msgESP[0] == 'N' &&
    msgESP[1] == 'T'
  ) {

    internetOK = false;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("WiFi conectado"));

    lcd.setCursor(0, 1);
    lcd.print(F("Testando NET"));

    return;
  }


  // ==================================================
  // SEM INTERNET
  // ==================================================

  if (
    msgESP[0] == 'N' &&
    msgESP[1] == 'F'
  ) {

    internetOK = false;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("Sem internet"));

    lcd.setCursor(0, 1);
    lcd.print(F("Proxima rede"));

    return;
  }


  // ==================================================
  // ADAFRUIT
  // ==================================================

  if (
    msgESP[0] == 'A' &&
    msgESP[1] == 'I'
  ) {

    internetOK = false;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("Internet OK"));

    lcd.setCursor(0, 1);
    lcd.print(F("Adafruit IO..."));

    return;
  }


  // ==================================================
  // ONLINE
  // ==================================================

  if (
    msgESP[0] == 'O' &&
    msgESP[1] == 'N'
  ) {

    internetOK = true;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("Internet OK"));

    lcd.setCursor(0, 1);
    lcd.print(F("NET:ON"));

    return;
  }


  // ==================================================
  // OFFLINE
  // ==================================================

  if (
    msgESP[0] == 'O' &&
    msgESP[1] == 'F'
  ) {

    internetOK = false;

    limparLCD();

    lcd.setCursor(0, 0);
    lcd.print(F("NET:OFF"));

    lcd.setCursor(0, 1);
    lcd.print(F("Reconectando"));

    return;
  }
}


// ======================================================
// LE MENSAGENS DA ESP
// ======================================================

void verificarESP() {

  while (espSerial.available()) {

    char c = espSerial.read();


    // Ignora retorno de carro
    if (c == '\r') {
      continue;
    }


    // Final da mensagem
    if (c == '\n') {

      msgESP[posESP] = '\0';

      if (posESP > 0) {

        processarESP();
      }

      posESP = 0;

    } else {

      if (posESP < sizeof(msgESP) - 1) {

        msgESP[posESP] = c;

        posESP++;
      }
    }
  }
}


// ======================================================
// MOSTRA MEDICOES
// ======================================================

void mostrarDados() {

  lcd.clear();


  // ==================================================
  // LINHA 1
  // Exemplo:
  // CO2:589 NET:ON
  // ==================================================

  lcd.setCursor(0, 0);

  lcd.print(F("CO2:"));

  lcd.print(co2);

  lcd.print(F(" NET:ON"));


  // ==================================================
  // LINHA 2
  // ==================================================

  lcd.setCursor(0, 1);

  lcd.print(F("T:"));

  lcd.print(temperatura, 1);

  lcd.print(F("C U:"));

  lcd.print(umidade, 0);

  lcd.print(F("%"));
}


// ======================================================
// LEITURA DO SENSOR
// ======================================================

void lerSensor() {

  bool dataReady = false;

  error =
    sensor.getDataReadyStatus(dataReady);


  if (error) {

    return;
  }


  if (!dataReady) {

    return;
  }


  error =
    sensor.readMeasurement(
      co2,
      temperatura,
      umidade
    );


  if (error) {

    return;
  }


  // ==================================================
  // MONITOR SERIAL
  // ==================================================

  Serial.print(F("CO2: "));
  Serial.print(co2);
  Serial.println(F(" ppm"));

  Serial.print(F("Temp: "));
  Serial.print(temperatura);
  Serial.println(F(" C"));

  Serial.print(F("Umid: "));
  Serial.print(umidade);
  Serial.println(F(" %"));


  // ==================================================
  // LCD SOMENTE SE ONLINE
  // ==================================================

  if (internetOK) {

    mostrarDados();
  }


  // ==================================================
  // ENVIA DADOS PARA ESP
  //
  // Mantém o formato original:
  // CO2,TEMP,UMIDADE
  // ==================================================

  if (internetOK) {

    espSerial.print(co2);

    espSerial.print(',');

    espSerial.print(
      temperatura,
      2
    );

    espSerial.print(',');

    espSerial.println(
      umidade,
      2
    );
  }


  // ==================================================
  // ALERTA CO2
  // ==================================================

  digitalWrite(
    PIN_LED_VERMELHO,
    HIGH
  );


  if (co2 >= LIMIAR_CO2) {

    digitalWrite(
      PIN_LED_VERDE,
      HIGH
    );

    tone(
      PIN_BUZZER,
      1000
    );

    delay(300);

    digitalWrite(
      PIN_LED_VERDE,
      LOW
    );

    noTone(PIN_BUZZER);

    delay(300);

  } else {

    digitalWrite(
      PIN_LED_VERDE,
      LOW
    );

    noTone(PIN_BUZZER);
  }
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);

  espSerial.begin(9600);


  // ==================================================
  // PINOS
  // ==================================================

  pinMode(
    PIN_LED_VERMELHO,
    OUTPUT
  );

  pinMode(
    PIN_LED_VERDE,
    OUTPUT
  );

  pinMode(
    PIN_BUZZER,
    OUTPUT
  );


  digitalWrite(
    PIN_LED_VERMELHO,
    HIGH
  );

  digitalWrite(
    PIN_LED_VERDE,
    LOW
  );

  noTone(PIN_BUZZER);


  // ==================================================
  // I2C
  // ==================================================

  Wire.begin();


  // ==================================================
  // LCD
  // ==================================================

  lcd.init();

  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Iniciando..."));

  lcd.setCursor(0, 1);
  lcd.print(F("RV Quality"));


  Serial.println(F("RV QUALITY"));


  delay(1000);


  // ==================================================
  // SENSOR SCD40
  // ==================================================

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("Iniciando"));

  lcd.setCursor(0, 1);
  lcd.print(F("sensor..."));


  sensor.begin(
    Wire,
    SCD41_I2C_ADDR_62
  );


  sensor.stopPeriodicMeasurement();

  delay(500);


  error =
    sensor.startPeriodicMeasurement();


  if (error) {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(F("Erro Sensor"));

  } else {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(F("Sensor OK"));

    lcd.setCursor(0, 1);
    lcd.print(F("Aguardando NET"));
  }


  // ==================================================
  // NÃO FAZ MAIS O TESTE PING BLOQUEANTE.
  //
  // A ESP vai começar a enviar R1, R2...
  // automaticamente.
  // ==================================================

  ultimaLeitura = millis();
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // Escuta continuamente a ESP
  verificarESP();


  // ==================================================
  // LEITURA A CADA 15 SEGUNDOS
  // ==================================================

  if (
    millis() - ultimaLeitura >=
    INTERVALO
  ) {

    ultimaLeitura = millis();

    lerSensor();
  }


  delay(5);
}