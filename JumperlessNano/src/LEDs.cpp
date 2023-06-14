#include "LEDs.h"
#include <Adafruit_NeoPixel.h>
#include "NetsToChipConnections.h"
#include "MatrixStateRP2040.h"

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

rgbColor netColors[MAX_NETS] = {0};

uint8_t saturation = 254;
uint8_t brightness = 254;

void initLEDs(void)
{
    pinMode(LED_PIN, OUTPUT);
    leds.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    leds.show();  // Turn OFF all pixels ASAP
    leds.setBrightness(BRIGHTNESS);
}

void colorWipe(uint32_t color, int wait)
{

    for (int i = 0; i < leds.numPixels(); i++)
    { // For each pixel in strip...

        leds.setPixelColor(i, color); //  Set pixel's color (in RAM)
        leds.show();                  //  Update strip to match
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
        leds.rainbow(firstPixelHue, 1, saturation, 255, true);
        // Above line is equivalent to:
        // strip.rainbow(firstPixelHue, 1, 255, 255, true);
        leds.show(); // Update strip with new contents
        delay(wait); // Pause for a moment
    }
}

void assignNetColors(void)
{
    // numberOfNets = 60;

    uint16_t colorDistance = 255 / numberOfNets;

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
    rgbColor specialNetColors[8] =
        {{00, 00, 00},
         {0x00, 0xFF, 0x30},
         {0xFF, 0x41, 0x14},
         {0xFF, 0x10, 0x40},
         {0xFF, 0x78, 0x00},
         {0xFF, 0x40, 0x78},
         {0xFF, 0xC8, 0xC8},
         {0xC8, 0xFF, 0xC8}};

    rgbColor railColors[4] =
        {
            {0xFF, 0x41, 0x14},
            {0x00, 0xFF, 0x30},
            {0xFF, 0x00, 0x40},
            {0x00, 0xFF, 0x30}};

    Serial.print("colorDistance: ");
    Serial.print(colorDistance);
    Serial.print("\n\r");
    Serial.print("numberOfNets: ");
    Serial.print(numberOfNets);
    Serial.print("\n\rassigning net colors\n\r");

    for (int i = 0; i < 8; i++)
    {
        hsvColor netHsv = RgbToHsv(specialNetColors[i]);

        netColors[i] = specialNetColors[i];
        net[i].color = netColors[i];
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

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {

            uint32_t color = packRgb(railColors[j].r, railColors[j].g, railColors[j].b);

            leds.setPixelColor(railsToPixelMap[j][i], color); // top positive rail
        }
        /*

        leds.setPixelColor(railsToPixelMap[1][i], railColors[1]); // top negative rail

        leds.setPixelColor(railsToPixelMap[2][i], railColors[2]); // bottom positive rail

        leds.setPixelColor(railsToPixelMap[3][i], railColors[3]); // bottom negative rail
 */
    }
    leds.show();

    
    int skipSpecialColors = 0;
    uint8_t hue = 0;

    for (int i = 8; i < numberOfNets; i++)
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        int foundColor = 0;

        for (uint8_t hueScan = ((i - 8) * colorDistance); hueScan < 255; hueScan += (colorDistance))
        {
            for (int k = 0; k < 8; k++)
            {
                hsvColor snColor = RgbToHsv(specialNetColors[k]);

                if (hueScan > snColor.h)
                {
                    if (hueScan - snColor.h < colorDistance)
                    {
                        skipSpecialColors = 1;
                        // Serial.print("skipping special color: ");
                        // Serial.print(k);

                        // continue;
                        break;
                    }
                }
                else if (snColor.h - hueScan < colorDistance)
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
            // hue = hueScan;
        }
        if (foundColor == 0)
        {
        }

        hsvColor netHsv = {hue, saturation, 255};
        netColors[i] = HsvToRgb(netHsv);

        // leds.setPixelColor(i, netColors[i]);

        net[i].color.r = netColors[i].r;
        net[i].color.g = netColors[i].g;
        net[i].color.b = netColors[i].b;
        Serial.print("\n\r");
        Serial.print(net[i].name);
        Serial.print("\t\t");
        Serial.print(net[i].color.r, HEX);
        Serial.print("\t");
        Serial.print(net[i].color.g, HEX);
        Serial.print("\t");
        Serial.print(net[i].color.b, HEX);
        Serial.print("\t\t");
        Serial.print(hue);
        Serial.print("\t");
        Serial.print(saturation);
        Serial.print("\t");
        Serial.print(brightness);
    }
}


uint32_t packRgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint32_t)r << 16 | (uint32_t)g << 8 | b;
}



void lightUpNet(int netNumber, int node, int onOff , int brightness2)
{

    if (net[netNumber].nodes[1] != 0 && net[netNumber].nodes[1] < 62)
    {

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[netNumber].nodes[j] == 0)
            {
                break;
            }
            if (net[netNumber].nodes[j] < 62)
            {
                if (net[netNumber].nodes[j] == node || node == -1)
                {
                    if (onOff == 1)
                    {

                        uint32_t color = packRgb((net[netNumber].color.r * brightness2) >> 8, (net[netNumber].color.g * brightness2) >> 8, (net[netNumber].color.b * brightness2) >> 8);
                        ///Serial.println(color);
                        leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], color);
                    }
                    else
                    {
                        leds.setPixelColor(nodesToPixelMap[net[netNumber].nodes[j]], 0);
                    }
                }
            }
        }

        leds.show();
        delay(1);
    }
}


void lightUpNode(int node)
{

    leds.setPixelColor(nodesToPixelMap[node], 0xffffff);




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
    leds.show();
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