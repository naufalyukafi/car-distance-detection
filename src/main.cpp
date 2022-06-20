#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define triggerPin D8
#define echoPin D7
#define buzzerPin D6
#define redLED D1
#define greenLED D2
#define blueLED D3

const char *ssid = "BAMBANG SAPTO"; // sesuaikan dengan username wifi
const char *password = "ayam1234";  // sesuaikan dengan password wifi
const char *mqtt_server = "broker.hivemq.com"; // isikan server broker

String macAddr = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println("Mqtt Node-RED");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
}

void loop()
{
  long duration, distance;

  if (!client.connected())
  {
    reconnect();
  }

  if (!client.loop())
  {
    client.connect(macAddr.c_str());
  }

  // menembakkan pulsa sinyal ultrasonik selama 10 micro second
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  //ketika glombang ultrasonik ditangkap kembali setiap pulsanya oleh pin echo 
  //maka waktu tersebut dirubah menjadi waktu duration
  duration = pulseIn(echoPin, HIGH);

  //dengan persamaan ini akan dikonversi menjadi nilai jarak dalam bentuk cm.
  distance = (duration / 2) / 29.1;
  
  Serial.println("distance :");
  Serial.print(distance);
  Serial.println(" cm");
  delay(1000);
  if (distance <= 10)
  {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
    Serial.println("RED LED NYALA");
  }
  delay(200);
  if (distance >= 11 && distance <= 50)
  {
    digitalWrite(buzzerPin, LOW);
    delay(500);
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    Serial.println("GREEN LED NYALA");
  }
  delay(200);
  static char distanceTemp[7];
  dtostrf(distance, 4, 2, distanceTemp);
  client.publish("distance", distanceTemp);
}