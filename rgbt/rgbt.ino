#define RED 11
#define GRN 12
#define BLU 13
const int digital = 2;

volatile bool callFunccc = false;

void setup() {
  pinMode(digital, INPUT);
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GRN, OUTPUT);
  pinMode(BLU, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(digital), triggerFunccc, LOW);
}

void loop() {
  if(digitalRead(digital)) {
    Serial.println("++");
  }
  
  if (callFunccc) {
    nulll();

    callFunccc = false;
  }
  else {
    funccc();

  }
}

void funccc() {
  analogWrite(GRN, random(1, 255));
  analogWrite(RED, random(1, 255));
  analogWrite(BLU, random(1, 255));
  delay(3000);
}

void nulll() {
  analogWrite(RED, 0);
  analogWrite(GRN, 0);
  analogWrite(BLU, 0);
}

void triggerFunccc() {
  callFunccc = true;
}
