#define BLYNK_TEMPLATE_ID   "TMPL4-VCDGDqL" // Id of template in Blynk
#define BLYNK_TEMPLATE_NAME "Leds Panel" // Name of template in Blynk
#define BLYNK_AUTH_TOKEN    "hscES1uxjzLC38lu2cE-LkbuugKL5ngK" // Auth token in Blynk

// Libraries

#include <Arduino.h>
#include <FastLED.h>
#include <TaskScheduler.h>
#include <BlynkSimpleEsp32.h>

#define NUM_LEDS_PER_PANEL 20 // Number of leds diods per panel - 1 panel will eventaully have 32 x 4 diodes
#define NUM_PANELS 4 // Number of quaters of 1 panel
#define MAX_BRIGHTNESS 100 

const char* ssid = "Konfesjonal"; //Wifi Name xxxxxxx
const char* pass = "x74mwu42"; //Wifi Pass xxxxxxx

static constexpr int LED_PINS[NUM_PANELS] = {4, 0, 2, 15}; // Pins for cables of led bars 

const int TOUCH_PINS[NUM_PANELS][4] = { // 2 dimensional array width divided sensor pins for 4 panels
  {33, 34, 35, 32}, // 1st panel - PINS P33 P34 P35 P32
  {25, 26, 27, 14}, // 2nd panel - PINS P25 P26 P27 P14
  {12, 23, 22, 1},  // 3rd panel - PINS P12 P23 P22 P1
  {21, 19, 18, 5}   // 4th panel - PINS P21 P19 P18 P5
};

CRGB leds[NUM_PANELS][NUM_LEDS_PER_PANEL]; // 2 dimensional CRGB array
Scheduler scheduler; // defining the thread with the help of the Scheduler
int sensorStates[NUM_PANELS][4] = {0}; // 2 dimensional array for sensors state

// rgb of all leds
int redValue = 144; // defalt red hue
int greenValue = 238; // default green hue
int blueValue = 144; // default blue hue

bool colorChanged = false; // variable to check if color was changed by app

void sensorsManager(); // definition of function that manage the work of sensors
void ledsSwitch(); // definition of function that lights up led bars
void updateLeds(); // definition of function that updates led bars color

Task sensorsTask(5, TASK_FOREVER, &sensorsManager); // definition of thread for sensors, 5 ms delay to prevent lag, task_forever - in loop,
Task ledsTask(5, TASK_FOREVER, &ledsSwitch); // definition of thread for switching on the light of led bars, 5 ms delay to prevent lag, task_forever - in loop,

void sensorsManager() { // logic of sensorsManager function
  for (int panel = 0; panel < NUM_PANELS; panel++) {
    for (int sensor = 0; sensor < 4; sensor++) {
      sensorStates[panel][sensor] = digitalRead(TOUCH_PINS[panel][sensor]);
    }
  }
}

void ledsSwitch() { // logic of ledsSwitch function
    if (colorChanged) {
    updateLeds();
    colorChanged = false;
  }
  for (int panel = 0; panel < NUM_PANELS; panel++) {
    for (int ledIndex = 0; ledIndex < (NUM_LEDS_PER_PANEL / 4); ledIndex++) {
      if (sensorStates[panel][0] == HIGH) {
        leds[panel][ledIndex] = CRGB(redValue, greenValue, blueValue);
        leds[panel][ledIndex].fadeLightBy(255 - MAX_BRIGHTNESS);
      } else {
        leds[panel][ledIndex] = CRGB(0, 0, 0);
        leds[panel][ledIndex].fadeLightBy(255); 
      }
      if (sensorStates[panel][1] == HIGH) {
        leds[panel][ledIndex+5] = CRGB(redValue, greenValue, blueValue);
        leds[panel][ledIndex+5].fadeLightBy(255 - MAX_BRIGHTNESS);
      } else {
        leds[panel][ledIndex+5] = CRGB(0, 0, 0);
        leds[panel][ledIndex+5].fadeLightBy(255); 
      }
      if (sensorStates[panel][2] == HIGH) {
        leds[panel][ledIndex+10] = CRGB(redValue, greenValue, blueValue);
        leds[panel][ledIndex+10].fadeLightBy(255 - MAX_BRIGHTNESS);
      } else {
        leds[panel][ledIndex+10] = CRGB(0, 0, 0);
        leds[panel][ledIndex+10].fadeLightBy(255); 
      }
      if (sensorStates[panel][3] == HIGH) {
        leds[panel][ledIndex+15] = CRGB(redValue, greenValue, blueValue);
        leds[panel][ledIndex+15].fadeLightBy(255 - MAX_BRIGHTNESS);
      } else {
        leds[panel][ledIndex+15] = CRGB(0, 0, 0);
        leds[panel][ledIndex+15].fadeLightBy(255); 
      }
      FastLED.show();
      delay(25); // delay for animation
    }
  }
}

void setup() {
  Serial.begin(115200); // must be the same as in platformio.ini file, recomended to be 115200 value

  // adding all leds to FastLED (pins with cables connected to them)
  FastLED.addLeds<WS2812, LED_PINS[0], GRB>(leds[0], NUM_LEDS_PER_PANEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, LED_PINS[1], GRB>(leds[1], NUM_LEDS_PER_PANEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, LED_PINS[2], GRB>(leds[2], NUM_LEDS_PER_PANEL).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, LED_PINS[3], GRB>(leds[3], NUM_LEDS_PER_PANEL).setCorrection(TypicalLEDStrip);

  // adding pinMode to all touch sensors
  for (int i = 0; i < NUM_PANELS; i++) {
    for (int j = 0; j < 4; j++) {
      pinMode(TOUCH_PINS[i][j], INPUT);
    }
  }

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // connection to app

  scheduler.addTask(sensorsTask); // adding thread for managing sensors
  sensorsTask.enable(); // enabling thread for managing sensors
  scheduler.addTask(ledsTask); // adding thread for switching on the light of led bars
  ledsTask.enable(); // enabling thread for switching on the light of led bars
}

void loop() {
  Blynk.run();
  scheduler.execute();
}

void updateLeds() { // logic of updating color of diodes
  redValue = map(redValue, 0, 255, 0, 255); 
  greenValue = map(greenValue, 0, 255, 0, 255); 
  blueValue = map(blueValue, 0, 255, 0, 255); 
  
  for (int panel = 0; panel < NUM_PANELS; panel++) {
    for (int ledIndex = 0; ledIndex < NUM_LEDS_PER_PANEL; ledIndex++) {
      leds[panel][ledIndex] = CRGB(redValue, greenValue, blueValue);
    }
  }
}

BLYNK_WRITE(V1) { // reading red hue
  redValue = param.asInt();
  colorChanged = true;
}

BLYNK_WRITE(V2) { // reading green hue
  greenValue = param.asInt();
  colorChanged = true;
}

BLYNK_WRITE(V3) { // reading blue hue
  blueValue = param.asInt();
  colorChanged = true;
}