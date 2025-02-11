// SPDX-License-Identifier: MIT
#ifndef LEDS_H
#define LEDS_H

#include "Adafruit_NeoMatrix.h"
#include "Adafruit_NeoPixel.h"
#include "JumperlessDefinesRP2040.h"
#include "NetsToChipConnections.h"
#include "RotaryEncoder.h"
#include <Arduino.h>
//#include <FastLED.h>

#define LED_PIN 17
#define LED_PIN_TOP 3
#define PROBE_LED_PIN 2

#define LED_COUNT 300
#define LED_COUNT_TOP 145
extern bool splitLEDs;

#define DEFAULTBRIGHTNESS 10
#define DEFAULTRAILBRIGHTNESS 15
#define DEFAULTSPECIALNETBRIGHTNESS 20

extern volatile bool core2busy;
// #define PCBEXTINCTION 0 //extra brightness for to offset the extinction
// through pcb
//this stuff is unused
#define PCBEXTINCTION                                                          \
  30 // extra brightness for to offset the extinction through pcb
#define PCBREDSHIFTPINK                                                        \
  -18 // extra hue shift to offset the hue shift through pcb
#define PCBGREENSHIFTPINK -25
#define PCBBLUESHIFTPINK 35

#define PCBREDSHIFTBLUE                                                        \
  -25 // extra hue shift to offset the hue shift through pcb
#define PCBGREENSHIFTBLUE -25
#define PCBBLUESHIFTBLUE 42

extern volatile uint8_t LEDbrightnessRail;
extern volatile uint8_t LEDbrightness;
extern volatile uint8_t LEDbrightnessSpecial;

extern volatile uint8_t pauseCore2;
extern Adafruit_NeoPixel bbleds;
extern Adafruit_NeoPixel probeLEDs;
extern uint8_t probeLEDstateMachine;



class ledClass { //I'm literally copying this from Adafruit_NeoPixel.h so I can split leds.show() into 2 strips without modifying the library 
  public:
  void begin(void);
  void show(void);
  void setPin(int16_t p);
  void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
  void setPixelColor(uint16_t n, uint32_t c);
  uint32_t getPixelColor(uint16_t n);
  uint16_t numPixels(void);
  void fill(uint32_t c = 0, uint16_t first = 0, uint16_t count = 0);
  void setBrightness(uint8_t);
  void clear(void);

  //void clear(int start = 0, int end = LED_COUNT+LED_COUNT_TOP);

};
  


extern ledClass leds;

//extern CRGB probeLEDs[1];
// extern Adafruit_NeoMatrix matrix;
//extern bool debugLEDs;

extern int brightenedNet;
extern int brightenedRail;
extern int brightenedAmount;
extern bool lightUpName;

extern int netColorMode; // 0 = rainbow, 1 = shuffle
extern int displayMode;
extern int numberOfShownNets;
//extern int showLEDsCore2;
extern int logoFlash;

typedef struct rgbColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} rgbColor;

typedef struct hsvColor {
  unsigned char h;
  unsigned char s;
  unsigned char v;
} hsvColor;

const uint8_t jumperlessText[301] = {
    1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1,
    0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1,
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0,

    1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1,
    0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
    0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1,

    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1,

    1, 0, 0, 0, 1, 1,
};

const int textMap[301] = {
    0,   30,  60,  90,  120, 1,   31,  61,  91,  121, 2,   32,  62,  92,  122,
    3,   33,  63,  93,  123, 4,   34,  64,  94,  124, 5,   35,  65,  95,  125,
    6,   36,  66,  96,  126, 7,   37,  67,  97,  127, 8,   38,  68,  98,  128,
    9,   39,  69,  99,  129, 10,  40,  70,  100, 130, 11,  41,  71,  101, 131,
    12,  42,  72,  102, 132, 13,  43,  73,  103, 133, 14,  44,  74,  104, 134,
    15,  45,  75,  105, 135, 16,  46,  76,  106, 136, 17,  47,  77,  107, 137,
    18,  48,  78,  108, 138, 19,  49,  79,  109, 139, 20,  50,  80,  110, 140,
    21,  51,  81,  111, 141, 22,  52,  82,  112, 142, 23,  53,  83,  113, 143,
    24,  54,  84,  114, 144, 25,  55,  85,  115, 145, 26,  56,  86,  116, 146,
    27,  57,  87,  117, 147, 28,  58,  88,  118, 148, 29,  59,  89,  119, 149,
    150, 180, 210, 240, 270, 151, 181, 211, 241, 271, 152, 182, 212, 242, 272,
    153, 183, 213, 243, 273, 154, 184, 214, 244, 274, 155, 185, 215, 245, 275,
    156, 186, 216, 246, 276, 157, 187, 217, 247, 277, 158, 188, 218, 248, 278,
    159, 189, 219, 249, 279, 160, 190, 220, 250, 280, 161, 191, 221, 251, 281,
    162, 192, 222, 252, 282, 163, 193, 223, 253, 283, 164, 194, 224, 254, 284,
    165, 195, 225, 255, 285, 166, 196, 226, 256, 286, 167, 197, 227, 257, 287,
    168, 198, 228, 258, 288, 169, 199, 229, 259, 289, 170, 200, 230, 260, 290,
    171, 201, 231, 261, 291, 172, 202, 232, 262, 292, 173, 203, 233, 263, 293,
    174, 204, 234, 264, 294, 175, 205, 235, 265, 295, 176, 206, 236, 266, 296,
    177, 207, 237, 267, 297, 178, 208, 238, 268, 298, 179, 209, 239, 269, 299,

};

const int numbers[120] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,
    90,  91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119};


const int nodesToPixelMap[120] = {
    0,  0,  1,  2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
    14, 15, 16, 17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
    29, 30, 31, 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,
    44, 45, 46, 47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,
    59, 60, 61, 0,   0,   0,   0,   0,   0,   0,

    81, 80, 84, 85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  82,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, -1,
    -1, -1, -1, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,

};

#define LOGO_COLOR_LENGTH 60

extern uint32_t logoColors[LOGO_COLOR_LENGTH+11];
extern uint32_t logoColorsHot[LOGO_COLOR_LENGTH+1];
extern uint32_t logoColorsCold[LOGO_COLOR_LENGTH+1];
extern uint32_t logoColorsYellow[LOGO_COLOR_LENGTH+1];
extern uint32_t logoColorsPink[LOGO_COLOR_LENGTH+1];
extern uint32_t logoColors8vSelect[LOGO_COLOR_LENGTH+11];
extern uint32_t logoColorsAll[8][LOGO_COLOR_LENGTH + 11];


const int bbPixelToNodesMap[120] = {
    0,         1,          2,        3,        4,          5,         6,
    7,         8,          9,        10,       11,         12,        13,
    14,        15,         16,       17,       18,         19,        20,
    21,        22,         23,       24,       25,         26,        27,
    28,        29,         30,       32,       33,         34,        35,
    36,        37,         38,       39,       40,         41,        42,
    43,        44,         45,       46,       47,         48,        49,
    50,        51,         52,       53,       54,         55,        56,
    57,        58,         59,       60,       61,         0,         0,
    0,         0,          0,        0,        0,          0,         0,
    0,         0,          0,        0,        0,          0,         0,
    0,         0,          NANO_D1,  NANO_D0,  NANO_RESET_1, GND,       NANO_D2,
    NANO_D3,   NANO_D4,    NANO_D5,  NANO_D6,  NANO_D7,    NANO_D8,   NANO_D9,
    NANO_D10,  NANO_D11,   NANO_D12, NANO_D13, SUPPLY_3V3, NANO_AREF, NANO_A0,
    NANO_A1,   NANO_A2,    NANO_A3,  NANO_A4,  NANO_A5,    NANO_A6,   NANO_A7,
    SUPPLY_5V, NANO_RESET_0, GND,      SUPPLY_5V

};

const int bbPixelToNodesMapV5[35][2] = {
{NANO_D1, 400}, {NANO_D0, 401}, {NANO_RESET_1, 402}, {NANO_D2, 404}, {NANO_D3, 405},
{NANO_D4, 406}, {NANO_D5, 407}, {NANO_D6, 408}, {NANO_D7, 409}, {NANO_D8, 410}, 
{NANO_D9, 411}, {NANO_D10, 412}, {NANO_D11, 413}, {NANO_D12, 414}, {NANO_D13, 415},
{NANO_AREF, 417}, {NANO_A0, 418}, {NANO_A1, 419}, {NANO_A2, 420},
{NANO_A3, 421}, {NANO_A4, 422}, {NANO_A5, 423}, {NANO_A6, 424}, {NANO_A7, 425},
{NANO_RESET_0, 427}
};

extern uint32_t
    rawSpecialNetColors[8]; // = {0x000000, 0x001C04, 0x1C0702, 0x1C0107,
                            // 0x231111, 0x230913, 0x232323, 0x232323};

extern uint32_t rawOtherColors[15];

extern uint32_t rawRailColors[3][4];

const int railsToPixelMap[4][25] = {
    {300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312,
     313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324},
    {325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 337,
     338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349},
    {350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362,
     363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374},
    {375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387,
     388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399}};

// int nodeColors[MAX_PATHS] = {0};



const int pixelsToRails[20] = {B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG,
                               B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG,
                               B_RAIL_NEG, B_RAIL_POS, T_RAIL_POS, T_RAIL_NEG,
                               T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG,
                               T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG};

extern rgbColor netColors[MAX_NETS];
extern uint32_t savedLEDcolors[NUM_SLOTS][LED_COUNT + 1];
extern rgbColor specialNetColors[8];

void printColorName(uint32_t color);
void printColorName(int hue);

int brightenNet(int node, int addBrightness = 5);

struct rgbColor shiftHue(struct rgbColor colorToShift, int hueShift = 0,
                         int brightnessShift = 0, int saturationShift = 0,
                         int specialFunction = -1);
void initLEDs(void);
char LEDbrightnessMenu(void);
void refreshSavedColors(int slot = -1);
int saveRawColors(int slot = -1);
void showSavedColors(int slot = -1);
void clearLEDs(void);
void randomColors(uint32_t color, int wait);
void rainbowy(int, int, int wait);
void showNets(void);
void assignNetColors(int preview = 0);
void lightUpRail(int logo = -1, int railNumber = -1, int onOff = 1,
                 int brightness = -1,
                 int supplySwitchPosition = 0);
void setupSwirlColors(void);
void logoSwirl(int start = 0, int spread = 5, int probe = 0);
uint32_t dimLogoColor(uint32_t color, int brightness = 20);
void lightUpNet(int netNumber = 0, int node = -1, int onOff = 1,
                int brightness = DEFAULTBRIGHTNESS,
                int hueShift = 0, int dontClear = 0, uint32_t forceColor = 0xffffff); //-1 means all nodes (default)
void lightUpNode(int node, uint32_t color);
rgbColor pcbColorCorrect(rgbColor colorToCorrect);
hsvColor RgbToHsv(rgbColor rgb);
rgbColor HsvToRgb(hsvColor hsv);
void applyBrightness(int brightness);
rgbColor unpackRgb(uint32_t color);

//uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b); 
uint32_t packRgb(rgbColor color);

uint32_t scaleUpBrightness(uint32_t hexColor, int scaleFactor = 8,
                           int minBrightness = 25);
uint32_t scaleDownBrightness(uint32_t hexColor, int scaleFactor = 8,
                             int maxBrightness = 15);
void showSkippedNodes(uint32_t onColor = 0x0f1f2f, uint32_t offColor =  0x040007);
void clearLEDsExceptRails();

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b);
void startupColors(void);
void startupColorsV5(void);
void rainbowBounce(int wait, int logo = 0);
uint32_t scaleBrightness(uint32_t color, int scaleFactor);


void clearLEDsExceptMiddle(int start = 1, int end = 60);
void clearLEDsMiddle(int start = 1, int end = 60);
#endif
