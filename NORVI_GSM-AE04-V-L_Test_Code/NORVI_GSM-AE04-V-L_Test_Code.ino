/*
 * NORVI-GSM-AE04-V-L FINAL
 * 
 * SD
 * RTC
 * OLED Display
 * 3 PUSH BUTTONS 
 * OLED Displa
 * 6 Digital Inputs 
 * 6 Analog Inputs(0-10V)
 * 2 Transistor Outputs
 * RS-485 Communication 
 * LTE Communication
 * 
 */
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include "FS.h"
#include "SD.h"
#include "RTClib.h"

#define ANALOG_PIN_0 36

#define INPUT1 27
#define INPUT2 34
#define INPUT3 35
#define INPUT4 14
#define INPUT5 13
#define INPUT6 5

#define OUTPUT1 12
#define OUTPUT2 2

#define RS485_RX 25
#define RS485_TX 26
#define RS485_FC 22

#define GSM_RX 33
#define GSM_TX 32
#define GSM_RESET 21

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
#define VOLTAGE_DIVIDER_RATIO 0.2065

RTC_DS3231 rtc; 
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int analog_value = 0;
  
int readSwitch(){
  analog_value = analogRead(ANALOG_PIN_0);
  return analog_value; //Read analog
}
unsigned long int timer1 = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  
  pinMode(RS485_FC, OUTPUT);
  digitalWrite(RS485_FC, HIGH);   
  
  Serial1.begin(9600, SERIAL_8N1, RS485_RX, RS485_TX); 
  Serial2.begin(115200, SERIAL_8N1, GSM_RX, GSM_TX); 

  pinMode(GSM_RESET, OUTPUT);
  digitalWrite(GSM_RESET, HIGH);   
  
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);

  pinMode(15, OUTPUT);
  digitalWrite(15,HIGH);

  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(INPUT3, INPUT);
  pinMode(INPUT4, INPUT);
  pinMode(INPUT5, INPUT);
  pinMode(INPUT6, INPUT);
  Wire.begin(16,17);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  if (!ads1.begin(0x48)) {
    Serial.println("Failed to initialize ADS 1 .");
    while (1);
  }
  if (!ads2.begin(0x49)) {
    Serial.println("Failed to initialize ADS 2 .");
    while (1);
  }
  Serial.println("");
  WiFi.mode(WIFI_MODE_APSTA);
  delay(300);
  wifi_test();
  Serial.println("");
  RTC_Check();
  delay(1000);
  Serial.println("");
  SD_CHECK();
  delay(1000);
  Serial.println("");
  
  Serial.println("Testing Modem");  
  timer1 = millis();
  Serial2.println("AT");
  while(millis()<timer1+3000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  timer1 = millis();
  Serial2.println("AT+CPIN?");
  while(millis()<timer1+2000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  timer1 = millis();
  Serial2.println("AT+CFUN?");
  while(millis()<timer1+2000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  timer1 = millis();
  Serial2.println("AT+csq");
  while(millis()<timer1+2000){
    while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
    }
  }
  Serial.println("Testing Modem Done");

  
  digitalWrite(RS485_FC, HIGH);   
}

void loop() {
  
 
  Serial.println(""); 
  Serial.print(digitalRead(INPUT1));
  Serial.print(digitalRead(INPUT2));
  Serial.print(digitalRead(INPUT3));
  Serial.print(digitalRead(INPUT4));
  Serial.print(digitalRead(INPUT5));
  Serial.print(digitalRead(INPUT6));
  Serial.println(""); 
  
  for (int i = 0; i < 4; i++) {
    int16_t adc = ads1.readADC_SingleEnded(i);
    float voltage = adc * 0.125 / 1000.0 / VOLTAGE_DIVIDER_RATIO;
    Serial.printf("V %d: %.2f V\n", i, voltage);
  }

  for (int i = 0; i < 2; i++) {
    int16_t adc = ads2.readADC_SingleEnded(i);
    float voltage = adc * 0.125 / 1000.0 / VOLTAGE_DIVIDER_RATIO;
    Serial.printf("V %d: %.2f V\n", i + 4, voltage);
  }


  Serial.println(""); 
  Serial.print("Push button  ");
  Serial.println(readSwitch());
  Serial.println(""); 
  
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, LOW);
  delay(300);
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, HIGH);
  delay(300);
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, LOW);
   
  while (Serial.available()) {
    int inByte = Serial.read();
    Serial2.write(inByte);
  }
  while (Serial2.available()) {
    int inByte = Serial2.read();
    Serial.write(inByte);
  }
  delay(100);
  digitalWrite(RS485_FC, HIGH);                    
  delay(100);
  Serial1.println(F("RS485 01 SUCCESS"));   
  delay(100);                                
  digitalWrite(RS485_FC, LOW) ;                    
                                            
  delay(100);     
  
  while (Serial1.available()) {  
    char c = Serial1.read();     
    Serial.write(c);             
  }
 delay(100);

  Serial.println("---------------------");
}

void displayTime(void) {
  DateTime now = rtc.now();  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);
}

void RTC_Check(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
 else{
 if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  }
   
  int a=1;
  while(a<6) {
    displayTime();   // printing time function for oled
    a=a+1;
  }
 }
}

void SD_CHECK(){
  uint8_t cardType = SD.cardType();
  if(SD.begin(15))
 {
  Serial.println("Card Mount: success");
  Serial.print("Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } 
  else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } 
  else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } 
  else {
      Serial.println("Unknown");
  }
  int cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Card Size: %lluMB\n", cardSize);
  }
  if(!SD.begin(15)) {
     Serial.println("NO SD card");            
  }
}

void wifi_test(){
    Serial.println("");
String str_macAddress;
byte mac[6];

WiFi.macAddress(mac);

   str_macAddress = (String(mac[0] >> 4,  HEX) + String(mac[0] & 0x0F, HEX)) + (":") +
                   (String(mac[1] >> 4,  HEX) + String(mac[1] & 0x0F, HEX)) + (":") +
                   (String(mac[2] >> 4,  HEX) + String(mac[2] & 0x0F, HEX)) + (":") +
                   (String(mac[3] >> 4,  HEX) + String(mac[3] & 0x0F, HEX)) + (":") +
                   (String(mac[4] >> 4,  HEX) + String(mac[4] & 0x0F, HEX)) + (":") +
                   (String(mac[5] >> 4,  HEX) + String(mac[5] & 0x0F, HEX));
  str_macAddress.toUpperCase();
  
  Serial.println("MAC Address: " + str_macAddress);

  String ssid =  str_macAddress; 
  const char* password = "12345678";   

  Serial.println("Setting up the Wi-Fi Access Point...");
  WiFi.softAP(ssid.c_str(), password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  Serial.print("Access Point SSID: ");
  Serial.println(ssid);

  Serial.println("Wi-Fi Access Point is active!");
  Serial.println("");
}
