#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
//-------- wifi-----------
const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";
//////////  dht ..........
#include "DHT.h"
#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);
 //...............device creedentials........................
#define ORG "1lpuff"
#define DEVICE_TYPE "NodeMCU.123"
#define DEVICE_ID "123"
#define TOKEN "0987654321"
String command;
char subtopic[] = "iot-2/cmd/home/fmt/String";
//-------- Customise the above values -------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubtopic[] = "iot-2/evt/dht11/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
void callback(char* topic, byte* payload, unsigned int payloadLength);
///////////////////
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
void setup() 
{
 Serial.begin(115200);
   dht.begin();
 Serial.println();
  pinMode(D1,OUTPUT);
  wifiConnect();
  mqttConnect();
}
void loop() {
  
float h = dht.readHumidity();
float t = dht.readTemperature();


 
PublishData(t,h);
delay(1000);
  if (!client.loop()) {
    mqttConnect();
  }
delay(100);
}

void wifiConnect() {
 Serial.print("Connecting to ");
 Serial.print(ssid);
 
 //connect to network
 WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
}


void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}




void PublishData(float temp, float humid)
{
 if(!client.connected())
 {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 
 while (!client.connect(clientId, authMethod, token))
 {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
 //json format

  String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload += "}}";
  
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(pubtopic, (char*) payload.c_str()))
 {
 Serial.println("Publish ok");
 } else
 {
 Serial.println("Publish failed");
 }
}



void callback(char* subtopic, byte* payload, unsigned int payloadLength)
{
  Serial.print("callback invoked for subtopic: "); Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++)
  {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);

if(command == "light_on")
{
  digitalWrite(D1,HIGH);
  Serial.println("Light is Switched ON");
}
else if(command == "light_off")
{
  digitalWrite(D1,LOW);
  Serial.println("Light is Switched OFF");
}
command ="";
}
