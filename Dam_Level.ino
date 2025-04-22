
//
/*
 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 
 This sketch originates from Virtualmix: http://goo.gl/kJ8Gl
 Has been modified by Winkle ink here: http://winkleink.blogspot.com.au/2012/05/arduino-hc-sr04-ultrasonic-distance.html
 And modified further by ScottC here: http://arduinobasics.blogspot.com.au/2012/11/arduinobasics-hc-sr04-ultrasonic-

sensor.html
 on 10 Nov 2012.
 */
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <stdlib_noniso.h>
#include <Time.h>

#define SLEEP_LENGTH 15
#define trigPin 5
#define echoPin 4
 
 //On board LED is on GPIO16 (D0)
#define ESP8266_LED 16

const char* ssid     = "JioFi3_467047";
const char* password = "rwhaf9rda7";
 
const char* host = "api.thingspeak.com";
const char* APIkey   = "KTG2B562076X58ZB";
// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO
 
void setup() {
 //Initialise input/output pins
 pinMode(ESP8266_LED, OUTPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
  Serial.begin(9600);
  delay(10);
 
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

 
void loop() {
  long duration;
  float distance;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  
  Serial.print(distance);
  
   char charVal[12];
 
  dtostrf(distance, 8, 2, charVal);
  Serial.println(" Centimeter:");
    Serial.print("connecting to ");
  Serial.println(host);
  delay(1000);
// Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  //Switch off LED
 digitalWrite(ESP8266_LED, 1);
  
 
 //Delay 50ms before next reading.
 delay(400);
}


  // We now create a URI for the request
  String url = "/update?key=";
  url += APIkey;
  url += "&field1=";
  url += charVal;//String(distance);
 
 
 
  Serial.print("Requesting URL: ");
  Serial.println(url);
 
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);
 
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
 
  Serial.println();
  Serial.println("closing connection");
}
