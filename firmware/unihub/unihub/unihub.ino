#include <SPI.h>
#include <MFRC522.h>

// GPIOs reais do ESP8266
#define SS_PIN 15    // corresponde ao D10 / SS da sua placa
#define RST_PIN 16   // corresponde ao D2 da sua placa

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("       UNIPASS - UNIHUB");
  Serial.println("==============================");
  Serial.println("Inicializando RC522...");

  // SPI padrao do ESP8266:
  // SCK  = GPIO14 -> D13/SCK
  // MISO = GPIO12 -> D12/MISO
  // MOSI = GPIO13 -> D11/MOSI
  // SS   = GPIO15 -> D10/SS
  SPI.begin();

  rfid.PCD_Init();
  delay(500);

  byte version = rfid.PCD_ReadRegister(rfid.VersionReg);

  Serial.print("Versao RC522: 0x");
  Serial.println(version, HEX);

  if (version == 0x00) {
    Serial.println("ERRO: RC522 retornou 0x00.");
    Serial.println("Verifique alimentacao, RST e conexoes SPI.");
  }
  else if (version == 0xFF) {
    Serial.println("ERRO: RC522 retornou 0xFF.");
    Serial.println("Provavel problema em MISO/SDA/SS ou falta de comunicacao SPI.");
  }
  else {
    Serial.println("RC522 conectado com sucesso!");
    Serial.println("Aproxime uma TAG...");
  }

  Serial.println();
}

void loop() {

  // Verifica se existe um cartao novo
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Tenta ler o cartao
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("------------------------------");
  Serial.println("TAG DETECTADA!");

  Serial.print("UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1) {
      Serial.print(":");
    }
  }

  Serial.println();
  Serial.println("------------------------------");
  Serial.println("Aproxime outra TAG...");
  Serial.println();

  // Finaliza comunicacao com a TAG atual
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(1000);
}