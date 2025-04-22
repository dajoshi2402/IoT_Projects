

/*
 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 
 */
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <stdlib_noniso.h>
#include <Time.h>

const int trigPin = 5;

const int echoPin = 4;

const int buzzer = 2;

long duration;

int distance;

int safetyDistance;
 
 //On board LED is on GPIO16 (D0)
#define ESP8266_LED 16

const char* ssid     = "JioFi3_467047";
const char* password = "rwhaf9rda7";
 
const char* host = "api.thingspeak.com";
const char* APIkey   = "5WR2FRYIIZYBABU3";
// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO

 
void setup() {
 //Initialise input/output pins
 pinMode(ESP8266_LED, OUTPUT);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
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
 digitalWrite(trigPin, LOW);

 delayMicroseconds(2);

 digitalWrite(trigPin, HIGH);

 delayMicroseconds(10);

 digitalWrite(trigPin, LOW);

 duration = pulseIn(echoPin, HIGH);

 distance= duration*0.034/2;
  safetyDistance = distance; if (safetyDistance <= 5){

  digitalWrite(buzzer, HIGH);

  }

  else{

  digitalWrite(buzzer, LOW);
  }

  Serial.print(distance);
  Serial.print(safetyDistance);
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
