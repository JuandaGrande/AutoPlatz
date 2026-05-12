#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN      5
#define RST_PIN     21
#define SCK_PIN     18
#define MISO_PIN    19
#define MOSI_PIN    23
#define SALIDA_PIN  15 //Esto va a ESP2
#define SENS1 25 //Interno
#define SENS2 32 //Interno
#define SENS3 33 //Interno
#define OCC1 2 //Va a ESP2
#define OCC2 4 //Va a ESP2
#define OCC3 16 //Va a ESP2


MFRC522 rfid(SS_PIN, RST_PIN);


byte tarjetasPermitidas[4][4] = {
  {0xE4, 0xC2, 0x57, 0x80},
  {0x54, 0x53, 0x5A, 0x80},
  {0x8D, 0x07, 0x57, 0x80},
  {0x32, 0x95, 0x5A, 0x80}
};

bool uidAutorizada(byte uidLeida[], byte tamanoUid) {
  if (tamanoUid != 4) {
    return false;
  }

  for (int i = 0; i < 4; i++) {
    bool coincide = true;

    for (int j = 0; j < 4; j++) {
      if (uidLeida[j] != tarjetasPermitidas[i][j]) {
        coincide = false;
        break;
      }
    }

    if (coincide) {
      return true;
    }
  }

  return false;
}

void actualizarEspacios(){
  digitalWrite(OCC1, digitalRead(SENS1) == LOW);
  digitalWrite(OCC2, digitalRead(SENS2) == LOW);
  digitalWrite(OCC3, digitalRead(SENS3) == LOW);
}

void setup() {
  Serial.begin(115200);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  pinMode(SALIDA_PIN, OUTPUT);
  pinMode(OCC1, OUTPUT);
  pinMode(OCC2, OUTPUT);
  pinMode(OCC3, OUTPUT);
  pinMode(SENS1, INPUT);
  pinMode(SENS2, INPUT);
  pinMode(SENS3, INPUT);
  digitalWrite(SALIDA_PIN, LOW);

  Serial.println("Acerca una tarjeta...");
}

void loop() {
  
  actualizarEspacios();
  
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID detectada: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (uidAutorizada(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Tarjeta autorizada");
    digitalWrite(SALIDA_PIN, HIGH);
    Serial.println("Voltaje generado");
    delay(150);
    digitalWrite(SALIDA_PIN, LOW);
  } else {
    Serial.println("Tarjeta NO autorizada");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(50);
}