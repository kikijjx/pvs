#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <EEPROM.h>

#define RST_PIN         9
#define SS_PIN          10
#define SERVO_PIN       6

#define ACCESS_MODE     1
#define WRITE_MODE      2
#define CLONE_MODE      3
#define RANDOM_MODE     4

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;

int mode = ACCESS_MODE;
byte storedData[18];
bool cardStored = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myServo.attach(SERVO_PIN);

  EEPROM.get(0, storedData);
  cardStored = (storedData[0] != 0xFF);

  myServo.write(0);
  Serial.println("1 для режима доступа, 2 для записи, 3 для клонирования, 4 для записи рандомных значений");
}

void loop() {
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == '1') {
      mode = ACCESS_MODE;
      Serial.println("режим доступа");
    } else if (receivedChar == '2') {
      mode = WRITE_MODE;
      Serial.println("режим записи");
    } else if (receivedChar == '3') {
      mode = CLONE_MODE;
      Serial.println("режим клонирования");
    } else if (receivedChar == '4') {
      mode = RANDOM_MODE;
      Serial.println("режим рандома");
    }
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  byte currentData[18];
  if (!readBlockData(4, currentData)) {
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1();
    return;
  }

  Serial.print("блок 3: ");
  for (byte i = 0; i < 18; i++) {
    Serial.print(currentData[i], HEX);
    if (i < 17) Serial.print(":");
  }
  Serial.println();

  if (mode == ACCESS_MODE) {
    if (cardStored && compareData(currentData, storedData)) {
      Serial.println("доступ разрешён. открытие...");
      myServo.write(90);
      delay(2000);
      myServo.write(0);
    } else {
      Serial.println("доступ запрещён");
    }

  } else if (mode == WRITE_MODE) {
    Serial.println("данные сохранены.");
    for (byte i = 0; i < 18; i++) {
      storedData[i] = currentData[i];
    }
    EEPROM.put(0, storedData);
    cardStored = true;

  } else if (mode == CLONE_MODE) {
    if (cardStored) {
      Serial.println("клонирование данных...");
      writeBlockData(4, storedData);
    } else {
      Serial.println("нет сохранённых данных");
    }
  } else if (mode == RANDOM_MODE) {
    Serial.println("запись рандомных данных...");
    byte zeroData[18] = {0};
    writeBlockData(4, zeroData);
  }

  rfid.PICC_HaltA(); 
  rfid.PCD_StopCrypto1();
}

bool compareData(byte *dataA, byte *dataB) {
  for (byte i = 0; i < 18; i++) {
    if (dataA[i] != dataB[i]) return false;
  }
  return true;
}

void writeBlockData(byte block, byte *data) {
  MFRC522::MIFARE_Key key;
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  MFRC522::StatusCode status = rfid.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.println(rfid.GetStatusCodeName(status));
    return;
  }

  status = rfid.MIFARE_Write(block, data, 16);

  if (status != MFRC522::STATUS_OK) {
    Serial.println(rfid.GetStatusCodeName(status));
  } else {
    Serial.println("запись успешна");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

bool readBlockData(byte block, byte *buffer) {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  MFRC522::StatusCode status = rfid.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Ошибка аутентификации: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }

  byte size = 18;
  status = rfid.MIFARE_Read(block, buffer, &size);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Ошибка чтения: ");
    Serial.println(rfid.GetStatusCodeName(status));
    return false;
  }
  return true;
}
