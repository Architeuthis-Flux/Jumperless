#include "LEDs.h"
#include <Adafruit_NeoPixel.h>
#include "NetsToChipConnections.h"
#include "MatrixStateRP2040.h"

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void initLEDs(void)
{

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

void assignNetColors (void)
{
    //numberOfNets = 60;




    uint16_t colorDistance = 0xffff / numberOfNets;
    uint32_t netColors[numberOfNets];

    uint32_t specialNetColors[8] = 
   {0x000000,
    0x00FF80,
    0xFF4114,
    0xFF0040,
    0xFF7800,
    0xFF4078,
    0xFFC8C8,
    0xC8FFC8};

    uint32_t railColors[4] =
    {
        0xFF4114,
        0x00FF80,
        0xFF0040,
        0x00FF80};



Serial.print("colorDistance: ");
Serial.print(colorDistance);
Serial.print("\n\r");
Serial.print("numberOfNets: "); 
Serial.print(numberOfNets);
    Serial.print("\n\rassigning net colors\n\r");



for (int i = 0; i < 8; i++)
{

    netColors[i] = specialNetColors[i];
            net[i].color = netColors[i];
        Serial.print("\n\r");
        Serial.print(net[i].name);
        Serial.print("\t");
        Serial.print(net[i].color, HEX);

}

for (int i = 0; i < 5; i++)
{

    
 
leds.setPixelColor(railsToPixelMap[0][i], railColors[0] ); //top positive rail


leds.setPixelColor(railsToPixelMap[1][i], railColors[1]); //top negative rail


leds.setPixelColor(railsToPixelMap[2][i], railColors[2]); //bottom positive rail


leds.setPixelColor(railsToPixelMap[3][i], railColors[3]); //bottom negative rail

//leds.show();

}

delay(1000);
int skipSpecialColors = 0;

    for(int i = 8; i < numberOfNets; i++)
    {
        
        for (int j = 1; j < 8; j++)
        {
            if (leds.gamma32(leds.ColorHSV(colorDistance * (j + skipSpecialColors),255,200) - leds.ColorHSV(specialNetColors[j],255,200)) < (colorDistance))
            {
                Serial.print("\n\rskipping color: ");
                Serial.print(colorDistance * (j + skipSpecialColors),HEX);
                Serial.print("\n\r");
                Serial.print(i);
                Serial.print("\t");
                
                Serial.print(leds.gamma32(leds.ColorHSV(colorDistance * (j + skipSpecialColors),255,200)),HEX);
                Serial.print("  ==   ");
                Serial.print(leds.gamma32(leds.ColorHSV(specialNetColors[j],255,200)),HEX);
                Serial.print("\t");
                Serial.print(j);
                Serial.print("\n\r");

                skipSpecialColors++;
                colorDistance = 0xffff / (numberOfNets + skipSpecialColors);
                Serial.print("colorDistance: ");
                Serial.print(colorDistance,HEX);
                Serial.print("\n\r");
                
            } else if (leds.gamma32(leds.ColorHSV(colorDistance * (j + skipSpecialColors),255,200) - leds.ColorHSV(specialNetColors[j],255,200)) < (colorDistance))
            {
                Serial.print("\n\rskipping color: ");
                Serial.print(colorDistance * (j + skipSpecialColors),HEX);
                Serial.print("\n\r");
                Serial.print(leds.gamma32(leds.ColorHSV(colorDistance * (j + skipSpecialColors),255,200)),HEX);
                Serial.print("  ==   ");
                Serial.print(leds.gamma32(leds.ColorHSV(specialNetColors[j],255,200)),HEX);
                Serial.print("\n\r");
                skipSpecialColors++;
                colorDistance = 0xffff / (numberOfNets + skipSpecialColors);
                Serial.print("colorDistance: ");
                Serial.print(colorDistance,HEX);
                Serial.print("\n\r");
            } else {
netColors[i] = leds.gamma32(leds.ColorHSV(colorDistance * (i + skipSpecialColors),255,200));
            }




        }


        

        

        //leds.setPixelColor(i, netColors[i]);
       

        net[i].color = netColors[i];
        Serial.print("\n\r");
        Serial.print(net[i].name);
        Serial.print("\t");
        Serial.print(net[i].color,HEX);
    }
//leds.show();
//delay(100000);
for (int i = 0; i < numberOfNets; i++)
{
        if (net[i].nodes[1] != 0 && net[i].nodes[1] < 62)
        {
                   
        
        for (int j = 0; j < MAX_NODES; j++)
        { 
            if (net[i].nodes[j] == 0)
            {
                break;
            }
            if (net[i].nodes[j] < 62)
            {
               leds.setPixelColor(nodesToPixelMap[net[i].nodes[j]], net[i].color); 
            }

        }

        leds.show();
        delay(1200);
        }
}



 



}

void showNets(void)
{

for (int i = 0; i < numberOfNets; i++)
    {
        for (int j = 0; j < numberOfPaths; j++)
        {
            leds.setPixelColor(bbPixelToNodesMap[net[i].nodes[j]], net[i].color);
        }
    }
    leds.show();




}