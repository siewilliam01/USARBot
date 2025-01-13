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
int clawServo;
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
  XClaw = map(analogRead(VRXClaw), 0, 1023, 0, 180);
  YArm = map(analogRead(VRYArm), 0, 1023, 0, 180);
  XRotate = map(analogRead(VRXRotate), 0, 1023, 0, 180);
  YArmBottom = map(analogRead(VRYArmBottom), 0, 1023, 0, 180);

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
  //if(Y<0)
  //{
    //int temp = leftMotor;
    //leftMotor = rightMotor;
    //rightMotor = temp;
  //}

  leftMotor = map(leftMotor, -90, 90, 0, 180);
  rightMotor = map(rightMotor, -90, 90, 0, 180);

  leftMotor = constrain(leftMotor, 0, 180);
  rightMotor = constrain(rightMotor, 0, 180);
}

void claw()
{
  if(XClaw !=89)
  {
    clawServo += map(XClaw,0,180,-10,10);
  }
  if(clawServo>179)
  {
    clawServo = 179;
  }
  if(clawServo<10)
  {
    clawServo = 10;
  }
}

void arm() //this does nothing
{
  armMotor = YArm;
  if(armMotor == 88 || armMotor == 87)
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
