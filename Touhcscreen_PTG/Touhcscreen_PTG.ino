#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // Touch Scrren library

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//Defining Touch Screen Pins
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//Defining Analong Pins
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// Data Line Connections for UNO
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// D0 and D1 connect to 28 and 29 respectively D2-D7 connect to 22-25
// (on the 2-row header at the end of the board).

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GOLD    0xFEA0
#define SILVER    0xC618
#define LIME    0x07E0


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

int currentpage = 0; // 0 - Start Screen
int flength;

void setup(void) {
  
  Serial.begin(9600);
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }
  
  tft.begin(identifier);

//VARIABLE DECLARATIONS
  int currentpage = 0;
  
  tft.setRotation(1);
  start_button();

}

void loop(void) {
  

  //reading touch sensor
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
  
  if (currentpage == 0){//START SCREEN

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
  
      if (p.x > 50 && p.x < 270 && p.y > 50 && p.y < 205){
        
        tft.fillScreen(BLACK);
        focal_length_screen(); //LOAD FOCAL LENGTH SCREEN
        currentpage = 1; //GO TO FOCAL LENGTH SCREEN
      }
    }
  }

  if (currentpage == 1){//FOCAL LENGTH SCREEN

    delay(500);  
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);        
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
      if (p.x > 0 && p.x < 50 && p.y > 130 && p.y < 170){
        //Serial.println("Plus");
        flength++;
      }   
    if (p.x > 0 && p.x < 50 && p.y > 90 && p.y < 130){
      //Serial.println("Minus");
      flength--;
      }
    Serial.print("the flength is ");
    Serial.println(flength);
    num_cover();
    tft.setCursor(145, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println(flength);
    }
    if (p.x > 270 && p.x < 320 && p.y > 0 && p.y < 40){

      tft.fillScreen(BLACK);
      start_button();
      currentpage = 1;
    }
  }
}

void please_enter(){
    tft.setCursor(80, 18);
    tft.setTextColor(GOLD);
    tft.setTextSize(2);
    tft.println("Please Enter:");
    return;
}

void back_button(){
    tft.fillRect(0, 0, 36, 36, WHITE);
    tft.fillRect(2, 2, 32, 32, SILVER);
    tft.fillTriangle(6, 17, 14, 10, 14, 24, WHITE);
    tft.fillRect(14, 15, 14, 5, WHITE); 
    
}
void plus_minus(){
    tft.fillRect(160, 180, 60, 60, WHITE);
    tft.fillRect(165, 185, 50, 50, RED);
    tft.fillRect(188, 195, 5, 30, WHITE);
    tft.fillRect(175, 207, 30, 5, WHITE);

    tft.fillRect(100, 180, 60, 60, WHITE);
    tft.fillRect(105, 185, 50, 50, BLUE);
    tft.fillRect(115, 207, 30, 5, WHITE);    
    return;
}

void num_cover(){
  tft.fillRect(125, 120, 70, 40, BLACK);
}

void start_button(){

  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.fillRoundRect(50, 50, 220, 140, 20, BLUE);
  tft.drawRoundRect(50, 50, 220, 140, 20, WHITE);
  
  tft.setCursor(90, 105);
  tft.setTextColor(GOLD);
  tft.setTextSize(5);
  tft.println("START");
  return;
}

void focal_length_screen(){

    plus_minus();
    please_enter();
    back_button();
    
    tft.fillRoundRect(70, 50, 180, 60, 20, SILVER);
    tft.drawRoundRect(70, 50, 180, 60, 20, WHITE);
    tft.setCursor(87, 75);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("FOCAL LENGTH");
    return;
}
