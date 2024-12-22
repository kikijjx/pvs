#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN);  // Создаем объект для работы с RFID

void setup() {
  Serial.begin(9600);  // Настройка скорости обмена по последовательному порту
  SPI.begin();         // Инициализация шины SPI
  rfid.PCD_Init();     // Инициализация MFRC522
  rfid.PCD_DumpVersionToSerial();
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID карты: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_DumpDetailsToSerial(&(rfid.uid));

  rfid.PICC_HaltA(); 
}
