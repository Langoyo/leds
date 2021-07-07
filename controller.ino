#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <FastLED.h>

#define LED_PIN     7
#define NUM_LEDS    10

#define STATIC 0
#define FADE 1
#define ALTERNATE 2
#define RAINBOW_MODE 3
#define PALETTES_CYCLE 4
#define PALETTES 5


#define UPDATES_PER_SECOND 100


#define RAINBOW 0
#define RAINBOW_STRIPE 1
#define RAINBOW_STRIPE_LINEAR 2
#define PURPLE_GREEN 3
#define RANDOM 4
#define BLACK_WHITE 5
#define BLACK_WHITE_LINEAR 6
#define CLOUD 7
#define PARTY 8
#define USA 9
#define USA_LINEAR 10
#define LAVA 11
#define OCEAN 12
#define HEAT 13
#define FOREST 14
#define ESPANA 15
#define CUSTOM 16

CRGB leds[NUM_LEDS];


SoftwareSerial hc06(2,3);
StaticJsonDocument<300> doc;


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

extern const TProgmemPalette16 espana_p PROGMEM;



String cmd="";

int mode = STATIC;
typedef struct color{
  int r;
  int g;
  int b;
} color;


color color1;
color color2;
color color3;
int lighter = 254;
int fader = 0;
bool down_up = true;
int delay_param = 0;
int brightness = 254;
int wait = 60;
int palette = 0;

void setup(){

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  FastLED.setBrightness(  brightness );

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  
  //Initialize Serial Monitor
  Serial.begin(9600);
  //Initialize Bluetooth Serial Port
  hc06.begin(9600);

  // Initialize mode and colors
  mode = STATIC;
  
  color1.r = 255;
  color1.g = 255;
  color1.b = 255;

  
  color2.r = 255;
  color2.g = 255;
  color2.b = 255;

  
  color3.r = 255;
  color3.g = 255;
  color3.b = 255;

  

  
}
void parseCommand(String cmd){

  // Obtain JSON object from recieved command
    DeserializationError error = deserializeJson(doc, cmd);
    Serial.println(cmd);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
  
    }else{
    mode  = doc["mode"];
    brightness = doc["br"];
    delay_param = doc["de"];
    palette = doc["pa"];

    color1.r = doc["c1r"];
    color1.g = doc["c1g"];
    color1.b = doc["c1b"];

  
    color2.r = doc["c2r"];
    color2.g = doc["c2g"];
    color2.b = doc["c2b"];

  
    color3.r = doc["c3r"];
    color3.g = doc["c3g"];
    color3.b = doc["c3b"];
    wait = delay_param * 10;

    FastLED.clear();
    }
    
  
}

void selectMode(){
  if(mode == FADE){
    
    for(int led = 0; led < NUM_LEDS; led++) { 
            
            leds[led] = CRGB(color1.r, color1.g, color1.b)%fader;
            if (down_up) {
              fader=fader + 1;
            }else{
              fader=fader - 1;
             }

              if(fader == 254 || fader == 0){
                
                down_up = !down_up;
                }
             delay(0);
              
        }
       
    } else if (mode == STATIC){
        for(int led = 0; led < NUM_LEDS; led++) { 
          
            
            leds[led] = CRGB(color1.r, color1.g, color1.b)%brightness;

        }
      
      } else if (mode == RAINBOW_MODE){
          uint8_t thisHue = beat8(delay_param,255);                     
  
          fill_rainbow(leds, NUM_LEDS, thisHue, brightness);
        
        } else if (mode == PALETTES_CYCLE){
             ChangePalettePeriodically();
             static uint8_t startIndex = 0;
             startIndex = startIndex + (uint8_t) delay_param/200*15; /* motion speed */

             FillLEDsFromPaletteColors( startIndex );
             FastLED.show();
             //FastLED.delay(1000 / UPDATES_PER_SECOND);
          
        } else if (mode == PALETTES){
             ChangePalette();
             static uint8_t startIndex = 0;
             startIndex = startIndex + (uint8_t) (delay_param*4)/200; /* motion speed */

             FillLEDsFromPaletteColors( startIndex );
             FastLED.show();
             //FastLED.delay(1000 / UPDATES_PER_SECOND);
          
        }
        
FastLED.show();
}

void loop(){
  //Read data from HC06
  while(hc06.available()>0){
    cmd+=(char)hc06.read();
  }

  //Select function with cmd
  if(cmd!=""){
    Serial.print("Command recieved : ");
    //Serial.println(cmd);
    parseCommand(cmd);

    cmd=""; //reset cmd
  }

  selectMode();

  delay(100);
}






void FillLEDsFromPaletteColors( uint8_t colorIndex)
{

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex = colorIndex+3;
    
  }
}


void ChangePalettePeriodically()
{
  
  uint8_t secondHand = (millis() / 1000) % wait;
  static uint8_t lastSecond = 99;
  static uint8_t n_palettes = 17;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    if ( secondHand ==  (int) wait*0/n_palettes)  {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*1/n_palettes)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == (int) wait*2/n_palettes)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*3/n_palettes)  {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*4/n_palettes)  {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*5/n_palettes)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if ( secondHand == (int) wait*6/n_palettes)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*7/n_palettes)  {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*8/n_palettes)  {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*9/n_palettes)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == (int) wait*10/n_palettes)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*11/n_palettes)  {
      currentPalette = LavaColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*12/n_palettes)  {
      currentPalette = OceanColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*13/n_palettes)  {
      currentPalette = HeatColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*14/n_palettes)  {
      currentPalette = ForestColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*15/n_palettes)  {
      currentPalette = espana_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == (int) wait*16/n_palettes)  {
      SetupCustomPalette();
      currentBlending = LINEARBLEND;
    }
  }
}

void ChangePalette()
{


  
    if ( palette ==  RAINBOW )  {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  RAINBOW_STRIPE )  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( palette ==  RAINBOW_STRIPE_LINEAR )  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  PURPLE_GREEN )  {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  RANDOM )  {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  BLACK_WHITE )  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if ( palette ==  BLACK_WHITE_LINEAR )  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  CLOUD )  {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  PARTY )  {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  USA )  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if ( palette ==  USA_LINEAR )  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  LAVA )  {
      currentPalette = LavaColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  OCEAN )  {
      currentPalette = OceanColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  HEAT )  {
      currentPalette = HeatColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  FOREST )  {
      currentPalette = ForestColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  ESPANA )  {
      currentPalette = espana_p;
      currentBlending = LINEARBLEND;
    }
    if ( palette ==  CUSTOM )  {
      SetupCustomPalette();
      currentBlending = LINEARBLEND;
    }
    
  
}



// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

void SetupCustomPalette()
{
  // 'black out' all 16 palette entries...
  
  // and set every fourth one to white.
  currentPalette[0] = CRGB(color1.r, color1.g, color1.b);
  currentPalette[1] = CRGB(color1.r, color1.g, color1.b);
  currentPalette[2] = CRGB(color1.r, color1.g, color1.b);
  currentPalette[3] = CRGB(color1.r, color1.g, color1.b);
  currentPalette[5] = CRGB(color1.r, color1.g, color1.b);

  currentPalette[6] = CRGB(color2.r, color2.g, color2.b);
  currentPalette[7] = CRGB(color2.r, color2.g, color2.b);
  currentPalette[8] = CRGB(color2.r, color2.g, color2.b);
  currentPalette[9] = CRGB(color2.r, color2.g, color2.b);
  
  currentPalette[10] = CRGB(color3.r, color3.g, color3.b);
  currentPalette[11] = CRGB(color3.r, color3.g, color3.b);
  currentPalette[12] = CRGB(color3.r, color3.g, color3.b);
  currentPalette[13] = CRGB(color3.r, color3.g, color3.b);
  currentPalette[14] = CRGB(color3.r, color3.g, color3.b);

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
                     green,  green,  black,  black,
                     purple, purple, black,  black,
                     green,  green,  black,  black,
                     purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};

const TProgmemPalette16 espana_p PROGMEM =
{
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,

  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,

  CRGB::Red,
  CRGB::Red,
  CRGB::Red,
  CRGB::Red,
  
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
};
