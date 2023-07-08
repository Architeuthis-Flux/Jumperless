#include "LEDs.h"
#include <Adafruit_NeoPixel.h>
#include "NetsToChipConnections.h"
#include "MatrixStateRP2040.h"
#include "fileParsing.h"

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

rgbColor netColors[MAX_NETS] = {0};

uint8_t saturation = 254;
uint8_t brightness = BRIGHTNESS;

int showLEDsCore2 = 0;

int netNumberC2 = 0;
int onOffC2 = 0;
int nodeC2 = 0;
int brightnessC2 = 0;
int hueShiftC2 = 0;
int lightUpNetCore2 = 0;

#ifdef EEPROMSTUFF
#include <EEPROM.h>
bool debugLEDs = 1; // EEPROM.read(DEBUG_LEDSADDRESS);

#else
bool debugLEDs = 1;
#endif

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
        {0xFF, 0x41, 0x14},
        {0x00, 0xFF, 0x30},
        {0xFF, 0x00, 0x40},
        {0x00, 0xFF, 0x30}};

void initLEDs(void)
{
    debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);
    if (debugLEDs != 0 && debugLEDs != 1)
    {
        debugLEDs = 1;
    }
    EEPROM.write(DEBUG_LEDSADDRESS, debugLEDs);

    pinMode(LED_PIN, OUTPUT);
    delay(20);
    leds.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    delay(20);
    showLEDsCore2 = 1; // Turn OFF all pixels ASAP
    delay(40);
    leds.setBrightness(BRIGHTNESS);
    delay(20);
    EEPROM.commit();
    delay(100);
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

    if (debugLEDs)
    {
        Serial.print("\n\rcolorDistance: ");
        Serial.print(colorDistance);
        Serial.print("\n\r");
        Serial.print("numberOfNets: ");
        Serial.print(numberOfNets);
        Serial.print("\n\rassigning net colors\n\r");
        Serial.print("\n\rNet\t\tR\tG\tB\t\tH\tS\tV");
    }
    delay(6);

    for (int i = 0; i < 8; i++)
    {
        hsvColor netHsv = RgbToHsv(specialNetColors[i]);

        netHsv.v = SPECIALNETBRIGHTNESS;

        rgbColor netRgb = HsvToRgb(netHsv);

        specialNetColors[i] = netRgb;

        netColors[i] = specialNetColors[i];
        net[i].color = netColors[i];

        if (debugLEDs)
        {
            Serial.print("\n\r");
            Serial.print(net[i].name);
            Serial.print("\t");
            Serial.print(net[i].color.r, HEX);
            Serial.print("\t");
            Serial.print(net[i].color.g, HEX);
            Serial.print("\t");
            Serial.print(net[i].color.b, HEX);
            Serial.print("\t\t");
            Serial.print(netHsv.h);
            Serial.print("\t");
            Serial.print(netHsv.s);
            Serial.print("\t");
            Serial.print(netHsv.v);
        }
        delay(10);
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            hsvColor netHsv = RgbToHsv(railColors[j]);

            netHsv.v = RAILBRIGHTNESS;

            rgbColor netRgb = HsvToRgb(netHsv);

            uint32_t color = packRgb(netRgb.r, netRgb.g, netRgb.b); // packRgb(railColors[j].r, railColors[j].g, railColors[j].b);

            leds.setPixelColor(railsToPixelMap[j][i], color); // top positive rail
        }
        /*

        leds.setPixelColor(railsToPixelMap[1][i], railColors[1]); // top negative rail

        leds.setPixelColor(railsToPixelMap[2][i], railColors[2]); // bottom positive rail

        leds.setPixelColor(railsToPixelMap[3][i], railColors[3]); // bottom negative rail
 */
    }
    showLEDsCore2 = 1;
    delay(2);

    int skipSpecialColors = 0;
    uint8_t hue = 8;

    for (int i = 8; i < numberOfNets; i++)
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

        hsvColor netHsv = {hue, saturation, BRIGHTNESS};
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
            Serial.print(brightness);
        }
        delay(3);
    }
}

uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}

void lightUpNet(int netNumber, int node, int onOff, int brightness2, int hueShift)
{

    if (net[netNumber].nodes[1] != 0 && net[netNumber].nodes[1] < 121)
    {

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[netNumber].nodes[j] == 0)
            {
                break;
            }
            if (net[netNumber].nodes[j] < 121)
            {
                if (net[netNumber].nodes[j] == node || node == -1)
                {
                    if (onOff == 1)
                    {

                        uint32_t color;

                        int pcbExtinction = 0;
                        int colorCorrection = 0;
                        int pcbHueShift = 0;

                        if (net[netNumber].nodes[j] >= NANO_D0 && net[netNumber].nodes[j] <= NANO_A7)
                        {
                            pcbExtinction = PCBEXTINCTION;
                            // hueShift += PCBHUESHIFT;
                            colorCorrection = 1;
                        }

                        struct rgbColor colorToShift = {net[netNumber].color.r, net[netNumber].color.g, net[netNumber].color.b};

                        struct rgbColor shiftedColor = shiftHue(colorToShift, hueShift, pcbExtinction, 254);

                        if (colorCorrection != 0)
                        {
                            shiftedColor = pcbColorCorrect(shiftedColor);
                        }

                        color = packRgb(shiftedColor.r, shiftedColor.g, shiftedColor.b);
                        /*
                                                    Serial.print("color: ");
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
                                                    Serial.println(brightness2);
                        */
                        leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], color);
                    }
                    else
                    {
                        leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], 0);
                    }
                }
            }
        }

        showLEDsCore2 = 1;
    }
}

struct rgbColor pcbColorCorrect(rgbColor colorToShift)
{

    uint8_t redShift = 0;
    uint8_t greenShift = 0;
    uint8_t blueShift = 0;

    int testNeg = 0;

    struct rgbColor colorToShiftRgb = colorToShift;

    struct hsvColor colorToShiftHsv = RgbToHsv(colorToShiftRgb);

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

struct rgbColor shiftHue(struct rgbColor colorToShift, int hueShift, int brightnessShift, int saturationShift)

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

void lightUpNode(int node)
{
    uint32_t randColor = random(0, 0xffffff);
    leds.setPixelColor(node, (randColor));
    showLEDsCore2 = 1;
    delay(2000);
    leds.setPixelColor(node, 0);
}

void lightUpRail(int rail, int onOff, int brightness2)
{
    leds.setPixelColor(110, 0xaa0011);

    leds.setPixelColor(83, packRgb((railColors[1].r * brightness2) >> 8, (railColors[1].g * brightness2) >> 8, (railColors[1].b * brightness2) >> 8));
    leds.setPixelColor(108, packRgb((railColors[1].r * brightness2) >> 8, (railColors[1].g * brightness2) >> 8, (railColors[1].b * brightness2) >> 8));
    leds.setPixelColor(109, packRgb((railColors[0].r * brightness2) >> 8, (railColors[0].g * brightness2) >> 8, (railColors[0].b * brightness2) >> 8));
    leds.setPixelColor(96, packRgb((railColors[2].r * brightness2) >> 8, (railColors[2].g * brightness2) >> 8, (railColors[2].b * brightness2) >> 8));
    leds.setPixelColor(106, packRgb((railColors[0].r * brightness2) >> 8, (railColors[0].g * brightness2) >> 8, (railColors[0].b * brightness2) >> 8));
    for (int j = 0; j < 4; j++)
    {
        if (j == rail || rail == -1)
        {
            for (int i = 0; i < 5; i++)
            {

                if (onOff == 1)
                {
                    uint32_t color = packRgb((railColors[j].r * brightness2) >> 8, (railColors[j].g * brightness2) >> 8, (railColors[j].b * brightness2) >> 8);

                    // Serial.println(color,HEX);
                    leds.setPixelColor(railsToPixelMap[j][i], color);
                }
                else
                {
                    leds.setPixelColor(railsToPixelMap[j][i], 0);
                }
            }
        }
    }
    showLEDsCore2 = 1;
    delay(3);
}

void showNets(void)
{

    for (int i = 0; i < numberOfNets; i++)
    {
        for (int j = 0; j < numberOfPaths; j++)
        {
            uint32_t color = packRgb(net[i].color.r, net[i].color.g, net[i].color.b);
            leds.setPixelColor(bbPixelToNodesMap[net[i].nodes[j]], color);
        }
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

void colorWipe(uint32_t color, int wait)
{

    for (int i = 0; i < leds.numPixels(); i++)
    { // For each pixel in strip...

        leds.setPixelColor(i, color); //  Set pixel's color (in RAM)
        showLEDsCore2 = 1;            //  Update strip to match
        delay(wait);                  //  Pause for a moment
    }
}

void rainbowy(int saturation, int brightness, int wait)
{

    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256)
    {
        // strip.rainbow() can take a single argument (first pixel hue) or
        // optionally a few extras: number of rainbow repetitions (default 1),
        // saturation and value (brightness) (both 0-255, similar to the
        // ColorHSV() function, default 255), and a true/false flag for whether
        // to apply gamma correction to provide 'truer' colors (default true).
        leds.rainbow(firstPixelHue, 1, saturation, brightness, true);
        // Above line is equivalent to:
        // strip.rainbow(firstPixelHue, 1, 255, 255, true);
        showLEDsCore2 = 1; // Update strip with new contents
        delay(wait);       // Pause for a moment
    }
}

void clearLEDs(void)
{
    for (int i = 0; i <= LED_COUNT; i++)
    { // For each pixel in strip...

        leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
                                  //  Update strip to match
    }
lightUpRail(-1,1);
    // for (int i = 80; i <= 109; i++)
    // { // For each pixel in strip...
    //     if (nodesToPixelMap[i] > NANO_D0 && nodesToPixelMap[i] < NANO_A7)
    //     {
    //         leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
    //     }
    //     // leds.setPixelColor(i, 0); //  Set pixel's color (in RAM)
    //     //  Update strip to match
    // }

    showLEDsCore2 = 1;
}