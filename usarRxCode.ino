//Rx code
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

Servo motorDriveLeft;
Servo motorDriveRight;
Servo motorArmAxis1; //shoulder left and right
Servo motorArmAxis2; //shoulder up and down
Servo motorArmAxis3; //elbow
Servo servoArmGrip;

const byte address[5] = {'R','x','A','A','A'}; //radio address, must match transmitter's

RF24 radio(9, 10); //CE and CSN pins

int dataReceived[6]; // this must match dataToSend in the TX, is the number of integers sent and received
bool newData = false;

const int failsafeDelay = 1000; //how long until shutdown when no signal
int failsafeCurrentTime = 0;


void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN); //loudness of radio, MIN LOW MED MAX
  radio.startListening();

  motorDriveLeft.attach(2, 1000, 2000);
  motorDriveRight.attach(3, 1000, 2000);
  motorArmAxis1.attach(4, 1000, 2000);
  motorArmAxis2.attach(5, 1000, 2000);
  motorArmAxis3.attach(6, 1000, 2000);
  servoArmGrip.attach(7, 1000, 2000);
  motorDriveLeft.write(90);
  motorDriveRight.write(90);
  motorArmAxis1.write(90);
  motorArmAxis2.write(90);
  motorArmAxis3.write(90);
  servoArmGrip.write(90);
}


void loop() {
  getData();
  showData();
  updateMotor();
  failsafe();
  //showMotor();
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

void updateMotor() {
  if (newData == true) {
    motorDriveLeft.write(dataReceived[0]);
    motorDriveRight.write(dataReceived[1]);
    motorArmAxis1.write(dataReceived[2]);
    motorArmAxis2.write(dataReceived[3]);
    motorArmAxis3.write(dataReceived[4]);
    servoArmGrip.write(dataReceived[5]);
  }
}

void failsafe() {
  if (newData == true) {
    failsafeCurrentTime = millis();
  }
  if (newData == false && millis() - failsafeCurrentTime >= failsafeDelay){
    Serial.println("womp womp");
    motorDriveLeft.write(90);
    motorDriveRight.write(90);
    motorArmAxis1.write(90);
    motorArmAxis2.write(90);
    motorArmAxis3.write(90);
  }
}

void showMotor() {
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
