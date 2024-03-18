#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
const char* ssid = "krishna home_2.4";
const char* password = "coldWinter";
const char* mqtt_server = "192.168.43.168";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int k=0;
float temperature = 0;
float thresholdmax=22;
float thresholdmin=17;
float humidity=0;
float moisture=0;
DHT dht(DHTPIN,DHTTYPE);
void setup() {
  Serial.begin(115200);
  setup_wifi();
  pinMode(21,INPUT);
  pinMode(5,OUTPUT);
  pinMode(13,OUTPUT);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
  while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str())) {
         Serial.println("mqtt broker connected");
          client.subscribe("tempout");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
  dht.begin();
}
}
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
  //Serial.println("IP address: ");
 // Serial.println(WiFi.localIP());
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
  if (String(topic) == "tempout") {
    k=messageTemp.toInt();
    delay(20);
    Serial.println(k);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str())) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("tempout");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  delay(2000);
  String client_id = "esp32-client-";
  client_id += String(WiFi.macAddress());
 // client.subscribe("esp32/output");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  float h=dht.readHumidity();
  float t=dht.readTemperature();
  long now = millis();
//  int m=analogRead(27);
  //Serial.println(k);
  
  if(t>thresholdmin && t<thresholdmax && k==0)
  {
    Serial.println("Working normally");
    digitalWrite(5,LOW);
    digitalWrite(13,HIGH);
  }
  else if(t>thresholdmax)
  {
    digitalWrite(5,LOW);
    digitalWrite(13,LOW);
    Serial.println("Cool");
  }
  else if(t<thresholdmin)
  {
    digitalWrite(13,HIGH);
    digitalWrite(5,LOW);
    Serial.println("Heat");
  }
  while(k>0){
  if(k>t)
  {
    digitalWrite(13,HIGH);
    digitalWrite(5,HIGH);
    Serial.println("Heater");
    delay(60000);
  }
  else if(k<t)
  {
    digitalWrite(5,LOW);
    digitalWrite(13,LOW);
    Serial.println("Fan working");
    delay(60000);
  }
  k=0;
  }
  int m=analogRead(21);
  //Serial.println(m);
  /*if(m>)
  {
   // digitalWrite();
  }
  */
  if (now - lastMsg > 5000) {
    lastMsg = now;
    temperature = t;   
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("tempin", tempString);
    int n=analogRead(21);
    moisture=n;
    char moistures[8];
    dtostrf(moisture,1,2,moistures);
    Serial.print("Moisture:");
    Serial.println(moistures);
    client.publish("moisture",moistures);
    
    humidity=h;
    char hst[8];
    dtostrf(humidity,1,2,hst);
    Serial.print("Humidity:");
    Serial.println(hst);
    client.publish("humidity",hst); 
  //k=0;

   //callback("esp/output"); 
  }
  k=0;
}
