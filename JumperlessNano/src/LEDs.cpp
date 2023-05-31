#include "LEDs.h"
#include <Adafruit_NeoPixel.h>

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
        leds.rainbow(firstPixelHue, 1, saturation, 220, true);
        // Above line is equivalent to:
        // strip.rainbow(firstPixelHue, 1, 255, 255, true);
        leds.show(); // Update strip with new contents
        delay(wait); // Pause for a moment
    }
}