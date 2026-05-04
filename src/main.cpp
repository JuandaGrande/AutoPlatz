#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN      5
#define RST_PIN     21
#define SCK_PIN     18
#define MISO_PIN    19
#define MOSI_PIN    23
#define SALIDA_PIN  2
#define SENS1 35 
#define SENS2 32
#define SENS3 33
#define SENS4 25
#define OCC1 15
#define OCC2 4
#define OCC3 0
#define OCC4 2


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

void actualizarEspacios(){
  if (digitalRead(SENS1)==LOW){digitalWrite(OCC1,HIGH);}
  else if (digitalRead(SENS1)==HIGH){digitalWrite(OCC1,LOW);}
  if (digitalRead(SENS2)==LOW){digitalWrite(OCC2,HIGH);}
  else if (digitalRead(SENS2)==HIGH){digitalWrite(OCC2,LOW);}
  if (digitalRead(SENS3)==LOW){digitalWrite(OCC3,HIGH);}
  else if (digitalRead(SENS3)==HIGH){digitalWrite(OCC3,LOW);}
  if (digitalRead(SENS4)==LOW){digitalWrite(OCC4,HIGH);}
  else if (digitalRead(SENS4)==HIGH){digitalWrite(OCC4,LOW);}
}

void setup() {
  Serial.begin(115200);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();

  pinMode(SALIDA_PIN, OUTPUT);
  pinMode(OCC1, OUTPUT);
  pinMode(OCC2, OUTPUT);
  pinMode(OCC3, OUTPUT);
  pinMode(OCC4, OUTPUT);
  pinMode(SENS1, INPUT);
  pinMode(SENS2, INPUT);
  pinMode(SENS3, INPUT);
  pinMode(SENS4, INPUT);
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
    delay(3000);
    digitalWrite(SALIDA_PIN, LOW);
  } else {
    Serial.println("Tarjeta NO autorizada");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(50);
}