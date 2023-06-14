#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "Adafruit_NeoPixel.h"
#include "NetsToChipConnections.h"

#define LED_PIN 25
#define LED_COUNT 160
#define BRIGHTNESS 50

extern Adafruit_NeoPixel leds;

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

const int nodesToPixelMap[69] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
                                    30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61};

const int  bbPixelToNodesMap[62] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
                                    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61};

const int railsToPixelMap[4][5] =  {{70,73,74,77,78},//top positive rail
                                   {71,72,75,76,79},//top negative rail 
                                   {69,66,65,62,61},//bottom positive rail
                                   {68,67,64,63,60}};//bottom negative rail
//int nodeColors[MAX_PATHS] = {0};

const int pixelsToRails[20] = {B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS,
                               T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG};


extern rgbColor netColors[MAX_NETS];

void initLEDs(void);

void colorWipe(uint32_t color, int wait);
void rainbowy(int ,int, int wait);
void showNets(void);
void assignNetColors (void);

void lightUpNet (int netNumber, int node = -1, int onOff = 1, int brightness = BRIGHTNESS);//-1 means all nodes (default)
void lightUpNode (int node);
hsvColor RgbToHsv(rgbColor rgb);
rgbColor HsvToRgb(hsvColor hsv);

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b);

#endif