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
#define LEDReceived 6

const byte slaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second

//the positions of the joysticks
int X = 0;
int Y = 0;
int XClaw = 0;
int YArm = 0;
int XRotate = 0;
int YArmBottom = 0;

//the actual value sent to the robot
int leftMotor;
int rightMotor;
int clawServo;
int armMotor = 90;
int rotateMotor;
int armBottomMotor;

String comma = ", ";
String space = "  ";

void setup() 
{
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
  radio.setPALevel(RF24_PA_MIN);
  pinMode(LEDReceived, OUTPUT);
}

void loop() 
{ 
  movement();
  rotate();
  armBottom();
  arm();
  claw();
  send();
}


void send() 
{

  int sentData[6];
  sentData[0] = leftMotor;
  sentData[1] = rightMotor;
  sentData[5] = clawServo;
  sentData[4] = armMotor;
  sentData[2] = rotateMotor;
  sentData[3] = armBottomMotor;

  bool rslt;
  rslt = radio.write( &sentData, sizeof(sentData) );
  Serial.print(sentData[0] + comma + sentData[1] + comma + sentData[2] + comma + sentData[3] + comma + sentData[4] + comma + sentData[5] + space + space + space);
  Serial.print(X + comma + Y + space);
  Serial.print(XRotate + comma + YArmBottom + space);
  Serial.print(YArm + space + XClaw + comma);

  if (rslt) //this if/else will make the LED flash red if the robot is not receiving, and will turn the LED off if it is receiving
  {
    Serial.println(" received");
    digitalWrite(LEDReceived, LOW);
  }
  else 
  {
    Serial.println(" failed");
    digitalWrite(LEDReceived, HIGH);
  }
}

void movement() //left and right motor evaluations/calcs, DL and DR
{
  X = map(analogRead(VRX), 0, 1023, 90, -90);
  Y = map(analogRead(VRY), 0, 1023, 90, -90);
  
  leftMotor = Y - X;
  rightMotor = Y + X;

  leftMotor = map(leftMotor, -90, 90, 0, 180);
  rightMotor = map(rightMotor, -90, 90, 0, 180);

  leftMotor = constrain(leftMotor, 0, 180);
  rightMotor = constrain(rightMotor, 0, 180);
 
  if(rightMotor<101 && rightMotor>82) //to account for the variability in the centering
  {
    rightMotor = 90;
  }
  else if(rightMotor >= 100)
  {
    //rightMotor = map(rightMotor, 90, 180, 105, 180);
  }
  else if(rightMotor <= 86)
  {
    //rightMotor = map(rightMotor, 0, 90, 0, 70);
  }
  if(leftMotor<101 && leftMotor>86)
  {
    leftMotor = 90;
  }
  else if(leftMotor >= 100)
  {
    //leftMotor = map(leftMotor, 90, 180, 105, 180);
  }
  else if(leftMotor <= 86)
  {
    //leftMotor = map(leftMotor, 0, 90, 0, 70);
  }
}

void rotate() //A1
{
  XRotate = map(analogRead(VRXRotate), 0, 1023, 180, 0);
  
  if(XRotate <= 85) { //to account for the variability in the centering
    rotateMotor = map(XRotate, 0, 90, 65, 90);
  }
  else if(XRotate >= 95) {
    rotateMotor = map(XRotate, 90, 180, 90, 135);
  }
  else {
    rotateMotor = 90;
  }
}

void armBottom() //A2
{
  YArmBottom = map(analogRead(VRYArmBottom), 0, 1023, 180, 0);
  
  if(YArmBottom <= 85) { //to account for the variability in the centering
    armBottomMotor = map(YArmBottom, 0, 90, 0, 90);
  }
  else if(YArmBottom >= 95) {
    armBottomMotor = map(YArmBottom, 90, 180, 90, 180);
  }
  if(YArmBottom > 82 && YArmBottom <95){
    armBottomMotor = 90;
  }
}

void arm() //A3
{
  YArm = map(analogRead(VRYArm), 0, 1023, 180, 0);
  
  if(YArm <= 85) { //to account for the variability in the joystick centering
    armMotor = map(YArm, 0, 90, 0, 90);
  }
  else if(YArm >= 95) {
    armMotor = map(YArm, 90, 180, 90, 180);
  }
  else {
    armMotor = 90;
  }
}

void claw() //AG
{
  XClaw = map(analogRead(VRXClaw), 0, 1023, 180, 0);
  
  clawServo = XClaw; //idk
  if(XClaw<110 && XClaw>70)
  {
    clawServo = 90;
  }
}
