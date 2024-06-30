#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <Servo.h>
#include <LiquidCrystal.h>

#define IR_PIN        A2
#define PIR_PIN       A4
#define TRIGGER_PIN   A0
#define ECHO_PIN      A1
#define SERVO_PIN     10
#define GAS_SENSOR_PIN A3

// Sensor variables
int irValue = 0;
int pirValue = 0;
int distance = 0;  // Define distance variable
int gasValue = 0;

Servo myServo;

bool isInRange = false;
bool servoActionDone = false;

SoftwareSerial espSerial(8, 9);

#define DEBUG false
String mySSID = "TEST";
String myPWD = "12345678";
String myAPI = "HFCDJXHUAD398CWC";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD1 = "field1"; 
String myFIELD2 = "field2";
String myFIELD3 = "field3"; 
String myFIELD4 = "field4"; 
String myFIELD5 = "field5";  
int sendVal;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("SMART WASHROOM");
  lcd.setCursor(0, 1);
  lcd.print("USING IoT");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting..");
  delay(2000);
  
  Serial.begin(9600);
  espSerial.begin(115200);
  espSerial.println("AT+RST");
  delay(1000);
  espSerial.println("AT+CWMODE=1");
  delay(1000);
  espSerial.print("AT+CWJAP=\"");
  espSerial.print(mySSID);
  espSerial.print("\",\"");
  espSerial.print(myPWD);
  espSerial.println("\"");
  delay(5000);
  while (!espSerial.find("OK")) {
    //Wait for connection
  }
  delay(1000);

  // Initialize servo
  myServo.attach(SERVO_PIN);
}

void loop() {
  // Ultrasonic sensor readings
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  int duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in cm

  gasValue = analogRead(GAS_SENSOR_PIN);
  Serial.println("distance");
  Serial.println(distance);
   Serial.println("SMELL");
  Serial.println(gasValue);
  
  // Servo control based on distance
  if (distance < 30) {
    myServo.write(90);
    delay(2000); // Wait for 2 seconds
    myServo.write(0); // Reset position
  } else {
    myServo.write(0); // Reset position
  }
  
  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");

  lcd.setCursor(0, 1);
  lcd.print("Gas Level: ");
  lcd.print(gasValue);
  delay(2000);

  // Construct the data string
  String sendData = "GET /update?api_key=" + myAPI + "&field1=" + String(gasValue, 2) + "&field2=" + String(gasValue) + "&field3=" + String(gasValue, 2) + "&field4=" + String(gasValue);

  // Send the data to ThingSpeak
  espSerial.println("AT+CIPMUX=1");
  delay(1000);
  espSerial.println("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT);
  delay(1000);
  espSerial.print("AT+CIPSEND=0,");
  espSerial.println(sendData.length() + 2); // +2 for CR and LF
  delay(500);
  espSerial.find(">");
  delay(1000);
  espSerial.println(sendData);
  delay(1500); // Wait for response
  espSerial.println("AT+CIPCLOSE");
}