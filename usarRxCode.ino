//Rx code
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

Servo motorDL;
Servo motorDR;
Servo motorA1; //shoulder left and right
Servo motorA2; //shoulder up and down
Servo motorA3; //elbow
Servo servoAG;

const byte address[5] = {'R','x','A','A','A'}; //radio address, must match transmitter's

RF24 radio(9, 10); //CE and CSN pins

int dataReceived[6]; // this must match dataToSend in the TX, is the number of integers sent and received
bool newData = false;

int angleDL;
int angleDR;
int angleA1;
int angleA2;
int angleA3;
int angleAG;

const int motorMax = 135;
const int motorMin = 45;

const int failsafeDelay = 1000; //how long until shutdown when no signal
int failsafeCurrentTime = 0;


void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN); //loudness of radio, MIN LOW MED MAX
  radio.startListening();

  motorDL.attach(2, 1000, 2000);
  motorDR.attach(3, 1000, 2000);
  motorA1.attach(4, 1000, 2000);
  motorA2.attach(5, 1000, 2000);
  motorA3.attach(6, 1000, 2000);
  servoAG.attach(7, 1000, 2000);
  motorDL.write(90);
  motorDR.write(90);
  motorA1.write(90);
  motorA2.write(90);
  motorA3.write(90);
  servoAG.write(90);
}


void loop() {
  getData();
  //showData();
  updateAngle();
  limitAngle();
  failsafe();
  updateMotor();
  showMotor();
  newData = false;
}


void getData() {
  if ( radio.available() ) {
    radio.read(dataReceived, sizeof(dataReceived) );
    newData = true;
  }
}

void showData() {
  if (newData == true) {
    Serial.print("DL ");
    Serial.print(dataReceived[0]);
    Serial.print(", DR ");
    Serial.print(dataReceived[1]);
    Serial.print(", A1 ");
    Serial.print(dataReceived[2]);
    Serial.print(", A2 ");
    Serial.print(dataReceived[3]);
    Serial.print(", A3 ");
    Serial.print(dataReceived[4]);
    Serial.print(", AG ");
    Serial.println(dataReceived[5]);
  }
}

void updateAngle() {
  if(newData == true && dataReceived[0] != 0 && dataReceived[1] != 0 && dataReceived[2] != 0 && dataReceived[3] != 0 && dataReceived[4] != 0 && dataReceived[5] != 0) {
    angleDL = dataReceived[0];
    angleDR = dataReceived[1];
    angleA1 = dataReceived[2];
    angleA2 = dataReceived[3];
    angleA3 = dataReceived[4];
    angleAG = dataReceived[5];
  }
}

void limitAngle() {
  if(angleDL < motorMin) {
    angleDL = motorMin;
  }
  if(angleDL > motorMax) {
    angleDL = motorMax;
  }
  if(angleDR < motorMin) {
    angleDR = motorMin;
  }
  if(angleDR > motorMax) {
    angleDR = motorMax;
  }
  if(angleA1 < motorMin) {
    angleA1 = motorMin;
  }
  if(angleA1 > motorMax) {
    angleA1 = motorMax;
  }
  if(angleA2 < motorMin) {
    angleA2 = motorMin;
  }
  if(angleA2 > motorMax) {
    angleA2 = motorMax;
  }
  if(angleA3 < motorMin) {
    angleA3 = motorMin;
  }
  if(angleA3 > motorMax) {
    angleA3 = motorMax;
  }
}

void updateMotor() {
  motorDL.write(angleDL);
  motorDR.write(angleDR);
  motorA1.write(angleA1);
  motorA2.write(angleA2);
  motorA3.write(angleA3);
  servoAG.write(angleAG);
}

void failsafe() {
  if (newData == true) {
    failsafeCurrentTime = millis();
  }
  if (newData == false && millis() - failsafeCurrentTime >= failsafeDelay){
    Serial.println("womp womp");
    angleDL = 90;
    angleDR = 90;
    angleA1 = 90;
    angleA2 = 90;
    angleA3 = 90;
  }
}

void showMotor() {
  if(newData == true) {
    Serial.print("DL ");
    Serial.print(angleDL);
    Serial.print(", DR ");
    Serial.print(angleDR);
    Serial.print(", A1 ");
    Serial.print(angleA1);
    Serial.print(", A2 ");
    Serial.print(angleA2);
    Serial.print(", A3 ");
    Serial.print(angleA3);
    Serial.print(", AG ");
    Serial.println(angleAG);
  }
}
