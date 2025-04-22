#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
#define DHTPIN A0     
#define DHTTYPE DHT11   
 
DHT dht(DHTPIN, DHTTYPE);
int hum;
float temp; //Stores temperature value
 
void setup() {    
  Serial.begin(9600);
  dht.begin();
  while (!Serial);
  Serial.println("LoRa Sender");
 
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}
 
void loop() 
{
  temp = dht.readTemperature();
  hum = dht.readHumidity();
 
  Serial.println("Sending packet: ");
 
  // send packet
  LoRa.beginPacket();
  LoRa.print("Humidity: ");
  LoRa.print(hum);
  LoRa.print("%");
  LoRa.print(" Temperature:");
  LoRa.print(temp);
  LoRa.print("C");
  
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%");
  Serial.print(" Temperature:");
  Serial.print(temp);
  Serial.println("C");
  Serial.println(""); 
 
  LoRa.endPacket();
  delay(1000);
}
