#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN      5
#define RST_PIN     21
#define SCK_PIN     18
#define MISO_PIN    19
#define MOSI_PIN    23
#define SALIDA_PIN  2

MFRC522 rfid(SS_PIN, RST_PIN);

// 4 tarjetas autorizadas
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

void setup() {
  Serial.begin(115200);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  pinMode(SALIDA_PIN, OUTPUT);
  digitalWrite(SALIDA_PIN, LOW);

  Serial.println("Acerca una tarjeta...");
}

void loop() {
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
    delay(3000);
    digitalWrite(SALIDA_PIN, LOW);
  } else {
    Serial.println("Tarjeta NO autorizada");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(500);
}