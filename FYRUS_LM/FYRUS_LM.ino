#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> /* define your oled size in the library */
#include <BH1750.h>
#include <Adafruit_TCS34725.h>
#include <EEPROM.h>
#include <avr/sleep.h>

/* 
 * 7 Pin and 4 Pin Oled option. Enable by removing comment SLASH
 */
/*/////////////////////////////////////////////////////////////*/
/* 7 Pin Oled Display
 *  
 *  VCC -> 5V
 *  GND -> GND
 *  the rest of the pin connect as below
*/
//#define OLED_CLK                8   // 10
//#define OLED_MOSI               9   // 9
//#define OLED_RESET              10  // 13
//#define OLED_DC                 11
//#define OLED_CS                 12
//Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/*///////////////////////////////////////////////////////////////*/
/* 4 Pin Oled display
 *
 * VCC -> 3.3V
 * GND -> GND
 * SDA -> pin A4
 * SCL -> pin A5
*/
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/*///////////////////////////////////////////////////////////////*/

BH1750 lightMeter;

#define DomeMultiplier          2.17      // Multiplier when using a white translucid Dome covering the sensor, EDIT THIS VALUE
#define MeteringButtonPin       2         // Metering button pin
#define PlusButtonPin           3         // Plus button pin
#define MinusButtonPin          4         // Minus button pin
#define ModeButtonPin           5         // Priority Mode button pin
#define MenuButtonPin           6         // Menu button pin
#define MeteringModeButtonPin   7         // Metering Mode (Ambient / Flash)

#define MaxISOIndex             57
#define MaxApertureIndex        27        // max 70 (default f1.0 ~ f22.0) editable
#define MaxTimeIndex            80
#define MaxNDIndex              8         // max 13 (default ND2 to ND256) editable
#define MaxFlashMeteringTime    3000      // flash metering in milisecond (default 3 seconds)

/* CONSTANT FOR WHITE BALANCE /////// */
#define TCS34725_R_Coef         0.136
#define TCS34725_G_Coef         1.000
#define TCS34725_B_Coef         -0.444
#define TCS34725_GA             1.0
#define TCS34725_DF             310.0
#define TCS34725_CT_Coef        3810.0
#define TCS34725_CT_Offset      1391.0
//#define C_const 129

static const unsigned char PROGMEM FLM [] = {
0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF,
0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x06, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFE, 0x0E, 0x00, 0x00, 0x70, 0x03,
0xFE, 0x00, 0x00, 0x03, 0xFE, 0x0E, 0x00, 0x00, 0x70, 0x03, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x0C,
0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xDE, 0xFF, 0xFF, 0x7B, 0xF8, 0x00, 0xFC,
0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0x1F, 0xF0, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x00, 0x03, 0x1F, 0xF0,
0x00, 0x01, 0xFF, 0xFB, 0x80, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xF9, 0xE0, 0x00,
0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xF9, 0xF0, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFE, 0x13,
0xFF, 0xF9, 0xF8, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x3C, 0xFF, 0xFD, 0xFE, 0x00, 0x00, 0x03,
0x80, 0x00, 0x00, 0x7E, 0x3F, 0xFD, 0xFF, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0xFF, 0x9F, 0xFD,
0xFF, 0x00, 0x00, 0x03, 0x80, 0x00, 0x01, 0xFF, 0xC7, 0xFD, 0xFF, 0x80, 0x00, 0x03, 0x80, 0x00,
0x03, 0xFF, 0xF3, 0xFD, 0xFF, 0xC0, 0x00, 0x03, 0x80, 0x00, 0x03, 0xFF, 0xF8, 0xFC, 0xFF, 0xC0,
0x00, 0x03, 0x80, 0x00, 0x07, 0xFF, 0xFE, 0x7C, 0xFF, 0xE0, 0x00, 0x03, 0x80, 0x00, 0x07, 0xFF,
0xFE, 0x1C, 0xFF, 0xC0, 0x00, 0x03, 0x80, 0x00, 0x0F, 0xFF, 0xE0, 0x0C, 0xFF, 0x90, 0x00, 0x03,
0x80, 0x00, 0x0F, 0xFF, 0x00, 0x06, 0xFF, 0x30, 0x00, 0x03, 0x80, 0x00, 0x0F, 0xF8, 0x00, 0x00,
0xFE, 0x70, 0x00, 0x03, 0x80, 0x00, 0x0F, 0xC3, 0x80, 0x00, 0xFC, 0xF0, 0x00, 0x03, 0x80, 0x00,
0x1E, 0x1F, 0x00, 0x00, 0xF9, 0xF0, 0x00, 0x03, 0x80, 0x00, 0x10, 0xFF, 0x00, 0x00, 0xF3, 0xF8,
0x00, 0x03, 0x80, 0x00, 0x03, 0xFF, 0x00, 0x00, 0xE7, 0xF8, 0x00, 0x03, 0x80, 0x00, 0x1F, 0xFE,
0x00, 0x00, 0xCF, 0xF8, 0x00, 0x03, 0x80, 0x00, 0x1F, 0xFE, 0x00, 0x00, 0x9F, 0xF8, 0x00, 0x03,
0x80, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x3F, 0xF8, 0x00, 0x03, 0x80, 0x00, 0x1F, 0xFD, 0x00, 0x00,
0x7F, 0xF0, 0x00, 0x03, 0x80, 0x00, 0x1F, 0xFD, 0x80, 0x00, 0xFF, 0xF0, 0x00, 0x03, 0x80, 0x00,
0x0F, 0xF9, 0x80, 0x01, 0xFF, 0xF0, 0x00, 0x03, 0x80, 0x00, 0x0F, 0xFB, 0xC0, 0x03, 0xFF, 0xF0,
0x00, 0x03, 0x80, 0x00, 0x0F, 0xF3, 0xC0, 0x0F, 0xFF, 0xF0, 0x00, 0x03, 0x80, 0x00, 0x07, 0xF3,
0xE0, 0x03, 0xFF, 0xE0, 0x00, 0x03, 0x80, 0x00, 0x07, 0xF7, 0xF7, 0xE0, 0x01, 0xE0, 0x00, 0x03,
0x80, 0x00, 0x07, 0xE7, 0xF3, 0xFF, 0xE0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x03, 0xE7, 0xFB, 0xFF,
0xFF, 0xC0, 0x5C, 0x03, 0x80, 0x00, 0x01, 0xEF, 0xF9, 0xFF, 0xFF, 0x80, 0x3E, 0x03, 0x80, 0x00,
0x01, 0xCF, 0xFC, 0xFF, 0xFF, 0x80, 0x3E, 0x03, 0x80, 0x00, 0x00, 0xCF, 0xFE, 0xFF, 0xFF, 0x00,
0x3C, 0x03, 0x80, 0x00, 0x00, 0x5F, 0xFE, 0x7F, 0xFE, 0x00, 0x18, 0x03, 0x80, 0x00, 0x00, 0x1F,
0xFF, 0x3F, 0xFC, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x1F, 0xFF, 0x3F, 0xF8, 0x00, 0x00, 0x03,
0x80, 0x00, 0x00, 0x07, 0xFF, 0x9F, 0xE0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x01, 0xFF, 0xCF,
0x80, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x7F, 0xCE, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00,
0x00, 0x00, 0x03, 0xE0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0x80, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x3E, 0xCC, 0xF9,
0x99, 0xF0, 0x20, 0xE7, 0x80, 0x00, 0x00, 0x30, 0x58, 0x89, 0x9B, 0x10, 0x20, 0xE7, 0xFF, 0xFF,
0xFE, 0x30, 0x70, 0x99, 0x9B, 0x00, 0x20, 0xEF, 0xFF, 0xFF, 0xFE, 0x3C, 0x30, 0xF1, 0x99, 0xF0,
0x20, 0xBB, 0x00, 0x00, 0x00, 0x30, 0x30, 0xB1, 0x98, 0x10, 0x20, 0x9B, 0x00, 0x00, 0x00, 0x30,
0x30, 0x99, 0x9B, 0x30, 0x30, 0xDB, 0x00, 0x00, 0x00, 0x30, 0x30, 0x98, 0xF1, 0xE0, 0x3C, 0xD3 };

static const unsigned char PROGMEM battery_full [] =
{ 0x7F, 0xE0, 0x40, 0x20, 0x5F, 0xB8, 0x5F, 0xB8, 0x5F, 0xB8, 0x5F, 0xB8, 0x40, 0x20, 0x7F, 0xE0 };

static const unsigned char PROGMEM battery_threequarters [] =
{ 0x7F, 0xE0, 0x40, 0x20, 0x5C, 0x38, 0x5E, 0x38, 0x5E, 0x38, 0x5F, 0x38, 0x40, 0x20, 0x7F, 0xE0 };

static const unsigned char PROGMEM battery_half [] =
{ 0x7F, 0xE0, 0x40, 0x20, 0x58, 0x38, 0x58, 0x38, 0x5C, 0x38, 0x5C, 0x38, 0x40, 0x20, 0x7F, 0xE0 };

static const unsigned char PROGMEM battery_low [] =
{ 0x7F, 0xE0, 0x40, 0x20, 0x50, 0x38, 0x50, 0x38, 0x50, 0x38, 0x50, 0x38, 0x40, 0x20, 0x7F, 0xE0 };

static const unsigned char PROGMEM battery_empty [] =
{ 0x7F, 0xE0, 0x40, 0x20, 0x40, 0x38, 0x40, 0x38, 0x40, 0x38, 0x40, 0x38, 0x40, 0x20, 0x7F, 0xE0 };


float   lux;
boolean Overflow = 0;                   // Sensor got Saturated and Display "Overflow"
float   ISOND;
boolean ISOmode = 0;
boolean NDmode = 0;

boolean PlusButtonState;                // "+" button state
boolean MinusButtonState;               // "-" button state
boolean MeteringButtonState;            // Metering button state
boolean ModeButtonState;                // Mode button state
boolean MenuButtonState;                // ISO button state
boolean MeteringModeButtonState;        // Metering mode button state (Ambient / Flash)

boolean ISOMenu = false;
boolean NDMenu = false;
boolean mainScreen = false;

// EEPROM for memory recording
#define ISOIndexAddr         1
#define apertureIndexAddr    2
#define modeIndexAddr        3
#define T_expIndexAddr       4
#define meteringModeAddr     5
#define ndIndexAddr          6

#define defaultApertureIndex 12
#define defaultISOIndex      11
#define defaultModeIndex     0
#define defaultT_expIndex    19

uint8_t ISOIndex =          EEPROM.read(ISOIndexAddr);
uint8_t apertureIndex =     EEPROM.read(apertureIndexAddr);
uint8_t T_expIndex =        EEPROM.read(T_expIndexAddr);
uint8_t modeIndex =         EEPROM.read(modeIndexAddr);
uint8_t meteringMode =      EEPROM.read(meteringModeAddr);
uint8_t ndIndex =           EEPROM.read(ndIndexAddr);

int value = 0;
float voltage;
float perc;


/* TCS34724 RGB SENSOR /////// */
class ColorSensor {
  
    public:
  
    void getColorTemp();
    uint16_t r, g, b, c, ir;
    uint16_t ct, lux, r_comp, g_comp, b_comp;
    float cpl;
} rgb_sensor;

void ColorSensor::getColorTemp() {

    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);
  
    tcs.getRawData(&r, &g, &b, &c);
    //lux = tcs.calculateLux(r, g, b);
    // DN40 calculations
    ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
    r_comp = r - ir;
    g_comp = g - ir;
    b_comp = b - ir;
    //c_comp = c - ir;
  
    cpl = (700) / (TCS34725_GA * TCS34725_DF);
    lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
    ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;
    //ct = ct*DomeSum;
}

/////// END OF WB SENSOR SETUP

#include "lightmeter.h"

void setup() {  
  pinMode(PlusButtonPin, INPUT_PULLUP);
  pinMode(MinusButtonPin, INPUT_PULLUP);
  pinMode(MeteringButtonPin, INPUT_PULLUP);
  pinMode(ModeButtonPin, INPUT_PULLUP);
  pinMode(MenuButtonPin, INPUT_PULLUP);
  pinMode(MeteringModeButtonPin, INPUT_PULLUP);

  //Serial.begin(115200);

  Wire.begin();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE_2);
  //lightMeter.begin(BH1750::ONE_TIME_LOW_RES_MODE); // for low resolution but 16ms light measurement time.

/* 7 pin Oled */
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

/* 4 pin Oled */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 

  display.setTextColor(WHITE);
  display.clearDisplay();

/////// SPLASH SCREEN

  display.drawBitmap(24, 0, FLM, 80, 64, 1);
  display.display();
  display.clearDisplay();
  delay(3000);
  display.setTextSize(1);
  
  display.setTextColor(WHITE);
  display.setCursor(30, 14); //(30 ,32);
  display.println("LIGHT METER");
  display.setCursor(22 ,32);
  display.println("Ambient, Flash");
  display.setCursor(14 ,42);
  display.println("and White Balance");

  display.display();
  display.clearDisplay();
  delay(2000);

/////// SPLASH END

  // IF NO MEMORY WAS RECORDED BEFORE, START WITH THIS VALUES otherwise it will read "255"
  if (apertureIndex > MaxApertureIndex) {
    apertureIndex = defaultApertureIndex;
  }

  if (ISOIndex > MaxISOIndex) {
    ISOIndex = defaultISOIndex;
  }

  if (T_expIndex > MaxTimeIndex) {
    T_expIndex = defaultT_expIndex;
  }

  if (modeIndex < 0 || modeIndex > 1) {
    // Aperture priority. Calculating shutter speed.
    modeIndex = 0;
  }

  if (meteringMode > 1) {
    meteringMode = 0;
  }

  if (ndIndex > MaxNDIndex) {
    ndIndex = 0;
  }

  lux = getLux();            // get Lux reading from BH1750
  rgb_sensor.getColorTemp(); // get White Balance Value from TCS34725
  refresh();
}

void loop() {
  
  value = analogRead(A0);
  voltage = value * 5.0/1023;
  perc = map(voltage, 3.6, 4.2, 0, 100);
  
  readButtons();

  menu();

  if (MeteringButtonState == 0) {
    // Save setting if Metering button pressed.
    SaveSettings();

    lux = 0;
    refresh();
    
    if (meteringMode == 0) {
      // Ambient light meter mode.
      lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE_2);

      lux = getLux();
      rgb_sensor.getColorTemp();

      if (Overflow == 1) {
        delay(10);
        getLux();
      }

      refresh();
      delay(200);
    } else if (meteringMode == 1) {
      // Flash light metering
      lightMeter.configure(BH1750::CONTINUOUS_LOW_RES_MODE);

      unsigned long startTime = millis();
      uint16_t currentLux = 0;
      lux = 0;

      while (true) {
        // check max flash metering time
        if (startTime + MaxFlashMeteringTime < millis()) {
          break;
        }

        currentLux = getLux();
        delay(16);
        
        if (currentLux > lux) {
          lux = currentLux;
        }
      }

      refresh();
    }
  }
}
