#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // Touch Scrren library
#include <math.h>

#define PI 3.1415926535897932384626433832795
#define sensor_width 36 //sensor width for a full frame camera
#define sensor_height 24 //sensor height for a full frame camera

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//Defining Touch Screen Pins
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 23   // can be a digital pin
#define XP 22   // can be a digital pin

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

//VARIABLE DECLARATIONS
int currentpage = 0; // 0 - Start Screen
int flength = 22, save_flength;
float d_centre = 3, save_d_centre;
float h_object = 5, save_h_object;
float d_object = 0, save_d_object;

int plus_1_x = -10;
int plus_2_x = 60;
int plus_1_y = 111;
int plus_2_y = 160;
int minus_1_x = -10;//same as plus_1_x
int minus_2_x = 60;//same as plus_2_x
int minus_1_y = 60;
int minus_2_y = 99;

int back_1_x = 240;
int back_2_x = 320;
int back_1_y = -10;
int back_2_y = 30;
int conf_1_x = 240;//same as back_1_x
int conf_2_x = 320;//same as back_2_x
int conf_1_y = 190;
int conf_2_y = 230;

double AOV_width; // angle of view for the width of the field of view
double AOV_height; //angle of view for the height of the field of view
double focal_length; //equivalent focal length of smartphone camera

double object_diameter;
double object_height;

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
  tft.setRotation(3);
  start_button();
  Serial.println("\nSTART SCREEN\n");
}

void loop(void) {

  //reading touch sensor
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
 
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
 
  p.x = map(p.x, TS_MAXX, TS_MINX, tft.width(), 0);
  p.y = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);

  if (currentpage == 0){//START SCREEN

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
/*
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z);
*/
      if (p.x > 50 && p.x < 250 && p.y > 20 && p.y < 200){
        tft.fillScreen(BLACK);
        focal_length_screen(); //LOAD FOCAL LENGTH SCREEN
        currentpage = 1; //GO TO FOCAL LENGTH SCREEN
        Serial.println("FOCAL LENGTH SCREEN");
        delay (600);
      }
    }
  }

  if (currentpage == 1){//FOCAL LENGTH SCREEN
    delay(100);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
      
      if (p.x > plus_1_x && p.x < plus_2_x && p.y > plus_1_y && p.y < plus_2_y){
        //Serial.println("Plus");
        flength++;
      }  
      if (p.x > minus_1_x && p.x < minus_2_x && p.y > minus_1_y && p.y < minus_2_y){
        //Serial.println("Minus");
        flength--;
      }

      if (flength >= 30){
        flength = 30;  
      }
      if (flength <= 22){
        flength = 22;  
      }
      num_cover();
      tft.setCursor(150, 130);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println(flength);
      mm();
      
      if (p.x > back_1_x && p.x < back_2_x && p.y > back_1_y && p.y < back_2_y){//RETURN TO START SCREEN
  
        tft.fillScreen(BLACK);
        start_button();
        flength = 22;
        currentpage = 0; //BACK TO START SCREEN
        Serial.println("START SCREEN\n");
      }
      if (p.x > conf_1_x && p.x < conf_2_x && p.y > conf_1_y && p.y < conf_2_y){//GO TO DISTANCE FROM CENTRE SCREEN
        save_flength = flength;//SAVED FOCAL LENGTH VARIABLE
        Serial.print("The saved focal length is ");
        Serial.println(save_flength);
        tft.fillScreen(BLACK);
        p.x = 0;
        p.y = 0;
        distance_from_centre();
        currentpage = 2; //GO TO DISTANCE FROM SCREEN
        Serial.println("\nDISTANCE FROM CENTRE SCREEN");
        delay (600);
      }
    }
  }


  if (currentpage == 2){//DISTANCE FROM CENTRE SCREEN
   
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
      if (p.x > plus_1_x && p.x < plus_2_x && p.y > plus_1_y && p.y < plus_2_y){
        d_centre += 0.1;
      }  
      if (p.x > minus_1_x && p.x < minus_2_x && p.y > minus_1_y && p.y < minus_2_y){
        d_centre -= 0.1;
      }
      if (d_centre >= 6){//MAX DISTANCE FROM CENTRE
        d_centre = 6;  
      }
      if (d_centre <= 3){//MIN DISTANCE FROM CENTRE
        d_centre = 3;  
      }
      num_cover();
      tft.setCursor(120, 130);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println(d_centre);
      cm();

      if (p.x > back_1_x && p.x < back_2_x && p.y > back_1_y && p.y < back_2_y){//RETURN TO FOCAL LENGTH SCREEN 
        tft.fillScreen(BLACK);
        focal_length_screen();
        d_centre = 3;
        currentpage = 1; //BACK TO FOCAL LENGTH SCREEN
        Serial.println("\nFOCAL LENGTH SCREEN");
      }
      if (p.x > conf_1_x && p.x < conf_2_x && p.y > conf_1_y && p.y < conf_2_y){//GO TO HEIGHT OF OBJECT SCREEN
        save_d_centre = d_centre;//SAVED DISTANCE FROM CENTRE
        Serial.print("The saved distance from centre is ");
        Serial.println(save_d_centre);
        p.x = 0;
        p.y = 0;
        height_of_object();
        currentpage = 3; //GO TO HEIGHT OF OBJECT SCREEN
        Serial.println("\nHEIGHT OF OBJECT SCREEN SCREEN");
        delay (600);
      }
    }
  }



  if (currentpage == 3){//HEIGHT OF OBJECT SCREEN
   
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
      if (p.x > plus_1_x && p.x < plus_2_x && p.y > plus_1_y && p.y < plus_2_y){
        h_object += 0.1;
      }  
      if (p.x > minus_1_x && p.x < minus_2_x && p.y > minus_1_y && p.y < minus_2_y){
        h_object -= 0.1;
      }
      if (h_object >= 25){//MAX HEIGHT OF OBJECT
        h_object = 25;  
      }
      if (h_object <= 5){//MIN HEIGHT OF OBJECT
        h_object = 5;  
      }
      num_cover();
      tft.setCursor(120, 130);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println(h_object);
      cm();

      if (p.x > back_1_x && p.x < back_2_x && p.y > back_1_y && p.y < back_2_y){//RETURN TO DISTANCE FROM CENTRE SCREEN 
        tft.fillScreen(BLACK);
        distance_from_centre();
        h_object = 5;
        currentpage = 2; //BACK TO DISTANCE FROM CENTRE SCREEN
        Serial.println("\nDISTANCE FROM CENTRE SCREEN");
      }
      if (p.x > conf_1_x && p.x < conf_2_x && p.y > conf_1_y && p.y < conf_2_y){//GO TO DIAMETER OF OBJECT SCREEN
        save_h_object = h_object;//SAVED DISTANCE FROM CENTRE
        Serial.print("The height of object is ");
        Serial.println(save_h_object);
        p.x = 0;
        p.y = 0;
        diameter_of_object();
        currentpage = 4; //GO TO DIAMETER OF OBJECT
        Serial.println("\nDIAMETER OF OBJECT SCREEN");
        delay (600);
      }
    }
  }



  if (currentpage == 4){//DIAMETER OF OBJECT SCREEN 
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {  
      if (p.x > plus_1_x && p.x < plus_2_x && p.y > plus_1_y && p.y < plus_2_y){
        d_object += 0.1;
      }  
      if (p.x > minus_1_x && p.x < minus_2_x && p.y > minus_1_y && p.y < minus_2_y){
        d_object -= 0.1;
      }
      if (d_object >= 20){//MAX DIAMETER OF OBJECT
        d_object = 20;  
      }
      if (d_object <= 0){//MIN DIAMETER OF OBJECT
        d_object = 0;  
      }
      num_cover();
      tft.setCursor(120, 130);
      tft.setTextColor(WHITE);
      tft.setTextSize(3);
      tft.println(d_object);
      cm();

      if (p.x > back_1_x && p.x < back_2_x && p.y > back_1_y && p.y < back_2_y){//RETURN TO HEIGHT OF OBJECT SCREEN 
        tft.fillScreen(BLACK);
        height_of_object();
        d_object = 0;
        currentpage = 3; //BACK TO HEIGHT OF OBJECT SCREEN
        Serial.println("\nHEIGHT OF OBJECT SCREEN");
      }
      if (p.x > conf_1_x && p.x < conf_2_x && p.y > conf_1_y && p.y < conf_2_y){//GO TO DIAMETER OF OBJECT SCREEN
        save_d_object =d_object;//SAVED DISTANCE FROM CENTRE
        Serial.print("The diameter of object is ");
        Serial.println(save_d_object);
        p.x = 0;
        p.y = 0;
        start_scanning();
        currentpage = 5; //START SCANNING
        Serial.println("\nSTART SCANNING");
        delay (600);

        calc_AOV();
      }
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
    tft.fillRect(0, 0, 46, 46, WHITE);
    tft.fillRect(2, 2, 42, 42, SILVER);
    tft.fillTriangle(8, 22, 16, 14, 16, 30, WHITE);
    tft.fillRect(16, 20, 18, 6, WHITE);
   
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
void confirm_button(){
    tft.fillRect(274, 0, 46, 46, WHITE);
    tft.fillRect(276, 2, 42, 42, GREEN);
    tft.fillTriangle(284, 26, 287, 23, 294, 34, WHITE);
    tft.fillTriangle(297, 29, 288, 22, 294, 34, WHITE);
    tft.fillTriangle(298, 30, 308, 12, 311, 15, WHITE);
    tft.fillTriangle(298, 30, 308, 12, 295, 27, WHITE);
   
}
void num_cover(){
  tft.fillRect(65, 115, 200, 60, BLACK);
}
void start_button(){
  tft.fillScreen(BLACK);
  tft.fillRoundRect(50, 50, 220, 140, 20, BLUE);
  tft.drawRoundRect(50, 50, 220, 140, 20, WHITE);
 
  tft.setCursor(90, 105);
  tft.setTextColor(GOLD);
  tft.setTextSize(5);
  tft.println("START");
  return;
}

void mm(){
    tft.setCursor(220, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println("mm");
    return;  
}

void cm(){
    tft.setCursor(220, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println("cm");
    return;  
}
void focal_length_screen(){

    tft.fillScreen(BLACK);
    plus_minus();
    please_enter();
    back_button();
    confirm_button();
   
    tft.fillRoundRect(70, 50, 180, 60, 20, SILVER);
    tft.drawRoundRect(70, 50, 180, 60, 20, WHITE);
    tft.setCursor(89, 74);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("FOCAL LENGTH");

    num_cover();
    tft.setCursor(150, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println(flength);
    mm();
    
    return;
}
void distance_from_centre(){

    tft.fillScreen(BLACK);
    plus_minus();
    please_enter();
    back_button();
    confirm_button();

    
    tft.fillRoundRect(6, 50, 308, 60, 16, SILVER);
    tft.drawRoundRect(6, 50, 308, 60, 16, WHITE);
    tft.setCursor(18, 64);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("DISTANCE FROM THE CENTRE");
    tft.setCursor(10, 82);
    tft.println(" OF YOUR PHONE TO CAMERA");

    num_cover();
    tft.setCursor(120, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println(d_centre);
    cm();
    return;
}

void height_of_object(){
    
    tft.fillScreen(BLACK);
    plus_minus();
    please_enter();
    back_button();
    confirm_button();
    
    tft.fillRoundRect(16, 50, 288, 60, 16, SILVER);
    tft.drawRoundRect(16, 50, 288, 60, 16, WHITE);
    tft.setCursor(40, 74);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("HEIGHT OF THE OBJECT");

    num_cover();
    tft.setCursor(120, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println(h_object);
    cm();
    return;
}

void diameter_of_object(){
    
    tft.fillScreen(BLACK);
    plus_minus();
    please_enter();
    back_button();
    confirm_button();
    
    tft.fillRoundRect(6, 50, 308, 60, 16, SILVER);
    tft.drawRoundRect(6, 50, 308, 60, 16, WHITE);
    tft.setCursor(28, 74);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("DIAMETER OF THE OBJECT");

    num_cover();
    tft.setCursor(120, 130);
    tft.setTextColor(WHITE);
    tft.setTextSize(3);
    tft.println(d_object);
    cm();
    return;
}

void start_scanning(){
    int i;
    int box = 20;
    
    tft.fillScreen(BLACK);
    tft.setCursor(40, 40);
    tft.setTextColor(WHITE);
    tft.setTextSize(5);
    tft.println("SCANNING");

    for(i=2; i <= 6; i+=2){
      delay(800);
      tft.fillRect(70+box*i, 100, box, box, WHITE);

      if(i == 6){
        delay(800);
        i = 0;
        tft.fillRect(100, 90, 180, 40, BLACK);
      }
    }
    return;
}

void calc_AOV(){

  int focal_length;
  focal_length = save_flength;
  
  AOV_width = 2*atan(sensor_width/(2*focal_length));
  AOV_height = 2*atan(sensor_height/(2*focal_length));
}

float calc_minimum_distance(double view_angle){
  double minimum_distance;  
  double min_distance_width;
  double min_distance_height;
  double alpha; 
  double beta;
  double gamma;
  double length_AC;
  double length_AD;
  double length_DC;
  double length_AE;

  double object_diameter;
  double object_height;
  double distance_camera;

  object_diameter = save_d_object;
  object_height = save_h_object;
  distance_camera = save_d_centre;

  alpha = AOV_height/2;
  beta = view_angle;
  length_AC = (object_height/2)/cos(beta);
  length_AD = sqrt(pow(object_height/2,2) + pow(object_diameter/2,2));
  gamma = (PI/2)-atan(object_height/object_diameter) - beta;
  length_DC = sqrt(pow(length_AD,2) + pow(length_AC,2)-2*(length_AD)*(length_AC)*cos(gamma));

  min_distance_height = (length_AC + sin(alpha+beta)/((sin(PI/2-alpha))/length_DC))/tan(alpha); 
  min_distance_width = object_diameter/(2*tan(AOV_width/2));

  if (min_distance_height<min_distance_width){
    minimum_distance = min_distance_width;
  }
  else{
    minimum_distance = min_distance_height;
  }

  Serial.print("Minimum distance is: ");
  Serial.println(minimum_distance);
  return minimum_distance;
  
}

