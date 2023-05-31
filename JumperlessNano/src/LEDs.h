#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "Adafruit_NeoPixel.h"

#define LED_PIN 25
#define LED_COUNT 80
#define BRIGHTNESS 50




extern Adafruit_NeoPixel leds;






const int  bbPixelToNodesMap[62] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
                                    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61};

const int railsToPixelMap[4][5] =  {{71,74,75,78,79},//top positive rail
                                   {72,73,76,77,80},//top negative rail 
                                   {70,67,66,63,62},//bottom positive rail
                                   {69,68,65,64,61}};//bottom negative rail


const int pixelsToRails[20] = {B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS, B_RAIL_POS, B_RAIL_NEG, B_RAIL_NEG, B_RAIL_POS,
                               T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG, T_RAIL_NEG, T_RAIL_POS, T_RAIL_POS, T_RAIL_NEG};

void initLEDs(void);

void colorWipe(uint32_t color, int wait);
void rainbowy(int ,int, int wait);

#endif