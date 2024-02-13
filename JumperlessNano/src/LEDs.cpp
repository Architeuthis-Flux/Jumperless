// SPDX-License-Identifier: MIT
#include "LEDs.h"
#include <Adafruit_NeoPixel.h>
#include "NetsToChipConnections.h"
#include "MatrixStateRP2040.h"
#include "FileParsing.h"

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

rgbColor netColors[MAX_NETS] = {0};

uint8_t saturation = 254;
volatile uint8_t LEDbrightness;
volatile uint8_t LEDbrightnessRail;
volatile uint8_t LEDbrightnessSpecial;

int showLEDsCore2 = 0;

int netNumberC2 = 0;
int onOffC2 = 0;
int nodeC2 = 0;
int brightnessC2 = 0;
int hueShiftC2 = 0;
int lightUpNetCore2 = 0;

int logoFlash = 0;

#ifdef EEPROMSTUFF
#include <EEPROM.h>
bool debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);

#else
bool debugLEDs = 1;
#endif

uint32_t rawSpecialNetColors[8] = //dim
    {0x000000,
     0x001C04,
     0x1C0702,
     0x1C0107,
     0x231111,
     0x230913,
     0x232323,
     0x232323};

 uint32_t rawOtherColors[8] =
    {0x020008, // headerglow
     0x550008, // logo / status
     0x0055AA, // logoflash / statusflash
     0x301A02, // +8V
     0x120932, // -8V
     0x443434, // UART TX
     0x324244,  // UART RX  
     0x232323}; // unassigned

rgbColor specialNetColors[8] =
    {{00, 00, 00},
     {0x00, 0xFF, 0x30},
     {0xFF, 0x41, 0x14},
     {0xFF, 0x10, 0x40},
     {0xeF, 0x78, 0x7a},
     {0xeF, 0x40, 0x7f},
     {0xFF, 0xff, 0xff},
     {0xff, 0xFF, 0xff}};

rgbColor railColors[4] =
    {
        {0xFF, 0x32, 0x30},
        {0x00, 0xFF, 0x30},
        {0xFF, 0x32, 0x30},
        {0x00, 0xFF, 0x30}};

uint32_t rawRailColors[3][4] = // depends on supplySwitchPosition 0 = 3.3V, 1 = 5V, 2 = +-8V
    {
        {0x1C0110, 0x001C04, 0x1C0110, 0x001C04},

        {0x210904, 0x001C04, 0x210904, 0x001C04},

        {0x301A02, 0x001C04, 0x120932, 0x001C04}};

void initLEDs(void)
{
    debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);
    if (debugLEDs != 0 && debugLEDs != 1)
    {
        debugLEDs = 1;
    }
    EEPROM.write(DEBUG_LEDSADDRESS, debugLEDs);

    pinMode(LED_PIN, OUTPUT);
    delay(1);
    leds.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    delay(1);
    leds.show();
    delay(2);
    // leds.setBrightness(100);
    delay(2);
    EEPROM.commit();
    delay(20);
}

char LEDbrightnessMenu(void)
{
    char input = ' ';
    Serial.print("\n\r\t\tLED Brightness Menu \t\n\n\r");
    Serial.print("\n\r\tl = LED brightness     =   ");
    Serial.print(LEDbrightness);
    Serial.print("\n\r\tr = Rail brightness    =   ");
    Serial.print(LEDbrightnessRail);
    Serial.print("\n\r\ts = Special brightness =   ");
    Serial.print(LEDbrightnessSpecial);
    Serial.print("\n\r\tt = All types\t");
    Serial.print("\n\n\r\td = Reset to defaults");
    Serial.print("\n\n\r\tb = Rainbow Bounce test");
    Serial.print("\n\r\tc = Random Color test\n\r");

    Serial.print("\n\r\tx = Exit\n\n\r");
    // Serial.print(leds.getBrightness());
    if (LEDbrightness > 50 || LEDbrightnessRail > 50 || LEDbrightnessSpecial > 70)
    {
        // Serial.print("\tBrightness settings above ~50 will cause significant heating, it's not recommended\n\r");
        delay(10);
    }

    while (Serial.available() == 0)
    {
        delayMicroseconds(10);
    }

    input = Serial.read();

    if (input == 'x')
    {
        EEPROM.write(LEDBRIGHTNESSADDRESS, LEDbrightness);
        EEPROM.write(RAILBRIGHTNESSADDRESS, LEDbrightnessRail);
        EEPROM.write(SPECIALBRIGHTNESSADDRESS, LEDbrightnessSpecial);
        EEPROM.commit();

        return ' ';
    }
    else if (input == 'd')
    {
        LEDbrightness = DEFAULTBRIGHTNESS;
        LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
        LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;
        showLEDsCore2 = 1;
        EEPROM.write(LEDBRIGHTNESSADDRESS, LEDbrightness);
        EEPROM.write(RAILBRIGHTNESSADDRESS, LEDbrightnessRail);
        EEPROM.write(SPECIALBRIGHTNESSADDRESS, LEDbrightnessSpecial);
        EEPROM.commit();

        return ' ';
    }
    else if (input == 'l')
    {
        Serial.print("\n\r\t+ = increase\n\r\t- = decrease\n\r\tx = exit\n\r");
        while (input == 'l')
        {

            while (Serial.available() == 0)
                ;
            char input2 = Serial.read();
            if (input2 == '+')
            {
                LEDbrightness += 1;

                if (LEDbrightness > 200)
                {

                    LEDbrightness = 200;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == '-')
            {
                LEDbrightness -= 1;

                if (LEDbrightness < 2)
                {
                    LEDbrightness = 1;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == 'x')
            {
                input = ' ';
            }
            else
            {
            }

            for (int i = 8; i <= numberOfNets; i++)
            {
                lightUpNet(i, -1, 1, LEDbrightness, 0);
            }
            showLEDsCore2 = 1;

            if (Serial.available() == 0)
            {

                Serial.print("LED brightness:  ");
                Serial.print(LEDbrightness);
                Serial.print("\n\r");
                if (LEDbrightness > 50)
                {
                    // Serial.print("Brightness settings above ~50 will cause significant heating, it's not recommended\n\r");
                }
            }
        }
    }
    else if (input == 'r')
    {
        Serial.print("\n\r\t+ = increase\n\r\t- = decrease\n\r\tx = exit\n\r");
        while (input == 'r')
        {

            while (Serial.available() == 0)
                ;
            char input2 = Serial.read();
            if (input2 == '+')
            {

                LEDbrightnessRail += 1;

                if (LEDbrightnessRail > 200)
                {

                    LEDbrightnessRail = 200;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == '-')
            {

                LEDbrightnessRail -= 1;

                if (LEDbrightnessRail < 2)
                {
                    LEDbrightnessRail = 1;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == 'x')
            {
                input = ' ';
            }
            else
            {
            }
            lightUpRail(-1, -1, 1, LEDbrightnessRail);

            if (Serial.available() == 0)
            {

                Serial.print("Rail brightness:  ");
                Serial.print(LEDbrightnessRail);
                Serial.print("\n\r");
                if (LEDbrightnessRail > 50)
                {
                    // Serial.println("Brightness settings above ~50 will cause significant heating, it's not recommended\n\n\r");
                }
            }
        }

        // Serial.print(input);
        Serial.print("\n\r");
    }

    else if (input == 's')
    {
        // Serial.print("\n\r\t+ = increase\n\r\t- = decrease\n\r\tx = exit\n\n\r");
        while (input == 's')
        {

            while (Serial.available() == 0)
                ;
            char input2 = Serial.read();
            if (input2 == '+')
            {

                LEDbrightnessSpecial += 1;

                if (LEDbrightnessSpecial > 200)
                {

                    LEDbrightnessSpecial = 200;
                }

                // showLEDsCore2 = 1;
            }
            else if (input2 == '-')
            {

                LEDbrightnessSpecial -= 1;

                if (LEDbrightnessSpecial < 2)
                {
                    LEDbrightnessSpecial = 1;
                }

                // showLEDsCore2 = 1;
            }
            else if (input2 == 'x')
            {
                input = ' ';
            }
            else
            {
            }

            for (int i = 0; i < 8; i++)
            {
                lightUpNet(i, -1, 1, LEDbrightnessSpecial, 0);
            }
            showLEDsCore2 = 1;

            if (Serial.available() == 0)
            {

                Serial.print("Special brightness:  ");
                Serial.print(LEDbrightnessSpecial);
                Serial.print("\n\r");
                if (LEDbrightnessSpecial > 70)
                {
                    // Serial.print("Brightness settings above ~70 for special nets will cause significant heating, it's not recommended\n\n\r ");
                }
            }
        }

        // Serial.print(input);
        Serial.print("\n\r");
    }
    else if (input == 't')
    {

        Serial.print("\n\r\t+ = increase\n\r\t- = decrease\n\r\tx = exit\n\n\r");
        while (input == 't')
        {

            while (Serial.available() == 0)
                ;
            char input2 = Serial.read();
            if (input2 == '+')
            {

                LEDbrightness += 1;
                LEDbrightnessRail += 1;
                LEDbrightnessSpecial += 1;

                if (LEDbrightness > 200)
                {

                    LEDbrightness = 200;
                }
                if (LEDbrightnessRail > 200)
                {

                    LEDbrightnessRail = 200;
                }
                if (LEDbrightnessSpecial > 200)
                {

                    LEDbrightnessSpecial = 200;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == '-')
            {

                LEDbrightness -= 1;
                LEDbrightnessRail -= 1;
                LEDbrightnessSpecial -= 1;

                if (LEDbrightness < 2)
                {
                    LEDbrightness = 1;
                }
                if (LEDbrightnessRail < 2)
                {
                    LEDbrightnessRail = 1;
                }
                if (LEDbrightnessSpecial < 2)
                {
                    LEDbrightnessSpecial = 1;
                }

                showLEDsCore2 = 1;
            }
            else if (input2 == 'x')
            {
                input = ' ';
            }
            else
            {
            }

            for (int i = 8; i <= numberOfNets; i++)
            {
                lightUpNet(i, -1, 1, LEDbrightness, 0);
            }

            lightUpRail(-1, -1, 1, LEDbrightnessRail);
            for (int i = 0; i < 8; i++)
            {
                lightUpNet(i, -1, 1, LEDbrightnessSpecial, 0);
            }
            showLEDsCore2 = 1;

            if (Serial.available() == 0)
            {

                Serial.print("LED brightness:      ");
                Serial.print(LEDbrightness);
                Serial.print("\n\r");
                Serial.print("Rail brightness:     ");
                Serial.print(LEDbrightnessRail);
                Serial.print("\n\r");
                Serial.print("Special brightness:  ");
                Serial.print(LEDbrightnessSpecial);
                Serial.print("\n\r");
                if (LEDbrightness > 50 || LEDbrightnessRail > 50 || LEDbrightnessSpecial > 70)
                {
                    // Serial.print("Brightness settings above ~50 will cause significant heating, it's not recommended\n\n\r ");
                }
            }
        }
    }
    else if (input == 'b')
    {
        Serial.print("\n\rPress any key to exit\n\n\r");
        while (Serial.available() == 0)
        {
            rainbowBounce(40);
        }

        input = '!'; // this tells the main fuction to reset the leds
    }
    else if (input == 'c')
    {
        Serial.print("\n\rPress any key to exit\n\n\r");
        while (Serial.available() == 0)
        {
            randomColors(0, 90);
        }
        delay(10000);
        input = '!';
    }
    else
    {
        EEPROM.write(LEDBRIGHTNESSADDRESS, LEDbrightness);
        EEPROM.write(RAILBRIGHTNESSADDRESS, LEDbrightnessRail);
        EEPROM.write(SPECIALBRIGHTNESSADDRESS, LEDbrightnessSpecial);
        EEPROM.commit();
        assignNetColors();

        return input;
    }
    return input;
}

void assignNetColors(void)
{
    // numberOfNets = 60;

    uint16_t colorDistance = (255 / (numberOfNets - 2));

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
    leds.setPixelColor(110, rawOtherColors[2]);
    logoFlash = 2;
    // showLEDsCore2 = 1;
    if (debugLEDs)
    {
        Serial.print("\n\rcolorDistance: ");
        Serial.print(colorDistance);
        Serial.print("\n\r");
        Serial.print("numberOfNets: ");
        Serial.print(numberOfNets);
        Serial.print("\n\rassigning net colors\n\r");
        Serial.print("\n\rNet\t\tR\tG\tB\t\tH\tS\tV");
        delay(6);
    }

    for (int i = 1; i < 8; i++)
    {
        if (net[i].machine == true)
        {
            rgbColor specialNetRgb = unpackRgb(rawSpecialNetColors[i]);

            net[i].color = specialNetRgb;
            specialNetColors[i] = specialNetRgb;

            netColors[i] = specialNetRgb;
            // continue;
        }
        else
        {

            hsvColor netHsv = RgbToHsv(specialNetColors[i]);

            if (i >= 1 && i <= 3)
            {
                netHsv.v = LEDbrightnessRail;
            }
            else if (i >= 4 && i <= 7)
            {
                netHsv.v = LEDbrightnessSpecial;
            }

            rgbColor netRgb = HsvToRgb(netHsv);

            specialNetColors[i] = netRgb;

            netColors[i] = specialNetColors[i];
            net[i].color = netColors[i];
        }

        if (debugLEDs)
        {
            Serial.print("\n\r");
            int netLength = Serial.print(net[i].name);
            if (netLength < 8)
            {
                Serial.print("\t");
            }
            Serial.print("\t");
            Serial.print(net[i].color.r, HEX);
            Serial.print("\t");
            Serial.print(net[i].color.g, HEX);
            Serial.print("\t");
            Serial.print(net[i].color.b, HEX);
            Serial.print("\t\t");
            // Serial.print(netHsv.h);
            Serial.print("\t");
            // Serial.print(netHsv.s);
            Serial.print("\t");
            // Serial.print(netHsv.v);
            delay(10);
        }
        //
    }

    int skipSpecialColors = 0;
    uint8_t hue = 8;

    for (int i = 8; i < numberOfNets; i++)
    {
        // if(net[i].nodes[0] == -1){
        //     continue;
        // }
        
        if (net[i].machine == true)
        {
            // Serial.println("number of nets: ");
            // Serial.println(numberOfNets);
            // rgbColor specialNetRgb = unpackRgb(rawSpecialNetColors[i]);

            // net[i].color = specialNetRgb;
            // specialNetColors[i] = specialNetRgb;

            // netColors[i] = specialNetRgb;
            // continue;
            //leds.setPixelColor(nodesToPixelMap[i], net[i].rawColor);
            netColors[i] = unpackRgb(net[i].rawColor);

        }
        else
        {

            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;

            int foundColor = 0;

            for (uint8_t hueScan = hue + (colorDistance / 4); hueScan <= 254; hueScan += (colorDistance))
            {
                for (int k = 0; k < 8; k++)
                {
                    hsvColor snColor = RgbToHsv(specialNetColors[k]);

                    if (hueScan > snColor.h)
                    {
                        if (hueScan - snColor.h < colorDistance / 2)
                        {
                            skipSpecialColors = 1;
                            // Serial.print("skipping special color: ");
                            // Serial.print(k);

                            // continue;
                            break;
                        }
                    }
                    else if (snColor.h - hueScan < colorDistance / 2)
                    {
                        skipSpecialColors = 1;
                        // continue;
                        // Serial.print("skipping special color: ");
                        // Serial.print(k);
                        break;
                    }
                    else if (hueScan - hue < colorDistance)
                    {
                        skipSpecialColors = 1;
                        // continue;
                        // Serial.print("skipping special color: ");
                        // Serial.print(k);
                        break;
                    }
                }
                if (skipSpecialColors == 1)
                {
                    skipSpecialColors = 0;
                    continue;
                }
                else
                {
                    foundColor = 1;
                    hue = hueScan;
                    break;
                }

                if (i == numberOfNets && foundColor == 0)
                {
                    // hueScan = 0;
                }
            }
            if (foundColor == 0)
            {
            }

            hsvColor netHsv = {hue, 254, LEDbrightness};
            // netHsv.v = 200;
            netColors[i] = HsvToRgb(netHsv);

            // leds.setPixelColor(i, netColors[i]);

            net[i].color.r = netColors[i].r;
            net[i].color.g = netColors[i].g;
            net[i].color.b = netColors[i].b;
            if (debugLEDs)
            {

                Serial.print("\n\r");
                Serial.print(net[i].name);
                Serial.print("\t\t");
                Serial.print(net[i].color.r, DEC);
                Serial.print("\t");
                Serial.print(net[i].color.g, DEC);
                Serial.print("\t");
                Serial.print(net[i].color.b, DEC);
                Serial.print("\t\t");
                Serial.print(hue);
                Serial.print("\t");
                Serial.print(saturation);
                Serial.print("\t");
                Serial.print(LEDbrightness);
                delay(3);
            }
        }
    }
    // logoFlash = 0;
}

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

void lightUpNet(int netNumber, int node, int onOff, int brightness2, int hueShift)
{
    uint32_t color;
    int pcbExtinction = 0;
    int colorCorrection = 0;
    int pcbHueShift = 0;
    // Serial.print("netNumber: ");
    // Serial.print(netNumber);

    //     Serial.print(" node: ");
    //     Serial.print(node);
    //     Serial.print(" onOff: ");
    //     Serial.print(onOff);
    if (net[netNumber].nodes[1] != 0 && net[netNumber].nodes[1] <= NANO_A7)
    {

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[netNumber].nodes[j] <= 0)
            {
                break;
            
            }

            if (net[netNumber].machine == true)
            {



                if (net[netNumber].nodes[j] == node || node == -1)
                {
                    if (onOff == 1)
                    {
                        if (nodesToPixelMap[net[netNumber].nodes[j]] > 0)
                        {
                            leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], scaleDownBrightness(net[netNumber].rawColor));
                        

                        if (debugLEDs)
                        {
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
                        // leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], net[netNumber].color.r, net[netNumber].color.g, net[netNumber].color.b);
                    }
                    else
                    {
                        // Serial.print("net: ");
                        // Serial.print(netNumber);
                        // Serial.print("  onOff:  ");
                        // Serial.println(onOff);
                        leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], 0);
                    }
                }
            }
            else
            {
                if (net[netNumber].nodes[j] <= NANO_A7)
                {
                    if (net[netNumber].nodes[j] == node || node == -1)
                    {
                        if (onOff == 1)
                        {

                            pcbExtinction = 0;
                            colorCorrection = 0;
                            pcbHueShift = 0;

                            if (net[netNumber].nodes[j] >= NANO_D0 && net[netNumber].nodes[j] <= NANO_A7)
                            {
                                pcbExtinction = PCBEXTINCTION;

                                // Serial.println (brightness2);
                                //  hueShift += PCBHUESHIFT;
                                //  colorCorrection = 1;
                            }
                            // pcbExtinction += (brightness2-DEFAULTBRIGHTNESS);

                            struct rgbColor colorToShift = {net[netNumber].color.r, net[netNumber].color.g, net[netNumber].color.b};

                            struct rgbColor shiftedColor = shiftHue(colorToShift, hueShift, pcbExtinction, 254);

                            if (colorCorrection != 0)
                            {
                                shiftedColor = pcbColorCorrect(shiftedColor);
                            }

                            hsvColor shiftedColorHsv = RgbToHsv(shiftedColor);

                            if (net[netNumber].specialFunction >= 100 && net[netNumber].specialFunction <= 105)
                            {
                                if (brightness2 != DEFAULTBRIGHTNESS)
                                {
                                    shiftedColorHsv.v = brightness2;
                                }
                                else
                                {
                                    shiftedColorHsv.v = LEDbrightnessRail;
                                }
                                shiftedColor = HsvToRgb(shiftedColorHsv);

                                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);
                                // color = packRgb((shiftedColor.r * LEDbrightnessRail) >> 8, (shiftedColor.g * LEDbrightnessRail) >> 8, (shiftedColor.b * LEDbrightnessRail) >> 8);
                                // Serial.print("rail color: ");
                                // Serial.print(color, HEX);
                            }
                            else if (net[netNumber].specialFunction >= 106 && net[netNumber].specialFunction <= 120)
                            {
                                if (brightness2 != DEFAULTBRIGHTNESS)
                                {
                                    shiftedColorHsv.v = brightness2;
                                }
                                else
                                {
                                    shiftedColorHsv.v = LEDbrightnessSpecial;
                                }

                                shiftedColor = HsvToRgb(shiftedColorHsv);

                                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);
                                // color = packRgb((shiftedColor.r * LEDbrightnessSpecial) >> 8, (shiftedColor.g * LEDbrightnessSpecial) >> 8, (shiftedColor.b * LEDbrightnessSpecial) >> 8);
                            }
                            else
                            {
                                if (brightness2 != DEFAULTBRIGHTNESS)
                                {
                                    shiftedColorHsv.v = brightness2;
                                }
                                else
                                {
                                    shiftedColorHsv.v = LEDbrightness;
                                }

                                shiftedColor = HsvToRgb(shiftedColorHsv);

                                color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);

                                // color = packRgb((shiftedColor.r * LEDbrightness) >> 8, (shiftedColor.g * LEDbrightness) >> 8, (shiftedColor.b * LEDbrightness) >> 8);
                            }

                            if (hueShift != 0)
                            {
                                rgbColor colorToShift = unpackRgb(color);
                                colorToShift = shiftHue(colorToShift, hueShift);
                                color = packRgb(colorToShift.r, colorToShift.g, colorToShift.b);
                            }
                            netColors[netNumber] = unpackRgb(color);
                            net[netNumber].rawColor = color;

                            leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], color);
                            if (debugLEDs)
                            {
                                Serial.print("net: ");
                                Serial.print(netNumber);
                                Serial.print(" node: ");
                                Serial.print(net[netNumber].nodes[j]);
                                Serial.print(" mapped to LED:");

                                Serial.println(nodesToPixelMap[net[netNumber].nodes[j]]);
                            }
                        }
                        else
                        {
                            leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], 0);
                        }
                    }
                }
            }
        }
        turnOffSkippedNodes();
        /*                                                            Serial.print("color: ");
                                                            Serial.print(color,HEX);
                                                            Serial.print(" r: ");
                                                            Serial.print(shiftedColor.r);
                                                            Serial.print(" g: ");
                                                            Serial.print(shiftedColor.g);
                                                            Serial.print(" b: ");
                                                            Serial.print(shiftedColor.b);
                                                            Serial.print(" hueShift: ");
                                                            Serial.print(hueShift);
                                                            Serial.print(" pcbExtinction: ");
                                                            Serial.print(pcbExtinction);
                                                            Serial.print(" brightness2: ");
                                                            Serial.println(brightness2);*/
    }
    // showLEDsCore2 = 1;
}

void turnOffSkippedNodes(void)
{

    for (int i = 0; i < numberOfPaths; i++)
    {

        if (path[i].skip == true)
        {
            leds.setPixelColor(nodesToPixelMap[path[i].node1], 0);
            leds.setPixelColor(nodesToPixelMap[path[i].node2], 0);
        }
    }
}
uint32_t scaleDownBrightness(uint32_t hexColor, int scaleFactor, int maxBrightness)
{
    int maxR = maxBrightness;
    int maxG = maxBrightness;
    int maxB = maxBrightness;

    int r = (hexColor >> 16) & 0xFF;
    int g = (hexColor >> 8) & 0xFF;
    int b = hexColor & 0xFF;

    int scaledBrightness = hexColor;

    if (r > maxR || g > maxG || b > maxB)
    {
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

uint32_t scaleUpBrightness(uint32_t hexColor, int scaleFactor, int minBrightness)
{
    int minR = minBrightness;
    int minG = minBrightness;
    int minB = minBrightness;

    int r = (hexColor >> 16) & 0xFF;
    int g = (hexColor >> 8) & 0xFF;
    int b = hexColor & 0xFF;

    int scaledBrightness = hexColor;

    if (r < minR && g < minG && b < minB)
    {
        scaledBrightness = 0;
        r = r * scaleFactor;
        g = g * scaleFactor;
        b = b * scaleFactor;

        if (r > 254)
        {
            r = 254;
        }
        if (g > 254)
        {
            g = 254;
        }
        if (b > 254)
        {
            b = 254;
        }

        scaledBrightness = scaledBrightness | (r << 16);
        scaledBrightness = scaledBrightness | (g << 8);
        scaledBrightness = scaledBrightness | b;
    }


    return scaledBrightness;

}

struct rgbColor pcbColorCorrect(rgbColor colorToShift)
{

    uint8_t redShift = 0;
    uint8_t greenShift = 0;
    uint8_t blueShift = 0;

    int testNeg = 0;

    struct rgbColor colorToShiftRgb = colorToShift;

    struct hsvColor colorToShiftHsv = RgbToHsv(colorToShiftRgb);

    // colorToShiftHsv.v += PCBEXTINCTION;

    if (colorToShiftHsv.h > 100 && colorToShiftHsv.h < 150)
    {

        // Serial.print("hue: ");
        // Serial.print(colorToShiftHsv.h);
        // Serial.print("\tcolorToShift.r: ");
        // Serial.print(colorToShift.r);
        // Serial.print("\tcolorToShift.g: ");
        // Serial.print(colorToShift.g);
        // Serial.print("\tcolorToShift.b: ");
        // Serial.print(colorToShift.b);

        if (PCBREDSHIFTBLUE < 0)
        {
            testNeg = colorToShiftRgb.r;
            testNeg -= abs(PCBREDSHIFTBLUE);

            if (testNeg < 0)
            {
                colorToShiftRgb.r = 0;
            }
            else
            {

                colorToShiftRgb.r = colorToShiftRgb.r - abs(PCBREDSHIFTBLUE);
            }
        }
        else
        {

            colorToShiftRgb.r = colorToShiftRgb.r + abs(PCBREDSHIFTBLUE);

            if (colorToShiftRgb.r > 254)
            {
                colorToShiftRgb.r = 254;
            }
        }

        if (PCBGREENSHIFTBLUE < 0)
        {

            testNeg = colorToShiftRgb.g;
            testNeg -= abs(PCBGREENSHIFTBLUE);

            if (testNeg < 0)
            {
                colorToShiftRgb.g = 0;
            }
            else
            {
                colorToShiftRgb.g = colorToShiftRgb.g - abs(PCBGREENSHIFTBLUE);
            }
        }
        else
        {
            colorToShiftRgb.g = colorToShiftRgb.g + abs(PCBGREENSHIFTBLUE);
            if (colorToShiftRgb.g > 254)
            {
                colorToShiftRgb.g = 254;
            }
        }

        if (PCBBLUESHIFTBLUE < 0)
        {

            testNeg = colorToShiftRgb.b;

            testNeg -= abs(PCBBLUESHIFTBLUE);

            if (testNeg < 0)
            {
                colorToShiftRgb.b = 0;
            }
            else
            {
                colorToShiftRgb.b = colorToShiftRgb.b - abs(PCBBLUESHIFTBLUE);
            }
        }
        else
        {
            colorToShiftRgb.b = colorToShiftRgb.b + abs(PCBBLUESHIFTBLUE);
            if (colorToShiftRgb.b > 254)
            {
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
    }
    else if (colorToShiftHsv.h >= 150 && colorToShiftHsv.h < 255)
    {

        // Serial.print("hue: ");
        // Serial.print(colorToShiftHsv.h);
        // Serial.print("\tcolorToShift.r: ");
        // Serial.print(colorToShift.r);
        // Serial.print("\tcolorToShift.g: ");
        // Serial.print(colorToShift.g);
        // Serial.print("\tcolorToShift.b: ");
        // Serial.print(colorToShift.b);

        if (PCBREDSHIFTPINK < 0)
        {
            testNeg = colorToShiftRgb.r;
            testNeg -= abs(PCBREDSHIFTPINK);

            if (testNeg < 0)
            {
                colorToShiftRgb.r = 0;
            }
            else
            {

                colorToShiftRgb.r = colorToShiftRgb.r - abs(PCBREDSHIFTPINK);
            }
        }
        else
        {

            colorToShiftRgb.r = colorToShiftRgb.r + abs(PCBREDSHIFTPINK);

            if (colorToShiftRgb.r > 254)
            {
                colorToShiftRgb.r = 254;
            }
        }

        if (PCBGREENSHIFTPINK < 0)
        {

            testNeg = colorToShiftRgb.g;
            testNeg -= abs(PCBGREENSHIFTPINK);

            if (testNeg < 0)
            {
                colorToShiftRgb.g = 0;
            }
            else
            {
                colorToShiftRgb.g = colorToShiftRgb.g - abs(PCBGREENSHIFTPINK);
            }
        }
        else
        {
            colorToShiftRgb.g = colorToShiftRgb.g + abs(PCBGREENSHIFTPINK);
            if (colorToShiftRgb.g > 254)
            {
                colorToShiftRgb.g = 254;
            }
        }

        if (PCBBLUESHIFTPINK < 0)
        {

            testNeg = colorToShiftRgb.b;

            testNeg -= abs(PCBBLUESHIFTPINK);

            if (testNeg < 0)
            {
                colorToShiftRgb.b = 0;
            }
            else
            {
                colorToShiftRgb.b = colorToShiftRgb.b - abs(PCBBLUESHIFTPINK);
            }
        }
        else
        {
            colorToShiftRgb.b = colorToShiftRgb.b + abs(PCBBLUESHIFTPINK);
            if (colorToShiftRgb.b > 254)
            {
                colorToShiftRgb.b = 254;
            }
        }
    }
    return colorToShiftRgb;
}

struct rgbColor shiftHue(struct rgbColor colorToShift, int hueShift, int brightnessShift, int saturationShift, int specialFunction)

{

    struct hsvColor colorToShiftHsv = RgbToHsv(colorToShift);

    colorToShiftHsv.h = colorToShiftHsv.h + hueShift;
    colorToShiftHsv.s = colorToShiftHsv.s + saturationShift;

    colorToShiftHsv.v = colorToShiftHsv.v + brightnessShift;

    if (colorToShiftHsv.v > 255)
    {
        colorToShiftHsv.v = 255;
    }

    if (colorToShiftHsv.s > 255)
    {
        colorToShiftHsv.s = 255;
    }

    if (colorToShiftHsv.h > 255)
    {
        colorToShiftHsv.h = colorToShiftHsv.h - 255;
    }

    struct rgbColor colorToShiftRgb = HsvToRgb(colorToShiftHsv);

    return colorToShiftRgb;
}

void lightUpNode(int node, uint32_t color)
{


    leds.setPixelColor(nodesToPixelMap[node],color);
    showLEDsCore2 = 1;

}

void lightUpRail(int logo, int rail, int onOff, int brightness2, int switchPosition)
{
    /*
    brightness2 = (uint8_t)LEDbrightnessRail;
    Serial.print("\n\rbrightness2: ");
    Serial.print(brightness2);
    Serial.print("\n\r");
    Serial.print("\n\rled brightness: ");
    Serial.print(LEDbrightness);
*/

    brightness2 = LEDbrightnessRail;

    if (logo == -1 && logoFlash == 0)
    {
        leds.setPixelColor(110, rawOtherColors[1]);
        // Serial.println(RgbToHsv(unpackRgb(0x550008)).v);
    }

    for (int i = 80; i <= 109; i++)
    {
        if (leds.getPixelColor(i) == 0 && leds.getPixelColor(i) != rawOtherColors[0])
        {
            leds.setPixelColor(i, rawOtherColors[0]);

        }
        
    }

    leds.setPixelColor(83, scaleDownBrightness( rawSpecialNetColors[1]));
    leds.setPixelColor(108,scaleDownBrightness( rawSpecialNetColors[1]));
    leds.setPixelColor(109, scaleDownBrightness(rawSpecialNetColors[2]));
    leds.setPixelColor(96, scaleDownBrightness(rawSpecialNetColors[3]));
    leds.setPixelColor(106, scaleDownBrightness(rawSpecialNetColors[2]));

    if (switchPosition == 2) //+-8V
    {
        rawRailColors[switchPosition][0] = scaleDownBrightness(rawOtherColors[3]);
        rawRailColors[switchPosition][2] = scaleDownBrightness(rawOtherColors[4]);
    }
    for (int j = 0; j < 4; j++)
    {
        if (j == rail || rail == -1)
        {
            // rgbColor rgbRail = railColors[j];
            // hsvColor hsvRail = RgbToHsv(rgbRail);
            // hsvRail.v = brightness2;
             //Serial.println (rawOtherColors[0], HEX);
            // rgbRail = HsvToRgb(hsvRail);
            // Serial.println (hsvRail.h);
            // Serial.println (hsvRail.s);
            // Serial.println (hsvRail.v);
            // Serial.println ("\n\r");

            uint32_t color = rawRailColors[switchPosition][j];
            for (int i = 0; i < 5; i++)
            {

                if (onOff == 1)
                {
                    // uint32_t color = packRgb((railColors[j].r * brightness2) >> 8, (railColors[j].g * brightness2) >> 8, (railColors[j].b * brightness2) >> 8);

                    /// Serial.println(color,HEX);
                    leds.setPixelColor(railsToPixelMap[j][i], scaleDownBrightness(color));
                }
                else
                {
                    leds.setPixelColor(railsToPixelMap[j][i], 0);
                }
            }
        }
    }
    // leds.show();
    // showLEDsCore2 = 1;
    // delay(3);
}

void showNets(void)
{

    for (int i = 0; i <= numberOfNets; i++)
    {
        //Serial.print(i);

        lightUpNet(i);
    }
    showLEDsCore2 = 1;
}

rgbColor HsvToRgb(hsvColor hsv)
{
    rgbColor rgb;
    unsigned char region, p, q, t;
    unsigned int h, s, v, remainder;

    if (hsv.s == 0)
    {
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

    switch (region)
    {
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

hsvColor RgbToHsv(rgbColor rgb)
{
    hsvColor hsv;
    unsigned char rgbMin, rgbMax;

    rgbMin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbMax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
    if (hsv.s == 0)
    {
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

void randomColors(uint32_t color, int wait)
{

    int count = 0;

    for (int i = 0; i < leds.numPixels(); i++)
    {

        count = random(0, 8);
        if (i > 80)
        {
            count = random(0, 22);
        }

        byte colorValR = random(60, 0x3f);
        byte colorValG = random(60, 0x30);
        byte colorValB = random(60, 0x3f);

        color = colorValR << 16 | colorValG << 8 | colorValB;
        switch (count)
        {
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
        lightUpRail(-1, -1, 1, LEDbrightnessRail);
        showLEDsCore2 = 2; //  Update strip to match
                           //  Pause for a moment
    }
    delay(500);
    delay(wait);
}

void rainbowy(int saturation, int brightness, int wait)
{

    hsvColor hsv;
    int bounce = 0;
    int offset = 1;

    for (long j = 0; j < 60; j += 1)
    {

        for (int i = 0; i < LED_COUNT; i++)
        {
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

void startupColors(void)
{
    hsvColor hsv;
    int bounce = 0;
    int offset = 1;
    int fade = 0;
    int done = 0;
    // int logoColor = 66;
    // int logoStep = 255/66;

    for (long j = 4; j < 162; j += 2)
    {
        if (j < DEFAULTBRIGHTNESS / 3)
        {
            fade = j * 3;
        }
        else
        {
            int fadeout = j - DEFAULTBRIGHTNESS;
            if (fadeout < 0)
            {
                fadeout = 0;
            }
            if (fadeout > DEFAULTBRIGHTNESS)
            {
                fadeout = DEFAULTBRIGHTNESS;
                done = 1;
                // Serial.println(j);
                //  break;
            }
            fade = DEFAULTBRIGHTNESS - fadeout;
        }

        for (int i = 0; i < LED_COUNT; i++)
        {
            float huef;
            float i2 = i;
            float j2 = j + 50;

            huef = ((i2 * j2)) * 0.1f; //*254.1;
            // hsv.h = (j*(i*j))%255;
            // hsv.h = (j*(int((sin(j+i)*4))))%254;
            hsv.h = ((int)(huef)) % 254;
            hsv.s = 254;
            if (((i + offset) % LED_COUNT) == 110)
            {
                hsv.v = 85;
                hsv.h = (189 + j);
            }
            else
            {

                hsv.v = fade;
            }
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
        if (done == 0)
        {
            delayMicroseconds((14000)); //*((j/20.0)));
        }
        else
        {
            break;
        }
        // Serial.println(j);
    }
    clearLEDs();
    // lightUpRail();
    //  showLEDsCore2 = 1;
}
void rainbowBounce(int wait)
{
    hsvColor hsv;
    int bounce = 0;
    for (long j = 0; j < 40; j += 1)
    {

        for (int i = 0; i < LED_COUNT; i++)
        {
            float huef;
            float i2 = i;
            float j2 = j;

            huef = sinf((i2 / (j2)) * 1.0f); //*(sinf((j2/i2))*19.0);
            // hsv.h = (j*(i*j))%255;
            // hsv.h = (j*(int((sin(j+i)*4))))%254;
            hsv.h = ((int)(huef * 255)) % 254;
            hsv.s = 254;
            hsv.v = 30;
            rgbColor rgb = HsvToRgb(hsv);
            uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
            // rgbPacked = rgbPacked * i
            leds.setPixelColor(i, rgbPacked);
        }

        showLEDsCore2 = 2;
        delayMicroseconds((wait * 1000) * ((j / 20.0)));
    }
    for (long j = 40; j >= 0; j -= 1)
    {

        for (int i = 0; i < LED_COUNT; i++)
        {
            float huef;
            float i2 = i;
            float j2 = j;

            huef = sinf((i2 / (j2)) * 1.0f); //*(sinf((j2/i2))*19.0);
            // hsv.h = (j*(i*j))%255;
            // hsv.h = (j*(int((sin(j+i)*4))))%254;
            hsv.h = ((int)(huef * 255)) % 254;
            hsv.s = 254;
            hsv.v = 30;
            rgbColor rgb = HsvToRgb(hsv);
            uint32_t rgbPacked = packRgb(rgb.r, rgb.g, rgb.b);
            // rgbPacked = rgbPacked * i
            leds.setPixelColor(i, rgbPacked);
        }

        showLEDsCore2 = 2;
        delayMicroseconds((wait * 1000) * ((j / 20.0)));
    }
}

struct rgbColor unpackRgb(uint32_t color)
{
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
void clearLEDs(void)
{
    for (int i = 0; i <= 116; i++)
    { // For each pixel in strip...

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
