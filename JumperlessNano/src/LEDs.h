// SPDX-License-Identifier: MIT
#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "Adafruit_NeoPixel.h"
#include "NetsToChipConnections.h"
#include "RotaryEncoder.h"

#define LED_PIN 25 //change this to 0 if you want to run this on a Pico to control the lights on a Jumperlux
#define LED_COUNT 111
#define DEFAULTBRIGHTNESS 32
#define DEFAULTRAILBRIGHTNESS 28
#define DEFAULTSPECIALNETBRIGHTNESS 35

// #define PCBEXTINCTION 0 //extra brightness for to offset the extinction through pcb



#define PCBEXTINCTION 30 //extra brightness for to offset the extinction through pcb
#define PCBREDSHIFTPINK -18    //extra hue shift to offset the hue shift through pcb
#define PCBGREENSHIFTPINK -25
#define PCBBLUESHIFTPINK 35

 #define PCBREDSHIFTBLUE -25    //extra hue shift to offset the hue shift through pcb
 #define PCBGREENSHIFTBLUE -25
 #define PCBBLUESHIFTBLUE 42

// #define PCBEXTINCTION 0 //extra brightness for to offset the extinction through pcb
//  #define PCBREDSHIFTBLUE 0    //extra hue shift to offset the hue shift through pcb
//  #define PCBGREENSHIFTBLUE 0
//  #define PCBBLUESHIFTBLUE 0


// #define PCBEXTINCTION 0 //extra brightness for to offset the extinction through pcb
// #define PCBREDSHIFTPINK 0    //extra hue shift to offset the hue shift through pcb
// #define PCBGREENSHIFTPINK 0
// #define PCBBLUESHIFTPINK 0
// #define PCBHUESHIFT 0

    extern volatile uint8_t LEDbrightnessRail;
    extern volatile uint8_t LEDbrightness;
    extern volatile uint8_t LEDbrightnessSpecial;

    extern volatile uint8_t pauseCore2;

extern Adafruit_NeoPixel leds;
extern bool debugLEDs;

extern int showLEDsCore2;
extern int logoFlash;


typedef struct rgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgbColor;



typedef struct hsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} hsvColor;

const int numbers[120]         = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
                                    31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,
                                    61,62,63,64,65,66,67,68,69,
                                    70,71,72,73,74,75,76,77,78,79,
                                    80,81,82,83,84,85,86,87,88,89,
                                    90,91,92,93,94,95,96,97,98,99,
                                    100,101,102,103,104,105,106,107,108,109,
                                    110,111,112,113,114,115,116,117,118,119};


const int nodesToPixelMap[120] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
                                    30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
                                    0,0,0,0,0,0,0,

                                    81,80,84,85,86,87,88,89,90,91,92,93,94,
                                    95,82,97,98,99,100,101,102,103,104,105,106,107,108,109,110,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

                                    };

const int  bbPixelToNodesMap[120] = { 0,1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
                                    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,
                                    0,0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,
                                    NANO_D1, NANO_D0, NANO_RESET, GND, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12,
                                    NANO_D13, SUPPLY_3V3, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7, SUPPLY_5V, NANO_RESET, GND, SUPPLY_5V
                                    
                                    };

extern uint32_t rawSpecialNetColors[8];// = {0x000000, 0x001C04, 0x1C0702, 0x1C0107, 0x231111, 0x230913, 0x232323, 0x232323};

extern uint32_t rawOtherColors[8]; 

extern uint32_t rawRailColors[3][4];

const int railsToPixelMap[4][5] =  {{70,73,74,77,78},//top positive rail
                                   {71,72,75,76,79},//top negative rail 
                                   {69,66,65,62,61},//bottom positive rail
                                   {68,67,64,63,60}};//bottom negative rail
//int nodeColors[MAX_PATHS] = {0};



const int pixelsToRails[20] = {B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS,
                               T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG};

extern uint32_t savedLEDcolors[NUM_SLOTS][LED_COUNT];


extern rgbColor netColors[MAX_NETS];
struct rgbColor shiftHue (struct rgbColor colorToShift, int hueShift = 0, int brightnessShift = 0, int saturationShift = 0,int specialFunction = -1);
void initLEDs(void);
char LEDbrightnessMenu(void);

void refreshSavedColors(void);
void saveRawColors(int slot = -1);
void showSavedColors(int slot = -1);

void clearLEDs(void);
void randomColors(uint32_t color, int wait);
void rainbowy(int ,int, int wait);
void showNets(void);
void assignNetColors ();
void lightUpRail (int logo = -1, int railNumber = -1, int onOff = 1, int brightness = DEFAULTRAILBRIGHTNESS, int supplySwitchPosition= 0);

void lightUpNet (int netNumber = 0 , int node = -1, int onOff = 1, int brightness = DEFAULTBRIGHTNESS, int hueShift = 0);//-1 means all nodes (default)
void lightUpNode (int node, uint32_t color);
rgbColor pcbColorCorrect (rgbColor colorToCorrect);
hsvColor RgbToHsv(rgbColor rgb);
rgbColor HsvToRgb(hsvColor hsv);
void applyBrightness (int brightness);
rgbColor unpackRgb(uint32_t color);
uint32_t scaleUpBrightness(uint32_t hexColor, int scaleFactor = 8, int minBrightness = 0x45);
uint32_t scaleDownBrightness(uint32_t hexColor, int scaleFactor = 5, int maxBrightness = 0x45);
void turnOffSkippedNodes();
void clearLEDsExceptRails();    

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b);
void startupColors(void);
void rainbowBounce(int wait);
#endif
