//Louis
//------------------------------------------------------
#define PIN_D0  16  // GPIO16       WAKE
#define PIN_D1   5  // GPIO5        User purpose
#define PIN_D2   4  // GPIO4        User purpose
#define PIN_D3   0  // GPIO0        Low on boot means enter FLASH mode
#define PIN_D4   2  // GPIO2        TXD1 (must be high on boot to go to UART0 FLASH mode)
#define PIN_D5  14  // GPIO14       HSCLK
#define PIN_D6  12  // GPIO12       HMISO
#define PIN_D7  13  // GPIO13       HMOSI  RXD2
#define PIN_D8  15  // GPIO15       HCS    TXD0 (must be low on boot to enter UART0 FLASH mode)
#define PIN_D9   3  //              RXD0
#define PIN_D10  1  //              TXD0

#define PIN_MOSI 8  // SD1          FLASH and overlap mode
#define PIN_MISO 7  // SD0
#define PIN_SCLK 6  // CLK
#define PIN_HWCS 0  // D3

#define PIN_D11  9  // SD2
#define PIN_D12 10  // SD4


#include <WiFiManager.h>
#include <PubSubClient.h>
#include <Wire.h>

//Defining network variables
const char* ssid = "Zwyciestwa";
const char* password = "mieszkanienumer6";
const char* mqtt_server = "192.168.1.39";

//wifi and mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//Pins used
const int analogMeasurePin = PIN_A0;

//mqtt send another message
unsigned long previousMillis = 0;
unsigned long currentMillis;


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

 
  if (String(topic) == "misc/plants") {// plant related topic
    
    if(messageTemp == "measure"){ // measure request
      Serial.print(" -> Humidity: ");
      
      //measuring the result
      const int result = (1000 - analogRead(analogMeasurePin))/10;

      //parsing the result
      char msg[10];
      sprintf(msg,"%d",result);
      
      //debug
      Serial.print((result));Serial.println("%");

      //publishing the result
      client.publish("moisture/sensor1",msg); // output visible on /measure/humidity

      /*
        Rasp should log the value or at least show it somehow (somehow nice)
      */
    
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("misc/plants");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void measure(){
  Serial.print(" -> Humidity: ");
      
      //measuring the result
      const int result = (1000 - analogRead(analogMeasurePin))/10;

      //parsing the result
      char msg[10];
      sprintf(msg,"%d",result);
      
      //debug
      Serial.print((result));Serial.println("%");

      //publishing the result
      client.publish("moisture/sensor1",msg);
}

void setup() {

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //setup pins
  pinMode(analogMeasurePin,INPUT);

    

}


void loop() {

  currentMillis = millis();

//update readings every 30 seconds
  if(currentMillis-previousMillis>=30000){
    measure();
    previousMillis=currentMillis;
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}