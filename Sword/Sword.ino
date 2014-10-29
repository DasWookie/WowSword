/* WOW Sword sketch to create specular pattern
 * You should see a pattern of various shades and brightness of green...
 * until a hit or impact is detected, at which point all LEDs go RED for a short duration
*/

#include <PololuLedStrip.h>

// Pin assignments
const byte         hitSensor  = A0;          // Using an analog pin would allow for either a vibration (piezo) or mechanical switch to be used. Using a piezo would require filtering for threshold minimum levels.  
const byte         modeSwitch = 10;          // Use a SPST momentary contact switch
PololuLedStrip<11> ledStrip1;                // Create ledStrip objects and specify pins used
PololuLedStrip<12> ledStrip2;                // Create ledStrip objects and specify pins used
const byte         heartbeat  = 13;          // Pin to use to track sword heartbeat, pin 13 uses the onboard LED

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT  55
rgb_color          colors1[LED_COUNT];
rgb_color          colors2[LED_COUNT];

// State Variables
bool               hitSensorReading;         // Value to determine if an impact has been detected
bool               modeSwitchReading;        // Value to determine if the mode switch button is being pressed
bool               heart             = true; // Value to track if heartbeat LED is enabled or disabled on current loop
int                currentMode;              // Tracker for illumination level mode
int                pass;                     // Tracker for loop iteration
int                setBrightness;            // Currently set brightness level
int                stepBrightness;           // Calculated value for max brightness / total modes

// User Define Variables to configure sword
int                hitDelay          = 250;  // Amount of time in milliseconds to show RED after an impact/hit is detected
int                passUpdate        = 100;  // Number of loops before we update the LED colors. In general each pass should take about 1ms, but that MAY differ based on the frequency of your crystal and the specific microcontroller used
int                totalModes        = 5;    // Total number of dimness levels / too high of granularity will make it difficult to tell which mode you are in but greater granularity of battery life
int                minBrightness     = 25;   // Lowest level of illumination  (0 is off recommend not going under 25)
int                maxBrightness     = 255;  // Maximum level of illumination (255 max)

void setup()
{
  // Globals
  pinMode(hitSensor, INPUT);
  pinMode(modeSwitch, INPUT);
  pinMode(heartbeat, OUTPUT); 
  stepBrightness  = (maxBrightness / totalModes);
  pass            = passUpdate;       // Set to update on first loop so we don't have to wait for initial light illumination
  currentMode     = totalModes;       // Set initial light illumination level to brightest level (0 = dimmest, total modes = brightest)

  startupTest();                      // Perform startup test to illuminate all LEDs sequentially to ensure they all work
}

void loop()
{
  pass++;                             // Increment pass count, we use this rather than large delays at each loop so we can keep the loops tight for hit detection
  checkHit();                         // Let the bodies hit the floor!
  if (pass >= passUpdate) {
    green();
    digitalWrite(heartbeat, heart);   // set the heartbeat LED state
    heart = !heart;                   // toggle the status on/off;
    pass = 0;
  } else {
    checkModeSwitch();                // See if our mode has changed
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
    blink(currentMode+1);                             // On Mode change, blink the sword currentMode+1 (it's base 0) number of times to show which mode you are currently set to
    delay(hitDelay * 3);                              // crude debounce just to make sure we don't skip over modes too quickly.      
  }
}

void blink(int count) {
    for(int i = 1; i <= count; i++) {
      rainbow();
      delay(250);
      off();
      delay(100);
    }
}

void checkHit() {
  hitSensorReading = digitalRead(hitSensor);    
  if (hitSensorReading == HIGH) {                     // We've got an impact, show the BLOOD!
    red();
    delay(hitDelay);
  }
}

void green() {
    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      colors1[i] = (rgb_color){ 
        0, illuminationLevel(setBrightness), 0};
      colors2[i] = (rgb_color){
        0, illuminationLevel(setBrightness), 0};
    }
    ledStrip1.write(colors1, LED_COUNT);  
    ledStrip2.write(colors2, LED_COUNT);
}

void red() {                                        // Set the blade to RED to show the "blood" of an impact
    for(uint16_t i = 0; i < LED_COUNT; i++) {
      colors1[i] = (rgb_color){
        illuminationLevel(maxBrightness), 0, 0};    // Deliberate override of users set current mode. Using MAX brightness rather than SET, to be RED, which if set dim could be rather... pink.
      colors2[i] = (rgb_color){
        illuminationLevel(maxBrightness), 0, 0};
    }
    ledStrip1.write(colors1, LED_COUNT);
    ledStrip2.write(colors2, LED_COUNT);
}

void rainbow() {                                    // Taste the Rainbow!
  for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      colors1[i] = (rgb_color){
      random(setBrightness), random(setBrightness), random(setBrightness) };
    }
    ledStrip1.write(colors1, LED_COUNT);  
    ledStrip2.write(colors1, LED_COUNT);      
}

int  illuminationLevel(int illumination) {          // Return a INT between minimum brightness level and max brightness level
  illumination = random(illumination);
  if (illumination < minBrightness) {
    illumination = minBrightness;
  }
  return (illumination);
}

void off() {
      for(uint16_t i = 0; i < LED_COUNT; i++)
      {
        colors1[i] = (rgb_color){ 
          0, 0, 0 };
      }
      ledStrip1.write(colors1, LED_COUNT);  
      ledStrip2.write(colors1, LED_COUNT);
}

void startupTest() {
  boolean side = true;                                // Bool to track which side is currently displaying gradient and which is showing rainbow
  for(int testLoop = 0; testLoop <= 10; testLoop++) {
    byte time = millis() >> 2;
    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      byte x = time - 8*i;
      colors1[i] = (rgb_color){
        x, 255 - x, x };                              // Gradient color one one side
      colors2[i] = (rgb_color){
        random(255), random(255), random(255) };      // Taste the Rainbow on the other!
    }
    if(side == true) {
      ledStrip1.write(colors1, LED_COUNT);  
      ledStrip2.write(colors2, LED_COUNT);  
    } else {
      ledStrip2.write(colors1, LED_COUNT);  
      ledStrip1.write(colors2, LED_COUNT);  
    }
    side = !side;                                     // Flip the sides the colors are being written to so each side gets both gradient as well as random rainbow tests
    delay(hitDelay);                                  // Delay between cycling to next LED
  }
}
