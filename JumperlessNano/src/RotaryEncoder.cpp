#include "CH446Q.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "Peripherals.h"
#include "JumperlessDefinesRP2040.h"
#include "FileParsing.h"
#include "NetManager.h"
#include "Probing.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "RotaryEncoder.h"
#include "quadrature.pio.h"

volatile int slotChanged = 0;
PIO pioEnc = pio1;
uint offsetEnc, smEnc;
int netSlot = 0;

int newPositionEncoder = 0;
int lastPositionEncoder = 0;
int encoderRaw = 0;
int lastPosition = 0;
int position = 0;

int lastButtonState = 0;

volatile int rotaryEncoderMode = 0;

volatile int slotPreview = 0;


void initRotaryEncoder(void)
{
    pinMode(0, INPUT_PULLUP);
    pinMode(QUADRATURE_A_PIN, INPUT_PULLUP);
    pinMode(QUADRATURE_B_PIN, INPUT_PULLUP);

    offsetEnc = pio_add_program(pioEnc, &quadrature_program);
    smEnc = pio_claim_unused_sm(pioEnc, true);
    quadrature_program_init(pioEnc, smEnc, offsetEnc, QUADRATURE_A_PIN, QUADRATURE_B_PIN);
}

void unInitRotaryEncoder(void)
{
    //pio_sm_unclaim(pioEnc, smEnc);
}

const char rotaryEncoderHelp[]= "\n\r"  //Github copilot, please don't help me with this 
 "\t\t  Rotary Encoder Help\n\r"
 "\t\t  -------------------\n\r"
 "\n\r"
 "            A  COM  B                                 \n\r"
 "    _________________________________________________   \n\r"
 "  /        D12 D11 D10                                \\ \n\r"
 " / .        [,][,][,][][][][][][][][][][][][][]        \\  \n\r"
 "|  ' `0    |  /```\\  |                                  | \n\r"
 "|  '   `   | (  O  ) |                                  | \n\r"
 "|  '     ` |  \\___/  |                                  | \n\r"
 "|  '     '  ['][ ][']{}{}{}{}{}{}{}{}{}[][][][]         | \n\r"
 "|  '     '  D13  AREF                                   | \n\r"
 "|            button                                     | \n\r"
"\n\r"
"Stick a 5 pin rotary encoder into the board as shown above. \n\n\r"
"When rotary encoder mode is on, these pins will be connected\n\r"
"to the Jumperless [A-UART_Rx(16), B-UART_Tx(17), SW-GPIO_0] \n\n\r"

"The LEDs under A0-A7 {shown in curly braces} show which of the \n\r"
"8 slots are active/connected(pink) and previewing(blue/green)\n\n\r"
"Press the encoder button to made the previewed slot active\n\r"
"(going into probing mode will make the previewed slot active)\n\n\r"

"You can cycle through slots by entering z(next) or x(previous)\n\r"
"or by turning the rotary encoder and then pressing (obviously)\n\n\r"

"You can show the contents of all the slot files by entering s\n\r"
"(copy/paste the output into a text file on your computer) \n\n\r"
"Load files by entering o and paste the text into this terminal \n\n\r"

"Wokwi sketches will be loaded into whichever slot is active\n\n\r"

"This is a WIP, so let me know if something's broken or you want\n\r"
"something added. \n\n\r "
 ;


void printRotaryEncoderHelp(void)
{
    Serial.print(rotaryEncoderHelp);
    return;
}
unsigned long previewLength = 3500;
unsigned long previewTimer = 0;

unsigned long buttonHoldTimer = 0;
unsigned long buttonHoldLength = 1000;

unsigned long buttonDebounceTimer = 0;
unsigned long buttonDebounceTimer2 = 0;
unsigned long debounceTime = 200000;
int showingPreview = 0;
int rotState = 0;
int encoderWasPressed = 1;
int encoderIsPressed = 0;
int buttonState = digitalRead(0);

int encoderAstate = 0;
int encoderBstate = 0;
int justPressed = 1;

int probeWasActive = 0;

int printSlotChanges = 0;

void rotaryEncoderStuff(void)
{
    probeWasActive = probeActive;

    if (probeActive == 1 || millis() < 500)
    {
    //     return;
    // }

    // if (probeWasActive == 1 && probeActive == 0)
    // {
        //Serial.print("not probing\n\r");
        netSlot = slotPreview;
       
    pio_sm_exec_wait_blocking(pioEnc, smEnc, pio_encode_in(pio_x, 32)); // PIO rotary encoder handler

    encoderRaw = (pio_sm_get_blocking(pioEnc, smEnc));
    lastPositionEncoder = encoderRaw;
    lastPosition = position;
    // if (millis() % 1000 == 0)
    // {
    //     Serial.print("\r probing");
    // }


    }


    if (buttonState == 0)
    {
        encoderIsPressed = 1;
    }
    else
    {
        encoderIsPressed = 0;
    }

    if (encoderIsPressed == 1 && encoderWasPressed == 0&& (micros() - buttonDebounceTimer2 > debounceTime))
    {
        buttonDebounceTimer2 = micros();
        encoderWasPressed = encoderIsPressed;
                //showSavedColors(slotPreview);
        lightUpRail();
        leds.show();
    }
    if (encoderIsPressed == 0 && encoderWasPressed == 1 && (micros() - buttonDebounceTimer2 > debounceTime))
    {
        buttonDebounceTimer2 = micros();
        encoderWasPressed = encoderIsPressed;
        //showSavedColors(slotPreview);
        lightUpRail();
        leds.show();
    }

    if ((buttonState == 0 && lastButtonState == 1) && (micros() - buttonDebounceTimer > debounceTime))
    {
        // Serial.println("pressed\n\r");

        lastPositionEncoder = encoderRaw;
        // Serial.print("encoderRaw: ");
        // Serial.print(encoderRaw);
        // Serial.print("\t");
        // Serial.print("lastPositionEncoder: ");
        // Serial.print(lastPositionEncoder);
        // Serial.println("\n\r");
        buttonDebounceTimer = micros();
        showingPreview = 0;
        rawOtherColors[1] = 0x550008,
        justPressed = 2;
        showSavedColors(slotPreview);
        lightUpRail();
        leds.show();

        slotChanged = 1;
        netSlot = slotPreview;
        buttonHoldTimer = millis();
        if(printSlotChanges == 1)
        {
        Serial.print("\r                                          \r");
        Serial.print("\rCurrent Slot: ");
        Serial.print(netSlot);
        Serial.print("\t");
        }
        // Serial.print("\t\t");
        // Serial.print("Selected Slot: ");
        // Serial.print(slotPreview);

    }
    else
    {

        // lastButtonState = buttonState;
        slotChanged = 0;
    }

    lastButtonState = buttonState;

    if (millis() - previewTimer > previewLength && showingPreview == 1 && netSlot != slotPreview)
    {
        slotPreview = netSlot;
        showSavedColors(netSlot);
        //showSavedColors(slotPreview);
        lightUpRail();
        leds.show();
        showingPreview = 0;
        rawOtherColors[1] = 0x550008;
        if (printSlotChanges == 1)
        {
        Serial.print("\r                                          \r");
        Serial.print("\rCurrent Slot: ");
        Serial.print(netSlot);
        Serial.print("\t");
        }
        previewTimer = millis();
    }

    while (slotChanged == 1)
        ;
    //delay(1);


    pio_sm_exec_wait_blocking(pioEnc, smEnc, pio_encode_in(pio_x, 32)); // PIO rotary encoder handler

    encoderRaw = (pio_sm_get_blocking(pioEnc, smEnc));


    if (abs(lastPositionEncoder - encoderRaw) >= justPressed)
    {
        justPressed = 1;
        //     Serial.print("encoderRaw: ");
        //  Serial.print(encoderRaw);
        // Serial.print("\t");
        // Serial.print("lastPositionEncoder: ");
        // Serial.print(lastPositionEncoder);
        // Serial.println("\n\r");

        // Serial.print ("\n\rencoderRaw: ");
        // Serial.print(encoderRaw);
        // Serial.print("\n\r");

        // newPositionEncoder = encoderRaw;

        if (lastPositionEncoder > encoderRaw)
        {
            slotPreview--;
            if (slotPreview < 0)
            {
                slotPreview = NUM_SLOTS - 1;
            }

            position -= 1;

            lastPositionEncoder = encoderRaw;
        }
        else if (lastPositionEncoder < encoderRaw)
        {
            // Serial.print("oldSlot: ");
            // Serial.print(netSlot);
            // Serial.print("\t");
            // Serial.print("newSlot: ");
            // Serial.print(netSlot);
            // Serial.print("\n\r");

            slotPreview++;
            if (slotPreview >= NUM_SLOTS)
            {
                slotPreview = 0;
            }

            position += 1;

            lastPositionEncoder = encoderRaw;
        }
        // Serial.print(position);
    }

    if (lastPosition != position)
    {
        if (rotState == 0)
        {
            rotState = 1;
        }
        else
        {
            rotState = 0;
        }

        lastPositionEncoder = encoderRaw;
        showingPreview = 1;
        // rawOtherColors[1] = 0x960095;
        rawOtherColors[1] = 0x1800FF;
        showSavedColors(slotPreview);
        lightUpRail();
        leds.show();

        previewTimer = millis();
        if (printSlotChanges == 1)
        {
        Serial.print("\r                                          \r");
        Serial.print("\rCurrent Slot: ");
        Serial.print(netSlot);
        Serial.print("\t");

        if (slotPreview != netSlot)
        {
        Serial.print("\tSelected Slot: ");
        Serial.print(slotPreview);
        }
        }
        // leds.setPixelColor((lastPosition)+98, 0);
        // leds.setPixelColor((position)+98, 0x460035);

        // Serial.print("position+98: ");
        // Serial.print(position+98);
        // leds.show();
        lastPosition = position;

        // leds.setPixelColor(110, rawOtherColors[1]);
        // showLEDsCore2 = 1;

        // showLEDsCore2 = 1;
        // logoFlash = 2;
    }
    buttonState = digitalRead(0);
    // if (millis() - buttonHoldTimer > buttonHoldLength && buttonState == 0 )
    // {
    //     //Serial.println("held\n\r");
    //     //refreshSavedColors();

    //     slotChanged = 1;
    //     netSlot = slotPreview;
    // }
}