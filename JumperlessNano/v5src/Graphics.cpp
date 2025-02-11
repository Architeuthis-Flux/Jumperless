#include "Graphics.h"
#include "Adafruit_NeoPixel.h"
#include "Commands.h"
#include "Doom.h"
#include "JumperlessDefinesRP2040.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "Peripherals.h"
#include "PersistentStuff.h"
#include "Probing.h"
#include "leds.h"
#include "Menus.h"  

/* clang-format off */

uint8_t upperCase [30][3] = {{
0x1e, 0x05, 0x1e, },{ 0x1f, 0x15, 0x0a, },{ 
0x1f, 0x11, 0x11, },{ 0x1f, 0x11, 0x0e, },{ 0x1f, 0x15, 0x11, },{ 0x1f, 0x05, 0x01, },{ 
0x0e, 0x15, 0x1d, },{ 0x1f, 0x04, 0x1f, },{ 0x11, 0x1f, 0x11, },{ 0x08, 0x10, 0x0f, },{ 
0x1f, 0x04, 0x1b, },{ 0x1f, 0x10, 0x10, },{ 0x1f, 0x07, 0x1f, },{ 0x1f, 0x01, 0x1f, },{ 
0x1f, 0x11, 0x1f, },{ 0x1f, 0x05, 0x07, },{ 0x0f, 0x09, 0x17, },{ 0x1f, 0x0d, 0x17, },{ 
0x17, 0x15, 0x1d, },{ 0x01, 0x1f, 0x01, },{ 0x1f, 0x10, 0x1f, },{ 0x0f, 0x10, 0x0f, },{ 
0x1f, 0x0c, 0x1f, },{ 0x1b, 0x04, 0x1b, },{ 0x03, 0x1c, 0x03, },{ 0x19, 0x15, 0x13, },{ //Z

}};

uint8_t lowerCase [30][3] = {{
    0x1c, 0x0a, 0x1c, },{ 0x1e, 0x14, 0x08, },{ 0x0c, 0x12, 0x12, },{ 0x08, 0x14, 0x1e, },{ 
0x0e, 0x16, 0x14, },{ 0x1c, 0x0a, 0x02, },{ 0x14, 0x16, 0x0e, },{ 0x1e, 0x04, 0x18, },{ 
0x00, 0x1d, 0x00, },{ 0x10, 0x0d, 0x00, },{ 0x1e, 0x08, 0x16, },{ 0x00, 0x1e, 0x10, },{ 
0x1e, 0x06, 0x1e, },{ 0x1e, 0x02, 0x1c, },{ 0x1e, 0x12, 0x1e, },{ 0x1e, 0x0a, 0x04, },{ 
0x04, 0x0a, 0x1c, },{ 0x1e, 0x02, 0x04, },{ 0x14, 0x1a, 0x0a, },{ 0x04, 0x1e, 0x14, },{ 
0x1e, 0x10, 0x1e, },{ 0x0e, 0x10, 0x0e, },{ 0x1e, 0x18, 0x1e, },{ 0x16, 0x08, 0x16, },{ 
0x06, 0x18, 0x06, },{ 0x12, 0x1a, 0x16, }}; //z

uint8_t fontNumbers [10][3] = {{
0x1f, 0x11, 0x1f, },{ 0x12, 0x1f, 0x10, },{ 0x1d, 0x15, 0x17, },{ 0x11, 0x15, 0x1f, },{ 
0x07, 0x04, 0x1f, },{ 0x17, 0x15, 0x1d, },{ 0x1f, 0x15, 0x1d, },{ 0x19, 0x05, 0x03, },{ 
0x1f, 0x15, 0x1f, },{ 0x17, 0x15, 0x1f, }}; //9

uint8_t symbols [50][3] = {
{ 0x00, 0x17, 0x00, }, //'!'
{ 0x16, 0x1f, 0x0d, }, //$
{ 0x19, 0x04, 0x13, }, //%
{ 0x02, 0x01, 0x02, }, //^
{ 0x02, 0x07, 0x02, }, //'*'
{ 0x10, 0x10, 0x10, }, //_
{ 0x04, 0x04, 0x04, }, //-
{ 0x04, 0x0e, 0x04, }, //+
{ 0x04, 0x15, 0x04, }, //÷
{ 0x0a, 0x04, 0x0a, }, //x
{ 0x0a, 0x0a, 0x0a, }, //=
{ 0x12, 0x17, 0x12, }, //±
{ 0x01, 0x1d, 0x07, }, //?
{ 0x04, 0x0a, 0x11, }, //<
{ 0x11, 0x0a, 0x04, }, //>
{ 0x06, 0x04, 0x0c, }, //~
{ 0x01, 0x02, 0x00, }, //'
{ 0x10, 0x08, 0x00, }, //,
{ 0x00, 0x10, 0x00, }, //.
{ 0x18, 0x04, 0x03, }, // '/'
{ 0x03, 0x04, 0x18, }, // '\'
{ 0x00, 0x0e, 0x11, }, // (
{ 0x11, 0x0e, 0x00, }, // )
{ 0x00, 0x1f, 0x11, }, // [
{ 0x00, 0x11, 0x1f, }, // ]
{ 0x04, 0x0e, 0x1b, }, // {
{ 0x1b, 0x0e, 0x04, }, // }
{ 0x00, 0x1f, 0x00, }, // |
{ 0x10, 0x0a, 0x00, }, // ;
{ 0x00, 0x0a, 0x00, }, // :
{ 0x1e, 0x08, 0x06, }, // µ
{ 0x07, 0x05, 0x07, }, // °
{ 0x04, 0x0e, 0x1f, }, // ❬ thicc <
{ 0x1f, 0x0e, 0x04, }, // ❭ thicc >
{ 0x03, 0x00, 0x03, }, // "
{ 0x00, 0x03, 0x00, }, // '
{ 0x0a, 0x0f, 0x08, }, // 𝟷
{ 0x0d, 0x0b, 0x00, }, // 𝟸
{ 0x09, 0x0b, 0x0f, }};// 𝟹

// char symbolMap[40] = {
// '!', '$', '%', '^', '*', '_', '-', '+', '÷', 'x', '=', '±', '?', '<', '>', '~', '\'', ',', '.', '/', '\\', '(', ')', '[', ']', '{', '}', '|', ';', ':', 'µ', '°', '❬', '❭', '"', '\'', '𝟷', '𝟸', '𝟹'};

wchar_t fontMap[120] = {
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
'!', '$', '%', '^', '*', '_', '-', '+', L'÷', 'x', '=', L'±', '?', '<', '>', '~', '\'', ',', '.', '/', '\\', 
'(', ')', '[', ']', '{', '}', '|', ';', ':', L'µ', L'°', L'❬', L'❭', '"', '\'', L'𝟷', L'𝟸', L'𝟹', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};



const uint8_t font[][3] = // 'JumperlessFontmap', 500x5px
{{
0x1f, 0x11, 0x1f, },{ 0x12, 0x1f, 0x10, },{ 0x1d, 0x15, 0x17, },{ 0x11, 0x15, 0x1f, },{ 
0x07, 0x04, 0x1f, },{ 0x17, 0x15, 0x1d, },{ 0x1f, 0x15, 0x1d, },{ 0x19, 0x05, 0x03, },{ 
0x1f, 0x15, 0x1f, },{ 0x17, 0x15, 0x1f, },{ //9

0x1e, 0x05, 0x1e, },{ 0x1f, 0x15, 0x0a, },{ 
0x1f, 0x11, 0x11, },{ 0x1f, 0x11, 0x0e, },{ 0x1f, 0x15, 0x11, },{ 0x1f, 0x05, 0x01, },{ 
0x0e, 0x15, 0x1d, },{ 0x1f, 0x04, 0x1f, },{ 0x11, 0x1f, 0x11, },{ 0x08, 0x10, 0x0f, },{ 
0x1f, 0x04, 0x1b, },{ 0x1f, 0x10, 0x10, },{ 0x1f, 0x07, 0x1f, },{ 0x1f, 0x01, 0x1f, },{ 
0x1f, 0x11, 0x1f, },{ 0x1f, 0x05, 0x07, },{ 0x0f, 0x09, 0x17, },{ 0x1f, 0x0d, 0x17, },{ 
0x17, 0x15, 0x1d, },{ 0x01, 0x1f, 0x01, },{ 0x1f, 0x10, 0x1f, },{ 0x0f, 0x10, 0x0f, },{ 
0x1f, 0x0c, 0x1f, },{ 0x1b, 0x04, 0x1b, },{ 0x03, 0x1c, 0x03, },{ 0x19, 0x15, 0x13, },{ //Z

0x1c, 0x0a, 0x1c, },{ 0x1e, 0x14, 0x08, },{ 0x0c, 0x12, 0x12, },{ 0x08, 0x14, 0x1e, },{ 
0x0e, 0x16, 0x14, },{ 0x1c, 0x0a, 0x02, },{ 0x14, 0x16, 0x0e, },{ 0x1e, 0x04, 0x18, },{ 
0x00, 0x1d, 0x00, },{ 0x10, 0x0d, 0x00, },{ 0x1e, 0x08, 0x16, },{ 0x00, 0x1e, 0x10, },{ 
0x1e, 0x06, 0x1e, },{ 0x1e, 0x02, 0x1c, },{ 0x1e, 0x12, 0x1e, },{ 0x1e, 0x0a, 0x04, },{ 
0x04, 0x0a, 0x1c, },{ 0x1e, 0x02, 0x04, },{ 0x14, 0x1a, 0x0a, },{ 0x04, 0x1e, 0x14, },{ 
0x1e, 0x10, 0x1e, },{ 0x0e, 0x10, 0x0e, },{ 0x1e, 0x18, 0x1e, },{ 0x16, 0x08, 0x16, },{ 
0x06, 0x18, 0x06, },{ 0x12, 0x1a, 0x16, },{ //z
    
0x00, 0x17, 0x00, },{ 0x16, 0x1f, 0x0d, },{ 
0x19, 0x04, 0x13, },{ 0x02, 0x01, 0x02, },{ 0x02, 0x07, 0x02, },{ 0x10, 0x10, 0x10, },{ 
0x04, 0x04, 0x04, },{ 0x04, 0x0e, 0x04, },{ 0x04, 0x15, 0x04, },{ 0x0a, 0x04, 0x0a, },{ 
0x0a, 0x0a, 0x0a, },{ 0x12, 0x17, 0x12, },{ 0x01, 0x1d, 0x07, },{ 0x04, 0x0a, 0x11, },{ 
0x11, 0x0a, 0x04, },{ 0x12, 0x17, 0x12, },{ 0x01, 0x02, 0x00, },{ 0x10, 0x08, 0x00, },{ 
0x00, 0x10, 0x00, },{ 0x18, 0x04, 0x03, },{ 0x03, 0x04, 0x18, },{ 0x00, 0x0e, 0x11, },{ 
0x11, 0x0e, 0x00, },{ 0x00, 0x1f, 0x11, },{ 0x00, 0x11, 0x1f, },{ 0x04, 0x0e, 0x1b, },{ 
0x1b, 0x0e, 0x04, },{ 0x00, 0x1f, 0x00, },{ 0x10, 0x0a, 0x00, },{ 0x00, 0x0a, 0x00, },{ 
0x1e, 0x08, 0x06, },{ 0x07, 0x05, 0x07, },{ 0x04, 0x0e, 0x1f, },{ 0x1f, 0x0e, 0x04, },{ 
0x03, 0x00, 0x03, },{ 0x00, 0x03, 0x00, },{ 0x0a, 0x0f, 0x08, },{ 0x0d, 0x0b, 0x00, },{ 
0x09, 0x0b, 0x0f, },{

0x1e, 0x12, 0x1e, },{ 0x14, 0x1e, 0x10, },{ 0x1a, 0x12, 0x16, },{ 0x12, 0x16, 0x1e, },{ //lowercase Numbers
0x0e, 0x08, 0x1e, },{ 0x16, 0x12, 0x1a, },{ 0x1e, 0x1a, 0x1a, },{ 0x12, 0x0a, 0x06, },{ 
0x1e, 0x1a, 0x1e, },{ 0x16, 0x16, 0x1e, }

};


//0=top rail, 1= gnd, 2 = bottom rail, 3 = gnd again, 4 = adc 1, 5 = adc 2, 6 = adc 3, 7 = adc 4, 8 = adc 5, 9 = adc 6, 10 = dac 0, 11 = dac 1, 12 = routable buffer in, 13 = routable buffer out, 14 = i sense +, 15 = isense -, 16 = gpio Tx, 17 = gpio Rx, 
uint32_t specialColors[13][5] = {
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000},
    {0x000000, 0x000000, 0x000000, 0x000000, 0x000000}};


    // struct specialRowAnimation{
    //     int net;
    //     int currentFrame;
    //     int numberOfFrames = 8;
    //     uint32_t frames[8][5] = {0xffffff};

        
    // };


int menuBrightnessSetting = -40; // -100 - 100 (0 default)

    bool animationsEnabled = true;
specialRowAnimation rowAnimations[26];
 volatile int doomOn = 0;

int wireStatus[64][5]; // row, led (net stored)
//char defconString[16] = " Fuck    You   ";
char defconString[16] = "Jumper less V5 ";

/* clang-format on */
int colorCycle = 0;
int defNudge = 0;

void defcon(int start, int spread, int color, int nudge) {
  spread = 13;
  nudge = defNudge;

  int scaleFactor = -70;

  b.clear();
  b.print(
      defconString[0],
      scaleBrightness(logoColorsAll[color][(start) % (LOGO_COLOR_LENGTH - 1)],
                      scaleFactor),
      (uint32_t)0xffffff, 0, 0, nudge);

  b.print(defconString[1],
          scaleBrightness(
              logoColorsAll[color][(start + spread) % (LOGO_COLOR_LENGTH - 1)],
              scaleFactor),
          (uint32_t)0xffffff, 1, 0, nudge);

  b.print(
      defconString[2],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 2) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 2, 0, nudge);
  b.print(
      defconString[3],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 3) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 3, 0, nudge);
  b.print(
      defconString[4],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 4) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 4, 0, nudge);
  b.print(
      defconString[5],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 5) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 5, 0, nudge);
  b.print(
      defconString[6],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 5) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 6, 0, nudge);
  b.print(
      defconString[7],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 6) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 0, 1, nudge);
  b.print(
      defconString[8],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 7) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 1, 1, nudge);
  b.print(
      defconString[9],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 8) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 2, 1, nudge);
  b.print(
      defconString[10],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 9) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 3, 1, nudge);
  b.print(
      defconString[11],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 10) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 4, 1, nudge);
  b.print(
      defconString[12],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 11) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 5, 1, nudge);
  b.print(
      defconString[13],
      scaleBrightness(
          logoColorsAll[color][(start + spread * 12) % (LOGO_COLOR_LENGTH - 1)],
          scaleFactor),
      (uint32_t)0xffffff, 6, 1, nudge);
  // railsToPixelMap[0][20] = 0;
  //  leds.setPixelColor(railsToPixelMap[0][20], 0x004f9f);
  //  leds.setPixelColor(railsToPixelMap[0][21], 0x3f000f);
  //  leds.setPixelColor(railsToPixelMap[0][22], 0x3f000f);
  //  leds.setPixelColor(railsToPixelMap[0][23], 0x3f000f);
  //  leds.setPixelColor(railsToPixelMap[0][24], 0x7f000f);

  // leds.setPixelColor(railsToPixelMap[1][20], 0x007aaf);
  // leds.setPixelColor(railsToPixelMap[1][21], 0x00009f);
  // leds.setPixelColor(railsToPixelMap[1][22], 0x00003f);
  // leds.setPixelColor(railsToPixelMap[1][23], 0x00005f);
  // leds.setPixelColor(railsToPixelMap[1][24], 0x00005f);

  int topScale = 70;

  int spreadnudge = 3;

  leds.setPixelColor(
      bbPixelToNodesMapV5[0][1],
      scaleBrightness(
          logoColorsAll[0][(start - spreadnudge) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[2][1],
      scaleBrightness(logoColorsAll[0][(start + spread - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));

  leds.setPixelColor(
      bbPixelToNodesMapV5[4][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 2 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[6][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 3 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[8][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 4 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[10][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 5 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[12][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 6 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[14][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 7 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[16][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 8 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[18][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 9 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[20][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 10 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[22][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 11 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[24][1],
      scaleBrightness(logoColorsAll[0][(start + spread * 12 - spreadnudge) %
                                       (LOGO_COLOR_LENGTH - 1)],
                      topScale));

  leds.setPixelColor(
      bbPixelToNodesMapV5[1][1],
      scaleBrightness(logoColorsAll[0][(start) % (LOGO_COLOR_LENGTH - 1)],
                      topScale));

  leds.setPixelColor(
      bbPixelToNodesMapV5[3][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread) % (LOGO_COLOR_LENGTH - 1)],
          topScale));

  leds.setPixelColor(
      bbPixelToNodesMapV5[5][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 2) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[7][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 3) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[9][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 4) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[11][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 5) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[13][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 6) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[15][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 7) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[17][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 8) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[19][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 9) % (LOGO_COLOR_LENGTH - 1)],
          topScale));

  leds.setPixelColor(
      bbPixelToNodesMapV5[21][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 10) % (LOGO_COLOR_LENGTH - 1)],
          topScale));
  leds.setPixelColor(
      bbPixelToNodesMapV5[23][1],
      scaleBrightness(
          logoColorsAll[0][(start + spread * 11) % (LOGO_COLOR_LENGTH - 1)],
          topScale));

  // b.print('M', logoColors[(start + spread * 2) % (LOGO_COLOR_LENGTH - 1)],
  // 2,0); b.print('P', logoColors[(start + spread * 3) % (LOGO_COLOR_LENGTH -
  // 1)], 3,0); b.print('E', logoColors[(start + spread * 4) %
  // (LOGO_COLOR_LENGTH - 1)], 4,0); b.print('R', logoColors[(start + spread *
  // 5) % (LOGO_COLOR_LENGTH - 1)], 5,0); b.print('L', logoColors[(start +
  // spread * 6) % (LOGO_COLOR_LENGTH - 1)], 6,0); b.print('E',
  // logoColors[(start + spread * 7) % (LOGO_COLOR_LENGTH - 1)], 8,1);
  // b.print('S', logoColors[(start + spread * 8) % (LOGO_COLOR_LENGTH - 1)],
  // 9,1); b.print('S', logoColors[(start + spread * 9) % (LOGO_COLOR_LENGTH -
  // 1)], 10,1); b.print(' ', logoColors[(start + spread * 10) %
  // (LOGO_COLOR_LENGTH - 1)], 11,1); b.print('V', logoColors[(start + spread *
  // 11) % (LOGO_COLOR_LENGTH - 1)], 12,1); b.print('5', logoColors[(start +
  // spread * 12) % (LOGO_COLOR_LENGTH - 1)], 13,1);
}

int filledPaths[64][4] = {-1}; // node1 node2 rowfilled

void drawWires(int net) {
  // int fillSequence[6] = {0,2,4,1,3,};
  // debugLEDs = 0;
  // Serial.print("c2debugLEDs = ");
  // Serial.println(debugLEDs);
  assignNetColors();

  // Serial.println("drawWires");
  // Serial.print("numberOfNets = ");

  // Serial.println(numberOfNets);
  // Serial.print("probeActive = ");
  // Serial.println(probeActive);
  // Serial.print("numberOfShownNets = ");
  // Serial.println(numberOfShownNets);

  int fillSequence[6] = {0, 1, 2, 3, 4, 0};
  int fillIndex = 0;

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 4; j++) {
      filledPaths[i][j] = -1;
    }
  }

  for (int i = 0; i < 62; i++) {
    for (int j = 0; j < 5; j++) {
      wireStatus[i][j] = 0;
    }
  }

  // for (int i = 0; i < numberOfNets; i++) {
  //   // Serial.print(i);
  //   Serial.print("netColors[");
  //   Serial.print(i);
  //   Serial.print("] = ");
  //   Serial.print(netColors[i].r, HEX);
  //   Serial.print(" ");
  //   Serial.print(netColors[i].g, HEX);
  //   Serial.print(" ");
  //   Serial.println(netColors[i].b, HEX);
  // }
  if (net == -1) {

    for (int i = 0; i <= numberOfPaths; i++) {
      // Serial.print(i);
      // Serial.print(" c3debugLEDs = ");
      // Serial.println(debugLEDs);
      // Serial.print("\n\n\rpath[");
      // Serial.print(i);
      // Serial.print("].net = ");
      // Serial.print(path[i].net);
      // Serial.print(" path[");
      // Serial.print(i);
      // Serial.print("].node1 = ");
      // Serial.print(path[i].node1);
      // Serial.print(" path[");
      // Serial.print(i);
      // Serial.print("].node2 = ");
      // Serial.println(path[i].node2);
      // Serial.print("number of paths = ");
      // Serial.println(numberOfPaths);

      int sameLevel = 0;
      int bothOnTop = 0;
      int bothOnBottom = 0;
      int bothOnBB = 0;
      int whichIsLarger = 0;

      if (path[i].node1 != -1 && path[i].node2 != -1 &&
          path[i].node1 != path[i].node2) {
        if ((path[i].node1 <= 60 &&
             path[i].node2 <= 60)) { //|| (path[i].node1 >= 110 &&
          // path[i].node1 <= 113) || (path[i].node2 >= 110 && path[i].node2 <=
          // 113)) {
          bothOnBB = 1;
          if (path[i].node1 > 0 && path[i].node1 < 30 && path[i].node2 > 0 &&
              path[i].node2 <= 30) {
            bothOnTop = 1;
            sameLevel = 1;
            if (path[i].node1 > path[i].node2) {
              whichIsLarger = 1;
            } else {
              whichIsLarger = 2;
            }
          } else if (path[i].node1 > 30 && path[i].node1 <= 60 &&
                     path[i].node2 > 30 && path[i].node2 <= 60) {
            bothOnBottom = 1;
            sameLevel = 1;
            if (path[i].node1 > path[i].node2) {
              whichIsLarger = 1;
            } else {
              whichIsLarger = 2;
            }
          }
        } else {
          // Serial.println("else ");
          // Serial.print("path[");
          // Serial.print(i);
          // Serial.print("].net = ");
          // Serial.print(path[i].net);

          lightUpNet(path[i].net);
        }

        // if (sameLevel == 0 && ((path[i].node1 >= 110 && path[i].node1 <= 113)
        // || (path[i].node2 >= 110 && path[i].node2 <= 113)))
        // {
        //   sameLevel = 1;

        // }

        if (sameLevel == 1) {
          int range = 0;
          int first = 0;
          int last = 0;
          if (whichIsLarger == 1) {
            range = path[i].node1 - path[i].node2;
            first = path[i].node2;
            last = path[i].node1;
          } else {
            range = path[i].node2 - path[i].node1;
            first = path[i].node1;
            last = path[i].node2;
          }

          // Serial.print("\nfirst = ");
          // Serial.println(first);
          // Serial.print("last = ");
          // Serial.println(last);
          // Serial.print("range = ");
          // Serial.println(range);
          // Serial.print("net = ");
          // Serial.println(path[i].net);

          int inside = 0;
          int largestFillIndex = 0;

          for (int j = first; j <= first + range; j++) {
            // Serial.print("j = ");
            // Serial.println(j);
            for (int w = 0; w < 5; w++) {
              if ((wireStatus[j][w] == path[i].net || wireStatus[j][w] == 0) &&
                  w >= largestFillIndex) {

                // wireStatus[j][w] = path[i].net;
                if (w > largestFillIndex) {
                  largestFillIndex = w;
                }
                // Serial.print("j = ");
                // Serial.println(j);
                // if (first > 30) {
                //   Serial.print("bottom ");
                // }

                break;
              }
            }
          }
          //           Serial.print("largestFillIndex = ");
          // Serial.println(largestFillIndex);
          // if (largestFillIndex > 4) {
          //   largestFillIndex = 0;
          // }

          for (int j = first; j <= first + range; j++) {
            if (j == first || j == last) {
              for (int k = largestFillIndex; k < 5; k++) {

                wireStatus[j][k] = path[i].net;
                // wireStatus[j][largestFillIndex] = path[i].net;
              }
            } else {
              wireStatus[j][largestFillIndex] = path[i].net;
            }
          }

          fillIndex = largestFillIndex;

          filledPaths[i][0] = first;
          filledPaths[i][1] = last;
          filledPaths[i][2] = fillSequence[fillIndex];

          // showLEDsCore2 = 1;
        } else {
          for (int j = 0; j < 5; j++) {

            if (path[i].node1 > 0 && path[i].node1 <= 60) {
              if (wireStatus[path[i].node1][j] == 0) {
                wireStatus[path[i].node1][j] = path[i].net;
              }

              // Serial.print("path[i].node1 = ");
              // Serial.println(path[i].node1);
            }
            if (path[i].node2 > 0 && path[i].node2 <= 60) {
              if (wireStatus[path[i].node2][j] == 0) {
                wireStatus[path[i].node2][j] = path[i].net;
              }
              // Serial.print("path[i].node2 = ");
              // Serial.println(path[i].node2);
            }
          }
          // lightUpNet(path[i].net);
        }
      } else {

        lightUpNet(path[i].net);
      }
    }
    for (int i = 0; i <= 60; i++) {
      for (int j = 0; j < 4; j++) {
        if (wireStatus[i][j] != 0) {
          if (wireStatus[i][j + 1] != wireStatus[i][j] &&
              wireStatus[i][j + 1] != 0 &&
              wireStatus[i][4] == wireStatus[i][j]) {
            wireStatus[i][j + 1] = wireStatus[i][j];
            // leds.setPixelColor((i * 5) + fillSequence[j], 0x000000);
          } else {
            // leds.setPixelColor((i * 5) + fillSequence[j], 0x100010);
          }
        }
      }
    }

    for (int i = 31; i <= 60; i++) { // reverse the bottom row

      int tempRow[5] = {wireStatus[i][0], wireStatus[i][1], wireStatus[i][2],
                        wireStatus[i][3], wireStatus[i][4]};
      wireStatus[i][0] = tempRow[4];
      wireStatus[i][1] = tempRow[3];
      wireStatus[i][2] = tempRow[2];
      wireStatus[i][3] = tempRow[1];
      wireStatus[i][4] = tempRow[0];
    }

    for (int i = 1; i <= 60; i++) {
      if (i <= 60) {

        for (int j = 0; j < 5; j++) {

          uint32_t color3 = 0x100010;

          rgbColor colorRGB = netColors[wireStatus[i][j]];

          hsvColor colorHSV = RgbToHsv(colorRGB);

          // colorHSV.v = colorHSV.v * 0.25;
          // colorHSV.s = colorHSV.s * 0.5;
          colorRGB = HsvToRgb(colorHSV);

          uint32_t color = packRgb(colorRGB.r, colorRGB.g, colorRGB.b);

          if (wireStatus[i][j] == 0) {
            // leds.setPixelColor((i * 5) + fillSequence[j], 0x000000);
          } else if (probeHighlight != i) {
            leds.setPixelColor((((i - 1) * 5) + j), color);
          }
        }
      } else {
        for (int j = 0; j < 5; j++) {

          uint32_t color3 = 0x100010;

          rgbColor colorRGB = netColors[wireStatus[i][j]];
          // Serial.print("netColors[wireStatus[");
          // Serial.print(i);
          // Serial.print("][");
          // Serial.print(j);
          // Serial.print("] = ");
          // Serial.print(netColors[wireStatus[i][j]].r, HEX);
          // Serial.print(" ");
          // Serial.print(netColors[wireStatus[i][j]].g, HEX);
          // Serial.print(" ");
          // Serial.println(netColors[wireStatus[i][j]].b, HEX);

          // int adcShow = 0;

          // hsvColor colorHSV = RgbToHsv(colorRGB);

          // colorHSV.v = colorHSV.v * 0.25;
          // colorHSV.s = colorHSV.s * 0.5;
          // colorRGB = HsvToRgb(colorHSV);

          uint32_t color = packRgb(colorRGB.r, colorRGB.g, colorRGB.b);
          // Serial.print("color = ");
          // Serial.println(color);

          if (wireStatus[i][j] == 0) {
            // leds.setPixelColor((i * 5) + fillSequence[j], 0x000000);
          } else if (probeHighlight != i) {
            leds.setPixelColor((((i - 1) * 5) + (4 - j)), color);
            // Serial.print((((i - 1) * 5) + (4 - j)));
            // Serial.print(" ");
          }
        }
      }
    }
  } else {
    // lightUpNet(net);
  }
}

void doomIntro(void) {

  // for (int f = 0; f <= 20; f++)
  //   {
  //    for (int r = 0; r < 30; r++)
  //    {

  //       for (int i = 0; i < 5 ; i++)
  //       {
  //         leds.setPixelColor((r * 5) + i,
  //         scaleBrightness(doomIntroFrames[r][5+i], -63));
  //       }

  //       for (int i = 0; i < 5  ; i++)
  //       {
  //          leds.setPixelColor((r*2 * 5) + i,
  //          scaleBrightness(doomIntroFrames[r][i], -63));
  //       }

  //    }
  //    leds.show();

  for (int f = 0; f < 60; f++) {
    for (int r = 0; r < 5; r++) {
      leds.setPixelColor((f * 5) + r, scaleBrightness(0x550500, -93));
    }
  }
  leds.show();

  delay(100);

  for (int f = 0; f < 40; f++) {
    for (int i = 0; i < 60; i++) {
      // if (random(0, 100) > 10)
      // {
      //   leds.setPixelColor((i*5)+random(0,5), 0x000000);
      // }
      // if (random(0, 100) > 20)
      // {
      // leds.setPixelColor((i*5)+random(0,5), 0x000000);
      // }
      // if (random(0, 100) > 30)
      // {
      //   leds.setPixelColor((i*5)+random(0,5), 0x000000);
      // }
      // if (random(0, 100) > 40)
      // {
      //   leds.setPixelColor((i*5)+random(0,5), 0x000000);
      //}
      // if (random(0, 100) > 50)
      // {
      //  leds.setPixelColor((i*5)+random(0,5), 0x000000);
      // }
      if (i < 30) {
        if (random(0, 100) > 70) {
          leds.setPixelColor((i * 5) + random(0, 5), 0x000000);
        }
      }
      if (random(0, 100) > 80) {
        leds.setPixelColor((i * 5) + random(0, 5), 0x000000);
      }
      if (random(0, 100) < 20) {
        leds.setPixelColor((i * 5) + random(0, 3), 0x000000);
      }
      if (random(0, 100) < 10) {
        leds.setPixelColor((i * 5) + random(0, 2), 0x000000);
      }

      if (random(0, 100) > 85) {
        leds.setPixelColor((i * 5) + random(0, 5), 0x000000);
      }

      if (random(0, 100) > 90) {
        leds.setPixelColor((i * 5) + random(0, 5), 0x000000);
        leds.setPixelColor((i * 5) + random(0, 3), 0x000000);
      }
      if (random(0, 100) > 95) {
        leds.setPixelColor((i * 5) + random(0, 5), 0x000000);
        leds.setPixelColor((i * 5) + random(0, 2), 0x000000);
      }
    }
    leds.show();
    delay(48);
  }

  //    delay(40);

  //    }
}

void playDoom(void) {
  // core1busy = 1;
  core2busy = 1;
  int pixMap[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
  // doomIntro();
  doomIntro();
  for (int l = 0; l < 4; l++) {
    
    for (int f = 0; f <= 39; f++) {
      for (int r = 0; r < 60; r++) {

        if (r < 30) {

          for (int i = 0; i < 5; i++) {
            leds.setPixelColor(
                (r * 5) + i,
                scaleBrightness(doomFrames[f][(r * 10) + 5 + (4 - i)], -93));
          }
        } else {
          for (int i = 0; i < 5; i++) {
            leds.setPixelColor(
                (r * 5) + i,
                scaleBrightness(doomFrames[f][((r - 30) * 10) + (4 - i)], -93));
          }
        }
      }
      leds.show();
      // if (l % 3 == 0) {

      // leds.clear();
      // leds.show();
      // delayMicroseconds(l*200);
      // }
      //       if (l % 3 == 1) {

      // leds.clear();
      // leds.show();
      // //delayMicroseconds(1);
      // }
      delay(150);
    }
  }

  // core1busy = 0;
  core2busy = 0;
}

int animationOrder[26] = {
    TOP_RAIL, GND, BOTTOM_RAIL, GND, ADC1, ADC2, ADC3, ADC4, ADC7,
    // ADC6,
    DAC0, DAC1, ROUTABLE_BUFFER_IN, ROUTABLE_BUFFER_OUT, ISENSE_PLUS,
    ISENSE_MINUS, RP_UART_TX, RP_UART_RX, RP_GPIO_1, RP_GPIO_2, RP_GPIO_3,
    RP_GPIO_4, MCP_GPIO_0, MCP_GPIO_1, MCP_GPIO_2, MCP_GPIO_3};

/* clang-format off */

uint32_t animations[10][15] = {

    {0x080001, 0x080002, 0x070003, 0x080002, 0x080001, 
     0x090000, 0x090000, 0x080100, 0x070200, 0x060300, 
     0x070200, 0x080100, 0x080000, 0x090000, 0x080000},

    {0x000900, 0x000a00, 0x020b00, 0x000a00, 0x000900, 
     0x000901, 0x000702, 0x000603, 0x000702, 0x000801, 
     0x000800, 0x010800, 0x020800, 0x020800, 0x010800},

    {0x080001, 0x080002, 0x070003, 0x080002, 0x080001, 
     0x090000, 0x090000, 0x080100, 0x070200, 0x060300, 
     0x070200, 0x080100, 0x080000, 0x090000, 0x080000},

};

/* clang-format on */

unsigned long lastRowAnimationTime = 0;
unsigned long rowAnimationInterval = 150;

void initRowAnimations() {
  // 0=top rail, 1= gnd, 2 = bottom rail, 3 = gnd again, 4 = adc 1, 5 = adc 2, 6
  // = adc 3, 7 = adc 4, 8 = adc 5, 9 = adc 6, 10 = dac 0, 11 = dac 1, 12 =
  // routable buffer in, 13 = routable buffer out, 14 = i sense +, 15 = isense
  // -, 16 = gpio Tx, 17 = gpio Rx,

  for (int i = 0; i < 3; i++) {
    rowAnimations[i].index = i;
    rowAnimations[i].net = animationOrder[i];
    rowAnimations[i].currentFrame = 0;
    rowAnimations[i].numberOfFrames = 8;
    for (int j = 0; j < 15; j++) {
      rowAnimations[i].frames[j] = animations[i][j];
    }
  }
  rowAnimations[0].direction = 1;
  rowAnimations[0].frameInterval = 160;
  rowAnimations[1].direction = 0;
  rowAnimations[1].frameInterval = 100;
  rowAnimations[2].direction = 0;
  rowAnimations[2].frameInterval = 160;
}

void showRowAnimation(int index, int net) {

  // net = 0;

  int structIndex = -1;
  int actualNet = -1;
  if (inPadMenu == 1) {
    return;
  }
  for (int i = 0; i < 27; i++) {
    if (rowAnimations[i].net == net) {
      structIndex = i;

      break;
    }
  }

  if (structIndex == -1) {
    return;
  }

  actualNet = findNodeInNet(net);
  net = structIndex;

  //    Serial.print("net = ");
  //  Serial.println(net);
  // Serial.print("actualNet = ");
  // Serial.println(actualNet);
  uint32_t frameColors[5];

  if (millis() - rowAnimations[net].lastFrameTime >
      rowAnimations[net].frameInterval) {
    rowAnimations[net].currentFrame++;
    rowAnimations[net].lastFrameTime = millis();
  } else {
    // return;
  }

  // if (rowAnimations[net].currentFrame >=
  //     rowAnimations[net].numberOfFrames + 5) {
  //       Serial.print("rowAnimations[net].currentFrame = ");
  //       Serial.println(rowAnimations[net].currentFrame);
  //   rowAnimations[net].currentFrame = 0;
  // }

  for (int i = 0; i < 5; i++) {

    // frameColors[i] =
    // rowAnimations[net].frames[(rowAnimations[net].currentFrame + i) %
    // (rowAnimations[net].numberOfFrames-1)];
    if (brightenedNet == actualNet) {
      // Serial.print("brightenedNet = ");
      // Serial.println(brightenedNet);

      frameColors[i] = scaleBrightness(
          rowAnimations[net].frames[(rowAnimations[net].currentFrame + i) % 15],
          brightenedAmount * 8);
    } else {
      frameColors[i] =
          rowAnimations[net].frames[(rowAnimations[net].currentFrame + i) % 15];
    }
    // Serial.print("brightenedAmount = ");  
    // Serial.println(brightenedAmount);

    // Serial.print("frameColors[");
    // Serial.print(i);
    // Serial.print("] = ");
    // Serial.print(frameColors[i], HEX);
    // Serial.print(" ");
    // Serial.println((rowAnimations[net].currentFrame + i) % 15);
  }
  // Serial.println(" ");
  int row = 2;

  if (rowAnimations[net].direction == 0) {

    uint32_t tempFrame[5] = {frameColors[0], frameColors[1], frameColors[2],
                             frameColors[3], frameColors[4]};
    frameColors[0] = tempFrame[4];
    frameColors[1] = tempFrame[3];
    frameColors[2] = tempFrame[2];
    frameColors[3] = tempFrame[1];
    frameColors[4] = tempFrame[0];
  }
  // Serial.print("\n\n\rnet = ");
  // Serial.print(net);
  // Serial.print("   actualNet = ");
  // Serial.print(actualNet);
  // Serial.print("   direction = ");
  // Serial.println(rowAnimations[net].direction);

  if (displayMode == 0 || numberOfShownNets > MAX_NETS_FOR_WIRES) {
    for (int i = 0; i <= numberOfPaths; i++) {
      if (path[i].net == actualNet) {
        if (path[i].skip == true) {
          continue;
        }

        if (path[i].node1 > 0 && path[i].node1 <= 60 &&
            path[i].node1 != probeHighlight) {
          for (int j = 0; j < 5; j++) {

            b.printRawRow(0b00010000 >> j, path[i].node1 - 1, frameColors[j],
                          0xfffffe, 4);
          }
        }
        if (path[i].node2 > 0 && path[i].node2 <= 60 &&
            path[i].node2 != probeHighlight) {
          for (int j = 0; j < 5; j++) {

            b.printRawRow(0b00010000 >> j, path[i].node2 - 1, frameColors[j],
                          0xfffffe, 4);
          }
        }
      }
    }

    // for (int i = 0; i < 5; i++) {
    //   b.printRawRow(0b00000001 << i, row, frameColors[i], 0xfffffe);
    // }
  } else {

    for (int i = 0; i <= 60; i++) {
      for (int j = 0; j < 5; j++) {
        if (wireStatus[i][j] == actualNet) {
          if (i == probeHighlight) {

          } else {
            leds.setPixelColor(((i - 1) * 5) + j, frameColors[j]);
          }
        }
      }
    }
  }

  for (int i = 0; i <= numberOfPaths; i++) {
        //     if (path[i].skip == true) {
        //   continue;
        // }
    if (path[i].net == actualNet) {

      if (path[i].node1 > NANO_D0 && path[i].node1 <= NANO_GND_1) {
        for (int j = 0; j < 35; j++) {
          if (bbPixelToNodesMapV5[j][0] == path[i].node1) {

            leds.setPixelColor(bbPixelToNodesMapV5[j][1],
                               scaleBrightness(frameColors[0], 200));
          }
        }
      }
      if (path[i].node2 > NANO_D0 && path[i].node2 <= NANO_GND_1) {
        for (int j = 0; j < 35; j++) {
          if (bbPixelToNodesMapV5[j][0] == path[i].node2) {

            leds.setPixelColor(bbPixelToNodesMapV5[j][1],
                               scaleBrightness(frameColors[0], 200));
          }
        }
      }
    }
  }

  // b.printRawRow(0b00000001, row, frameColors[0], 0xfffffe);
  // b.printRawRow(0b00000010, row, frameColors[1], 0xfffffe);
  // b.printRawRow(0b00000100, row, frameColors[2], 0xfffffe);
  // b.printRawRow(0b00001000, row, frameColors[3], 0xfffffe);
  // b.printRawRow(0b00010000, row, frameColors[4], 0xfffffe);

  // showLEDsCore2 = 2;
  showSkippedNodes();
}

void showAllRowAnimations() {
  // showRowAnimation(2, rowAnimations[1].net);
  // showRowAnimation(2, rowAnimations[2].net);
  for (int i = 0; i < 3; i++) {
    showRowAnimation(i, rowAnimations[i].net);
  }
}

void printWireStatus(void) {

  for (int s = 1; s <= 30; s++) {
    Serial.print(s);
    Serial.print(" ");
    if (s < 9) {
      Serial.print(" ");
    }
  }
  Serial.println();

  int level = 1;
  for (int r = 0; r < 5; r++) {
    for (int s = 1; s <= 30; s++) {
      Serial.print(wireStatus[s][r]);
      Serial.print(" ");
      if (wireStatus[s][r] < 10) {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  Serial.println("\n\n");
  for (int s = 31; s <= 60; s++) {
    Serial.print(s);
    Serial.print(" ");
    if (s < 9) {
      Serial.print(" ");
    }
  }
  Serial.println();
  for (int r = 0; r < 5; r++) {
    for (int s = 31; s <= 60; s++) {
      Serial.print(wireStatus[s][r]);
      Serial.print(" ");
      if (wireStatus[s][r] < 10) {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
}
// }
uint32_t defaultColor = 0x001012;

bread::bread() {
  // defaultColor = 0x060205;
}

void bread::print(const char c) { printChar(c, defaultColor); }

void bread::print(const char c, int position) {
  printChar(c, defaultColor, position);
}

void bread::print(const char c, uint32_t color) { printChar(c, color); }

void bread::print(const char c, uint32_t color, int position, int topBottom) {
  printChar(c, color, position, topBottom);
}

void bread::print(const char c, uint32_t color, int topBottom) {
  printChar(c, color, topBottom);
}

void bread::print(const char c, uint32_t color, uint32_t backgroundColor) {
  printChar(c, color, backgroundColor);
}

void bread::print(const char c, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom) {
  printChar(c, color, backgroundColor, position, topBottom);
}

void bread::print(const char c, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom, int nudge) {
  printChar(c, color, backgroundColor, position, topBottom, nudge);
}

void bread::print(const char c, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom, int nudge, int lowercaseNumber) {
  printChar(c, color, backgroundColor, position, topBottom, nudge,
            lowercaseNumber);
}

void bread::print(const char *s) {
  // Serial.println("1");
  printString(s, defaultColor);
}

void bread::print(const char *s, int position) {
  // Serial.println("2");
  printString(s, defaultColor, 0xffffff, position);
}

void bread::print(const char *s, uint32_t color) {
  // Serial.println("3");
  printString(s, color);
}

void bread::print(const char *s, uint32_t color, uint32_t backgroundColor) {
  // Serial.println("4");
  printString(s, color, backgroundColor);
}

void bread::print(const char *s, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom) {
  // Serial.println("5");
  printString(s, color, backgroundColor, position, topBottom);
}

void bread::print(const char *s, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom, int nudge) {
  // Serial.println("5");
  printString(s, color, backgroundColor, position, topBottom, nudge);
}

void bread::print(const char *s, uint32_t color, uint32_t backgroundColor,
                  int position, int topBottom, int nudge, int lowercaseNumber) {
  // Serial.println("5");
  printString(s, color, backgroundColor, position, topBottom, nudge,
              lowercaseNumber);
}

void bread::print(const char *s, uint32_t color, uint32_t backgroundColor,
                  int topBottom) {
  // Serial.println("6");
  printString(s, color, backgroundColor, 0, topBottom);
}

void bread::print(int i) {
  // Serial.println("7");
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, defaultColor);
  // Serial.println(buffer);
}

// void bread::print(int i, int position) {
//   char buffer[15];
//   itoa(i, buffer, 10);
//   printString(buffer, defaultColor, 0xffffff, position);
// }

void bread::print(int i, uint32_t color) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color);
}

void bread::print(int i, uint32_t color, int position) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color, 0xffffff, position);
}

void bread::print(int i, uint32_t color, int position, int topBottom) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color, 0xffffff, position, topBottom);
}
void bread::print(int i, uint32_t color, int position, int topBottom,
                  int nudge) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color, 0xffffff, position, topBottom, nudge);
}
void bread::print(int i, uint32_t color, int position, int topBottom, int nudge,
                  int lowercase) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color, 0xffffff, position, topBottom, nudge);
}

void bread::print(int i, uint32_t color, uint32_t backgroundColor) {
  char buffer[15];
  itoa(i, buffer, 10);
  printString(buffer, color, backgroundColor);
}

void bread::printMenuReminder(int menuDepth, uint32_t color) {
  uint8_t columnMask[5] = // 'JumperlessFontmap', 500x5px
      {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000};
  uint8_t graphicRow[3] = {0x00, 0x00, 0x00};

  if (menuDepth > 6) {
    menuDepth = 6;
  }

  switch (menuDepth) {
  case 1:

    graphicRow[2] = 0b00000000;
    graphicRow[1] = 0b00010000;
    graphicRow[0] = 0b00010000;
    break;
  case 2:
    graphicRow[2] = 0b00000000;
    graphicRow[1] = 0b00001000;
    graphicRow[0] = 0b00011000;

    break;

  case 3:
    graphicRow[2] = 0b00000000;
    graphicRow[1] = 0b00000100;
    graphicRow[0] = 0b00011100;

    break;

  case 4:

    graphicRow[2] = 0b00000000;
    graphicRow[1] = 0b00000010;
    graphicRow[0] = 0b00011110;

    break;

  case 5:

    graphicRow[2] = 0b00000000;
    graphicRow[1] = 0b00000001;
    graphicRow[0] = 0b00011111;

    break;

  case 6:

    graphicRow[2] = 0b00000001;
    graphicRow[1] = 0b00000001;
    graphicRow[0] = 0b00011111;

    break;
  }

  if (color == 0xFFFFFF) {
    color = defaultColor;
  }

  for (int i = 0; i < 3; i++) {

    printGraphicsRow(graphicRow[i], i, color);
  }
}

void bread::printRawRow(uint8_t data, int row, uint32_t color, uint32_t bg, int scale) {

color = scaleBrightness(color, (menuBrightnessSetting/scale));
  if (row <= 60) {
    printGraphicsRow(data, row, color, bg);
  } else {
    for (int i = 0; i < 35; i++) {
      if (bbPixelToNodesMapV5[i][0] == row + 1) {
        leds.setPixelColor(bbPixelToNodesMapV5[i][1], color);
        return;
      }
    }
  }
}

void bread::barGraph(int position, int value, int maxValue, int leftRight,
                     uint32_t color, uint32_t bg) {}
/*

||||||||||||||||||||||||||||||
  |0| |1| |2| |3| |4| |5| |6|
||||||||||||||||||||||||||||||

||||||||||||||||||||||||||||||
  |7| |8| |9| |A| |B| |C| |D|
||||||||||||||||||||||||||||||


*/
void printGraphicsRow(uint8_t data, int row, uint32_t color, uint32_t bg) {
  uint8_t columnMask[5] = // 'JumperlessFontmap', 500x5px
      {0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};

  if (color == 0xFFFFFF) {
    color = defaultColor;
  }
  if (bg == 0xFFFFFF) {

    for (int j = 4; j >= 0; j--) {
      // Serial.println(((data) & columnMask[j]) != 0 ? "1" : "0");
      if (((data)&columnMask[j]) != 0) {
        color = scaleBrightness(color, menuBrightnessSetting);
        leds.setPixelColor(((row) * 5) + j, color);
      } else {
        leds.setPixelColor(((row) * 5) + j, 0);
      }
    }
  } else if (bg == 0xFFFFFE) {

    for (int j = 4; j >= 0; j--) {
      // Serial.println(((data) & columnMask[j]) != 0 ? "1" : "0");
      if (((data)&columnMask[j]) != 0) {
        color = scaleBrightness(color, menuBrightnessSetting);
        leds.setPixelColor(((row) * 5) + j, color);
      } else {
        // leds.getPixelColor(((row) * 5) + j);
        // leds.setPixelColor(((row) * 5) + j, 0);
      }
    }
  } else {
    for (int j = 4; j >= 0; j--) {
      if (((data)&columnMask[j]) != 0) {
        color = scaleBrightness(color, menuBrightnessSetting);
        leds.setPixelColor(((row) * 5) + j, color);
      } else {
        leds.setPixelColor(((row) * 5) + j, bg);
      }
    }
  }
}

void printChar(const char c, uint32_t color, uint32_t bg, int position,
               int topBottom, int nudge, int lowercaseNumber) {

  int charPosition = position;

  color = scaleBrightness(color, menuBrightnessSetting);

  if (topBottom == 1) {
    charPosition = charPosition % 7;
    charPosition += 7;
    if (charPosition > 13) {
      return;
    }
  }
  if (topBottom == 0) {
    if (charPosition > 6) {
      return;
      // charPosition = charPosition % 7;
    }
    charPosition = charPosition % 7;
  }

  charPosition = charPosition % 14;

  charPosition = charPosition * 4;

  if (charPosition > (6 * 4)) {
    charPosition = charPosition + 2;
  }
  // charPosition = charPosition * 4;
  charPosition = charPosition + 2;

  if (color == 0xFFFFFF) {
    color = defaultColor;
  }
  int fontMapIndex = -1;
  int start = 0;

  if (lowercaseNumber > 0 && c >= 48 && c <= 57) {
    start = 90;
  }

  for (int i = start; i < 120; i++) {
    if (c == fontMap[i]) {
      fontMapIndex = i;
      break;
    }
  }
  if (fontMapIndex == -1) {
    return;
  }
  uint8_t columnMask[5] = // 'JumperlessFontmap', 500x5px
      {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000};

  if (bg == 0xFFFFFF) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        if (((font[fontMapIndex][i]) & columnMask[j]) != 0) {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, color);
        } else {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, 0);
        }
      }
    }
  } else if (bg == 0xFFFFFD) {
    for (int j = 0; j < 5; j++) {

      leds.setPixelColor(((charPosition + nudge - 1) * 5) + j, 0);
    }

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        if (((font[fontMapIndex][i]) & columnMask[j]) != 0) {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, color);
        } else {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, 0);
        }
      }
    }
    for (int j = 0; j < 5; j++) {

      leds.setPixelColor(((charPosition + nudge + 3) * 5) + j, 0);
    }
  } else if (bg == 0xFFFFFE) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        if (((font[fontMapIndex][i]) & columnMask[j]) != 0) {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, color);
        } else {
          // leds.setPixelColor((i*5)+j, bg);
        }
      }
    }
  } else {
    if (charPosition + nudge != 0) {
      for (int j = 0; j < 5; j++) {

        leds.setPixelColor(((charPosition + nudge - 1) * 5) + j, bg);
      }
    }
    for (int i = 0; i < 4; i++) {
      if (i < 3) {
        for (int j = 0; j < 5; j++) {
          if (((font[fontMapIndex][i]) & columnMask[j]) != 0) {
            leds.setPixelColor(((charPosition + i + nudge) * 5) + j, color);
          } else {
            leds.setPixelColor(((charPosition + i + nudge) * 5) + j, bg);
          }
        }
      } else {
        for (int j = 0; j < 5; j++) {
          leds.setPixelColor(((charPosition + i + nudge) * 5) + j, bg);
        }
      }
    }
  }
}

void printString(const char *s, uint32_t color, uint32_t bg, int position,
                 int topBottom, int nudge, int lowercaseNumber) {
  // int position = 0;

  for (int i = 0; i < strlen(s); i++) {

    // if (topBottom == 1) {
    //   position = position % 7;

    // } else if (topBottom == 0) {
    //   position = position % 7;

    // } else {
    //   position = position % 14;
    // }
    // Serial.print(s[i]);
    // Serial.print(" ");
    // Serial.println(position);
    // if (i > strlen(s))
    // {
    //     printChar(' ', 0x000000, 0x000000, position, topBottom);
    // } else {
    // Serial.println(position);
    printChar(s[i], color, bg, position, topBottom, nudge, lowercaseNumber);
    // }

    position++;
  }
  // Serial.println();
}

void bread::clear(int topBottom) {
  if (topBottom == -1) {
    for (int i = 0; i < 60; i++) {
      for (int j = 0; j < 5; j++) {
        leds.setPixelColor((i * 5) + j, 0x00, 0x00, 0x00);
      }
    }
  } else if (topBottom == 0) {
    for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 5; j++) {
        leds.setPixelColor((i * 5) + j, 0x00, 0x00, 0x00);
      }
    }
  } else if (topBottom == 1) {
    for (int i = 30; i < 60; i++) {
      for (int j = 0; j < 5; j++) {
        leds.setPixelColor((i * 5) + j, 0x00, 0x00, 0x00);
      }
    }
  }
  // leds.show();
}

void scrollFont() {
  // pauseCore2 = 1;
  //  scroll font
  //  uint8_t font[] = // 'JumperlessFontmap', 500x5px
  //  {0x1f, 0x11, 0x1f, 0x00, 0x12, 0x1f, 0x10, 0x00, 0x1d, 0x15, 0x17, 0x00,
  //  0x11, 0x15, 0x1f, 0x00,};
  uint32_t color = 0x060205;
  int scrollSpeed = 120;
  int scrollPosition = 0;

  uint8_t columnMask[5] = // 'JumperlessFontmap', 500x5px
      {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000};
  while (Serial.available() == 0) {

    for (int i = 0; i < 60; i++) {
      for (int j = 0; j < 5; j++) {
        if ((font[(i + scrollPosition) % 500][0] & columnMask[j]) != 0) {
          // Serial.print("1");
          leds.setPixelColor((i * 5) + j, color);
        } else {
          // Serial.print("0");
          leds.setPixelColor((i * 5) + j, 0x00, 0x00, 0x00);
        }
      }
    }
    leds.show();
    delay(scrollSpeed);
    scrollPosition++;
    if (scrollPosition > 499) {
      scrollPosition = 0;
    }
  }
}


void printTextFromMenu(void)
{

    
    b.clear();
int scroll = 0;
    char f[80] = {' '};
    int index = 0;
    b.clear();
    while (Serial.available() > 0) {
      if (index > 79) {
        break;
      }
      f[index] = Serial.read();
      index++;
      // b.print(f);
      // delayMicroseconds(30);
      // leds.show();
    }

    if (index > 14) {
      scroll = 1;
    }
    f[index] = ' ';
    f[index + 1] = ' ';
    f[index + 2] = ' ';
    index += 3;
    uint32_t color = 0x100010;
    // Serial.print(index);
    defconString[0] = f[0];
    defconString[1] = f[1];
    defconString[2] = f[2];
    defconString[3] = f[3];
    defconString[4] = f[4];
    defconString[5] = f[5];
    defconString[6] = f[6];
    defconString[7] = f[7];
    defconString[8] = f[8];
    defconString[9] = f[9];
    defconString[10] = f[10];
    defconString[11] = f[11];
    defconString[12] = f[12];
    defconString[13] = f[13];
    defconString[14] = f[14];
    defconString[15] = f[15];
    //defconString[16] = f[16];

    defconDisplay = 0;
    unsigned long timerScrollTimer = millis();
    while (Serial.available() != 0) {
      char trash = Serial.read();
    }

int speed = 200000;
    int cycleCount = 15;
    Serial.println("\n\rPress any key to exit\n\r");

    if (scroll == 1) {
Serial.println("scroll wheel to change speed)\n\r");
    }
    
    while (Serial.available() == 0) {
      if (scroll == 1) {
        rotaryEncoderStuff();
        if (encoderDirectionState == UP) {
          if (speed > 10000) {
            speed = speed - 10000;
          } else {
            speed-=500;
          }        if (speed < 0) {
          speed = 0;
        } 
          // Serial.print("\r                          \rspeed = ");
          // Serial.print(speed);
          
          //encoderDirectionState = NONE;
        } else if (encoderDirectionState == DOWN) {
          speed = speed + 10000;
          //           Serial.print("\r                          \rspeed = ");
          // Serial.print(speed);
          //encoderDirectionState = NONE; 
          if (speed > 1000000) {
          speed = 1000000;
        }
        }

        if ((micros() - timerScrollTimer) > speed) {
          // Serial.print("defNudge = ");
          // Serial.println(defNudge);
          timerScrollTimer = micros();
          defNudge--;
          if (defNudge < -3) {
            defNudge = 0;
          cycleCount++;
          
          
          char temp = f[(cycleCount)%index];
          for (int i = 0; i < 15; i++) {
            defconString[i] = defconString[i + 1];
          }
          defconString[15] = temp;
          //b.print(f, color);
          }
          //delay(100);
          //leds.show();
          //showLEDsCore2 =2;
        }
      }
    }

    // while (Serial.available() == 0) {
    //   // b.print(f, color);
    //   // delay(100);
    //   // leds.show();
    // }
}
int attractMode (void){

        if (encoderDirectionState == DOWN) {
        // attractMode = 0;
        defconDisplay = -1;
        netSlot++;
        if (netSlot >= NUM_SLOTS) {
          netSlot = -1;
          defconDisplay = 0;
        }
        Serial.print("netSlot = ");
        Serial.println(netSlot);
        slotChanged = 1;
        showLEDsCore2 = -1;
        encoderDirectionState = NONE;
        return 1;
        // goto menu;
      } else if (encoderDirectionState == UP) {
        // attractMode = 0;
        defconDisplay = -1;
        netSlot--;
        if (netSlot <= -1) {
          netSlot = NUM_SLOTS;
          defconDisplay = 0;
        }
        Serial.print("netSlot = ");
        Serial.println(netSlot);
        slotChanged = 1;
        showLEDsCore2 = -1;
        encoderDirectionState = NONE;
        return 1;
        // goto menu;
      }
      return 0;
}