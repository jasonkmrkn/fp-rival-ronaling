#include <WebServer.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// SSID and PASSW
const char* ssid = "RONALING";
const char* passw = "ADMIN12345";
WiFiServer server(8080);

// M Kiri 
#define MKa_PWM  23
#define MKa_B    22
#define MKa_F    21

// M Kanan 
#define MK_PWM  17
#define MK_B    19
#define MK_F    18

// Servo
#define ARM_PIN   32
#define GRIP_PIN  33

#define ECHO_PIN 25
#define TRIG_PIN 26

Servo arm;      
Servo gripper;  

int zeroPositionArm = 0;    // Default position for arm (0 degrees)
int targetPositionArm = 90; // Target position for arm (90 degrees)
int zeroPositionGripper = 180;  // Default position for gripper (open)
int targetPositionGripper = 95; // Target position for gripper (closed)

int armDelayTime = 20;  // Delay between arm movements (in ms)
int gripperDelayTime = 20; // Delay between gripper movements (in ms)

// PWM Properties
const int freq = 1000;
const int resolution = 8;

long duration;
float distance;

void setup()
{
  Serial.begin(9600);
  delay(1000);
  Serial.print("Setting Access Point...");
  Serial.println(WiFi.softAP(ssid,passw) ? "Access Point Ready" : "Access Point Failed");
  Serial.print("IP Address = ");
  Serial.println(WiFi.softAPIP());
  // Define endpoints
  server.begin();
  Serial.println("Server started");

  pinMode(MK_PWM, OUTPUT);
  pinMode(MK_F, OUTPUT);
  pinMode(MK_B, OUTPUT);
    
  pinMode(MKa_PWM, OUTPUT);
  pinMode(MKa_F, OUTPUT);
  pinMode(MKa_B, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  ledcAttachChannel(MK_PWM, freq, resolution, 0);
  ledcAttachChannel(MKa_PWM, freq, resolution, 0);

  // Attach the arm and gripper servos to the control pins
  gripper.attach(GRIP_PIN);
  arm.attach(ARM_PIN);

  // Move the gripper to the initial position (open)
  gripper.write(zeroPositionGripper);
  delay(1000);
  gripper.write(targetPositionGripper);
  delay(1000);
  gripper.write(zeroPositionGripper);
  delay(1000);
  // Move the arm to the initial position (zero position)
  arm.write(zeroPositionArm);
  delay(2000);
  arm.write(targetPositionArm);
  delay(2000);
  Serial.println("Vroom Vroom!!");
  delay(2000);
}

void loop()
{
  WiFiClient client = server.available();
  Forward(240);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) * (0.0344);
  Serial.print("Jarak : ");
  Serial.println(distance);
  
  if (client)
  {
    Serial.println("Client connected!");

    while(client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.println(c);
        command(c, client);
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }

  delay(500);
}


void Forward(int speed) {
  digitalWrite(MK_B, LOW);
  digitalWrite(MK_F, HIGH);
  digitalWrite(MKa_B, LOW);
  digitalWrite(MKa_F, HIGH);

  ledcWrite(MK_PWM, speed);
  ledcWrite(MKa_PWM, speed);
}

void RotateClockwise(int speed) {
  digitalWrite(MK_B, LOW);
  digitalWrite(MK_F, HIGH);
  digitalWrite(MKa_B, HIGH);
  digitalWrite(MKa_F, LOW);

  ledcWrite(MK_PWM, speed);
  ledcWrite(MKa_PWM, speed);
}

void RotateCounterClockwise(int speed) {
  digitalWrite(MK_B, HIGH);
  digitalWrite(MK_F, LOW);
  digitalWrite(MKa_B, LOW);
  digitalWrite(MKa_F, HIGH);

  ledcWrite(MK_PWM, speed);
  ledcWrite(MKa_PWM, speed);
}

void Backward(int speed) {
  digitalWrite(MK_B, HIGH);
  digitalWrite(MK_F, LOW);
  digitalWrite(MKa_B, HIGH);
  digitalWrite(MKa_F, LOW);

  ledcWrite(MK_PWM, speed);
  ledcWrite(MKa_PWM, speed);
}

void Stop() {
  digitalWrite(MK_B, LOW);
  digitalWrite(MK_F, LOW);
  digitalWrite(MKa_B, LOW);
  digitalWrite(MKa_F, LOW);

  ledcWrite(MK_PWM, 0);
  ledcWrite(MKa_PWM, 0);
}

void objUp() { 
  for (int pos = zeroPositionArm; pos <= targetPositionArm; pos++) {
    arm.write(pos);  // Move the arm to the current position
    delay(armDelayTime);  // Slow down the arm movement
  }
  delay(1000);

  // Move the gripper to the target position (closed)
  gripper.write(targetPositionGripper);
  delay(2000);
  
  // Move the arm slowly from 90 to 0 degrees
  for (int pos = targetPositionArm; pos >= zeroPositionArm; pos--) {
    arm.write(pos);  // Move the arm to the current position
    delay(armDelayTime);  // Slow down the arm movement
  }
  delay(1000);
}
void objDown(){
  for (int pos = zeroPositionArm; pos <= targetPositionArm; pos++) {
    arm.write(pos);  // Move the arm to the current position
    delay(armDelayTime);  // Slow down the arm movement
  }
  delay(1000);

  // Move the gripper back to the zero position (open)
  gripper.write(zeroPositionGripper);
  delay(2000);  // Wait for 2 seconds at the gripper's open position
}

void command(char c, WiFiClient client)
{
  switch (c)
  {
    case 'A':
      client.println("2");
      while(!(distance <= 11.7))
      {
        Forward(240);

        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        duration = pulseIn(ECHO_PIN, HIGH);
        distance = (duration / 2) * (0.0344);

        Serial.print("Jarak : ");
        Serial.println(distance);
        delay(500);
      }
      Stop();
      Serial.println("MANTAP!");
      delay(2000);
      objUp();
      Backward(240);
      delay(3000);
      Stop();
      Serial.println("MANTAP!");
      break;
    
    case 'L':
      RotateCounterClockwise(235);
      delay(250);
      Stop();
      break;

    case 'R':
      RotateClockwise(235);
      delay(250);
      Stop();
      break;

    case 'T':
      client.println("1");
      while(!(distance < 20));
      {
        Forward(240);
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        duration = pulseIn(ECHO_PIN, HIGH);
        distance = (duration / 2) * (0.0344);

        Serial.print("Jarak : ");
        Serial.println(distance);
        delay(500);
      }
      Stop();
      Serial.println("MANTAP!");
      delay(2000);
      objDown();
      Backward(240);
      delay(2000);
      Stop();
      Serial.println("MANTAP!");
      break;
  }
}

