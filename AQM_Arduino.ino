#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "DHT.h"

//DHT Section setup
#define DHTPIN D3 // 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h, t;

//Server Setup Section

String apiKey = "Your API of thingsspeak";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "Your wifi Network name";     // replace with your wifi ssid and wpa2 key
const char *pass =  "Network password";
const char* server = "api.thingspeak.com";

WiFiClient client;

//Dust Sensor
int measurePin = A0;
int ledPower = D0;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;


void setup() {
  pinMode(ledPower, OUTPUT);
  pinMode(measurePin, OUTPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");


}

void loop() {
  // put your main code here, to run repeatedly:
  temp();
  senddata();
  dustdata();
  delay(10000);

}

//Functions written now on

void temp() {
  h = dht.readHumidity();
  t = dht.readTemperature();
}

void dustdata() {
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5.0V mapped to 0 - 1023 integer values
  calcVoltage = voMeasured * (5.0 / 1024);

  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = (0.17 * calcVoltage - 0.1) * 1000;

  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);

  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);

  Serial.print(" - Dust Density: ");
  Serial.println(dustDensity);


  total = total - readings[readIndex];
  // read from the sensor:
  //readings[readIndex] = analogRead(inputPin);
  readings[readIndex] = dustDensity;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits

  if (average < 0) {
    average = -0.833 * average ;
    Serial.println(average);
  }
  else {
    Serial.println(average);
  }
}

void senddata() {
  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(average);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%. Send to Thingspeak.");
    Serial.print("DustPM2.5: ");
    Serial.print(average);
  }
  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}


