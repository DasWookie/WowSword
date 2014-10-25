/* WOW Sword sketch to create specular pattern
 * you should see a pattern on the LED strip that fades from
 * green to pink and also moves along the strip.
*/

#include <PololuLedStrip.h>

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<11> ledStrip1;
PololuLedStrip<12> ledStrip2;

// Create READ pin for impact sensor
#define hitSensor  A0
#define modeSwitch 10

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT  55
rgb_color          colors1[LED_COUNT];
rgb_color          colors2[LED_COUNT];

// State Variables
bool               hitSensorReading;
bool               modeSwitchReading;
int                hitDelay;
int                pass;
int                currentMode;
int                totalModes;
int                minBrightness;
int                maxBrightness;
int                setBrightness;
int                stepBrightness;

void setup()
{
  totalModes      = 5;               // Total number of dimness levels / too high of granularity will make it difficult to tell which mode you are in but greater granularity of battery life
  minBrightness   = 25;              // Lowest level of illumination  (0 is off recommend not going under 25)
  maxBrightness   = 255;             // Maximum level of illumination (255 max)
  hitDelay        = 150;             // Amount of time in milliseconds to show RED after an impact/hit is detected

  // Globals
  pinMode(hitSensor, INPUT);
  pinMode(modeSwitch, INPUT);
  stepBrightness  = (maxBrightness / totalModes);
  pass            = 0;
  currentMode     = 0;
}

void loop()
{
  pass++;        // Increment pass count, we use this rather than delays at each loop so we keep the loops tight for hit detection
  checkHit();    // check the impact sensor
  if (pass >= 10) {
    // Update the colors.
    byte time = millis() >> 2;
    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      colors1[i] = (rgb_color){ 
        0, random(setBrightness), 0     };
    }
  
    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      colors2[i] = (rgb_color){ 
        0, random(setBrightness), 0     };
    }

    // Write the colors to the LED strip.
    ledStrip1.write(colors1, LED_COUNT);  
    ledStrip2.write(colors2, LED_COUNT);
    pass = 0;
  } else {
    delay(1);
  }
}

void checkModeSwitch() {
  modeSwitchReading = digitalRead(modeSwitch);    

  if (modeSwitchReading == HIGH) {
    currentMode++;
    if (currentMode >= totalModes) {
      currentMode = 0;
    }
    setBrightness = stepBrightness + (stepBrightness * currentMode);
    if (setBrightness >= maxBrightness) {
      setBrightness = maxBrightness;
    }
    if (setBrightness <= minBrightness) {
      setBrightness = minBrightness;
    }
  }
  // TODO: Show current mode by setting LEDs to a pattern or other visual cue other than brightness to show current mode
  
}

void checkHit() {
  hitSensorReading = digitalRead(hitSensor);    
  if (hitSensorReading == HIGH) {
    for(uint16_t i = 0; i < LED_COUNT; i++) {
      colors1[i] = (rgb_color){ 
        random(setBrightness), 0, 0};
    }
    // Write the colors to the LED strip.
    ledStrip1.write(colors1, LED_COUNT);  
    ledStrip2.write(colors1, LED_COUNT);
    delay(hitDelay);
  }
}
