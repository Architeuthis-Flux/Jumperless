// SPDX-License-Identifier: MIT
#include "LEDs.h"
#include "Commands.h"
#include "FileParsing.h"
#include "Graphics.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"
#include "Probing.h"
#include <Adafruit_GFX.h>
// #include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// #include <FastLED.h>

// CRGB probeLEDs[1];

// bool splitLEDs;

#if REV < 4
bool splitLEDs = 0;
Adafruit_NeoPixel bbleds(LED_COUNT + LED_COUNT_TOP, LED_PIN,
                         NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel topleds(1, -1, NEO_GRB + NEO_KHZ800);
#elif REV >= 4
bool splitLEDs = 1;
Adafruit_NeoPixel bbleds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel topleds(LED_COUNT_TOP, LED_PIN_TOP, NEO_GRB + NEO_KHZ800);
#endif

Adafruit_NeoPixel probeLEDs(1, PROBE_LED_PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel probeLEDs(1, 9, NEO_GRB + NEO_KHZ800);

void ledClass::begin(void) {
  if (splitLEDs == 1) {
    topleds.begin();
  }
  bbleds.begin();
}

void ledClass::show(void) {
  if (splitLEDs == 1) {
    topleds.show();
  }
  bbleds.show();
}

void ledClass::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if (n > LED_COUNT && splitLEDs == 1) {
    topleds.setPixelColor(n - LED_COUNT, r, g, b);
  } else {
    bbleds.setPixelColor(n, r, g, b);
  }
}

void ledClass::setPixelColor(uint16_t n, uint32_t c) {
  if (n > LED_COUNT && splitLEDs == 1) {

    topleds.setPixelColor(n - LED_COUNT, c);

  } else {
    bbleds.setPixelColor(n, c);
  }
}

void ledClass::fill(uint32_t c, uint16_t first, uint16_t count) {
  if (splitLEDs == 1) {
    topleds.fill(c, first, count);
  }
  bbleds.fill(c, first, count);
}

void ledClass::setBrightness(uint8_t b) {
  if (splitLEDs == 1) {
    topleds.setBrightness(b);
  }
  bbleds.setBrightness(b);
}

void ledClass::clear(void) {
  if (splitLEDs == 1) {
    topleds.clear();
  }
  bbleds.clear();
}

uint32_t ledClass::getPixelColor(uint16_t n) {
  if (n > LED_COUNT && splitLEDs == 1) {
    return topleds.getPixelColor(n - LED_COUNT);
  } else {
    return bbleds.getPixelColor(n);
  }
}

uint16_t ledClass::numPixels(void) {
  if (splitLEDs == 1) {
    return topleds.numPixels();
  }
  return bbleds.numPixels();
}

ledClass leds;

// ledClass::clear(int start = 0, int end = LED_COUNT+LED_COUNT_TOP) {
//   if (splitLEDs == 1) {
//     topleds.clear();
//   }
//   bbleds.clear();
// }

// Adafruit_NeoMatrix matrix =
//     Adafruit_NeoMatrix(30, 5, LED_PIN,
//                        NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS
//                        +
//                            NEO_MATRIX_PROGRESSIVE,
//  NEO_GRB + NEO_KHZ800);

// const uint16_t colors[] = {matrix.Color(70, 0, 50), matrix.Color(0, 30, 0),
//                            matrix.Color(0, 0, 8)};

rgbColor netColors[MAX_NETS] = {0};

uint8_t saturation = 254;
volatile uint8_t LEDbrightness = DEFAULTBRIGHTNESS;
volatile uint8_t LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
volatile uint8_t LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;

int netNumberC2 = 0;
int onOffC2 = 0;
int nodeC2 = 0;
int brightnessC2 = 0;
int hueShiftC2 = 0;
int lightUpNetCore2 = 0;

int brightenedNet = 0;
int brightenedRail = -1;
int brightenedAmount = 80;

int logoFlash = 0;
int numberOfShownNets = 0;
#ifdef EEPROMSTUFF
#include <EEPROM.h>
bool debugLEDs = 0; //= EEPROM.read(DEBUG_LEDSADDRESS);

#else
bool debugLEDs = 1;
#endif

int netColorMode = EEPROM.read(NETCOLORMODE_ADDRESS);

uint32_t rawSpecialNetColors[8] = // dim
    {0x000000, 0x001C04, 0x1C0702, 0x1C0107,
     0x231111, 0x230913, 0x232323, 0x232323};

uint32_t rawOtherColors[15] = {
    0x010006, // headerglow
    0x6000A8, // logo / status
    0x0055AA, // logoflash / statusflash
    0x301A02, // +8V
    0x120932, // -8V
    0x443434, // UART TX
    0x324244, // UART RX
    0x232323,

    0x380303, // ADC inner
    0x166800, // DAC inner
    0x0005E5, // GPIO inner

    0x400048, // ADC outer
    0x453800, // DAC outer
    0x00a045, // GPIO outer

};

rgbColor specialNetColors[8] = {{00, 00, 00},       {0x00, 0xFF, 0x30},
                                {0xFF, 0x41, 0x14}, {0xFF, 0x10, 0x40},
                                {0xeF, 0x78, 0x7a}, {0xeF, 0x40, 0x7f},
                                {0xFF, 0xff, 0xff}, {0xff, 0xFF, 0xff}};

rgbColor railColors[4] = {{0xFF, 0x32, 0x30},
                          {0x00, 0xFF, 0x30},
                          {0xFF, 0x32, 0x30},
                          {0x00, 0xFF, 0x30}};

uint32_t rawRailColors[3][4] = // depends on supplySwitchPosition 0 = 3.3V, 1 =
                               // 5V, 2 = +-8V
    {{0x1C0110, 0x001C04, 0x1C0110, 0x001C04},

     {0x210904, 0x001C04, 0x210904, 0x001C04},

     {0x301A02, 0x001C04, 0x120932, 0x001C04}};

// int x = matrix.width();
int pass = 0;
// #define DATA_PIN 2
void initLEDs(void) {

  leds.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
                // delay(1);
  leds.show();

  // if (splitLEDs == 1) {
  //   topleds.begin();
  //   topleds.show();
  // }

  int claimedSms[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}};

  // for (int i = 0; i < 4; i++) {
  //   if (pio_sm_is_claimed(pio0, i)) {
  //     claimedSms[0][i] = 1;
  //   }
  //   Serial.print("SM ");
  //   Serial.print(i);
  //   Serial.print(" is claimed: ");
  //   Serial.println(claimedSms[0][i]);
  // }

  probeLEDs.begin();
  probeLEDs.setPixelColor(0, 0x111111);
  probeLEDs.show();

  // Serial.println("\n\rprobeLEDs.begin()\n\r");

  //   for (int i = 0; i < 4; i++) {
  //   if (pio_sm_is_claimed(pio0,i)) {
  //     claimedSms[1][i] = 1;
  //   }
  //   Serial.print("SM ");
  //   Serial.print(i);
  //   Serial.print(" is claimed: ");
  //   Serial.println(claimedSms[1][i]);
  // }

  // EEPROM.commit();
  // delay(20);
}

//uint32_t savedLEDcolors[NUM_SLOTS][LED_COUNT + 1];

int slotLEDpositions[20] = {
    418, 419, 420, 421, 422, 423, 424, 425, 426,
};
int rotaryEncoderPositions[6] = {
    97, // AREF
    95, // D13 (button)
    94, // D12 (encoder A)
    93, // D11 (encoder GND)
    92, // D10 (encoder B)
};

uint32_t slotSelectionColors[12] = {
    0x084080, // preview
    0x005555, // active
    0x102000, // inactive
    0x000000, // off
    0x881261, // preview+active

    0x253500, // rotary encoder High
    0x000060, // rotary encoder Low

    0x550011, // button High
    0x001C05, // button Low

};

void printColorName(int hue) {
  char colorNames[17][14] = {"red          ", "orange      ", "amber       ",
                             "yellow      ",  "chartreuse  ", "green       ",
                             "seafoam    ",   "aqua        ", "blue        ",
                             "royal blue  ",  "violet      ", "purple      ",
                             "pink        ",  "magenta     ", "white       ",
                             "black       ",  "grey        "};
  char colorNamesSim[8][14] = {"red          ", "orange      ", "yellow      ",
                               "green       ",  "blue        ", "purple      ",
                               "pink        "};
  b.clear();
  for (int i = 0; i < 60; i++) {
    hue = i * 4.4;
    hsvColor hsv = {(uint8_t)hue, 254, 10};
    rgbColor rgb = HsvToRgb(hsv);
    Serial.print(i + 1);
    Serial.print("\t");
    Serial.print(hue % 254);
    Serial.print("\t");
    char colorName[14];
    switch (hue % 255) {
    case 0 ... 11:
      sprintf(colorName, "red");
      break;
    case 12 ... 22:
      sprintf(colorName, "orange");
      break;
    case 23 ... 36:
      sprintf(colorName, "amber");
      break;
    case 37 ... 64:
      sprintf(colorName, "yellow");
      break;
    case 65 ... 79:
      sprintf(colorName, "chartreuse");
      break;
    case 80 ... 94:
      sprintf(colorName, "green");
      break;
    case 95 ... 122:
      sprintf(colorName, "aqua");
      break;
    case 123 ... 145:
      sprintf(colorName, "teal");
      break;
    case 146 ... 175:
      sprintf(colorName, "blue");
      break;
    case 176 ... 191:
      sprintf(colorName, "indigo");
      break;
    case 192 ... 210:
      sprintf(colorName, "purple");
      break;
    case 211 ... 223:
      sprintf(colorName, "violet");
      break;
    case 224 ... 235:
      sprintf(colorName, "pink");
      break;
    case 236 ... 255:
      sprintf(colorName, "magenta");
      break;
      // case 246 ... 255:
      //   sprintf(colorName, "white");
      //   break;

    default:
      sprintf(colorName, "grey");
      break;
    }
    Serial.println(colorName);
    if (i == 29) {
      Serial.println();
    }

    // Serial.print(hue/16);
    // Serial.print("\t");
    // Serial.println(colorNames[hue/16]);
    uint32_t color = packRgb(rgb.r, rgb.g, rgb.b);

    b.printRawRow(0b00011111, i, color, 0xffffff);
  }
}

int saveRawColors(int slot) {

  // // if (savedLEDcolors[slot][LED_COUNT] == 0xFFFFFF) // put this to say it was
  // // already saved
  // // {
  // return 0;
  // // }

  // if (slot == -1) {
  //   slot = netSlot;
  // }

  // for (int i = 0; i < 300; i++) {
  //   if (i >= slotLEDpositions[0] && i <= slotLEDpositions[NUM_SLOTS - 1]) {
  //     // savedLEDcolors[slot][i] = slotSelectionColors[1];
  //     //  Serial.print(i);
  //     //  Serial.print("\t");

  //     continue;
  //   }
  //   savedLEDcolors[slot][i] = leds.getPixelColor(i);
  // }
  // savedLEDcolors[slot][LED_COUNT] = 0xAAAAAA;
  return 0;
}

void refreshSavedColors(int slot) {
  // if (slot == -1) {
  //   for (int i = 0; i < NUM_SLOTS; i++) {
  //     savedLEDcolors[i][LED_COUNT] = 0x000000;
  //   }
  // } else {
  //   savedLEDcolors[slot][LED_COUNT] = 0x000000;
  // }
}

void showSavedColors(int slot) {
  if (slot == -1) {
    slot = netSlot;
  }


    clearAllNTCC();
    openNodeFile(slot,0);
    // printNodeFile(slot, 0);
    // clearLEDs();
    clearLEDsExceptRails();
    getNodesToConnect();
    bridgesToPaths();
    // leds.clear();
    clearLEDsExceptRails();

    assignNetColors();

    // saveRawColors(slot);



  showLEDsCore2 = -1;
  // leds.show();
}

void assignNetColors(int preview) {
  // numberOfNets = 60;\


  uint16_t colorDistance = (254 / (numberOfShownNets));
  if (numberOfShownNets < 4) {
    colorDistance = (254 / (4));
  }

  /* rgbColor specialNetColors[8] =
       {0x000000,
        0x00FF80,
        0xFF4114,
        0xFF0040,
        0xFF7800,
        0xFF4078,
        0xFFC8C8,
        0xC8FFC8};
*/
  // leds.setPixelColor(110, rawOtherColors[2]);
  // logoFlash = 2;
  // showLEDsCore2 = 1;
  //  if (debugLEDs) {
  // Serial.print("\n\rcolorDistance: ");
  // Serial.print(colorDistance);
  // Serial.print("\n\r");
  // Serial.print("numberOfNets: ");
  // Serial.println(numberOfNets);
  // Serial.print("numberOfShownNets: ");
  // Serial.println(numberOfShownNets);
  // Serial.print("\n\rassigning net colors\n\r");
  //   Serial.print("\n\rNet\t\tR\tG\tB\t\tH\tS\tV");
  //  delay(1);
  //  }

  for (int i = 1; i < 6; i++) {
    if (net[i].machine == true) {
      rgbColor specialNetRgb = unpackRgb(rawSpecialNetColors[i]);

      net[i].color = specialNetRgb;
      specialNetColors[i] = specialNetRgb;

      netColors[i] = specialNetRgb;
      // continue;
    } else {

      hsvColor netHsv = RgbToHsv(specialNetColors[i]);

      if (i >= 1 && i <= 3) {
        netHsv.v = LEDbrightnessRail;
      } else if (i >= 4 && i <= 7) {
        netHsv.v = LEDbrightnessSpecial;
      }

      uint32_t railColor;

      switch (i) {
      case 1:
        railColor = 0x000801;
        if (brightenedRail == 1 || brightenedRail == 3) {
          rgbColor railRgb = unpackRgb(railColor);
          hsvColor railHsv = RgbToHsv(railRgb);
          railHsv.v += brightenedAmount;
          railRgb = HsvToRgb(railHsv);
          railColor = packRgb(railRgb.r, railRgb.g, railRgb.b);
        }

        netColors[i] = unpackRgb(railColor);
        net[i].color = netColors[i];
        specialNetColors[i] = netColors[i];
        break;
      case 2:
        railColor = logoColors8vSelect[map((long)(railVoltage[0] * 10), -80, 80,
                                           0, 59)];
        netColors[i] = unpackRgb(railColor);
        net[i].color = netColors[i];
        specialNetColors[i] = netColors[i];
        // Serial.print("railVoltage[0]: ");
        // Serial.println(railVoltage[0]);
        // Serial.print("map: ");
        // Serial.println(map((int)(railVoltage[0]*10), -80, 80, 0, 59));
        // Serial.print("hue: ");
        // Serial.println(netHsv.h);
        break;
      case 3:
        railColor = logoColors8vSelect[map((long)(railVoltage[1] * 10), -80, 80,
                                           0, 59)];
        netColors[i] = unpackRgb(railColor);
        net[i].color = netColors[i];
        specialNetColors[i] = netColors[i];
        break;
      case 4:
        railColor =
            logoColors8vSelect[map((long)(dacOutput[0] * 10), -80, 80, 0, 59)];
        netColors[i] = unpackRgb(railColor);
        net[i].color = netColors[i];
        specialNetColors[i] = netColors[i];
        break;
      case 5:
        railColor =
            logoColors8vSelect[map((long)(dacOutput[1] * 10), -80, 80, 0, 59)];
        netColors[i] = unpackRgb(railColor);
        net[i].color = netColors[i];
        specialNetColors[i] = netColors[i];
        break;
      case 6:
        netHsv.h = 240;
        break;
      case 7:
        // netHsv.h = 300;
        break;
      }

      // rgbColor netRgb = HsvToRgb(netHsv);

      // specialNetColors[i] = netRgb;
      // Serial.print("\n\r");
      // Serial.print(i);
      // Serial.print("\t");
      // Serial.print(netRgb.r, HEX);
      // Serial.print("\t");
      // Serial.print(netRgb.g, HEX);
      // Serial.print("\t");
      // Serial.print(netRgb.b, HEX);

      // netColors[i] = specialNetColors[i];
      // net[i].color = netColors[i];
    }

    // if (debugLEDs) {
    //   Serial.print("\n\r");
    //   int netLength = Serial.print(net[i].name);
    //   if (netLength < 8) {
    //     Serial.print("\t");
    //   }
    //   Serial.print("\t");
    //   Serial.print(net[i].color.r, HEX);
    //   Serial.print("\t");
    //   Serial.print(net[i].color.g, HEX);
    //   Serial.print("\t");
    //   Serial.print(net[i].color.b, HEX);
    //   Serial.print("\t\t");
    //   // Serial.print(netHsv.h);
    //   Serial.print("\t");
    //   // Serial.print(netHsv.s);
    //   Serial.print("\t");
    //   // Serial.print(netHsv.v);
    //   delay(10);
    // }
    //
  }

  int skipSpecialColors = 0;
  uint8_t hue = 1;

  int colorSlots[60] = {-1};

  int colorSlots1[20] = {-1};
  int colorSlots2[20] = {-1};
  int colorSlots3[20] = {-1};
  // Serial.print("number of nets: ");
  // Serial.println(numberOfNets);
  // Serial.print("number of shown nets: ");
  // Serial.println(numberOfShownNets);
  if (numberOfNets < 60 && numberOfShownNets > 0) {
    for (int i = 0; i <= numberOfShownNets; i++) {

      colorSlots[i] = abs(224 - ((((i)*colorDistance)))) % 254;
    }

    int backIndex = numberOfShownNets;

    int index1 = 0;
    int index2 = 0;
    int index3 = 0;
    //   int third = (numberOfNets - 8) / 3;

    for (int i = 0; i <= (numberOfShownNets); i++) {
      // Serial.print(colorSlots[i]);
      // Serial.print(" ");
      switch (i % 3) {
      case 0:
        colorSlots1[index1] = colorSlots[i];
        index1++;
        break;
      case 1:
        colorSlots2[index2] = colorSlots[i];
        index2++;
        break;
      case 2:
        colorSlots3[index3] = colorSlots[i];
        index3++;
        // backIndex--;
        break;
      }
    }
    //  if (debugLEDs) {
    // Serial.print("\n\n\rnumber of shown nets: ");
    // Serial.println(numberOfShownNets);
    // Serial.print("colorDistance: ");
    // Serial.println(colorDistance);
    // for (int i = 0; i < index1; i++) {
    //   Serial.print(colorSlots1[i]);
    //   Serial.print(" ");
    // }
    // Serial.println();
    // for (int i = 0; i < index2; i++) {
    //   // colorSlots2[i] = colorSlots2[(i + third) % index2];
    //   Serial.print(colorSlots2[i]);
    //   Serial.print(" ");
    // }

    // Serial.println();
    // for (int i = 0; i < index3; i++) {
    //   Serial.print(colorSlots3[i]);
    //   Serial.print(" ");
    // }
    // Serial.println("\n\r");
    //  }
    //   for (int i = 0; i < index1; i++) {
    //     colorSlots[i] = colorSlots1[(i)%index1];
    //   }
    //   for (int i = 0; i < index2; i++) {
    //     colorSlots[i + index1] = colorSlots2[(i+third)%index2];
    //   }
    //   for (int i = 0; i < index3; i++) {
    //     colorSlots[i + index1 + index2] = colorSlots3[(i+third*2)%index3];
    //   }
    int loop1index = 0;
    int loop2index = 0;
    int loop3index = 0;

    if (netColorMode == 0) {
      loop1index = 0;
      loop2index = 0;
      loop3index = 0;
    }
    if (netColorMode == 1) {
      loop1index = 0;
      loop2index = index2 / 2;
      loop3index = index3 - 1;
    }
    // Serial.print("netColorMode: ");
    // Serial.println(netColorMode);

    // Serial.print("loopInecies: ");
    // Serial.println(index1 + index2 + index3);

    for (int i = 0; i < index1 + index2 + index3; i++) {
      switch (i % 3) {
      case 0:
        colorSlots[i] = colorSlots1[loop1index];
        loop1index++;
        loop1index = loop1index % index1;
        break;
      case 1:

        colorSlots[i] = colorSlots2[loop2index];
        loop2index++;
        loop2index = loop2index % index2;
        break;
      case 2:

        colorSlots[i] = colorSlots3[loop3index];
        loop3index++;
        loop3index = loop3index % index3;
        // loop3index = loop3index % index3;
        // backIndex--;
        break;
      }
    }
  }
  //  if (debugLEDs) {
  // for (int i = 0; i < numberOfShownNets; i++) {

  //   Serial.print(colorSlots[i]);
  //   Serial.print(" ");
  // }
  // Serial.println("\n\n\n\r");
  // }
  // for (int i = 0; i < numberOfNets; i++) {
  //   Serial.println(colorSlots[i]);
  // }
  // Serial.println();
  // Serial.println();
  // int lastColor = numberOfNets - 8;
  // for (int i=0; i<(numberOfNets-8)/2; i++){
  //   int tempColor = colorSlots[i];
  //   colorSlots[i] = colorSlots[lastColor];
  //   colorSlots[lastColor] = tempColor;
  //   lastColor--;

  // }
  // for (int i = 0; i < numberOfShownNets; i++) {
  //   Serial.print("colorSlots[");
  //   Serial.print(i);
  //   Serial.print("]: ");
  //   Serial.println(colorSlots[i]);
  // }
  int frontIndex = 0;
  for (int i = 6; i <= numberOfNets; i++) {
    if (net[i].visible == 0) {
      // Serial.print("net ");
      // Serial.print(i);
      // Serial.println(" is not visible");

      continue;
    }

    int showingReading = 0;
    if (preview == 0) {

    
    for (int a = 0; a < 8; a++) {
      if (i == showADCreadings[a]) {
        // netColors[i] = unpackRgb(rawOtherColors[8]);
        net[i].color = unpackRgb(adcReadingColors[a]);
        netColors[i] = net[i].color;
        showingReading = 1;
        // Serial.print("showing reading: ");
        // Serial.println(i);
        break;
      }
      if (i == gpioNet[a]) {
        net[i].color = unpackRgb(gpioReadingColors[a]);
        netColors[i] = net[i].color;
        // Serial.print("showing gpio: ");
        // Serial.println(i);

        showingReading = 1;
        break;
      }
    }
    }
    if (showingReading == 0 || preview != 0) {

      uint8_t r = 0;
      uint8_t g = 0;
      uint8_t b = 0;

      // int foundColor = 0;
      // Serial.print("\n\ri: ");
      // Serial.println(i);
      // Serial.print("frontIndex: ");
      // Serial.println(frontIndex);

      hue = colorSlots[frontIndex];
      // Serial.print("hue: ");
      // Serial.println(hue);
      frontIndex++;

      hsvColor netHsv = {hue, 254, LEDbrightness};

      if (brightenedNet != 0 && i == brightenedNet) {
        netHsv.v += brightenedAmount;
      }
      // netHsv.v = 200;
      net[i].color = HsvToRgb(netHsv);
      netColors[i] = net[i].color;

      // leds.setPixelColor(i, netColors[i]);

      // net[i].color.r = netColors[i].r;
      // net[i].color.g = netColors[i].g;
      // net[i].color.b = netColors[i].b;
      // if (debugLEDs) {
      //   Serial.print("\n\r");
      //   Serial.print(net[i].name);
      //   Serial.print("\t\t");
      //   Serial.print(net[i].color.r, DEC);
      //   Serial.print("\t");
      //   Serial.print(net[i].color.g, DEC);
      //   Serial.print("\t");
      //   Serial.print(net[i].color.b, DEC);
      //   Serial.print("\t\t");
      //   Serial.print(hue);
      //   Serial.print("\t");
      //   Serial.print(saturation);
      //   Serial.print("\t");
      //   Serial.print(LEDbrightness);
      //   delay(3);
      // }
    }
  }
  // listSpecialNets();
  // listNets();
  // logoFlash = 0;
}

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b) {
  return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

void lightUpNet(int netNumber, int node, int onOff, int brightness2,
                int hueShift, int dontClear, uint32_t forceColor) {
  uint32_t color;
  int pcbExtinction = 0;
  int colorCorrection = 0;
  int pcbHueShift = 0;
  // Serial.print("netNumber: ");
  // Serial.println(netNumber);

  //     Serial.print(" node: ");
  //     Serial.print(node);
  //     Serial.print(" onOff: ");
  //     Serial.print(onOff);
  // return;
  if (netNumber <= 0 || netNumber >= MAX_NETS) {
    return;
  }
  if (net[netNumber].nodes[1] != 0 &&
      net[netNumber].nodes[1] <= 141) { // NANO_A7) {

    for (int j = 0; j < MAX_NODES; j++) {
      if (net[netNumber].nodes[j] <= 0) {
        break;
      }

      if (net[netNumber].machine == true) {
        Serial.println("machine");
        if (net[netNumber].nodes[j] == node || node == -1) {
          if (onOff == 1) {
            if (nodesToPixelMap[net[netNumber].nodes[j]] > 0) {
              leds.setPixelColor(
                  (nodesToPixelMap[net[netNumber].nodes[j]]),
                  scaleDownBrightness(net[netNumber].rawColor));


              if (debugLEDs) {
                Serial.print("net: ");
                Serial.print(netNumber);
                Serial.print(" node: ");
                Serial.print(net[netNumber].nodes[j]);
                Serial.print(" mapped to LED:");
                Serial.println(nodesToPixelMap[net[netNumber].nodes[j]]);

                Serial.print("rawColor: ");
                Serial.println(net[netNumber].rawColor, HEX);
              }
            }
          }
        }
      } else {
        if (net[netNumber].nodes[j] <= NANO_A7) {
          if (net[netNumber].nodes[j] == node || node == -1) {
            if (onOff == 1) {

              pcbExtinction = 0;
              colorCorrection = 0;
              pcbHueShift = 0;

              if (net[netNumber].nodes[j] >= NANO_D0 &&
                  net[netNumber].nodes[j] <= NANO_A7) {
                // pcbExtinction = PCBEXTINCTION;

                // Serial.println (brightness2);
                // hueShift += PCBHUESHIFT;
                colorCorrection = 1;
              }
              // pcbExtinction += (brightness2-DEFAULTBRIGHTNESS);

              struct rgbColor colorToShift = {net[netNumber].color.r,
                                              net[netNumber].color.g,
                                              net[netNumber].color.b};

              if (forceColor != 0xffffff) {
                colorToShift = unpackRgb(forceColor);
              }

              struct rgbColor shiftedColor =
                  shiftHue(colorToShift, hueShift, pcbExtinction, 254);

              if (colorCorrection != 0) {
                // shiftedColor = pcbColorCorrect(shiftedColor);

                uint32_t correctedColor =
                    packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);

                shiftedColor = unpackRgb(scaleBrightness(correctedColor, 100));
              }

              hsvColor shiftedColorHsv = RgbToHsv(shiftedColor);

              if (net[netNumber].specialFunction >= 100 &&
                  net[netNumber].specialFunction <= 105) {
                if (brightness2 != DEFAULTBRIGHTNESS) {
                  shiftedColorHsv.v = brightness2;
                } else {
                  shiftedColorHsv.v = LEDbrightnessRail;
                }
                if (brightenedNet == netNumber) {
                  shiftedColorHsv.v += brightenedAmount;
                }
                shiftedColor = HsvToRgb(shiftedColorHsv);

                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);
                // color = packRgb((shiftedColor.r * LEDbrightnessRail) >> 8,
                // (shiftedColor.g * LEDbrightnessRail) >> 8, (shiftedColor.b
                // * LEDbrightnessRail) >> 8);
                //  Serial.print("rail color: ");
                //  Serial.print(color, HEX);
              } else if (net[netNumber].specialFunction >= 100 &&
                         net[netNumber].specialFunction <= 120) {
                if (brightness2 != DEFAULTBRIGHTNESS) {
                  shiftedColorHsv.v = brightness2;
                } else {
                  shiftedColorHsv.v = LEDbrightnessSpecial;
                }
                if (brightenedNet == netNumber) {
                  shiftedColorHsv.v += brightenedAmount;
                }
                shiftedColor = HsvToRgb(shiftedColorHsv);

                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);
                // color = packRgb((shiftedColor.r * LEDbrightnessSpecial) >>
                // 8, (shiftedColor.g * LEDbrightnessSpecial) >> 8,
                // (shiftedColor.b
                // * LEDbrightnessSpecial) >> 8);
              } else {
                if (brightness2 != DEFAULTBRIGHTNESS) {
                  shiftedColorHsv.v = brightness2;
                } else {
                  shiftedColorHsv.v = LEDbrightness;
                }
                if (brightenedNet == netNumber) {
                  shiftedColorHsv.v += brightenedAmount;
                }
                shiftedColor = HsvToRgb(shiftedColorHsv);

                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);

                // color = packRgb((shiftedColor.r * LEDbrightness) >> 8,
                // (shiftedColor.g * LEDbrightness) >> 8, (shiftedColor.b *
                // LEDbrightness) >> 8);
              }
              int allOnTop = 1;
              if (net[netNumber].nodes[j] >= NANO_D0) {
                rgbColor colorToShift = unpackRgb(color);
                // colorToShift = shiftHue(colorToShift, hueShift);
                hsvColor brighterColor = RgbToHsv(colorToShift);
                if (brightenedNet == netNumber) {
                  shiftedColorHsv.v += brightenedAmount * 1;
                }
                brighterColor.v += PCBEXTINCTION;
                rgbColor bright = HsvToRgb(brighterColor);

                color = packRgb(bright.r, bright.g, bright.b);

                for (int k = 0; k < MAX_NODES; k++) {
                  if (net[netNumber].nodes[k] < NANO_D0 &&
                      net[netNumber].nodes[k] > 0) {
                    allOnTop = 0;
                    break;
                  }
                }
              }
              if (allOnTop == 1) {
                netColors[netNumber] = unpackRgb(color);
              }
              // net[netNumber].rawColor = color;
              // net[netNumber].color = unpackRgb(color);

              if (probeHighlight - 1 !=
                  (nodesToPixelMap[net[netNumber].nodes[j]])) {
                // Serial.print("nodesToPixelMap[net[netNumber].nodes[j]] = ");
                // Serial.println(nodesToPixelMap[net[netNumber].nodes[j]]);
                if (net[netNumber].nodes[j] >= NANO_D0) {
                  if (brightenedNet == netNumber) {
                    color = scaleBrightness(color, brightenedAmount * 3);
                  } else {
                    color = scaleBrightness(color, brightenedAmount * 2);
                  }
                  leds.setPixelColor(
                      (nodesToPixelMap[net[netNumber].nodes[j]]) + 80, color);

                } else {

                  leds.setPixelColor(
                      (nodesToPixelMap[net[netNumber].nodes[j]]),
                      color);


                  // if (logoTopSetting[
                }
              } else {

              }


            } else {

            }
          }
        }
      }
    }
    // turnOffSkippedNodes();
    /*                                                            Serial.print("color:
       "); Serial.print(color,HEX); Serial.print(" r: ");
                                                        Serial.print(shiftedColor.r);
                                                        Serial.print(" g: ");
                                                        Serial.print(shiftedColor.g);
                                                        Serial.print(" b: ");
                                                        Serial.print(shiftedColor.b);
                                                        Serial.print("
       hueShift:
       "); Serial.print(hueShift); Serial.print(" pcbExtinction: ");
                                                        Serial.print(pcbExtinction);
                                                        Serial.print("
       brightness2: "); Serial.println(brightness2);*/
  }
  // showRowAnimation(-1, GND);
  //  showLEDsCore2 = 1;
  showSkippedNodes();
}
unsigned long lastSkippedNodesTime = 0;
int toggleSkippedNodes = 0;
void showSkippedNodes(uint32_t onColor, uint32_t offColor) {
  // return;
  // onColor = 0x1f1f2f;
  // uint32_t offColor = 0x0f000f;

  static int colorCycleOn = 0;
  colorCycleOn++;
  if (colorCycleOn > 254) {
    colorCycleOn = 0;
  }
  static int colorCycleOff = 200;
  colorCycleOff--;
  if (colorCycleOff < 0) {
    colorCycleOff = 254;
  }

  rgbColor onColorRgb = unpackRgb(onColor);
  rgbColor offColorRgb = unpackRgb(offColor);
  hsvColor onColorHsv = RgbToHsv(onColorRgb);
  hsvColor offColorHsv = RgbToHsv(offColorRgb);
  onColorHsv.h = (onColorHsv.h + colorCycleOn) % 254;
  offColorHsv.h = (offColorHsv.h + colorCycleOff) % 254;
  onColorRgb = HsvToRgb(onColorHsv);
  offColorRgb = HsvToRgb(offColorHsv);
  onColor = packRgb(onColorRgb.r, onColorRgb.g, onColorRgb.b);
  offColor = packRgb(offColorRgb.r, offColorRgb.g, offColorRgb.b);

  if (millis() - lastSkippedNodesTime > 1111) {
    // Serial.println("skipped nodes");

    toggleSkippedNodes = !toggleSkippedNodes;

    lastSkippedNodesTime = millis();

  } else {
    // onColor = 0x1f1f2f;
    // onColor = 0;
    // onColor = 0x1f1f2f;
    // onColor = 0;
    // onColor = 0x1f1f2f;
  }

  for (int i = 0; i < numberOfPaths; i++) {

    if (path[i].skip == true) {
      // colorCycleOff = (colorCycleOff) % 254;
      // colorCycleOn = (colorCycleOn + (numberOfUnconnectablePaths)) % 254;
      if (path[i].node1 > 0 && path[i].node1 <= 60) {
        if (toggleSkippedNodes == 1) {

          leds.setPixelColor((path[i].node1 - 1)  + 0, onColor);




        } else {



          leds.setPixelColor((path[i].node1 - 1), onColor);


          //}
        }

      } else if (path[i].node1 >= NANO_D0 && path[i].node1 <= NANO_5V) {
        hsvColor onColorHsv = RgbToHsv(onColorRgb);
        hsvColor offColorHsv = RgbToHsv(offColorRgb);
        onColorHsv.h = (onColorHsv.h + colorCycleOn + 40) % 254;
        offColorHsv.h = (offColorHsv.h + colorCycleOff +40) % 254;
        onColorHsv.v += 90;
        offColorHsv.v += 40;
        onColorHsv.s = 80;
        offColorHsv.s = 120;
        onColorRgb = HsvToRgb(onColorHsv);
        offColorRgb = HsvToRgb(offColorHsv);
        uint32_t onColorHeader = packRgb(onColorRgb.r/3, onColorRgb.g/2, onColorRgb.b);
        uint32_t offColorHeader = packRgb(offColorRgb.r/3, offColorRgb.g/2, offColorRgb.b);
        if (toggleSkippedNodes == 1) {
          for (int j = 0; j < 35; j++) {
            if (bbPixelToNodesMapV5[j][0] == path[i].node1) {
              leds.setPixelColor(bbPixelToNodesMapV5[j][1],onColorHeader);
            }
          }
        } else {
          for (int j = 0; j < 35; j++) {
            if (bbPixelToNodesMapV5[j][0] == path[i].node1) {
              leds.setPixelColor(bbPixelToNodesMapV5[j][1],offColorHeader);
            }
          }
        }
      }

      if (path[i].node2 > 0 && path[i].node2 <= 60) {
        if (toggleSkippedNodes == 0) {

          leds.setPixelColor((path[i].node2 - 1) * 5 + 0, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 0, offColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 1, onColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 1, offColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 2, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 2, offColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 3, onColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 3, offColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 4, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 4, offColor);

        } else {

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 0, onColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 0, offColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 1, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 1, offColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 2, onColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 2, offColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 3, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 3, offColor);

          leds.setPixelColor((path[i].node2 - 1) * 5 + 4, onColor);

          // leds.setPixelColor((path[i].node2 - 1) * 5 + 4, offColor);
          //}
        }

      } else if (path[i].node1 >= NANO_D0 && path[i].node2 <= NANO_5V) {
        hsvColor onColorHsv = RgbToHsv(onColorRgb);
        hsvColor offColorHsv = RgbToHsv(offColorRgb);
        onColorHsv.h = (onColorHsv.h + colorCycleOn + 40) % 254;
        offColorHsv.h = (offColorHsv.h + colorCycleOff + 40) % 254;
        onColorHsv.v += 90;
        offColorHsv.v += 40;
        onColorHsv.s = 80;
        offColorHsv.s = 120;
        onColorRgb = HsvToRgb(onColorHsv);
        offColorRgb = HsvToRgb(offColorHsv);
        uint32_t onColorHeader = packRgb(onColorRgb.r/3, onColorRgb.g/2, onColorRgb.b);
        uint32_t offColorHeader = packRgb(offColorRgb.r/3, offColorRgb.g/2, offColorRgb.b);
        if (toggleSkippedNodes == 0) {
          for (int j = 0; j < 35; j++) {
            if (bbPixelToNodesMapV5[j][0] == path[i].node2) {
              leds.setPixelColor(bbPixelToNodesMapV5[j][1],onColorHeader);
            }
          }
        } else {
          for (int j = 0; j < 35; j++) {
            if (bbPixelToNodesMapV5[j][0] == path[i].node2) {
              leds.setPixelColor(bbPixelToNodesMapV5[j][1],offColorHeader);
            }
          }
        }
      }

      // leds.show();
    }
  }
}

uint32_t scaleBrightness(uint32_t hexColor, int scaleFactor) {

  float scaleFactorF = scaleFactor / 100.0;

  // if (scaleFactor > 0) {
  scaleFactorF += 1.0;
  //}
  // Serial.print("scaleFactorF: ");
  // Serial.println(scaleFactorF);
  // Serial.print("hexColor: ");
  // Serial.println(hexColor, HEX);

  hsvColor colorToShiftHsv = RgbToHsv(unpackRgb(hexColor));

  float hsvF = colorToShiftHsv.v * scaleFactorF;

  colorToShiftHsv.v = (unsigned char)hsvF;

  rgbColor colorToShiftRgb = HsvToRgb(colorToShiftHsv);
  // Serial.print("shifted: ");
  // Serial.println(packRgb(colorToShiftRgb.r, colorToShiftRgb.g,
  // colorToShiftRgb.b), HEX);
  return packRgb(colorToShiftRgb.r, colorToShiftRgb.g, colorToShiftRgb.b);
}

uint32_t scaleDownBrightness(uint32_t hexColor, int scaleFactor,
                             int maxBrightness) {
  int maxR = maxBrightness;
  int maxG = maxBrightness;
  int maxB = maxBrightness;

  int r = (hexColor >> 16) & 0xFF;
  int g = (hexColor >> 8) & 0xFF;
  int b = hexColor & 0xFF;

  int scaledBrightness = hexColor;

  if (r > maxR || g > maxG || b > maxB) {
    scaledBrightness = 0;
    r = r / scaleFactor;
    g = g / scaleFactor;
    b = b / scaleFactor;

    scaledBrightness = scaledBrightness | (r << 16);
    scaledBrightness = scaledBrightness | (g << 8);
    scaledBrightness = scaledBrightness | b;
  }

  return scaledBrightness;
}

uint32_t scaleUpBrightness(uint32_t hexColor, int scaleFactor,
                           int minBrightness) {
  int minR = minBrightness;
  int minG = minBrightness;
  int minB = minBrightness;

  int r = (hexColor >> 16) & 0xFF;
  int g = (hexColor >> 8) & 0xFF;
  int b = hexColor & 0xFF;

  int scaledBrightness = hexColor;

  if (r < minR && g < minG && b < minB) {
    scaledBrightness = 0;
    r = r * scaleFactor;
    g = g * scaleFactor;
    b = b * scaleFactor;

    if (r > 254) {
      r = 254;
    }
    if (g > 254) {
      g = 254;
    }
    if (b > 254) {
      b = 254;
    }

    scaledBrightness = scaledBrightness | (r << 16);
    scaledBrightness = scaledBrightness | (g << 8);
    scaledBrightness = scaledBrightness | b;
  }

  return scaledBrightness;
}

struct rgbColor pcbColorCorrect(rgbColor colorToShift) {

  uint8_t redShift = 0;
  uint8_t greenShift = 0;
  uint8_t blueShift = 0;

  int testNeg = 0;

  struct rgbColor colorToShiftRgb = colorToShift;

  struct hsvColor colorToShiftHsv = RgbToHsv(colorToShiftRgb);

  colorToShiftHsv.v += PCBEXTINCTION;

  if (colorToShiftHsv.h > 100 && colorToShiftHsv.h < 150) {

    // Serial.print("hue: ");
    // Serial.print(colorToShiftHsv.h);
    // Serial.print("\tcolorToShift.r: ");
    // Serial.print(colorToShift.r);
    // Serial.print("\tcolorToShift.g: ");
    // Serial.print(colorToShift.g);
    // Serial.print("\tcolorToShift.b: ");
    // Serial.print(colorToShift.b);

    if (PCBREDSHIFTBLUE < 0) {
      testNeg = colorToShiftRgb.r;
      testNeg -= abs(PCBREDSHIFTBLUE);

      if (testNeg < 0) {
        colorToShiftRgb.r = 0;
      } else {

        colorToShiftRgb.r = colorToShiftRgb.r - abs(PCBREDSHIFTBLUE);
      }
    } else {

      colorToShiftRgb.r = colorToShiftRgb.r + abs(PCBREDSHIFTBLUE);

      if (colorToShiftRgb.r > 254) {
        colorToShiftRgb.r = 254;
      }
    }

    if (PCBGREENSHIFTBLUE < 0) {

      testNeg = colorToShiftRgb.g;
      testNeg -= abs(PCBGREENSHIFTBLUE);

      if (testNeg < 0) {
        colorToShiftRgb.g = 0;
      } else {
        colorToShiftRgb.g = colorToShiftRgb.g - abs(PCBGREENSHIFTBLUE);
      }
    } else {
      colorToShiftRgb.g = colorToShiftRgb.g + abs(PCBGREENSHIFTBLUE);
      if (colorToShiftRgb.g > 254) {
        colorToShiftRgb.g = 254;
      }
    }

    if (PCBBLUESHIFTBLUE < 0) {

      testNeg = colorToShiftRgb.b;

      testNeg -= abs(PCBBLUESHIFTBLUE);

      if (testNeg < 0) {
        colorToShiftRgb.b = 0;
      } else {
        colorToShiftRgb.b = colorToShiftRgb.b - abs(PCBBLUESHIFTBLUE);
      }
    } else {
      colorToShiftRgb.b = colorToShiftRgb.b + abs(PCBBLUESHIFTBLUE);
      if (colorToShiftRgb.b > 254) {
        colorToShiftRgb.b = 254;
      }
    }

    // Serial.print("\t\tShifted.r: ");

    // Serial.print(colorToShiftRgb.r);
    // Serial.print("\tShifted.g: ");
    // Serial.print(colorToShiftRgb.g);
    // Serial.print("\tShifted.b: ");
    // Serial.println(colorToShiftRgb.b);
    // Serial.print("\n\n\r");
  } else if (colorToShiftHsv.h >= 150 && colorToShiftHsv.h < 255) {

    // Serial.print("hue: ");
    // Serial.print(colorToShiftHsv.h);
    // Serial.print("\tcolorToShift.r: ");
    // Serial.print(colorToShift.r);
    // Serial.print("\tcolorToShift.g: ");
    // Serial.print(colorToShift.g);
    // Serial.print("\tcolorToShift.b: ");
    // Serial.print(colorToShift.b);

    if (PCBREDSHIFTPINK < 0) {
      testNeg = colorToShiftRgb.r;
      testNeg -= abs(PCBREDSHIFTPINK);

      if (testNeg < 0) {
        colorToShiftRgb.r = 0;
      } else {

        colorToShiftRgb.r = colorToShiftRgb.r - abs(PCBREDSHIFTPINK);
      }
    } else {

      colorToShiftRgb.r = colorToShiftRgb.r + abs(PCBREDSHIFTPINK);

      if (colorToShiftRgb.r > 254) {
        colorToShiftRgb.r = 254;
      }
    }

    if (PCBGREENSHIFTPINK < 0) {

      testNeg = colorToShiftRgb.g;
      testNeg -= abs(PCBGREENSHIFTPINK);

      if (testNeg < 0) {
        colorToShiftRgb.g = 0;
      } else {
        colorToShiftRgb.g = colorToShiftRgb.g - abs(PCBGREENSHIFTPINK);
      }
    } else {
      colorToShiftRgb.g = colorToShiftRgb.g + abs(PCBGREENSHIFTPINK);
      if (colorToShiftRgb.g > 254) {
        colorToShiftRgb.g = 254;
      }
    }

    if (PCBBLUESHIFTPINK < 0) {

      testNeg = colorToShiftRgb.b;

      testNeg -= abs(PCBBLUESHIFTPINK);

      if (testNeg < 0) {
        colorToShiftRgb.b = 0;
      } else {
        colorToShiftRgb.b = colorToShiftRgb.b - abs(PCBBLUESHIFTPINK);
      }
    } else {
      colorToShiftRgb.b = colorToShiftRgb.b + abs(PCBBLUESHIFTPINK);
      if (colorToShiftRgb.b > 254) {
        colorToShiftRgb.b = 254;
      }
    }
  }
  return colorToShiftRgb;
}

struct rgbColor shiftHue(struct rgbColor colorToShift, int hueShift,
                         int brightnessShift, int saturationShift,
                         int specialFunction)

{

  struct hsvColor colorToShiftHsv = RgbToHsv(colorToShift);

  colorToShiftHsv.h = colorToShiftHsv.h + hueShift;
  colorToShiftHsv.s = colorToShiftHsv.s + saturationShift;

  colorToShiftHsv.v = colorToShiftHsv.v + brightnessShift;

  if (colorToShiftHsv.v > 255) {
    colorToShiftHsv.v = 255;
  }

  if (colorToShiftHsv.s > 255) {
    colorToShiftHsv.s = 255;
  }

  if (colorToShiftHsv.h > 255) {
    colorToShiftHsv.h = colorToShiftHsv.h - 255;
  }

  struct rgbColor colorToShiftRgb = HsvToRgb(colorToShiftHsv);

  return colorToShiftRgb;
}
bool photos = true;

void lightUpNode(int node, uint32_t color) {

  leds.setPixelColor(nodesToPixelMap[node], color);
  showLEDsCore2 = 1;
}
uint32_t dimLogoColor(uint32_t color, int brightness) {
  // return color;
  rgbColor dimColor = unpackRgb(color);
  // if (dimColor.b != 0)
  // {
  //     dimColor.b = dimColor.b * 2;
  // }

  hsvColor colorHsv = RgbToHsv(dimColor);

  colorHsv.v = brightness;
  if (photos == true || true) {
    return packRgb(HsvToRgb(colorHsv).r/3, HsvToRgb(colorHsv).g /3,
                   HsvToRgb(colorHsv).b );
  } else {
    return packRgb(HsvToRgb(colorHsv).r, HsvToRgb(colorHsv).g * 2,
                   HsvToRgb(colorHsv).b * 3);
  }
}

uint32_t logoColors[LOGO_COLOR_LENGTH + 11] = {
    0x800058, 0x750053, 0x700068, 0x650063, 0x600078, 0x550073, 0x500088,
    0x450083, 0x400098, 0x350093, 0x3000A8, 0x2500A3, 0x2000B8, 0x1500B3,
    0x1000C8, 0x0502C3, 0x0204D8, 0x0007E3, 0x0010E8, 0x0015F3, 0x0020F8,
    0x0025FA, 0x0030FF, 0x0035E0, 0x0240BF, 0x0545A0, 0x10509F, 0x15558F,
    0x20607F, 0x25656F, 0x30705F, 0x35754F, 0x40803F, 0x45722F, 0x506518,
    0x55481A, 0x603A2A, 0x653332, 0x702538, 0x751948, 0x791052, 0x7E0562,

};
uint32_t logoColorsCold[LOGO_COLOR_LENGTH + 1];

uint32_t logoColorsHot[LOGO_COLOR_LENGTH + 1];

uint32_t logoColorsPink[LOGO_COLOR_LENGTH + 1];

uint32_t logoColorsGreen[LOGO_COLOR_LENGTH + 1];

uint32_t logoColorsYellow[LOGO_COLOR_LENGTH + 1];

uint32_t logoColors8vSelect[LOGO_COLOR_LENGTH + 11];

uint32_t logoColorsAll[8][LOGO_COLOR_LENGTH + 11];

uint8_t eightSelectHues[LOGO_COLOR_LENGTH + 11] = {
    195, 191, 187, 183, 179, 175, 171, 168, 166, 164, 162, 160, 158, 156, 153,
    148, 140, 130, 120, 111, 104, 99,  96,  93,  91,  89,  87,  85,  83,  81,
    80,  79,  78,  76,  73,  70,  67,  64,  59,  54,  49,  44,  39,  35,  31,
    27,  23,  19,  16,  13,  10,  7,   4,   3,   2,   1,   0,   254, 253, 251,
    248, 242, 236, 230, 224, 218, 212, 207, 202, 199, 197};

void setupSwirlColors(void) {
  rgbColor logoColorsRGB[LOGO_COLOR_LENGTH + 12];
  int fudgeMult = 1;

  for (int i = 0; i < (LOGO_COLOR_LENGTH / 2) + 1; i++) {

    hsvColor connectHSV;
    connectHSV.h = (i * 2 + 130) % 255;
    connectHSV.s = 254;
    connectHSV.v = 254;
    rgbColor connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColorsCold[i] = packRgb(logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8,
                                logoColorsRGB[i].b / 8);
    logoColorsCold[LOGO_COLOR_LENGTH - i] = logoColorsCold[i];
    logoColorsAll[1][LOGO_COLOR_LENGTH - i] = logoColorsCold[i];
    logoColorsAll[1][i] = logoColorsCold[i];

    connectHSV.h = (i * 2 + 230) % 255;
    connectHSV.s = 254;
    connectHSV.v = 254;
    connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColorsHot[i] = packRgb(logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8,
                               logoColorsRGB[i].b / 8);
    logoColorsHot[LOGO_COLOR_LENGTH - i] = logoColorsHot[i];
    logoColorsAll[2][LOGO_COLOR_LENGTH - i] = logoColorsHot[i];
    logoColorsAll[2][i] = logoColorsHot[i];

    connectHSV.h = (i * 2 + 155) % 255;
    connectHSV.s = 254;
    connectHSV.v = 254;
    connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColorsPink[i] = packRgb(logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8,
                                logoColorsRGB[i].b / 8);
    logoColorsPink[LOGO_COLOR_LENGTH - i] = logoColorsPink[i];
    logoColorsAll[3][LOGO_COLOR_LENGTH - i] = logoColorsPink[i];
    logoColorsAll[3][i] = logoColorsPink[i];

    connectHSV.h = (i * 2 + 55) % 255;
    connectHSV.s = 254;
    connectHSV.v = 254;
    connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColorsYellow[i] = packRgb(
        logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8, logoColorsRGB[i].b / 8);

    logoColorsYellow[LOGO_COLOR_LENGTH - i] = logoColorsYellow[i];
    logoColorsAll[4][LOGO_COLOR_LENGTH - i] = logoColorsYellow[i];
    logoColorsAll[4][i] = logoColorsYellow[i];

    connectHSV.h = (i * 2 + 85) % 255;
    connectHSV.s = 254;
    connectHSV.v = 254;
    connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColorsGreen[i] = packRgb(logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8,
                                 logoColorsRGB[i].b / 8);
    logoColorsGreen[LOGO_COLOR_LENGTH - i] = logoColorsGreen[i];
    logoColorsAll[5][LOGO_COLOR_LENGTH - i] = logoColorsGreen[i];
    logoColorsAll[5][i] = logoColorsGreen[i];
  }

  for (int i = 0; i <= LOGO_COLOR_LENGTH + 10; i++) {

    hsvColor connectHSV;
    connectHSV.h = i * (255 / LOGO_COLOR_LENGTH);
    connectHSV.s = 254;
    connectHSV.v = 254;
    rgbColor connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColors[i] = packRgb(logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8,
                            logoColorsRGB[i].b / 8);
    logoColorsAll[0][i] = logoColors[i];
    connectHSV.h = eightSelectHues[i];

    // connectHSV.h = (connectHSV.h - 8);
    // connectHSV.h<0?connectHSV.h = 254 - connectHSV.h:connectHSV.h;
    connectHSV.s = 254;
    connectHSV.v = 254;
    connectRGB = HsvToRgb(connectHSV);
    logoColorsRGB[i] = connectRGB;

    logoColors8vSelect[i] = packRgb(
        logoColorsRGB[i].r / 8, logoColorsRGB[i].g / 8, logoColorsRGB[i].b / 8);

    logoColorsAll[6][(LOGO_COLOR_LENGTH + 10) - i] = logoColors8vSelect[i];
    //  logoColors8vSelect[LOGO_COLOR_LENGTH - i] =   logoColors8vSelect[i];
  }

  //   //delay(2000);
  //   for (int i = 0; i < LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColorsCold: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColorsCold[i], HEX);

  //   }
  //   Serial.println(" ");
  //     for (int i = 0; i <= LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColorsHot: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColorsHot[i], HEX);

  //   }

  //     for (int i = 0; i < LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColorsPink: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColorsPink[i], HEX);

  //   }

  //     for (int i = 0; i < LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColorsGreen: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColorsGreen[i], HEX);

  //   }

  //     for (int i = 0; i < LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColorsYellow: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColorsYellow[i], HEX);

  //   }

  //     for (int i = 0; i < LOGO_COLOR_LENGTH; i++) {
  // Serial.print("logoColors8vSelect: ");
  // Serial.print(i);
  // Serial.print(" ");
  // Serial.println(logoColors8vSelect[i], HEX);

  //   }
}

void logoSwirl(int start, int spread, int probe) {

  // int fiddyNine = 58;

  if (probe == 1) {
    int selectionBrightness = 13;

    // for (int i = 0; i < 42; i++) {

    //   b.printRawRow(0b11111111, i,  logoColorsCold[i], 0xffffff);
    // }
    // leds.show();
    if (connectOrClearProbe == 1 && node1or2 == 0) {
      // for (int i = 0; i < 30; i++) {

      //   b.printRawRow(0b11111111, i,  logoColorsCold[i*2], 0xffffff);
      // }
      //       for (int i = 0; i < 30; i++) {

      //   b.printRawRow(0b11111111, i+31,  logoColorsHot[i*2], 0xffffff);
      // }
      // leds.show();
      // Serial.println("fuck");
      leds.setPixelColor(
          436, dimLogoColor(logoColorsCold[start % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          437,
          dimLogoColor(
              logoColorsCold[(start + (spread)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(438,
                         dimLogoColor(logoColorsCold[(start + (spread * 2)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(439,
                         dimLogoColor(logoColorsCold[(start + (spread * 3)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(440,
                         dimLogoColor(logoColorsCold[(start + (spread * 4)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(441,
                         dimLogoColor(logoColorsCold[(start + (spread * 5)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(442,
                         dimLogoColor(logoColorsCold[(start + (spread * 6)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(443,
                         dimLogoColor(logoColorsCold[(start + (spread * 7)) %
                                                     (LOGO_COLOR_LENGTH - 1)]));
    } else if (connectOrClearProbe == 1 && node1or2 != 0) {
      leds.setPixelColor(
          436, dimLogoColor(logoColorsPink[start % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          437, dimLogoColor(
                   logoColorsPink[(start + (spread)) % (LOGO_COLOR_LENGTH - 1)],
                   selectionBrightness));
      leds.setPixelColor(
          438,
          dimLogoColor(
              logoColorsPink[(start + (spread * 2)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));
      leds.setPixelColor(
          439,
          dimLogoColor(
              logoColorsPink[(start + (spread * 3)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));
      leds.setPixelColor(
          440,
          dimLogoColor(
              logoColorsPink[(start + (spread * 4)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));
      leds.setPixelColor(
          441,
          dimLogoColor(
              logoColorsPink[(start + (spread * 5)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));
      leds.setPixelColor(
          442,
          dimLogoColor(
              logoColorsPink[(start + (spread * 6)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));
      leds.setPixelColor(
          443,
          dimLogoColor(
              logoColorsPink[(start + (spread * 7)) % (LOGO_COLOR_LENGTH - 1)],
              selectionBrightness));

    } else {
      // for (int i = 0; i < (LOGO_COLOR_LENGTH-1); i++) {

      //   b.printRawRow(0b11111111, i,   logoColors8vSelect[i], 0xffffff);

      // }

      // Serial.println("fuck");
      // leds.show();

      leds.setPixelColor(
          436, dimLogoColor(logoColorsHot[start % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          437,
          dimLogoColor(
              logoColorsHot[(start + (spread)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          438,
          dimLogoColor(
              logoColorsHot[(start + (spread * 2)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          439,
          dimLogoColor(
              logoColorsHot[(start + (spread * 3)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          440,
          dimLogoColor(
              logoColorsHot[(start + (spread * 4)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          441,
          dimLogoColor(
              logoColorsHot[(start + (spread * 5)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          442,
          dimLogoColor(
              logoColorsHot[(start + (spread * 6)) % (LOGO_COLOR_LENGTH - 1)]));
      leds.setPixelColor(
          443,
          dimLogoColor(
              logoColorsHot[(start + (spread * 7)) % (LOGO_COLOR_LENGTH - 1)]));
    }

  } else {
    //     for (int i = 0; i < (LOGO_COLOR_LENGTH-1); i++) {

    //  // b.printRawRow(0b11111111, i, 0, 0xffffff);
    // }
    leds.setPixelColor(
        436, dimLogoColor(logoColors[start % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        437,
        dimLogoColor(logoColors[(start + (spread)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        438, dimLogoColor(
                 logoColors[(start + (spread * 2)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        439, dimLogoColor(
                 logoColors[(start + (spread * 3)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        440, dimLogoColor(
                 logoColors[(start + (spread * 4)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        441, dimLogoColor(
                 logoColors[(start + (spread * 5)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        442, dimLogoColor(
                 logoColors[(start + (spread * 6)) % (LOGO_COLOR_LENGTH - 1)]));
    leds.setPixelColor(
        443, dimLogoColor(
                 logoColors[(start + (spread * 7)) % (LOGO_COLOR_LENGTH - 1)]));
  }
  // Serial.println(start % 42);
  // Serial.println((start + 5) % 42);
  // Serial.println((start + 10) % 42);
  // Serial.println((start + 15) % 42);
  // Serial.println((start + 20) % 42);
  // Serial.println((start + 25) % 42);
  // Serial.println((start + 30) % 42);
  // Serial.println((start + 35) % 42);
  // Serial.println("\n\r");
  // leds.show();
  // delay(200);
  //  showLEDsCore2 = 1;
}
bool lightUpName = false;
int brightenNet(int node, int addBrightness) {

  if (node == -1) {
    brightenedNet = 0;
    brightenedRail = -1;
    return -1;
  }
  addBrightness = 00;

  for (int i = 0; i <= numberOfPaths; i++) {

    if (node == path[i].node1 || node == path[i].node2) {
      /// if (brightenedNet != i) {
      brightenedNet = path[i].net;
      // Serial.print("\n\n\rbrightenedNet: ");
      // Serial.println(brightenedNet);
      // Serial.print("net ");
      // Serial.print(path[i].net);
      if (brightenedNet == 1) {
        brightenedRail = 1;
        // lightUpRail(-1, 1, 1, addBrightness);
      } else if (brightenedNet == 2) {
        brightenedRail = 0;
        // lightUpRail(-1, 0, 1, addBrightness);
      } else if (brightenedNet == 3) {
        brightenedRail = 2;
        // lightUpRail(-1, 2, 1, addBrightness);
      }

      assignNetColors();
      return brightenedNet;
    }
  }
  switch (node) {
  case (GND): {
    brightenedNet = 1;
    brightenedRail = 1;
    // lightUpRail(-1, 1, 1, addBrightness);
    return 1;
  }
  case (TOP_RAIL): {
    brightenedNet = 2;
    brightenedRail = 0;
    // lightUpRail(-1, 0, 1, addBrightness);
    return 2;
  }
  case (BOTTOM_RAIL): {
    brightenedNet = 3;
    brightenedRail = 2;
    // lightUpRail(-1, 2, 1, addBrightness);
    return 3;
  }
  }

  // for (int i = 0; i < numberOfNets; i++) {
  //     if (node == net[i].node1 || node == net[i].node2) {
  //       if (brightenedNet != i) {
  //         brightenedNet = i;
  //         net[i].color = unpackRgb(scaleBrightness(packRgb(net[i].color),
  //         addBrightness));
  //         // brightenedRail = -1;
  //        // lightUpNet(i, addBrightness);
  //         return 1;
  //       }
  //     }
  //   }

  // brightenedNet = 0;
  // brightenedRail = 1;
  // lightUpRail(-1, 0, 1, addBrightness);
  // showLEDsCore2 = 2;

  return 0;
}

void lightUpRail(int logo, int rail, int onOff, int brightness2,
                 int switchPosition) {
  /*
  brightness2 = (uint8_t)LEDbrightnessRail;
  Serial.print("\n\rbrightness2: ");
  Serial.print(brightness2);
  Serial.print("\n\r");
  Serial.print("\n\rled brightness: ");
  Serial.print(LEDbrightness);
*/
  // if (brightness2 == -1)
  // {
  //   brightness2 = LEDbrightnessRail;
  // }

  if (logo == -1 && logoFlash == 0) {
    // leds.setPixelColor(436, rawOtherColors[1]);
    // leds.setPixelColor(437, rawOtherColors[1]);
    // leds.setPixelColor(438, rawOtherColors[1]);
    // leds.setPixelColor(439, rawOtherColors[1]);
    // leds.setPixelColor(440, rawOtherColors[1]);
    // leds.setPixelColor(441, rawOtherColors[1]);
    // leds.setPixelColor(442, rawOtherColors[1]);
    // leds.setPixelColor(443, rawOtherColors[1]);
    // Serial.println(RgbToHsv(unpackRgb(0x550008)).v);
  }

  //   for (int i = 400; i <= 429; i++) {
  //     if (leds.getPixelColor(i) == 0 &&
  //         leds.getPixelColor(i) != rawOtherColors[0]) {
  //       leds.setPixelColor(i, rawOtherColors[0]);
  //     }
  //   }
  leds.setPixelColor(402, scaleDownBrightness(0x2000b9, 4, 35));
  leds.setPixelColor(427, scaleDownBrightness(0x0020f9, 4, 35));

  leds.setPixelColor(403, scaleDownBrightness(rawSpecialNetColors[1], 2, 35));
  leds.setPixelColor(428, scaleDownBrightness(rawSpecialNetColors[1], 2, 35));
  leds.setPixelColor(429, scaleDownBrightness(0xa0a000, 5, 35));
  leds.setPixelColor(416, scaleDownBrightness(rawSpecialNetColors[3], 5, 35));
  leds.setPixelColor(426, scaleDownBrightness(rawSpecialNetColors[2], 5, 35));

  if (sfProbeMenu == 1) {
    leds.setPixelColor(430, scaleBrightness(rawOtherColors[8], -40));
    leds.setPixelColor(431, scaleBrightness(rawOtherColors[11], -40));
    leds.setPixelColor(432, 0);
    leds.setPixelColor(433, 0);
    leds.setPixelColor(434, 0);
    leds.setPixelColor(435, 0);

  } else if (sfProbeMenu == 2) {
    leds.setPixelColor(430, 0);
    leds.setPixelColor(431, 0);
    leds.setPixelColor(432, scaleBrightness(rawOtherColors[9], -40));
    leds.setPixelColor(433, scaleBrightness(rawOtherColors[12], -40));
    leds.setPixelColor(434, 0);
    leds.setPixelColor(435, 0);
  } else if (sfProbeMenu == 3) {
    leds.setPixelColor(430, 0);
    leds.setPixelColor(431, 0);
    leds.setPixelColor(432, 0);
    leds.setPixelColor(433, 0);
    leds.setPixelColor(434, scaleBrightness(rawOtherColors[10], -40));
    leds.setPixelColor(435, scaleBrightness(rawOtherColors[13], -40));

  } else {
    if (photos == true && false) {
      leds.setPixelColor(430, scaleBrightness(rawOtherColors[8], -40));
      leds.setPixelColor(431, scaleBrightness(rawOtherColors[11], -40));
      leds.setPixelColor(432, scaleBrightness(rawOtherColors[9], -40));
      leds.setPixelColor(433, scaleBrightness(rawOtherColors[12], -40));
      leds.setPixelColor(434, scaleBrightness(rawOtherColors[10], -40));
      leds.setPixelColor(435, scaleBrightness(rawOtherColors[13], -40));
    } else {
      leds.setPixelColor(430, scaleBrightness(rawOtherColors[8], -40));
      leds.setPixelColor(431, scaleBrightness(rawOtherColors[11], -40));
      leds.setPixelColor(432, scaleBrightness(rawOtherColors[9], -40));
      leds.setPixelColor(433, scaleBrightness(rawOtherColors[12], -40));
      leds.setPixelColor(434, scaleBrightness(rawOtherColors[10], -40));
      leds.setPixelColor(435, scaleBrightness(rawOtherColors[13], -40));
    }
  }
  if (switchPosition == 2) //+-8V
  {
    rawRailColors[switchPosition][0] = scaleDownBrightness(rawOtherColors[3]);
    rawRailColors[switchPosition][2] = scaleDownBrightness(rawOtherColors[4]);
  }
  for (int j = 0; j < 4; j++) {
    if (j == rail || rail == -1) {
      // rgbColor rgbRail = railColors[j];
      // hsvColor hsvRail = RgbToHsv(rgbRail);
      // hsvRail.v = brightness2;
      // Serial.println (rawOtherColors[0], HEX);
      // rgbRail = HsvToRgb(hsvRail);
      // Serial.println (hsvRail.h);
      // Serial.println (hsvRail.s);
      // Serial.println (hsvRail.v);
      // Serial.println ("\n\r");

      uint32_t color = rawRailColors[switchPosition][j];
      for (int i = 0; i < 25; i++) {

        if (onOff == 1) {
          // uint32_t color = packRgb((railColors[j].r * brightness2) >> 8,
          // (railColors[j].g * brightness2) >> 8, (railColors[j].b *
          // brightness2) >> 8);

          /// Serial.println(color,HEX);
          // Serial.print("brightenedRail ");
          // Serial.println(brightenedRail);
          if (brightenedRail == j ||
              ((j == 1 || j == 3) &&
               (brightenedRail == 1 || brightenedRail == 3))) {
            // color = scaleBrightness(color, brightness2);

            if (j % 2 == 0) {
              int powerRail = j / 2;
              color = scaleBrightness(color, 30);
              if (railVoltage[powerRail] < 0.0) {
                if ((i == 24 - (abs((int)(railVoltage[powerRail] * 5))))) {
                  leds.setPixelColor(railsToPixelMap[j][i],
                                     scaleBrightness(0x040416, 290));

                } else if (i >
                           24 - (abs((int)((railVoltage[powerRail] * 5))))) {
                  leds.setPixelColor(railsToPixelMap[j][i], color);
                  // scaleDownBrightness(color, 6, 55));
                } else {
                  leds.setPixelColor(railsToPixelMap[j][i],
                                     scaleBrightness(color, -92));
                  // scaleDownBrightness(color, 10, 195));
                }

              } else if ((i ==
                          abs((int)((railVoltage[powerRail] - 0.1) * 5)))) {

                leds.setPixelColor(railsToPixelMap[j][i],
                                   scaleBrightness(0x040416, 290));
              } else if (i <
                         abs((int)(((railVoltage[powerRail] + 0.1) * 5) - 1))) {
                leds.setPixelColor(railsToPixelMap[j][i], color);
                // scaleDownBrightness(color, 6, 55));
              } else {
                leds.setPixelColor(railsToPixelMap[j][i],
                                   scaleBrightness(color, -92));
                // scaleDownBrightness(color, 10, 195));
              }
            } else {
              leds.setPixelColor(railsToPixelMap[j][i],
                                 scaleBrightness(color, -30));

              //  scaleDownBrightness(color, 5, 55));
            }

          } else {
            // Serial.println("brightness2");
            // Serial.println(brightness2);
            if (j % 2 == 0) {
              int powerRail = j / 2;

              if (railVoltage[powerRail] < 0.0) {
                if ((i == 24 - (abs((int)(railVoltage[powerRail] * 5))))) {
                  leds.setPixelColor(railsToPixelMap[j][i], 0x040416);

                } else if (i >
                           24 - (abs((int)((railVoltage[powerRail] * 5))))) {
                  leds.setPixelColor(railsToPixelMap[j][i],
                                     scaleDownBrightness(color, 6, 45));
                } else {
                  leds.setPixelColor(railsToPixelMap[j][i],
                                     scaleDownBrightness(color, 10, 25));
                }

              } else if ((i ==
                          abs((int)((railVoltage[powerRail] - 0.1) * 5)))) {

                leds.setPixelColor(railsToPixelMap[j][i], 0x040416);
              } else if (i <
                         abs((int)(((railVoltage[powerRail] + 0.1) * 5) - 1))) {
                leds.setPixelColor(railsToPixelMap[j][i],
                                   scaleDownBrightness(color, 6, 45));
              } else {
                leds.setPixelColor(railsToPixelMap[j][i],
                                   scaleDownBrightness(color, 10, 25));
              }
            } else {
              leds.setPixelColor(railsToPixelMap[j][i],
                                 scaleDownBrightness(color, 10, 25));
            }
          }

        } else {
          leds.setPixelColor(railsToPixelMap[j][i], 0);
        }
      }
    }
  }
  

  if (lightUpName == true) {
    leds.setPixelColor(railsToPixelMap[0][20], 0x0010a0);
    leds.setPixelColor(railsToPixelMap[0][21], 0x1f0050);
    leds.setPixelColor(railsToPixelMap[0][22], 0x1d0030);
    leds.setPixelColor(railsToPixelMap[0][23], 0x1e0020);
    leds.setPixelColor(railsToPixelMap[0][24], 0x2f0010);

    leds.setPixelColor(railsToPixelMap[1][20], 0x0040f0);
    leds.setPixelColor(railsToPixelMap[1][21], 0x0f0050);
    leds.setPixelColor(railsToPixelMap[1][22], 0x0d0030);
    leds.setPixelColor(railsToPixelMap[1][23], 0x0e0020);
    leds.setPixelColor(railsToPixelMap[1][24], 0x2f0000);
  }
  // leds.show();
  // showLEDsCore2 = 1;
  // delay(3);
}
int displayMode = 1; // 0 = lines 1= wires

void showNets(void) {
  // Serial.println(rp2040.cpuid());
  // core2busy = true;
  //                if (debugNTCC > 0) {
  //      Serial.println(debugNTCC);
  //    }
  //  if (rp2040.cpuid() == 0) {
  //    core1busy = true;
  //   } else {
  core2busy = true;
  int skipShow = 0;
  //}
  // if (debugNTCC > 0) {
  //   Serial.println(debugNTCC);
  // }

  if (displayMode == 0 || numberOfShownNets > MAX_NETS_FOR_WIRES) {
    assignNetColors();
    for (int i = 0; i <= numberOfNets; i++) {
      // Serial.print(i);
      skipShow = 0;
      for (int j = 0; j < 3; j++) {
        if (showADCreadings[j] == i) {
          skipShow = 1;
          continue;
        }
      }
      if (skipShow == 1) {
        continue;
      }

      lightUpNet(i);
    }
  } else if (displayMode == 1) {
    // if (i==1){
    //   continue;
    // }
    drawWires();
  }
  //               if (debugNTCC > 0) {
  //   Serial.println(debugNTCC);
  // }
  // showLEDsCore2 = 1;
  //     if (rp2040.cpuid() == 0) {
  //   core1busy = false;
  //  } else {
  core2busy = false;
  // }
}

rgbColor HsvToRgb(hsvColor hsv) {
  rgbColor rgb;
  unsigned char region, p, q, t;
  unsigned int h, s, v, remainder;

  if (hsv.s == 0) {
    rgb.r = hsv.v;
    rgb.g = hsv.v;
    rgb.b = hsv.v;
    return rgb;
  }

  // converting to 16 bit to prevent overflow
  h = hsv.h;
  s = hsv.s;
  v = hsv.v;

  region = h / 43;
  remainder = (h - (region * 43)) * 6;

  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  switch (region) {
  case 0:
    rgb.r = v;
    rgb.g = t;
    rgb.b = p;
    break;
  case 1:
    rgb.r = q;
    rgb.g = v;
    rgb.b = p;
    break;
  case 2:
    rgb.r = p;
    rgb.g = v;
    rgb.b = t;
    break;
  case 3:
    rgb.r = p;
    rgb.g = q;
    rgb.b = v;
    break;
  case 4:
    rgb.r = t;
    rgb.g = p;
    rgb.b = v;
    break;
  default:
    rgb.r = v;
    rgb.g = p;
    rgb.b = q;
    break;
  }

  return rgb;
}

hsvColor RgbToHsv(rgbColor rgb) {
  hsvColor hsv;
  unsigned char rgbMin, rgbMax;

  rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b)
                         : (rgb.g < rgb.b ? rgb.g : rgb.b);
  rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b)
                         : (rgb.g > rgb.b ? rgb.g : rgb.b);

  hsv.v = rgbMax;
  if (hsv.v == 0) {
    hsv.h = 0;
    hsv.s = 0;
    return hsv;
  }

  hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
  if (hsv.s == 0) {
    hsv.h = 0;
    return hsv;
  }

  if (rgbMax == rgb.r)
    hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
  else if (rgbMax == rgb.g)
    hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
  else
    hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);

  return hsv;
}

void randomColors(uint32_t color, int wait) {

  int count = 0;

  for (int i = 0; i < leds.numPixels(); i++) {

    count = random(0, 10);
    // if (i > 80)
    // {
    //     count = random(0, 22);
    // }

    byte colorValR = random(2, 0x10);
    byte colorValG = random(2, 0x10);
    byte colorValB = random(2, 0x10);

    color = colorValR << 16 | colorValG << 8 | colorValB;
    switch (count) {
    case 0:
      color = color & 0x00ffff;
      break;
    case 1:
      color = color & 0xff00ff;
      break;
    case 2:
      color = color & 0xffff00;
      break;
    case 3:
      color = color & 0x0000ff;
      break;
    case 4:
      color = color & 0x00ff00;
      break;
    case 5:
      color = color & 0xff0000;
      break;
    default:
      color = color & 0x000000;
      break;
    }
    // color = color | (color >> 1);

    leds.setPixelColor(i, color); //  Set pixel's color (in RAM)
    // lightUpRail(-1, -1, 1, LEDbrightnessRail);
    showLEDsCore2 = 2; //  Update strip to match
                       //  Pause for a moment
  }
  // delay(500);
  delay(wait);
}

void rainbowy(int saturation, int brightness, int wait) {

  hsvColor hsv;
  int bounce = 0;
  int offset = 1;

  for (long j = 0; j < 60; j += 1) {

    for (int i = 0; i < LED_COUNT; i++) {
      float huef;
      float i2 = i;
      float j2 = j;

      // huef = ((i * j)) * 0.1f; //*254.1;
      //  hsv.h = (j*(i*j))%255;
      // hsv.h = (j*(int((sin(j+i)*4))))%254;
      //
      huef = sinf((i2 * (j2)) * 3.0f); //*(sinf((j2*i2))*19.0);
      hsv.h = ((int)(huef)) % 254;

      hsv.s = 254;
      hsv.v = 80;
      rgbColor rgb = HsvToRgb(hsv);
      uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
      // rgbPacked = rgbPacked * i
      leds.setPixelColor((i + offset) % LED_COUNT, rgbPacked);
    }

    offset += 1;
    // offset = offset % 80;
    showLEDsCore2 = 1;
    delayMicroseconds((wait * 1000)); //*((j/20.0)));
  }
}

void startupColors(void) {
  hsvColor hsv;
  int bounce = 0;
  int offset = 1;
  int fade = 0;
  int done = 0;
  // int logoColor = 66;
  // int logoStep = 255/66;

  for (long j = 4; j < 162; j += 2) {
    if (j < DEFAULTBRIGHTNESS / 3) {
      fade = j * 3;
    } else {
      int fadeout = j - DEFAULTBRIGHTNESS;
      if (fadeout < 0) {
        fadeout = 0;
      }
      if (fadeout > DEFAULTBRIGHTNESS) {
        fadeout = DEFAULTBRIGHTNESS;
        done = 1;
        // Serial.println(j);
        //  break;
      }
      fade = DEFAULTBRIGHTNESS - fadeout;
    }

    for (int i = 0; i < LED_COUNT; i++) {
      float huef;
      float i2 = i;
      float j2 = j + 50;

      huef = ((i2 * j2)) * 0.1f; //*254.1;
      // hsv.h = (j*(i*j))%255;
      // hsv.h = (j*(int((sin(j+i)*4))))%254;
      hsv.h = ((int)(huef)) % 254;
      hsv.s = 254;
      // if (((i + offset) % LED_COUNT) == 110)
      // {
      //     hsv.v = 85;
      //     hsv.h = (189 + j);
      // }
      // else
      // {

      //     hsv.v = fade;
      // }
      rgbColor rgb = HsvToRgb(hsv);
      uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
      // rgbPacked = rgbPacked * i

      leds.setPixelColor((i + offset) % LED_COUNT, rgbPacked);
    }

    offset += 1;
    // offset = offset % 80;
    // lightUpRail(1);
    // showLEDsCore2 = 1;
    leds.show();
    if (done == 0) {
      delayMicroseconds((14000)); //*((j/20.0)));
    } else {
      break;
    }
    // Serial.println(j);
  }
  // clearLEDs();
  //  lightUpRail();
  //   showLEDsCore2 = 1;
}

uint32_t chillinColors[LED_COUNT + 200] = {
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003, 0x050003,
    0x050003, 0x050003, 0x050003, 0x050003, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500, 0x000500,
    0x000500, 0x010006, 0x010006, 0x010006, 0x001C04, 0x010006, 0x010006,
    0x010006, 0x010006, 0x010006, 0x010006, 0x010006, 0x010006, 0x010006,
    0x010006, 0x010006, 0x010006, 0x1C0107, 0x010006, 0x010006, 0x010006,
    0x010006, 0x010006, 0x010006, 0x010006, 0x010006, 0x010006, 0x1C0702,
    0x010006, 0x001C04, 0x1C0702, 0x380303, 0x380303, 0x253803, 0x253803,
    0x0000E5, 0x0000E5, 0x07000F, 0x030015, 0x000015, 0x000015, 0x010815,
    0x050E03, 0x07000F, 0x030015, 0x000000};

void startupColorsV5(void) {
  int logo = 1;

  hsvColor hsv;
  int bounce = 0;
  // leds.clear();
  //   for (long j = 150; j < 340; j += 1) {

  //     for (int i = 0; i < LED_COUNT - 9; i++) {
  //       float huef;
  //       float i2 = i;
  //       float j2 = j;

  //       huef = sinf((i2 / (j2)) * 1.5f); //*(sinf((j2/i2))*19.0);
  //       // hsv.h = (j*(i*j))%255;
  //       // hsv.h = (j*(int((sin(j+i)*4))))%254;
  //       hsv.h = ((int)(huef * 255)) % 254;
  //       hsv.s = 254;
  //       if (i < 400) {
  //         hsv.v = 6;
  //       } else {
  //         hsv.v = 16;
  //       }
  //       // hsv.v = 6;
  //       rgbColor rgb = HsvToRgb(hsv);
  //       uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
  //       // rgbPacked = rgbPacked * i
  //       if (i < 300 && logo == 1) {
  //         if (jumperlessText[textMap[i]] == 0) {
  //           leds.setPixelColor(i, rgbPacked);
  //         } else {
  //           leds.setPixelColor(i, 0);
  //         }
  //       } else {
  //         leds.setPixelColor(i, rgbPacked);
  //       }
  //     }

  //     showLEDsCore2 = 3;
  //     delayMicroseconds(500);
  //   }
  //   //delay(1000);
  long jStart = 250;
  for (long j = 250; j >= 0; j -= 1) {

    for (int i = 0; i < LED_COUNT - 9; i++) {
      float huef;

      float i2 = i;
      float j2 = j;

      if (i >= 300) {
        i2 = (LED_COUNT - 9) - i;
        j2 = jStart - j;
      }

      huef = sinf((i2 / (j2)) * 2.0f); //*(sinf((j2/i2))*19.0);
      // hsv.h = (j*(i*j))%255;
      // hsv.h = (j*(int((sin(j+i)*4))))%254;
      hsv.h = ((int)(huef * 255)) % 254;
      hsv.s = 254;
      if (i <= 400) {
        hsv.v = 6;
      } else {
        hsv.v = 16;
      }
      rgbColor rgb = HsvToRgb(hsv);
      if (i >= 300) {

        rgbColor chillin = unpackRgb(chillinColors[i]);
        rgb.r = (((rgb.r) * (j)) + (chillin.r) * ((jStart - j))) / (jStart);
        rgb.g = (((rgb.g) * (j)) + (chillin.g) * ((jStart - j))) / (jStart);
        rgb.b = (((rgb.b) * (j)) + (chillin.b) * ((jStart - j))) / (jStart);
      } else if (j < 60) {
        rgb.r = ((rgb.r * j)) / 60;
        rgb.g = ((rgb.g * j)) / 60;
        rgb.b = ((rgb.b * j)) / 60;

        //   rgb.g = rgb.g;
        //   rgb.b = rgb.b;
      }
      //   rgb.r = (rgb.r + chillin.r) / 2;

      //     rgb.g = (rgb.g + chillin.g) / 2;

      //     rgb.b = (rgb.b + chillin.b) / 2;

      //   rgb.r = ((rgb.r * (jStart - j)) + (chillin.r * j) / jStart) / 2;

      //   rgb.g = ((rgb.g * j) + (chillin.g * (jStart - j)) / jStart) / 2;

      //   rgb.b = ((rgb.b * j) + (chillin.b * (jStart - j)) / jStart) / 2;

      uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);

      // rgbPacked = rgbPacked * i
      if (i < 300 && logo == 1) {

        if (jumperlessText[textMap[i]] == 0) {
          leds.setPixelColor(i, rgbPacked);
        } else {
          leds.setPixelColor(i, 0);
        }
      } else {
        leds.setPixelColor(i, rgbPacked);
      }
      // leds.setPixelColor(i, rgbPacked);
    }
    // leds.setPixelColor(399, 0x151515);

    showLEDsCore2 = 3;
    delayMicroseconds((12));
  }
  // lightUpRail();
  showLEDsCore2 = 1;
  // delay(1000);
}
void rainbowBounce(int wait, int logo) {
  hsvColor hsv;
  int bounce = 0;
  // leds.clear();
  for (long j = 0; j < 140; j += 1) {

    for (int i = 0; i < LED_COUNT - 9; i++) {
      float huef;
      float i2 = i;
      float j2 = j;

      huef = sinf((i2 / (j2)) * 1.5f); //*(sinf((j2/i2))*19.0);
      // hsv.h = (j*(i*j))%255;
      // hsv.h = (j*(int((sin(j+i)*4))))%254;
      hsv.h = ((int)(huef * 255)) % 254;
      hsv.s = 254;
      if (i < 400) {
        hsv.v = 6;
      } else {
        hsv.v = 16;
      }
      // hsv.v = 6;
      rgbColor rgb = HsvToRgb(hsv);
      uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
      // rgbPacked = rgbPacked * i
      if (i < 300 && logo == 1) {
        if (jumperlessText[textMap[i]] == 0) {
          leds.setPixelColor(i, rgbPacked);
        } else {
          leds.setPixelColor(i, 0);
        }
      } else {
        leds.setPixelColor(i, rgbPacked);
      }
    }

    showLEDsCore2 = 3;
    delayMicroseconds((wait * 1000) * ((j / 20.0)));
  }
  for (long j = 140; j >= 0; j -= 1) {

    for (int i = 0; i < LED_COUNT - 9; i++) {
      float huef;
      float i2 = i;
      float j2 = j;

      huef = sinf((i2 / (j2)) * 1.5f); //*(sinf((j2/i2))*19.0);
      // hsv.h = (j*(i*j))%255;
      // hsv.h = (j*(int((sin(j+i)*4))))%254;
      hsv.h = ((int)(huef * 255)) % 254;
      hsv.s = 254;
      if (i < 400) {
        hsv.v = 6;
      } else {
        hsv.v = 16;
      }
      rgbColor rgb = HsvToRgb(hsv);
      uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
      // rgbPacked = rgbPacked * i
      if (i < 300 && logo == 1) {
        if (jumperlessText[textMap[i]] == 0) {
          leds.setPixelColor(i, rgbPacked);
        } else {
          leds.setPixelColor(i, 0);
        }
      } else {
        leds.setPixelColor(i, rgbPacked);
      }
      // leds.setPixelColor(i, rgbPacked);
    }

    showLEDsCore2 = 3;
    delayMicroseconds((wait * 1000) * ((j / 20.0)));
  }
}

struct rgbColor unpackRgb(uint32_t color) {
  struct rgbColor rgb;
  rgb.r = (color >> 16) & 0xFF;
  rgb.g = (color >> 8) & 0xFF;
  rgb.b = color & 0xFF;
  /* Serial.print("r: ");
   Serial.print(rgb.r);
   Serial.print(" g: ");
   Serial.print(rgb.g);
   Serial.print(" b: ");
   Serial.println(rgb.b);*/
  return rgb;
}

// uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b) {
//   return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
// }
uint32_t packRgb(rgbColor rgb) {
  return ((uint32_t)rgb.r << 16) | ((uint32_t)rgb.g << 8) | rgb.b;
}

void clearLEDs(void) {
  for (int i = 0; i <= 436; i++) { // For each pixel in strip...

    leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
                              //  Update strip to match
                              // showLEDsCore2 = 1;
  }
  // lightUpRail();
  //  for (int i = 80; i <= 109; i++)
  //  { // For each pixel in strip...
  //      if (nodesToPixelMap[i] > NANO_D0 && nodesToPixelMap[i] < NANO_A7)
  //      {
  //          leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
  //      }
  //      // leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
  //      //  Update strip to match
  //  }

  // showLEDsCore2 = 1;
}

void clearLEDsExceptRails(void) {
  for (int i = 0; i < 300; i++) {

    leds.setPixelColor(i, 0);
  }

  for (int i = 400; i < 430; i++) {
    if (i != 403 && i != 402 && i != 428 && i != 429 && i != 416 && i != 426 &&
        i != 427) {
      leds.setPixelColor(i, 0);
    }
  }
  // leds.setPixelColor(430, 0x051010);
}

void clearLEDsMiddle(int start, int end) {
  for (int i = start - 1; i < end; i++) {
    if (i > 60) {
      return;
    }
    leds.setPixelColor(i * 5 + 2, 0);
  }
}

void clearLEDsExceptMiddle(int start, int end) {

  if (end != -1) {
    for (int i = start; i < end; i++) {

      leds.setPixelColor(i * 5, 0);
      leds.setPixelColor(i * 5 + 1, 0);
      leds.setPixelColor(i * 5 + 3, 0);
      leds.setPixelColor(i * 5 + 4, 0);
    }
  } else {

    if (start > 0 && start <= 60)
      start = start - 1;
    leds.setPixelColor(start * 5, 0);
    leds.setPixelColor(start * 5 + 1, 0);
    leds.setPixelColor(start * 5 + 3, 0);
    leds.setPixelColor(start * 5 + 4, 0);
  }
}