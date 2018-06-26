/*This code explains the Compost Monitoring and to measure the temperature during
  Decompostioon of food waste under three different conditions
  Uses DS18b20 sensor to measure food waste temperatures and
  DHT22 to measure temperature outside the bin.
  LCD displays the values on screen
*/
//Libraries
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino 

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//Address
LiquidCrystal_I2C lcd(0x3F, 16, 2);



const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "vF2991BD53E2C4B1"; //device ID from Pushingbox 

//DHT, DSB
#define DHTPIN D4
DHT dht (DHTPIN, DHT22); // DHT22 on nodemcu pin D4
#define ONE_WIRE_BUS D5 // DS18B20 on NodeMCU pin D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
//DSB Variables declaration
float temp_0;
//float temp_1;
//float temp_2;
//DHT22 variables declaration
int humidity;
int temp;

WiFiClient client;

void setup()
{

  Serial.begin(115200);
  DS18B20.begin();
  dht.begin();
  Serial.println("Testing DSB and DHT Sensor data");
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Compost Monitor");
  lcd.setCursor(0,1);
  lcd.print("Setting Up!!");
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.setTimeout(60);

  if (!wifiManager.autoConnect("SetupWifionCompost")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  lcd.clear();
}

void loop() {
  delay(2000);
  dsb(); //Function to call DSB values
  ht();  //function to call DHT values
  showonlcd();
  senddata();
  //pushbox();
}
void dsb() {

  DS18B20.requestTemperatures();
  temp_0 = DS18B20.getTempCByIndex(0); // Sensor 0 will capture Temp in Celcius for Bin 0
  //temp_1 = DS18B20.getTempCByIndex(1); // Sensor 1 will capture Temp in Celcius for Bin 1
  //temp_2 = DS18B20.getTempCByIndex(2); // Sensor 1 will capture Temp in Celcius for Bin 2
  Serial.print("DSB Data");
  Serial.println("Temp_0: ");
  Serial.print(temp_0);
  Serial.println("Temp_1: ");
  Serial.print(temp_1);

  Serial.println("Temp_2:");
  Serial.print(temp_2);*/
  //delay(1000);
}

void ht() {
  delay(1000);
  humidity = dht.readHumidity();
  temp = dht.readTemperature();
  Serial.println("DHT22 Data");
  Serial.println("\n Humidity =");
  Serial.print(humidity);
  Serial.print("% \n");
  Serial.println("Temperature =");
  Serial.print(temp);
  Serial.print(" *C");
}
void showonlcd() {
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("T(C):");
  lcd.print(temp);
  //lcd.setCursor(0, 8);
  lcd.print(" H(%):");
  lcd.print(humidity);
  lcd.setCursor(0,1);
  lcd.print("Bin Temp(C):");
  lcd.print(temp_0);
  lcd.setCursor(0, 2);
  lcd.print("Temp_1)C):");
  lcd.print(temp_1);
  lcd.setCursor(0, 3);
  lcd.print("Temp_2(C):");
  lcd.print(temp_2);
}

void senddata() {
  if (client.connect(server, 3000)) { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp_0);
    postStr += "&field2=";
   postStr += String(temp_1);
    postStr += "&field3=";
    postStr += String(temp_2);*/
    postStr += "&field4=";
    postStr += String(temp);
    postStr += "&field5=";
    postStr += String(humidity);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    //client.print("Host: api.thingspeak.com\n");
    client.print("Host: http://52.66.114.79:3000/update\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(3000);
  }
  client.stop();
}
void pushbox() {
  if (client.connect(WEBSITE, 80))
  {
    client.print("GET /pushingbox?devid=" + devid
                 
                 + "&dhttemp=" + (String) dhttemp
                 + "&dhthum=" + (String) dhthum
                 + "&box1t="  + (String) bin1t
                 
                );

    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(WEBSITE);
    client.println("User-Agent: ESP8266/1.0");
    client.println("Connection: close");
    client.println();
  }
}

