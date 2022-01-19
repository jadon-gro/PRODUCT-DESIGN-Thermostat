#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
/**
 * Thermostat control center for ESP-32 board.
 * This is programmed for the SSD1306 OLED screen by adafruit.
 * 
 * Dependencies:
 * ESP32Servo library made by Kevin Harrington
 * Adafruit's GFX and SSD1306 libraries
 * 
 * This program allows the esp32 to manipulate the thermostat with room for error and use buttons
 * to wake the OLED screen and to change the target temperature
 * 
 * The OLED goes to sleep after 2 min of being idle. This is to prevent dimming caused by prolonged
 * on-time
 * 
 * Note: Servo will wiggle around a bit when changing position. This is to solve a problem where
 * my weak old servo would start stalling at the end of a stroke because the knob became too hard to move.
 */

#include <ESP32Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Screen set up -----------------------------------------------------------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI:
#define OLED_MOSI   23
#define OLED_CLK   18
#define OLED_DC   19
#define OLED_CS    5
#define OLED_RESET 17
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
bool displaying = true;
int sleepCounter = 13;





// Servo set up -----------------------------------------------------------------------------
// These are angles. The thermostat has a position for cold, and if the target temperature 
// isn't reached after some time, it moves to colder. Same for hot/hotter
int pos = 65; // initial servo position
int coldPos = 55; 
int colderPos = 5;
int hotPos = 125;
int hotterPos = 175;
float targetTemp = 70.0;
Servo tempservo; // create servo object
int servoPin = 13;


// MISC set up ------------------------------------------------------------------------------
float currTemp[] = {70.0}; // add another entity for every recieving node you have
char *locations[] = {"Jadon's Room"};
int numNodes = 1;
int changeCounter = 0; //counts checks since last change
int checked = 0; // checked is a boolean that prevents the servo from adjusting to default values after a reset
char* message = "idle";



// Button set up --------------------------------------------------------------------------
#define COLD_BUTTON   34
#define WARM_BUTTON   35
#define WAKE_BUTTON   32






/*
 * This is the message struct for recieving data from nodes
 * node ID is user defined and can help make more readable identifiers for nodes
 */
typedef struct message {
  int nodeId;
  float temperature;
  float humidity;
};

struct message myMessage;

void onDataReceiver(const uint8_t * mac, const uint8_t *incomingData, int len) {
  checked = 1;
  Serial.println("Message received.");
  memcpy(&myMessage, incomingData, sizeof(myMessage));
  Serial.println("=== Data ===");
  Serial.print("Mac address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }
  Serial.print("\n");
  Serial.print(locations[myMessage.nodeId]);
  Serial.print("\n\nTemperature: ");
  Serial.println(myMessage.temperature);
  Serial.print("\n\nHumidity: ");
  Serial.println(myMessage.humidity);
  Serial.println();
  currTemp[myMessage.nodeId] = myMessage.temperature;

}

void setup() {
  Serial.begin(115200);
  // Servo set up -----------------------------------
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  tempservo.setPeriodHertz(50);    // standard 50 hz servo
  tempservo.attach(servoPin, 685, 3000); // attaches the servo on pin 18 to the servo object
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep



  // Wifi set up ------------------------------------
  WiFi.mode(WIFI_STA);

  // Get Mac Add
  Serial.print("Mac Address: ");
  Serial.print(WiFi.macAddress());
  Serial.println("ESP32 ESP-Now Broadcast");

  // Initializing the ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Problem during ESP-NOW init");
    return;
  }
  esp_now_register_recv_cb(onDataReceiver);



  // Init screen
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  // Button set up
  pinMode(COLD_BUTTON, INPUT_PULLUP);
  pinMode(WARM_BUTTON, INPUT_PULLUP);
  pinMode(WAKE_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(COLD_BUTTON), dec, FALLING);
  attachInterrupt(digitalPinToInterrupt(WARM_BUTTON), inc, FALLING);
  attachInterrupt(digitalPinToInterrupt(WAKE_BUTTON), wake, FALLING);


  pos = 90;
  tempservo.write(pos+2);
  delay(500);
  tempservo.write(pos-2);
  delay(500);
  tempservo.write(pos);
}

void loop() {
  float avgTemp = average(currTemp, numNodes);
  Serial.print("\n\n== Average temp check ==\n Average Temp:");
  Serial.println(avgTemp);
  Serial.println("========================\n\n");


  /*
   * This whole structure is the meat of the program and controls the servo
   * 
   * If the average temperature is more than .7 degree from the target temperature,
   * the servo will move to correct for this. Either it will move to "hotpos" or "coldpos"
   * If the target temperature hasn't been reached for 15 min, it will move to a more severe position,
   * i.e. "hotterpos" or "colderpos". If the target temperature has been reached withing 1.4 degrees,
   * the servo will move back to idle.
   */
  if (checked) {
    if (targetTemp - avgTemp > .7) {
      Serial.println("Making warmer");
      if (changeCounter == 0 && (pos == hotPos || pos == hotterPos)) {
        message = "warming+";
        pos = hotterPos;
        tempservo.write(pos + 3);
        delay(500);
        tempservo.write(pos);
      } else if (changeCounter < 30) {
        message = "warming";
        pos = hotPos;
        tempservo.write(pos + 3);
        delay(500);
        tempservo.write(pos);
      }

      changeCounter = 90; // waits 90*10 seconds (15 min) before changing again
    } else if (targetTemp - avgTemp < -.7) {
      Serial.println("Making colder");
      if (changeCounter == 0 && (pos == coldPos || pos == colderPos)) {
        message = "cooling+";
        pos = colderPos;
        tempservo.write(pos - 3);
        delay(500);
        tempservo.write(pos);
      } else if (changeCounter < 30) {
        message = "cooling";
        pos = coldPos;
        tempservo.write(pos - 3);
        delay(500);
        tempservo.write(pos);
      }

      changeCounter = 90; // waits 90*10 seconds (15 min) before changing again
    } else { // settles down to a normal temp
      message = "idle";
      if (pos > 90) {
        pos = 115;
        delay(500);
        tempservo.write(pos);
      }
      if (pos < 90) {
        pos = 65;
        tempservo.write(pos - 3);
        delay(500);
        tempservo.write(pos);
      }

    }
    if (changeCounter != 0) {
      changeCounter--;
    }
  }
  // Display routine
  updateDisplay();

  delay(10000); // 10 seconds delay between checks
}
/*
 * This method updates the display
 * 
 * This is governed by the displaying boolean which will become false after 2 min of no activity
 */
void updateDisplay(void) {
  if (displaying) {
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(targetTemp, 1);
    display.cp437(true);
    display.setCursor(70, 5);
    display.setTextSize(1);
    display.print(message);
    display.drawLine(51, 0, 51, 17, SSD1306_WHITE);
    display.drawLine(0, 17, 51, 17, SSD1306_WHITE);
    display.setTextSize(1);
    for (int i = 0; i < numNodes; i++) {
      display.setCursor(0, (3 + (i * 3)) * 8);
      if (i == 0) {
        display.print("Jadon's Room:");
      }

      display.setCursor(0, (4 + (i * 3)) * 8);
      display.print("    Temp:     ");
      display.print(myMessage.temperature);
      display.write(248);
      display.println("F");
      display.print("    Humidity: ");
      display.print(myMessage.humidity);
      display.print("%");
    }
    if (sleepCounter == 0) {
      displaying = false;
      display.clearDisplay();
      display.setCursor(80, 28);
      display.print("sleeping...");
      display.display();
      delay(2000);
      display.clearDisplay();
      display.display();
    } else {
      sleepCounter--;
    }
    display.display();





  }
}

// This method just takes the average of an array
float average (float * array, int len)  // assuming array is int.
{
  float sum = 0.0;  // sum will be larger than an item, long for safety.
  for (int i = 0; i < len; i++)
    sum += array [i];
  return  ((float) sum) / (float)len;
}

// ISR for cooling button. This also debounces.
void dec()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {

    sleepCounter = 12;
    displaying = true; // wake
    targetTemp = targetTemp - .1; // decrement target temp by .1 degrees
    updateDisplay(); // update display
  }
  last_interrupt_time = interrupt_time;
}

// ISR for warming button. This also debounces.
void inc()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {

    sleepCounter = 12;
    displaying = true; // wake
    targetTemp = targetTemp + .1; // increment target temp by .1 degrees
    updateDisplay(); // update display
  }
  last_interrupt_time = interrupt_time;
}

// ISR for wake button. This also debounces.
void wake()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    sleepCounter = 12;
    displaying = true; // wake

    updateDisplay();
  }
  last_interrupt_time = interrupt_time;
}
