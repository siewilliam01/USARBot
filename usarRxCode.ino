#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

Servo motorDL;  //left and right drive motors
Servo motorDR;
Servo motorA1;  //shoulder left and right
Servo motorA2;  //shoulder up and down
Servo motorA3;  //elbow
Servo servoAG;  //grip servo

const byte address[5] = { 'R', 'x', 'A', 'A', 'A' };  //radio address, must match transmitter's

RF24 radio(9, 10);  //CE and CSN pins

int dataReceived[6];  // this must match dataToSend in the TX, is the number of integers sent and received
bool newData = false;

int angleDL;  //values to be inserted into servo code, in the form of 0-180, servo code turns into pwm
int angleDR;
int angleA1;
int angleA2;
int angleA3;
int angleAG;

const int motorMin = 45;  //max speeds of motors reverse and forward in the form of servo angle
const int motorMax = 135;

const int failsafeDelay = 1000;  //how long until shutdown when no signal
int failsafeCurrentTime = 0;
int currentmillis = 0;
int millis2 = 0;


void setup() {
  delay(1000);
  
  Serial.begin(9600);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);  //loudness of radio, MIN LOW MED MAX
  radio.startListening();

  motorDL.attach(2, 1000, 2000);  //set up motor pins, pwm from 1000microsec to 2000 microsec correspond to 0-180 deg
  motorDR.attach(3, 1000, 2000);
  motorA1.attach(4, 1000, 2000);
  motorA2.attach(5, 1000, 2000);
  motorA3.attach(6, 1000, 2000);
  servoAG.attach(7);
  motorDL.write(90);  //set motors to neutral
  motorDR.write(90);
  motorA1.write(90);
  motorA2.write(90);
  motorA3.write(90);
  servoAG.write(0);

  delay(4000);
}


void loop() {
  getData();
  if(millis() - currentmillis >= 10)
  {
    updateAngle();
    failsafe();
    updateMotor();
    showAngle();
    currentmillis = millis();
    newData = false;  //resets loop until new data comes in
  }
}

//read data coming in from radio if it is available
void getData() {
  if (radio.available()) {
    radio.read(dataReceived, sizeof(dataReceived));  //reading the data
    newData = true;                                  //used by several functions which will only run when there is new data
  }
}

//setting each angle to the one coming in from the data, as long as there is new data and it is reasonable
void updateAngle() {
  if (newData == true && !(dataReceived[0] == 0 && dataReceived[1] == 0 && dataReceived[2] == 0 && dataReceived[3] == 0 && dataReceived[4] == 0 && dataReceived[5] == 0)) {  //this condition was made to solve a problem that no longer exists //nevermind, there seems to be some interference sometimes
    angleDL = dataReceived[0];
    angleDR = dataReceived[1];
    angleA1 = dataReceived[2];
    angleA2 = dataReceived[3];
    angleA3 = dataReceived[4];
    angleAG = dataReceived[5];
  }
}

//send angles into servo library and send to pins as pwm, not newData == true because must be continuous
void updateMotor() {
  motorDL.write(angleDL);
  motorDR.write(angleDR);
  motorA1.write(angleA1);
  motorA2.write(angleA2);
  motorA3.write(angleA3);
  servoAG.write(angleAG);
}

//stops motors of no new data for over a second
void failsafe() {
  if (newData == true) {
    failsafeCurrentTime = millis();
  }
  if (newData == false && millis() - failsafeCurrentTime >= failsafeDelay) {
    Serial.println("womp womp");
    angleDL = 90;
    angleDR = 90;
    angleA1 = 90;
    angleA2 = 90;
    angleA3 = 90;
  }
}

//just print statements to console
void showAngle() {
  if (newData == true) {
    Serial.print("showAngle DL ");
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
