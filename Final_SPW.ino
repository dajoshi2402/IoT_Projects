#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>  
#include <WiFiManager.h>  
#include <PinButton.h> 
#include <ESP8266WiFi.h> 

#include<FirebaseArduino.h>
#define FIREBASE_HOST "plantwateringsystem-a5e49.firebaseio.com" //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "MLtx14lWzzE4RB7fD6iSKnZ6bQHTIKkU0XsQK4TH" //Your Firebase Database Secret goes here

#define Ledpin D0
#define trigPin 5
#define echoPin 4
const int buttonPin = D3;
//bool ledState;
byte sensorInterrupt = 0; // 0 = digital pin 2

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;



const int buttonread = 0;
unsigned long debounce = 25;  
unsigned long lastdebouncetime = 0;
unsigned long gtime = 0; 
unsigned int baud = 115200; 
//const char* files = "/11.txt";
//const char* url  = "iot.eclipse.org";
//const int port = 1883; 
//const char* topic = "esp/test";
//String vale;
//char value[40];

#define PATH "/count"
PinButton mybutton(0); 
WiFiManager wifiManager;
//WiFiClient esclient;
//PubSubClient client(esclient);
/*
 * This function sets up MQTT connection.
 */
void setup() {
  Serial.begin(baud);
 pinMode(buttonPin, INPUT);
  pinMode(A0,INPUT);
  delay(10);
  pinMode(16,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 digitalWrite(16,HIGH);
  wifiManager.autoConnect("AutoConnectAP");
// vale = WiFi.macAddress();
  //strcpy(value, vale.c_str());
  
 /// client.setServer(url,port);
  //client.setCallback(callback);
  pinMode(buttonread,INPUT_PULLUP);
 Firebase.begin(FIREBASE_HOST);  
 Firebase.stream(PATH);

  pulseCount = 0;
    flowRate = 0.0;
    flowMilliLitres = 0;
    totalMilliLitres = 0;
    oldTime = 0;

    digitalWrite(buttonPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(buttonPin), pulseCounter, RISING);
}

void firebasereconnect()
{
 
  Serial.println("Trying to reconnect");
    Firebase.begin(FIREBASE_HOST);
  }


int l;

void loop() {
  
  mybutton.update();
  if(mybutton.isLongClick())
  {
    wifiManager.resetSettings();
    wifiManager.autoConnect("AutoConnectAP");
    Serial.println(WiFi.localIP());
  }

 if ((millis() - oldTime) > 1000) // Only process counters once per second
    {
        // Disable the interrupt while calculating flow rate and sending the value to
        // the host
        detachInterrupt(sensorInterrupt);

        // Because this loop may not complete in exactly 1 second intervals we calculate
        // the number of milliseconds that have passed since the last execution and use
        // that to scale the output. We also apply the calibrationFactor to scale the output
        // based on the number of pulses per second per units of measure (litres/minute in
        // this case) coming from the sensor.
        flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

        // Note the time this processing pass was executed. Note that because we've
        // disabled interrupts the millis() function won't actually be incrementing right
        // at this point, but it will still return the value it was set to just before
        // interrupts went away.
        oldTime = millis();

        // Divide the flow rate in litres/minute by 60 to determine how many litres have
        // passed through the sensor in this 1 second interval, then multiply by 1000 to
        // convert to millilitres.
        flowMilliLitres = (flowRate / 60) * 1000;

        // Add the millilitres passed in this second to the cumulative total
        totalMilliLitres += flowMilliLitres;

        unsigned int frac;

        // Print the flow rate for this second in litres / minute
        Serial.print("Flow rate: ");
        Serial.print(int(flowRate)); // Print the integer part of the variable
        Serial.print("."); // Print the decimal point
        // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
        frac = (flowRate - int(flowRate)) * 10;
        Serial.print(frac, DEC); // Print the fractional part of the variable
        Serial.print("L/min");
        // Print the number of litres flowed in this second
        Serial.print("  Current Liquid Flowing: "); // Output separator
        Serial.print(flowMilliLitres);
        Serial.print("mL/Sec");

        // Print the cumulative total of litres flowed since starting
        Serial.print("  Output Liquid Quantity: "); // Output separator
        Serial.print(totalMilliLitres);
        Serial.println("mL");
             
  Firebase.setString("ashish/Flowrate",String(flowMilliLitres));
         
        
        // Reset the pulse counter so we can start incrementing again
        pulseCount = 0;

        // Enable the interrupt again now that we've finished sending output
        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    }


/////////////////////////////////////////////
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
  Serial.println(charVal);
   Firebase.setString("ashish/level",charVal);
  
  int temp = analogRead(A0);
   String postStr = String(temp);
   Firebase.setString("ashish/moisture",postStr);
  if (Firebase.failed()) {
      Serial.print("setting number failed:");
      Serial.println(Firebase.error());
      digitalWrite(16,HIGH);
      delay(1000);
   //   Firebase.stream(PATH);
    //  setup();E
    //  ESP.reset();
   //  ESP.restart();
  // wifiManager.autoConnect("AutoConnectAP");
   // #define FIREBASE_HOST_RECONNECT "plantwateringsystem-a5e49.firebaseio.com"
   // Firebase.begin(FIREBASE_HOST_RECONNECT);
      firebasereconnect();
      return;
  }
  l=Firebase.getString("ashish/status").toInt();//The value read from the firebase is read in the form of String and is converted into Integer
  if(l==1){
      //digitalWrite(Ledpin,HIGH);
      Serial.println("lightON");
      digitalWrite(16,LOW);
    }
    else if(l==0){
      digitalWrite(16,HIGH);
      Serial.println("lightOFF");
    }
}
void pulseCounter() {
    // Increment the pulse counter
    pulseCount++;
}
