/**
* Satallite probe for sending temperature and humidity data to the thermostat.
* This is coded for an ESP8266 using a SHT31-D temperature module
* 
* This program uses the ESP-NOW protocol developed for the ESP boards by espressif
* 
* Make sure to install the Adafruit SHT31 library which seems to work fine for the
* module I linked in the GitHub README on amazon for cheaper
*
* Version: 1.1
* Author: Jadon G
*/
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"
uint8_t loopCnt = 0;

// ID of the node 
#define id 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// MAC ADDRESS of the 
uint8_t peer1[] = {0x40, 0x91, 0x51, 0xAB, 0x63, 0xF8};

 /**
  * This is the structure for the message
  * 
  * The node ID is a user defined integer that relays the 
  * identity of the probe to the hub in a more readable way than
  * the MAC address. This also allows you to make custom names like "jadon's room"
  * on the receiving end for display purposes.
  * 
  * The temperature and humidity properities are what they're named
  */
typedef struct message {
  int nodeId;
  float temperature;
  float humidity;
};
 
struct message myMessage;


 // Serial monitor sent status for debugging
void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
 Serial.println("Status:");
 Serial.println(sendStatus);
}
 
void setup() {
 Serial.begin(115200);
 
 
 WiFi.mode(WIFI_STA);
 
 // Get Mac Add
 Serial.print("Mac Address: ");
 Serial.print(WiFi.macAddress());
 Serial.println("ESP-Now Sender");
 
 // Initializing the ESP-NOW
 if (esp_now_init() != 0) {
   Serial.println("Problem during ESP-NOW init");
   return;
 }
 
 esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
 // Register the peer
 Serial.println("Registering a peer");
 esp_now_add_peer(peer1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
 Serial.println("Registering send callback function");
 esp_now_register_send_cb(onSent);



 //temp sensor init
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}
 
void loop() {

  //read the data from the sensor
 float temperatureC = sht31.readTemperature();
 float humidity = sht31.readHumidity();

 //send the data
  if (! isnan(temperatureC)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.print(temperatureC); Serial.print("\t\t");
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0; // convert to fahrenheit
    Serial.print("Temp *F = "); Serial.print(temperatureF); Serial.print("\t\t");
    myMessage.temperature = temperatureF;
  } else { 
    Serial.println("Failed to read temperature");
  }
  if (! isnan(humidity)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(humidity);
    myMessage.humidity = humidity;
  } else { 
    Serial.println("Failed to read humidity");
  }
 
 myMessage.nodeId = id; // assign the ID to the message


 // Send the message
 Serial.println("Send a new message");
 esp_now_send(NULL, (uint8_t *) &myMessage, sizeof(myMessage));

 
 delay(10000);
  /*
   * This a heating cycle that occurs once every 5 minuites and takes 1 min
   * This is necessary so that moisture doesn't accumulate on the sensor
   * No measurements will be taken during the cycle to prevent innacurate data (due to heating)
   * 
   * The first 30 seconds is heating and the second 30 seconds is for cooling down
   */
 if (++loopCnt == 30) {
    sht31.heater(true);
    Serial.println("Heating Sensor...");
    for (int i = 1; i < 31; i++) {
      delay(1000);
      Serial.print("Heater on for ");
      Serial.print(i);
      Serial.println("/30 seconds");
    }
    sht31.heater(false);
    for (int i = 1; i < 31; i++) {
      delay(1000);
      Serial.print("Sensor cooling for ");
      Serial.print(i);
      Serial.println("/30 seconds");
    }

    loopCnt = 0;
  }
 
}
