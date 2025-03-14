#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;
volatile unsigned long timePointsOpen, timePointsClosed, lastChange;
volatile unsigned int numBangs;
float enginerpm;
float enginetemp;
int tempPin = 0;   //temp sensor
int oilPin = 1;    // oil sensor
int boostPin = 2;  //boost sensor
int egtPin = 3;    //egt sensor
float oilpressure;
float boostpressure;
float exhuasttemp;
// xpos = dist from left edge, ypos = dist down from top, radius = size
// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
#define RA8875_GREY 0x2104  // Dark grey 16 bit colour

void pointsOpening() {
  unsigned long t;
  t = millis();
  if (lastChange > 0) {
    timePointsClosed += (t - lastChange);
  }
  lastChange = t;
  ++numBangs;
}

void setup() {
  {
    Serial.begin(9600);
    pinMode(2, INPUT);  //set for digital rpm input
  }

  {
    tft.begin(RA8875_800x480);
    tft.displayOn(true);
    tft.GPIOX(true);                               // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);  // PWM output for backlight
    tft.PWM1out(255);
    tft.textMode();
    tft.drawCircle(400, 299, 152, RA8875_WHITE);
    tft.drawCircle(400, 299, 182, RA8875_WHITE);
    tft.drawCircle(120, 130, 82, RA8875_WHITE);
    tft.drawCircle(120, 130, 92, RA8875_WHITE);
    tft.drawCircle(120, 330, 82, RA8875_WHITE);
    tft.drawCircle(120, 330, 92, RA8875_WHITE);
    tft.drawCircle(680, 130, 82, RA8875_WHITE);
    tft.drawCircle(680, 130, 92, RA8875_WHITE);
    tft.drawCircle(680, 330, 82, RA8875_WHITE);
    tft.drawCircle(680, 330, 92, RA8875_WHITE);
  }
}
void displayvalue()  //Displays text values from inputs
{

  //tft.displayOn(true);
  //tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  //tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  //tft.PWM1out(255);
  //tft.textMode();
  char rpmstring[30];
  char Tempstring[30];
  char Oilstring[30];
  char Booststring[30];
  char Exhuaststring[30];
  dtostrf(enginetemp, 2, 0, Tempstring);      //from temp float
  dtostrf(oilpressure, 2, 0, Oilstring);      //from oil float
  dtostrf(boostpressure, 2, 0, Booststring);  //from boost float
  dtostrf(exhuasttemp, 2, 0, Exhuaststring);  // from egt float
  dtostrf(enginerpm, 3, 0, rpmstring);        //from temp floattft.textSetCursor(10, 10);

  //Display RPM text
  tft.textSetCursor(360, 320);
  tft.textColor(RA8875_WHITE, RA8875_BLACK);
  tft.textEnlarge(2);
  tft.textWrite("RPM");
  tft.textEnlarge(2);
  tft.textSetCursor(350, 370);
  tft.textWrite(rpmstring);

  //Display Oil pressure text
  tft.textEnlarge(1);
  tft.textSetCursor(102, 130);
  tft.textWrite(Oilstring);
  tft.textSetCursor(98, 158);
  tft.textWrite("Oil");

  //Display enine temp text
  tft.textEnlarge(1);
  tft.textSetCursor(102, 335);
  tft.textWrite(Tempstring);
  tft.textSetCursor(90, 363);
  tft.textWrite("Temp");

  //Display exhuast gas temp text
  tft.textEnlarge(1);
  tft.textSetCursor(655, 132);
  tft.textWrite(Exhuaststring);
  tft.textSetCursor(660, 160);
  tft.textWrite("EGT");

  //Display boost text
  tft.textEnlarge(1);
  tft.textSetCursor(658, 335);
  tft.textWrite(Booststring);
  tft.textSetCursor(648, 363);
  tft.textWrite("Boost");

  //Dial numbers
  tft.textEnlarge(1);
  tft.textSetCursor(350, 120);
  tft.textTransparent(RA8875_WHITE);
  tft.textWrite("3");
  tft.textSetCursor(270, 165);
  tft.textWrite("2");
  tft.textSetCursor(225, 270);
  tft.textWrite("1");
  tft.textSetCursor(460, 130);
  tft.textWrite("4");
  tft.textSetCursor(543, 210);
  tft.textWrite("5");
  tft.textSetCursor(552, 325);
  tft.textWrite("6");
  tft.textSetCursor(245, 360);
  tft.textWrite("0");
}

void loop()

// values for ((int) enginerpm/oil/boost/egt are derived from respective float values as displayed in texwrite, not from pin values
{
  // Set the the position, gap between meters, and inner radius of the meters
  int xpos = 0, ypos = 0, gap = 4, radius = 0;

  // Draw engine rpm
  xpos = 250, ypos = 150, gap = 50, radius = 150;  //sets size and position on screen

  xpos = gap + ringMeter((int)enginerpm, 0, 6500, xpos, ypos, radius, 1, RA8875_RED);  //0, 5000 are range of meter ie 0-5000rpm

  // Draw oil pressure
  xpos = 40, ypos = 50, gap = 50, radius = 80;  //sets size and position on screen

  oilpressure = analogRead(oilPin);
  oilpressure = (5.0 * oilpressure * 100.0) / 1024.0 / 5.8;
  displayvalue();
  xpos = gap + ringMeter1((int)oilpressure, 0, 50, xpos, ypos, radius, 1, RED2GREEN);  //0, 50 are range off meter ie 0-50 oilPSI

  // Draw enginetemp
  xpos = 40, ypos = 250, gap = 50, radius = 80;  //sets size and position on screen

  enginetemp = analogRead(tempPin);
  enginetemp = (5.0 * enginetemp * 100.0) / 1024.0;
  displayvalue();
  xpos = gap + ringMeter1((int)enginetemp, 0, 120, xpos, ypos, radius, 1, BLUE2RED);  //0, 120 are range off meter ie 0-120*c

  //Draw EGT
  xpos = 600, ypos = 50, gap = 50, radius = 80;  //sets size and position on screen

  exhuasttemp = analogRead(egtPin);
  exhuasttemp = (5.0 * exhuasttemp * 100.0) / 1024.0 * 2;
  displayvalue();
  xpos = gap + ringMeter1((int)exhuasttemp, 0, 750, xpos, ypos, radius, 1, BLUE2RED);  //0, 750 are range off meter ie 0-750*c EGT Temp

  // Draw boost
  xpos = 600, ypos = 250, gap = 50, radius = 80;  //sets size and position on screen

  boostpressure = analogRead(boostPin);
  boostpressure = (5.0 * boostpressure * 100.0) / 1024 / 14;
  displayvalue();
  xpos = gap + ringMeter1((int)boostpressure, 0, 30, xpos, ypos, radius, 1, GREEN2GREEN);  //0, 30 are range off meter ie 0-30psi boost*c

  // Number of coil pulses for RPM
  numBangs = 0;
  attachInterrupt(0, pointsOpening, RISING);
  delay(500);
  detachInterrupt(0);
  numBangs = ((6000 / 50) * numBangs) / 6;  // /2 = 2 cylinder 6 =6 cylinder
  Serial.print("RPM: ");
  Serial.println(numBangs);
  enginerpm = numBangs;

  //EGT alarm

  if (exhuasttemp > 650) {
    tft.fillCircle(520, 60, 50, RA8875_RED);

    tft.textEnlarge(2);
    tft.textSetCursor(485, 35);
    tft.textTransparent(RA8875_WHITE);
    tft.textWrite("EGT");
  }
  delay(500);
  if (exhuasttemp > 650) {
    tft.fillCircle(520, 60, 50, RA8875_BLACK);
  }

  //Oil alarm
  if (oilpressure < 10) {
    tft.fillCircle(400, 60, 50, RA8875_RED);

    tft.textEnlarge(2);
    tft.textSetCursor(365, 35);
    tft.textTransparent(RA8875_WHITE);
    tft.textWrite("Oil");
  }

  delay(500);
  if (oilpressure < 10) {
    tft.fillCircle(400, 60, 50, RA8875_BLACK);
  } else {
    tft.fillCircle(400, 60, 50, RA8875_BLACK);
  }

  //Temp alarm
  if (enginetemp > 30) {
    tft.fillCircle(280, 60, 50, RA8875_RED);
    tft.textEnlarge(2);
    tft.textSetCursor(232, 35);
    tft.textTransparent(RA8875_WHITE);
    tft.textWrite("Temp");
  }

  delay(500);
  if (enginetemp > 30) {
    tft.fillCircle(280, 60, 50, RA8875_BLACK);
  }

  else {
    tft.fillCircle(280, 60, 50, RA8875_BLACK);
  }
  displayvalue();
}

// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
// int ringmeter for RPM
int ringMeter(int temp, int vmin, int vmax, int x, int y, int r, int thick, byte scheme) {
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option

  x += r;
  y += r;  // Calculate coords of centre of ring

  int w = r / thick;  // Width of outer ring is 1/4 of radius

  int angle = 120;  // Half the sweep angle of meter (300 degrees)

  int text_colour = 0;  // To hold the text colour

  int v = map(temp, vmin, vmax, -angle, angle);  // Map the value to an angle v

  byte seg = 10;  // Segments are 5 degrees wide = 60 segments for 300 degrees
  byte inc = 10;  // Draw segments every 5 degrees, increase to 10 for segmented ring

  // Draw colour blocks every inc degrees
  for (int i = -angle; i < angle; i += inc) {

    // Choose colour from scheme
    int colour = 0;
    switch (scheme) {

      case 0: colour = RA8875_RED; break;                               // Fixed colour
      case 1: colour = RA8875_GREEN; break;                             // Fixed colour
      case 2: colour = RA8875_BLUE; break;                              // Fixed colour
      case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break;   // Full spectrum blue to red
      case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break;  // Green to red (high temperature etc)
      case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break;  // Red to green (low battery etc)

      default: colour = RA8875_BLUE; break;  // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i >= (v - inc) && (i < v)) {  // Fill in coloured segments with 2 triangles
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      text_colour = colour;  // Save the last colour drawn
    } else                   // Fill in blank segments
    {

      tft.fillTriangle(x0, y0, x1, y1, x2, y2, RA8875_BLACK);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, RA8875_BLACK);
    }
  }

  // Convert value to a string
  char buf[10];
  byte len = 4;
  if (temp > 999) len = 5;
  dtostrf(temp, len, 0, buf);

  // Set the text colour to default

  tft.setTextColor(RA8875_WHITE, RA8875_BLACK);
  // Uncomment next line to set the text colour to the last segment value!
  tft.textColor(text_colour, RA8875_BLACK);

  // Calculate and return right hand side x coordinate
  return x + r;
}

//second ringmeter int for small dials

int ringMeter1(int temp, int vmin, int vmax, int x, int y, int r, const char *units, byte scheme) {
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option

  x += r;
  y += r;  // Calculate coords of centre of ring

  int w = r / 4;  // Width of outer ring is 1/4 of radius

  int angle = 120;  // Half the sweep angle of meter (300 degrees)

  int text_colour = 0;  // To hold the text colour

  int v = map(temp, vmin, vmax, -angle, angle);  // Map the value to an angle v

  byte seg = 5;  // Segments are 5 degrees wide = 60 segments for 300 degrees
  byte inc = 5;  // Draw segments every 5 degrees, increase to 10 for segmented ring

  // Draw colour blocks every inc degrees
  for (int i = -angle; i < angle; i += inc) {

    // Choose colour from scheme
    int colour = 0;
    switch (scheme) {

      case 0: colour = RA8875_RED; break;                               // Fixed colour
      case 1: colour = RA8875_GREEN; break;                             // Fixed colour
      case 2: colour = RA8875_BLUE; break;                              // Fixed colour
      case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break;   // Full spectrum blue to red
      case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break;  // Green to red (high temperature etc)
      case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break;  // Red to green (low battery etc)

      default: colour = RA8875_BLUE; break;  // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i < v) {  // Fill in coloured segments with 2 triangles
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      text_colour = colour;  // Save the last colour drawn
    } else                   // Fill in blank segments
    {

      tft.fillTriangle(x0, y0, x1, y1, x2, y2, RA8875_GREY);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, RA8875_GREY);
    }
  }

  // Convert value to a string
  char buf[10];
  byte len = 4;
  if (temp > 999) len = 5;
  dtostrf(temp, len, 0, buf);
}
// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value) {
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  byte red = 0;    // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;  // Green is the middle 6 bits
  byte blue = 0;   // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}
