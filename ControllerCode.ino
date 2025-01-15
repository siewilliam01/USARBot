#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10
#define VRX A1
#define VRY A0
#define VRXClaw A3 //these are flipped in the code here
#define VRYArm A2
#define VRXRotate A5 //these are flipped aswell
#define VRYArmBottom A4

const byte slaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

int X = 0;
int Y = 0;
String comma;
String space;
int leftMotor;
int rightMotor;
int XClaw = 0;
int YArm = 0;
int clawServo = 2200;
int armMotor;
int XRotate = 0;
int YArmBottom = 0;
int rotateMotor;
int armBottomMotor;

void setup() 
{
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
  radio.setPALevel(RF24_PA_MIN);
}

void loop() 
{
  currentMillis = millis();
  X = map(analogRead(VRX), 0, 1023, 0, 180);
  Y = map(analogRead(VRY), 0, 1023, 0, 180);
  comma = ",";
  space = "  ";
  XClaw = map(analogRead(VRXClaw), 0, 1023, 800, 2200);
  YArm = map(analogRead(VRYArm), 0, 1023, 65, 115);
  XRotate = map(analogRead(VRXRotate), 0, 1023, 80, 110);
  YArmBottom = map(analogRead(VRYArmBottom), 0, 1023, 45, 135);

  //if(currentMillis - prevMillis >= txIntervalMillis) //freezes the output when numbers is too high or low for some reason
  //{
    send();
    //prevMillis = millis();
  //}
  delay(50);
}


void send() 
{
  //movement();
  movement2();
  claw();
  arm();
  rotate();
  armBottom();
  int sentData[6];
  sentData[0] = leftMotor;
  sentData[1] = rightMotor;
  sentData[5] = clawServo;
  sentData[4] = armMotor;
  sentData[2] = rotateMotor;
  sentData[3] = armBottomMotor;

  bool rslt;
  rslt = radio.write( &sentData, sizeof(sentData) );
  //Serial.print(leftMotor + comma + rightMotor + comma + clawServo + comma + armMotor + comma + rotateMotor + comma + armBottomMotor + space);
  Serial.print(sentData[0] + comma + sentData[1] + comma + sentData[2] + comma + sentData[3] + comma + sentData[4] + comma + sentData[5] + space);
  Serial.print(X + comma + Y + space);
  Serial.print(XRotate + comma + YArmBottom + space);
  Serial.print(XClaw + comma + YArm + space);

  if (rslt) 
  {
    Serial.println(" received");
  }
  else 
  {
    Serial.println(" failed");
  }
}

void movement() //left and right motor evaluations/calcs
{
  leftMotor = 90;
  rightMotor = 90;
  if((Y>95 || Y<85) && X<=95 && X>=85)
  {
    leftMotor = Y;
    rightMotor = Y;
    if(Y>95) //just to make it clearer for me which direction it is indicating
    {
      Serial.print("↑ ");
    }
    else if (Y<85)
    {
      Serial.print("↓ ");
    }
  }
  if(Y<=95 && Y>=85)
  {
    if(X<85)
    {
      leftMotor = X;
      rightMotor = map(X,90,0,90,180);
      Serial.print("← ");
    }
    else if(X>95)
    {
      leftMotor = X;
      rightMotor = map(X,90,180,90,0);
      Serial.print("→ ");
    }
  }
}

void movement2() //left and right motor evaluations/calcs
{
  leftMotor = 90;
  rightMotor = 90;
  
  Y = map(Y, 0, 180, -90, 90);
  X = map(X, 0, 180, -90, 90);

  leftMotor = Y + X;
  rightMotor = Y - X;

  leftMotor = map(leftMotor, -90, 90, 0, 180);
  rightMotor = map(rightMotor, -90, 90, 0, 180);

  leftMotor = constrain(leftMotor, 0, 180);
  rightMotor = constrain(rightMotor, 0, 180);

  if(rightMotor<94 && rightMotor>86)
  {
    rightMotor = 90;
  }
  else
  {
    rightMotor = map(rightMotor, 0, 180, 0, 135);
  }
  if(leftMotor<94 && leftMotor>86)
  {
    leftMotor = 90;
  }
  else
  {
    leftMotor = map(leftMotor, 0, 180, 0, 135);
  }


}

void claw()
{
  if(!(XClaw > 1480 && XClaw < 1510))
  {
    clawServo += map(XClaw,700,2200,100,-100);
  }
  if(clawServo>2200)
  {
    clawServo=2200;
  }
  if(clawServo<700)
  {
    clawServo=700;
  }
}


void arm() //this does nothing
{
  armMotor = YArm;
  if(armMotor <= 89 && armMotor >= 87)
  {
    armMotor = 90;
  }
}

void rotate()
{
  rotateMotor = XRotate;
  if(XRotate==89)
  {
    rotateMotor = 90;
  }
}

void armBottom()
{
  armBottomMotor = YArmBottom;
  if(YArmBottom > 86 && YArmBottom < 90)
  {
    armBottomMotor = 90;
  }
}
