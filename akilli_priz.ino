#include <WiFi.h>
#include <PubSubClient.h>

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Bağlantısı *********************************/

#define WLAN_SSID       "<ssid>"
#define WLAN_PASS       "<password>"

/************************* Adafruit.io Ayarlar *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "<username>"
#define AIO_KEY       "<key>"

/************************* Adafruit.io Ayarlar *********************************/

WiFiClient clientAdafruit;
Adafruit_MQTT_Client mqtt(&clientAdafruit, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe role_ac_kapat = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ev/roleDurum");

/****************************** Feeds ***************************************/

/****************************** Node-Red MQTT Ayarları ***************************************/
/*
const char* mqttServer = "192.168.43.192";
const int mqttPort = 1883;
const char* mqtt_topic = "bilgisayarliKontrol/roleDurum";
const char* clientID = "esp32";
const char* mqttUser = "";
const char* mqttPassword = "";
 
WiFiClient espClient;
PubSubClient client(espClient);
*/
/****************************** Node-Red MQTT Ayarları ***************************************/

void MQTT_connect();

int role_pin = 21;

void setup() {
  delay(10);

  pinMode (role_pin, OUTPUT);
  digitalWrite(role_pin, HIGH);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqtt.subscribe(&role_ac_kapat);
/*
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    if (client.connect(clientID, mqttUser, mqttPassword )) {
    } else {
      delay(2000);
    }
  }
  client.subscribe(mqtt_topic);*/
}

String roleSonDurum;

void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(1000))) {
    
    String roleDurum=(char *)role_ac_kapat.lastread;
    char* charRoleDurum = (char *)role_ac_kapat.lastread;

    if(roleDurum != roleSonDurum) {

      roleSonDurum = roleDurum;

      if(roleDurum == "on"){
        digitalWrite(role_pin, LOW);
        /*
        if (client.publish(mqtt_topic, charRoleDurum)) {
        }
        else {
          client.connect(clientID);
          delay(10);
          client.publish(mqtt_topic, charRoleDurum);
        }*/
      }
      else if(roleDurum == "off") {
        digitalWrite(role_pin, HIGH);
        /*
        if (client.publish(mqtt_topic, charRoleDurum)) {
        }
        else {
          client.connect(clientID);
          delay(10);
          client.publish(mqtt_topic, charRoleDurum);
        }*/
      }
      
    }
    
  }
  //client.loop();
}

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}
