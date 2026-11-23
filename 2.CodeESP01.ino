
#include <ESP8266WiFi.h>
#include "AdafruitIO_WiFi.h"

// ======================================================
// 1. ADAFRUIT IO
// ======================================================

#define IO_USERNAME "rverissimo"
#define IO_KEY      "SUA_CHAVE_ADAFRUIT_IO"

// ======================================================
// 2. CINCO REDES WIFI
// ======================================================

const char* ssids[] = {
  "ETECB 4",
  "REDE_2",
  "REDE_3",
  "REDE_4",
  "REDE_5"
};

const char* senhas[] = {
  "SENHA_REDE_1",
  "SENHA_REDE_2",
  "SENHA_REDE_3",
  "SENHA_REDE_4",
  "SENHA_REDE_5"
};

const int TOTAL_REDES = 5;

// ======================================================
// 3. ADAFRUIT IO
// ======================================================

// A biblioteca exige SSID e senha no construtor.
// O gerenciamento das redes será feito manualmente.

AdafruitIO_WiFi io(
  IO_USERNAME,
  IO_KEY,
  ssids[0],
  senhas[0]
);

// ======================================================
// 4. FEEDS
// ======================================================

AdafruitIO_Feed *feedCO2 = io.feed("co2");
AdafruitIO_Feed *feedTemp = io.feed("temperatura");
AdafruitIO_Feed *feedUmid = io.feed("umidade");

// ======================================================
// 5. VARIÁVEIS DE CONTROLE
// ======================================================

String linhaRecebida = "";

unsigned long ultimaTentativa = 0;

const unsigned long INTERVALO_RECONEXAO = 10000;

bool adafruitConectado = false;

// ======================================================
// 6. CONECTAR A UMA REDE DISPONÍVEL
// ======================================================

bool conectarWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  for (int i = 0; i < TOTAL_REDES; i++) {

    Serial.print("Tentando WiFi: ");
    Serial.println(ssids[i]);

    WiFi.begin(ssids[i], senhas[i]);

    unsigned long inicio = millis();

    while (
      WiFi.status() != WL_CONNECTED &&
      millis() - inicio < 8000
    ) {
      delay(250);
      yield();
    }

    if (WiFi.status() == WL_CONNECTED) {

      Serial.print("WiFi conectado: ");
      Serial.println(ssids[i]);

      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      return true;
    }

    WiFi.disconnect();
    delay(200);
  }

  Serial.println("Nenhuma rede WiFi disponivel.");

  return false;
}

// ======================================================
// 7. CONECTAR AO ADAFRUIT IO
// ======================================================

void conectarAdafruit() {

  if (WiFi.status() != WL_CONNECTED) {
    adafruitConectado = false;
    return;
  }

  if (io.status() == AIO_CONNECTED) {
    adafruitConectado = true;
    return;
  }

  Serial.println("Conectando ao Adafruit IO...");

  io.connect();

  unsigned long inicio = millis();

  while (
    io.status() != AIO_CONNECTED &&
    millis() - inicio < 10000
  ) {
    io.run();
    delay(100);
    yield();
  }

  adafruitConectado = (
    io.status() == AIO_CONNECTED
  );

  if (adafruitConectado) {
    Serial.println("Adafruit IO conectado.");
  } else {
    Serial.println("Adafruit IO indisponivel.");
  }
}

// ======================================================
// 8. PROCESSAR DADOS RECEBIDOS DO ARDUINO
// ======================================================

// Formato esperado:
// CO2,TEMPERATURA,UMIDADE
//
// Exemplo:
// 650,27.35,65.20

void processarDados(String linha) {

  linha.trim();

  int virgula1 = linha.indexOf(',');
  int virgula2 = linha.indexOf(',', virgula1 + 1);

  if (virgula1 < 0 || virgula2 < 0) {
    return;
  }

  float co2 = linha.substring(
    0, virgula1
  ).toFloat();

  float temperatura = linha.substring(
    virgula1 + 1, virgula2
  ).toFloat();

  float umidade = linha.substring(
    virgula2 + 1
  ).toFloat();

  if (
    co2 <= 0 ||
    temperatura < -40 ||
    temperatura > 85 ||
    umidade < 0 ||
    umidade > 100
  ) {
    return;
  }

  if (
    WiFi.status() != WL_CONNECTED ||
    io.status() != AIO_CONNECTED
  ) {
    Serial.println(
      "Dados recebidos, mas sem conexao com Adafruit IO."
    );
    return;
  }

  feedCO2->save(co2);
  feedTemp->save(temperatura);
  feedUmid->save(umidade);

  Serial.println("Dados enviados ao Adafruit IO.");
}

// ======================================================
// 9. SETUP
// ======================================================

void setup() {

  Serial.begin(9600);

  delay(1000);

  Serial.println();
  Serial.println("Inicializando ESP-01...");

  if (conectarWiFi()) {
    conectarAdafruit();
  }
}

// ======================================================
// 10. LOOP PRINCIPAL
// ======================================================

void loop() {

  // Gerenciar conexao WiFi.

  if (WiFi.status() != WL_CONNECTED) {

    adafruitConectado = false;

    if (
      millis() - ultimaTentativa >=
      INTERVALO_RECONEXAO
    ) {

      ultimaTentativa = millis();

      if (conectarWiFi()) {
        conectarAdafruit();
      }
    }

  } else {

    // Manter comunicacao com Adafruit IO.

    io.run();

    if (
      io.status() != AIO_CONNECTED &&
      millis() - ultimaTentativa >=
      INTERVALO_RECONEXAO
    ) {

      ultimaTentativa = millis();
      conectarAdafruit();
    }
  }

  // Receber dados pela serial.

  while (Serial.available()) {

    char caractere = Serial.read();

    if (caractere == '\n') {

      processarDados(linhaRecebida);

      linhaRecebida = "";

    } else if (caractere != '\r') {

      if (linhaRecebida.length() < 100) {
        linhaRecebida += caractere;
      } else {
        linhaRecebida = "";
      }
    }
  }
}
