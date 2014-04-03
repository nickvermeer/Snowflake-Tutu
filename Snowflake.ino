// #define FORCE_SOFTWARE_SPI
// #define FORCE_SOFTWARE_PINS
#include "FastLED.h"
#include <Adafruit_NeoPixel.h>
///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
// 

// How many leds are in the strip?
#define NUM_LEDS 4
#define NUM_LEDS_METER 8.0
#define MAX_SNOWFLAKES 6
#define BRIGHTNESS 230
// Data pin that led data will be written out over
#define R1_PIN 12
#define R2_PIN 13
#define R3_PIN 14
#define L1_PIN 21
#define L2_PIN 20
#define L3_PIN 19

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8
typedef struct snow_s{
  float pos;
  float vel;
  int active;
  int brightness;
  long last_update;
}snow_t;
// This is an array of leds.  One item for each led in your strip.
CRGB leds[6][NUM_LEDS];
snow_t snow[6][MAX_SNOWFLAKES];

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS, R1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, R2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS, R3_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(NUM_LEDS, L1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(NUM_LEDS, L2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip6 = Adafruit_NeoPixel(NUM_LEDS, L3_PIN, NEO_GRB + NEO_KHZ800);

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(2000);
        MPU_setup();
        for (int strand=0;strand<1;strand++){
          for (int i=0;i<MAX_SNOWFLAKES;i++){
              snow[strand][i].pos=0.0;
              snow[strand][i].vel=0.0;
              snow[strand][1].active=0;
          }
        }
        randomSeed(analogRead(0));
        //snow[0].active=1;
        //snow[0].vel=.2*60;
        //snow[0].last_update=millis();
      // Uncomment one of the following lines for your leds arrangement.
      // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, R1_PIN, RGB>(leds[0], NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, R2_PIN, RGB>(leds[1], NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, R3_PIN, RGB>(leds[2], NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, L1_PIN, RGB>(leds[3], NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, L2_PIN, RGB>(leds[4], NUM_LEDS);
      //FastLED.addLeds<NEOPIXEL, L3_PIN, RGB>(leds[5], NUM_LEDS);
      
      // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);

      // FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, RGB>(leds, NUM_LEDS);

      // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
      strip1.begin();
      strip2.begin();
      strip3.begin();
      strip4.begin();
      strip5.begin();
      strip6.begin();
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() {
   MPU_loop();
   // Move a single white led 
   for(int strand = 0; strand < 6; strand++){
     for(int clearLed = 0; clearLed < NUM_LEDS; clearLed++) {
       leds[strand][clearLed] = CRGB(0,0,0);//CRGB::Black;  
     }
   }
   for(int strand = 0; strand < 6; strand++){
       for(int snowflake=0;snowflake<MAX_SNOWFLAKES;snowflake++){
         if (snow[strand][snowflake].active==1){
           long now = millis();
           float delta_t=((float)now-snow[strand][snowflake].last_update)/1000;
           float pos_increment=snow[strand][snowflake].vel*NUM_LEDS_METER*delta_t;
           snow[strand][snowflake].pos+=pos_increment;
           snow[strand][snowflake].last_update=now;
           if(snow[strand][snowflake].pos >= NUM_LEDS){
             snow[strand][snowflake].pos=0;
             snow[strand][snowflake].active=0;
           }else{ //falling snow
             int pos=(int)snow[strand][snowflake].pos;
             int pos1_val=0;
             int pos2_val=0;
             float interpolation=0.0;
             if(snow[strand][snowflake].vel<.20){
               interpolation=snow[strand][snowflake].pos-((float)pos);
               pos1_val=(snow[strand][snowflake].brightness*(1.0-interpolation));
               pos2_val=snow[strand][snowflake].brightness-pos1_val;
             }else{
               interpolation=(snow[strand][snowflake].pos-((float)pos))*2.0;
               if(interpolation < 1.0){
                 pos1_val=snow[strand][snowflake].brightness;
                 pos2_val=0;
               }else{
                 pos1_val=snow[strand][snowflake].brightness*(2.0-interpolation);
                 pos2_val=snow[strand][snowflake].brightness-pos1_val;
               }
             }
             float blueness=0.7;
             //blueness=blueness+(float)millis()/100000.0;
             //if (blueness > 0.7) blueness = 0.7;
             leds[strand][pos]+=CRGB(pos1_val,pos1_val,pos1_val*blueness);
             if(pos<NUM_LEDS-1){
               leds[strand][pos+1]+=CRGB(pos2_val,pos2_val,pos2_val*blueness);
             }
           }
         }else if(random(0,50000)<300){
           int16_t abs_av=abs(avg_delta_v());
           int16_t abs_ar=abs(avg_delta_r());
           if (abs_av > 50 | abs_ar > 150){
             abs_av=abs_av-50;
             if (abs_av > 1000){
               abs_av = 1000;
             }
             abs_ar=abs_ar-150;
             if (abs_ar > 1000){
               abs_ar = 1000;
             }
             float scale_a=(float)abs_av/1000.0;
             float scale_r=(float)abs_ar/1000.0;
             float scale = 0;
             if (scale_a > scale_r){
               scale=scale_a;
             }else{
               scale=scale_r;
             }
             snow[strand][snowflake].active=1;
             snow[strand][snowflake].pos=0.0;
             snow[strand][snowflake].last_update=millis();
             float rand=(((float)random(10,45))/100.0);
             snow[strand][snowflake].vel=rand;
             snow[strand][snowflake].brightness=25+((float)(BRIGHTNESS-25));//*(1-((rand-.1)/.35)));
             snow[strand][snowflake].brightness=snow[strand][snowflake].brightness*scale;
           }
         }
       }
    }
    //FastLED.show();
    showLeds();
}

void showLeds(){
  for(int led = 0;led < NUM_LEDS; led++){
       strip1.setPixelColor(led,strip1.Color(leds[0][led][0],leds[0][led][1],leds[0][led][2]));
  }
  for(int led = 0;led < NUM_LEDS; led++){
       strip2.setPixelColor(led,strip2.Color(leds[1][led][0],leds[1][led][1],leds[1][led][2]));
  }
  for(int led = 0;led < NUM_LEDS; led++){
       strip3.setPixelColor(led,strip3.Color(leds[2][led][0],leds[2][led][1],leds[2][led][2]));
  }
  for(int led = 0;led < NUM_LEDS; led++){
       strip4.setPixelColor(led,strip4.Color(leds[3][led][0],leds[3][led][1],leds[3][led][2]));
  }
  for(int led = 0;led < NUM_LEDS; led++){
       strip5.setPixelColor(led,strip5.Color(leds[4][led][0],leds[4][led][1],leds[4][led][2]));
  }
  for(int led = 0;led < NUM_LEDS; led++){
       strip6.setPixelColor(led,strip6.Color(leds[5][led][0],leds[5][led][1],leds[5][led][2]));
  }
  strip1.show();
  strip2.show();
  strip3.show();
  strip4.show();
  strip5.show();
  strip6.show();
}

