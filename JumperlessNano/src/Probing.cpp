

#include "CH446Q.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "Peripherals.h"
#include "JumperlessDefinesRP2040.h"
#include "FileParsing.h"
#include "NetManager.h"
#include "Probing.h"
#include "AdcUsb.h"
#include <algorithm>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <EEPROM.h>
#include "RotaryEncoder.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_CONNECTED 0

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int probeSwap = 0;
int probeHalfPeriodus = 20;

unsigned long probingTimer = 0;
long probeFrequency = 25000;

unsigned long probeTimeout = 0;

int lastFoundIndex = 0;

int lastFoundHistory[50] = {-1};

int connectedRowsIndex = 0;
int connectedRows[32] = {-1};

int connectedRows2Index[4] = {0, 0, 0, 0};
int connectedRows2[4][32];

int nodesToConnect[2] = {-1, -1};

int node1or2 = 0;

int probePin = 19;
int buttonPin = 18;

unsigned long probeButtonTimer = 0;

int justSelectedConnectedNodes = 0;

int voltageSelection = SUPPLY_3V3;
int voltageChosen = 0;

int wasRotaryMode = 0; 


int rainbowList[13][3] = {
    {40, 50, 80},
    {88, 33, 70},
    {30, 15, 45},
    {8, 27, 45},
    {45, 18, 19},
    {35, 42, 5},
    {02, 45, 35},
    {18, 25, 45},
    {40, 12, 45},
    {10, 32, 45},
    {18, 5, 43},
    {45, 28, 13},
    {8, 12, 8}};
int rainbowIndex = 0;

int nextIsSupply = 0;
int nextIsGnd = 0;
int justCleared = 1;

char oledBuffer[32] = "                               ";

void drawchar(void)
{

    if (OLED_CONNECTED == 0)
    {
        return;
    }
    display.clearDisplay();
    if (isSpace(oledBuffer[7]) == true)
    {
        display.setTextSize(3);  // Normal 1:1 pixel scale
        display.setCursor(0, 5); // Start at top-left corner
    }
    else if (isSpace(oledBuffer[10]) == true && isSpace(oledBuffer[11]) == true)
    {
        display.setTextSize(2);  // Normal 1:1 pixel scale
        display.setCursor(0, 9); // Start at top-left corner
    }
    else
    {
        display.setTextSize(1);  // Normal 1:1 pixel scale
        display.setCursor(0, 0); // Start at top-left corner
    }

    display.setTextColor(SSD1306_WHITE); // Draw white text

    display.cp437(true); // Use full 256 char 'Code Page 437' font

    // Not all the characters will fit on the display. This is normal.
    // Library will draw what it can and the rest will be clipped.
    display.write(oledBuffer);

    display.display();

    for (int i = 0; i < 32; i++)
    {
        oledBuffer[i] = ' ';
    }
    /// delay(2000);
}

int probeMode(int pin, int setOrClear)
{
    // wasRotaryMode = rotaryEncoderMode;
    // rotaryEncoderMode = 0;
    if (OLED_CONNECTED == 1)
    {

        Serial.print("\n\r\t  Probing mode\n\n\r");

        display.display();
        display.clearDisplay();
        if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
        {
            //  Serial.println("SSD1306 allocation failed");
            // for(;;); // Don't proceed, loop forever
        }
        else
        {
            // Serial.println("SSD1306 allocation success");
        }
        if (setOrClear == 1)
        {
            sprintf(oledBuffer, "connect  ");
        }
        else
        {
            sprintf(oledBuffer, "clear");
        }

        drawchar();
    }

    Serial.print("\n\r\t  Probing mode\n\n\r");
    probeSwap = EEPROM.read(PROBESWAPADDRESS);

    if (probeSwap == 0)
    {
        probePin = 19;
        buttonPin = 18;
    }
    else
    {
        probePin = 18;
        buttonPin = 19;
    }
restartProbing:
probeActive = 1;
    int lastRow[10];

    int pokedNumber = 0;

    // Serial.print(numberOfNets);

    if (numberOfNets == 0)
    {
       // clearNodeFile(netSlot);
    }
    clearAllNTCC();
    openNodeFile(netSlot);
    getNodesToConnect();

    bridgesToPaths();
     clearLEDs();
    assignNetColors();
    showNets();
    //delay(18);
    showLEDsCore2 = 1;
    //delay(28);
    int probedNodes[40][2];
    int probedNodesIndex = 0;

    int row[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    row[1] = -2;
    probeTimeout = millis();

    int readPercentage[101];
    probingTimer = millis();

    // Serial.print("Press any key to exit and commit paths (or touch probe to gpio 18)   ");
    Serial.print("\n\r\t  Probing mode\n\n\r");
    Serial.print("   long press  = connect (pink) / clear (orange)\n\r");
    Serial.print("   short press = commit\n\r");

    if (setOrClear == 1)
    {
        sprintf(oledBuffer, "connect  ");
        drawchar();
        Serial.print("\n\r\t  connect nodes\n\n\n\r");
        rawOtherColors[1] = 0x4500e8;
        rainbowIndex = 0;
    }
    else
    {
        sprintf(oledBuffer, "clear");
        drawchar();
        Serial.print("\n\r\t  clear nodes\n\n\n\r");
        rawOtherColors[1] = 0x6644A8;
        rainbowIndex = 12;
    }

    if (setOrClear == 0)
    {
        probeButtonTimer = millis();

        // probingTimer = millis() -400;
    }

    unsigned long doubleSelectTimeout = millis();
    int doubleSelectCountdown = 0;

    while (Serial.available() == 0 && (millis() - probeTimeout) < 6200)
    {
        delayMicroseconds(38000);

        connectedRowsIndex = 0;

        row[0] = scanRows(ADC0_PIN);

        if (row[0] == -18 && (millis() - probingTimer > 500) && checkProbeButton() == 1 && (millis() - probeButtonTimer) > 1000)
        {
            if (longShortPress(750) == 1)
            {
                setOrClear = !setOrClear;
                probingTimer = millis();
                probeButtonTimer = millis();
                goto restartProbing;
                break;
            }

            // Serial.print("\n\rCommitting paths!\n\r");
            row[1] = -2;
            probingTimer = millis();

            connectedRowsIndex = 0;

            node1or2 = 0;
            nodesToConnect[0] = -1;
            nodesToConnect[1] = -1;

            // showLEDsCore2 = 1;

            break;
        }
        else
        {
            // probingTimer = millis();
        }

        if (row[0] != -1 && row[0] != row[1])
        {
            // row[1] = row[0];

            if (connectedRowsIndex > 1)
            {

                for (int k = 0; k < 3; k++) // check a few more times to make sure we got all of them
                {
                    connectedRows2Index[k] = 0;
                    for (int i = 0; i < connectedRowsIndex; i++)
                    {
                        if (connectedRows[i] != SUPPLY_5V && connectedRows[i] != GND && connectedRows[i] != SUPPLY_3V3)
                        {

                            connectedRows2[k][connectedRows2Index[k]] = connectedRows[i];
                            connectedRows2Index[k]++;
                        }
                    }
                    // connectedRows2Index[k] = connectedRowsIndex;
                    connectedRowsIndex = 0;
                    scanRows(ADC0_PIN);
                    delayMicroseconds(10);
                }
                int maxIndex = 0;
                for (int i = 0; i < 3; i++)
                {
                    if (connectedRows2Index[i] > connectedRows2Index[maxIndex])
                    {
                        maxIndex = i;
                    }
                }
                for (int i = 0; i < connectedRows2Index[maxIndex]; i++)
                {
                    if (connectedRows2[maxIndex][i] != SUPPLY_5V && connectedRows2[maxIndex][i] != GND && connectedRows2[maxIndex][i] != SUPPLY_3V3)
                    {
                        connectedRows[i] = connectedRows2[maxIndex][i];
                    }
                }
                connectedRowsIndex = connectedRows2Index[maxIndex];
            }

            if (connectedRowsIndex > 1) // if GND or 5V, we're not including them in the multiple nodes found
            {
                std::sort(connectedRows, connectedRows + connectedRowsIndex);

                nodesToConnect[node1or2] = selectFromLastFound();

                // leds.setPixelColor(nodesToPixelMap[nodesToConnect[node1or2]], rainbowList[0][0], rainbowList[0][1], rainbowList[0][2]);

                connectedRows[0] = nodesToConnect[node1or2];

                node1or2++;
                probingTimer = millis();
                // probeButtonTimer = millis();
                doubleSelectTimeout = millis();
                doubleSelectCountdown = 1000;
                showLEDsCore2 = 2;
                delay(10);
            }
            else if (connectedRowsIndex == 1)
            {
                nodesToConnect[node1or2] = connectedRows[0];
                printNodeOrName(nodesToConnect[0]);

                // for (int o = 0; 0<30; o++)
                // {

                //     oledBuffer[o] = ' ';

                // }

                // itoa(nodesToConnect[0], oledBuffer, 10);
                // drawchar();
                Serial.print("\r\t");

                if (nodesToConnect[node1or2] == SUPPLY_3V3 || nodesToConnect[node1or2] == SUPPLY_5V && voltageChosen == 0)
                {
                    voltageSelection = voltageSelect();
                    nodesToConnect[node1or2] = voltageSelection;
                    voltageChosen = 1;
                    showLEDsCore2 = 2;
                }

                if (node1or2 == 1 && setOrClear == 1 && nodesToConnect[0] == GND)
                {
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[1]], 0, 45, 5);
                }
                else if (node1or2 == 1 && setOrClear == 1 && nodesToConnect[0] == SUPPLY_5V)
                {
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[1]], 45, 5, 0);
                }
                else if (node1or2 == 1 && setOrClear == 1 && nodesToConnect[1] == GND)
                {
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[0]], 0, 45, 5);
                }
                else if (node1or2 == 1 && setOrClear == 1 && nodesToConnect[1] == SUPPLY_5V)
                {
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[0]], 45, 5, 0);
                }
                else
                {
                    // Serial.print("!!!!!");
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[node1or2]], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
                    // leds.show();
                }

                node1or2++;
                probingTimer = millis();
                showLEDsCore2 = 2;
                doubleSelectTimeout = millis();
                doubleSelectCountdown = 1500;
                delay(10);

                // delay(3);
            }
            if (node1or2 == 1)
            {
                sprintf(oledBuffer, "%s", definesToChar(nodesToConnect[0]));
                drawchar();
            }

            if (node1or2 >= 2 || (setOrClear == 0 && node1or2 >= 1))
            {
                // Serial.print("\n\n\n\r!!!!!!!!!\n\n\n\r");
                // if (nodesToConnect[0] != nodesToConnect[1])
                // {

                // }

                if (setOrClear == 1 && (nodesToConnect[0] != nodesToConnect[1]) && nodesToConnect[0] != -1 && nodesToConnect[1] != -1)
                {
                    // char oledBuffer2[32];
                    // int charsPrinted = 0;
                    Serial.print("\r           \r");
                    // itoa(nodesToConnect[0], oledBuffer, 10);
                    printNodeOrName(nodesToConnect[0]);
                    Serial.print(" - ");
                    printNodeOrName(nodesToConnect[1]);

                    Serial.print("   \t ");
                    Serial.print("connected\n\r");

                    char node1Name[12];

                    // node1Name =  (char)definesToChar(nodesToConnect[0]);

                    strcpy(node1Name, definesToChar(nodesToConnect[0]));

                    char node2Name[12];

                    // node2Name =  (char)definesToChar(nodesToConnect[1]);

                    strcpy(node2Name, definesToChar(nodesToConnect[1]));

                    sprintf(oledBuffer, "%s - %s           ", node1Name, node2Name);

                    drawchar();

                    addBridgeToNodeFile(nodesToConnect[0], nodesToConnect[1], netSlot);
                    // rainbowIndex++;
                    if (rainbowIndex > 1)
                    {
                        rainbowIndex = 0;
                    }
                    showSavedColors(netSlot);

                    // clearAllNTCC();
                    // openNodeFile(netSlot);
                    // getNodesToConnect();

                    // bridgesToPaths();
                    // // clearLEDs();
                    // leds.clear();
                    // assignNetColors();
                    // Serial.print("bridgesToPaths\n\r");
                    // delay(18);
                    // showNets();
                    showLEDsCore2 = 1;
                    // digitalWrite(RESETPIN, HIGH);

                    // delayMicroseconds(10);

                    //  sendAllPathsCore2 = 1;
                    // digitalWrite(RESETPIN, LOW);
                    delay(35);

                    // row[1] = -1;

                    // doubleSelectTimeout = millis();

                    doubleSelectTimeout = millis();
                    doubleSelectCountdown = 2000;
                }
                else if (setOrClear == 0)
                {
                    Serial.print("\r           \r");
                    printNodeOrName(nodesToConnect[0]);
                    Serial.print("\t cleared\n\r");
                    removeBridgeFromNodeFile(nodesToConnect[0], -1, netSlot);
                    leds.setPixelColor(nodesToPixelMap[nodesToConnect[0]], 0, 0, 0);

                    // leds.setPixelColor(nodesToPixelMap[nodesToConnect[1]], 0, 0, 0);
                    rainbowIndex = 12;
                    // printNodeFile();
                    showSavedColors(netSlot);
                    // clearAllNTCC();
                    // openNodeFile(netSlot);
                    // getNodesToConnect();

                    // bridgesToPaths();
                    // // clearLEDs();
                    // leds.clear();
                    // assignNetColors();
                    // // Serial.print("bridgesToPaths\n\r");
                    // // delay(18);
                    // // showNets();
                    // showLEDsCore2 = 1;
                    // sendAllPathsCore2 = 1;
                    // logoFlash = 1;
                    delay(25);
                    // row[1] = -1;
                }
                node1or2 = 0;
                nodesToConnect[0] = -1;
                nodesToConnect[1] = -1;
                // row[1] = -2;
                doubleSelectTimeout = millis();
                doubleSelectCountdown = 2000;
            }

            row[1] = row[0];
        }
        // Serial.print("\n\r");
        // Serial.print(" ");
        // Serial.print(row[0]);

        if (justSelectedConnectedNodes == 1)
        {
            justSelectedConnectedNodes = 0;
        }

        if (setOrClear == 1) // makes the LED brighter when you have one node selected
        {

            if (node1or2 == 0)
            {

                rawOtherColors[1] = 0x4500e8;
            }
            else
            {
                if (nodesToConnect[0] == GND)
                {
                    rawOtherColors[1] = 0x00ff00;
                }
                else if (nodesToConnect[0] == SUPPLY_5V)
                {
                    rawOtherColors[1] = 0xff0000;
                }
                else if (nodesToConnect[1] == SUPPLY_3V3)
                {
                    rawOtherColors[1] = 0xff0066;
                }
                else
                {
                    rawOtherColors[1] = 0x8510f8;
                }
                // rawOtherColors[1] = 0x8510f8;
            }
            showLEDsCore2 = 2;
        }
        else
        {
            rawOtherColors[1] = 0x6644A8;
            showLEDsCore2 = 2;
        }

        if ((node1or2 == 0 && doubleSelectCountdown <= 0))
        {
            // Serial.println("doubleSelectCountdown");
            row[1] = -2;
            doubleSelectTimeout = millis();
            doubleSelectCountdown = 1000;
        }

        // Serial.println(doubleSelectCountdown);

        if (doubleSelectCountdown <= 0)
        {

            doubleSelectCountdown = 0;
        }
        else
        {
            doubleSelectCountdown = doubleSelectCountdown - (millis() - doubleSelectTimeout);

            doubleSelectTimeout = millis();
        }

        probeTimeout = millis();
    }
    digitalWrite(RESETPIN, LOW);
    clearAllNTCC();
    openNodeFile(netSlot);
    getNodesToConnect();

    bridgesToPaths();
    // clearLEDs();
    leds.clear();
    assignNetColors();
    // // Serial.print("bridgesToPaths\n\r");
    // delay(18);
     showNets();
     showLEDsCore2 = 1;
    rawOtherColors[1] = 0x550004;
    showLEDsCore2 = 1;

    sendAllPathsCore2 = 1;
    // delay(25);
    // pinMode(probePin, INPUT);
    delay(300);
    row[1] = -2;

    // sprintf(oledBuffer, "        ");
    drawchar();

   // rotaryEncoderMode = wasRotaryMode;
    return 1;
}

unsigned long blinkTimer = 0;

int voltageSelect(void)
{
    int selected = SUPPLY_3V3;
    int selectionConfirmed = 0;
    int selected2 = 0;
    int lastSelected = 0;
    connectedRows[0] = -1;
    Serial.print("\r                                \r");
    Serial.print("\n\r");
    Serial.print("      select voltage\n\n\r");
    Serial.print("  short press = cycle through voltages\n\r");
    Serial.print("  long press  = select\n\r");

    Serial.print("\n\r ");

    while (selectionConfirmed == 0)
    {

        if (lastSelected != selected)
        {
            if (selected == SUPPLY_3V3)
            {
                Serial.print("\r3.3V   ");
                clearLEDsExceptRails();
                leds.setPixelColor(nodesToPixelMap[1], 55, 0, 12);
                leds.setPixelColor(nodesToPixelMap[2], 55, 0, 12);
                leds.setPixelColor(nodesToPixelMap[3], 55, 0, 12);

                leds.setPixelColor(nodesToPixelMap[31], 15, 0, 0);
                leds.setPixelColor(nodesToPixelMap[32], 15, 0, 0);
                leds.setPixelColor(nodesToPixelMap[33], 15, 0, 0);

                leds.setPixelColor(nodesToPixelMap[34], 15, 0, 0);
                leds.setPixelColor(nodesToPixelMap[35], 15, 0, 0);
            }
            else if (selected == SUPPLY_5V)
            {
                Serial.print("\r5V    ");
                clearLEDsExceptRails();
                leds.setPixelColor(nodesToPixelMap[1], 15, 0, 6);
                leds.setPixelColor(nodesToPixelMap[2], 15, 0, 6);
                leds.setPixelColor(nodesToPixelMap[3], 15, 0, 6);

                leds.setPixelColor(nodesToPixelMap[31], 55, 0, 0);
                leds.setPixelColor(nodesToPixelMap[32], 55, 0, 0);
                leds.setPixelColor(nodesToPixelMap[33], 55, 0, 0);

                leds.setPixelColor(nodesToPixelMap[34], 55, 0, 0);
                leds.setPixelColor(nodesToPixelMap[35], 55, 0, 0);
            }
            // showLEDsCore2 = 1;

            leds.show();
            lastSelected = selected;
        }

        delay(30);

        int longShort = longShortPress();
        if (longShort == 1)
        {
            selectionConfirmed = 1;
            voltageSelection = selected;
            // break;
        }
        else if (longShort == 0)
        {
            if (selected == SUPPLY_3V3)
            {
                selected = SUPPLY_5V;
            }
            else
            {
                selected = SUPPLY_3V3;
            }
        }
    }

    // Serial.print("\n\r");
    leds.clear();
    showNets();
    showLEDsCore2 = 2;

    return voltageSelection;
}

int selectFromLastFound(void)
{

    rawOtherColors[1] = 0x0010ff;

    blinkTimer = 0;
    int selected = 0;
    int selectionConfirmed = 0;
    int selected2 = connectedRows[selected];
    Serial.print("\n\r");
    Serial.print("      multiple nodes found\n\n\r");
    Serial.print("  short press = cycle through nodes\n\r");
    Serial.print("  long press  = select\n\r");

    Serial.print("\n\r ");
    for (int i = 0; i < connectedRowsIndex; i++)
    {

        printNodeOrName(connectedRows[i]);
        if (i < connectedRowsIndex - 1)
        {
            Serial.print(", ");
        }
    }
    Serial.print("\n\n\r");
    delay(10);

    uint32_t previousColor[connectedRowsIndex];

    for (int i = 0; i < connectedRowsIndex; i++)
    {
        previousColor[i] = leds.getPixelColor(nodesToPixelMap[connectedRows[i]]);
    }
    int lastSelected = -1;

    while (selectionConfirmed == 0)
    {
        probeTimeout = millis();
        // if (millis() - blinkTimer > 100)
        // {
        if (lastSelected != selected && selectionConfirmed == 0)
        {
            for (int i = 0; i < connectedRowsIndex; i++)
            {
                if (i == selected)
                {
                    leds.setPixelColor(nodesToPixelMap[connectedRows[i]], rainbowList[1][0], rainbowList[1][1], rainbowList[1][2]);
                }
                else
                {
                    // uint32_t previousColor = leds.getPixelColor(nodesToPixelMap[connectedRows[i]]);
                    if (previousColor[i] != 0)
                    {
                        int r = (previousColor[i] >> 16) & 0xFF;
                        int g = (previousColor[i] >> 8) & 0xFF;
                        int b = (previousColor[i] >> 0) & 0xFF;
                        leds.setPixelColor(nodesToPixelMap[connectedRows[i]], (r / 4) + 5, (g / 4) + 5, (b / 4) + 5);
                    }
                    else
                    {

                        leds.setPixelColor(nodesToPixelMap[connectedRows[i]], rainbowList[1][0] / 8, rainbowList[1][1] / 8, rainbowList[1][2] / 8);
                    }
                }
            }
            lastSelected = selected;

            Serial.print(" \r");
            // Serial.print("");
            printNodeOrName(connectedRows[selected]);
            Serial.print("  ");
        }
        // leds.show();
        showLEDsCore2 = 2;
        blinkTimer = millis();
        //  }
        delay(30);
        int longShort = longShortPress();
        delay(5);
        if (longShort == 1)
        {
            selectionConfirmed = 1;
            // for (int i = 0; i < connectedRowsIndex; i++)
            // {
            //     if (i == selected)
            //     // if (0)
            //     {
            //         leds.setPixelColor(nodesToPixelMap[connectedRows[i]], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
            //     }
            //     else
            //     {
            //         leds.setPixelColor(nodesToPixelMap[connectedRows[i]], 0, 0, 0);
            //     }
            // }
            // showLEDsCore2 = 1;
            // selected = lastFound[node1or2][selected];
            //  clearLastFound();

            // delay(500);
            selected2 = connectedRows[selected];
            // return selected2;
            break;
        }
        else if (longShort == 0)
        {

            selected++;
            blinkTimer = 0;

            if (selected >= connectedRowsIndex)
            {

                selected = 0;
            }
            // delay(100);
        }
        delay(15);
        //  }
        //}

        // showLEDsCore2 = 1;
    }
    selected2 = connectedRows[selected];

    for (int i = 0; i < connectedRowsIndex; i++)
    {
        if (i == selected)
        {
            leds.setPixelColor(nodesToPixelMap[connectedRows[selected]], rainbowList[0][0], rainbowList[0][1], rainbowList[0][2]);
        }
        else if (previousColor[i] != 0)
        {

            int r = (previousColor[i] >> 16) & 0xFF;
            int g = (previousColor[i] >> 8) & 0xFF;
            int b = (previousColor[i] >> 0) & 0xFF;
            leds.setPixelColor(nodesToPixelMap[connectedRows[i]], r, g, b);
        }
        else
        {

            leds.setPixelColor(nodesToPixelMap[connectedRows[i]], 0, 0, 0);
        }
    }

    // leds.setPixelColor(nodesToPixelMap[selected2], rainbowList[0][0], rainbowList[0][1], rainbowList[0][2]);
    // leds.show();
    // showLEDsCore2 = 1;
    probeButtonTimer = millis();
    // connectedRowsIndex = 0;
    justSelectedConnectedNodes = 1;
    return selected2;
}

int longShortPress(int pressLength)
{
    int longShort = 0;
    unsigned long clickTimer = 0;

    clickTimer = millis();

    if (checkProbeButton() == 1)
    {

        while (millis() - clickTimer < pressLength)
        {
            if (checkProbeButton() == 0)
            {
                return 0;
            }
            delay(5);
        }
    }
    else
    {
        return -1;
    }

    return 1;
}

int checkProbeButton(void)
{
    int buttonState = 0;

    pinMode(buttonPin, INPUT);
    startProbe(10000);

    if (readFloatingOrState(buttonPin, 0) == probe)
    {
        buttonState = 1;
    }
    else
    {
        buttonState = 0;
    }
    stopProbe();
    pinMode(buttonPin, OUTPUT);
    digitalWrite(buttonPin, LOW);

    return buttonState;
}

int readFloatingOrState(int pin, int rowBeingScanned)
{

    enum measuredState state = unknownState;
    // enum measuredState state2 = floating;

    int readingPullup = 0;
    int readingPullup2 = 0;
    int readingPullup3 = 0;

    int readingPulldown = 0;
    int readingPulldown2 = 0;
    int readingPulldown3 = 0;

    pinMode(pin, INPUT_PULLUP);

    if (rowBeingScanned != -1)
    {

        analogWrite(probePin, 128);

        while (1) // this is the silliest way to align to the falling edge of the probe PWM signal
        {
            if (gpio_get(probePin) != 0)
            {
                if (gpio_get(probePin) == 0)
                {
                    break;
                }
            }
        }
    }

    delayMicroseconds((probeHalfPeriodus * 5) + (probeHalfPeriodus / 2));

    readingPullup = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 3);
    readingPullup2 = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 1);
    readingPullup3 = digitalRead(pin);

    pinMode(pin, INPUT_PULLDOWN);

    if (rowBeingScanned != -1)
    {
        while (1) // this is the silliest way to align to the falling edge of the probe PWM signal
        {
            if (gpio_get(probePin) != 0)
            {
                if (gpio_get(probePin) == 0)
                {
                    break;
                }
            }
        }
    }

    delayMicroseconds((probeHalfPeriodus * 5) + (probeHalfPeriodus / 2));

    readingPulldown = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 3);
    readingPulldown2 = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 1);
    readingPulldown3 = digitalRead(pin);

    // if (readingPullup == 0 && readingPullup2 == 1 && readingPullup3 == 0 && readingPulldown == 1 && readingPulldown2 == 0 && readingPulldown3 == 1)
    // {
    //     state = probe;
    // }

    if ((readingPullup != readingPullup2 || readingPullup2 != readingPullup3) && (readingPulldown != readingPulldown2 || readingPulldown2 != readingPulldown3) && rowBeingScanned != -1)
    {
        state = probe;

        // if (readingPulldown != readingPulldown2 || readingPulldown2 != readingPulldown3)
        // {
        //     state = probe;

        // } else
        // {
        //     Serial.print("!");
        // }
    }
    else
    {

        if (readingPullup2 == 1 && readingPulldown2 == 0)
        {

            state = floating;
        }
        else if (readingPullup2 == 1 && readingPulldown2 == 1)
        {
            //              Serial.print(readingPullup);
            // // Serial.print(readingPullup2);
            // // Serial.print(readingPullup3);
            // // //Serial.print(" ");
            //  Serial.print(readingPulldown);
            // // Serial.print(readingPulldown2);
            // // Serial.print(readingPulldown3);
            //  Serial.print("\n\r");

            state = high;
        }
        else if (readingPullup2 == 0 && readingPulldown2 == 0)
        {
            //  Serial.print(readingPullup);
            // // Serial.print(readingPullup2);
            // // Serial.print(readingPullup3);
            // // //Serial.print(" ");
            //  Serial.print(readingPulldown);
            // // Serial.print(readingPulldown2);
            // // Serial.print(readingPulldown3);
            //  Serial.print("\n\r");
            state = low;
        }
        else if (readingPullup == 0 && readingPulldown == 1)
        {
            // Serial.print("shorted");
        }
    }

    // Serial.print("\n");
    // showLEDsCore2 = 1;
    // leds.show();
    // delayMicroseconds(100);

    return state;
}

void startProbe(long probeSpeed)
{
    // pwm_set_irq_enabled(1, true);
    // pwm_set_clkdiv_mode(1,PWM_DIV_B_RISING);

    probeFrequency = probeSpeed;
    probeHalfPeriodus = 1000000 / probeSpeed / 2;
    pinMode(probePin, OUTPUT);
    analogWriteFreq(probeSpeed);
    analogWrite(probePin, 128);
    // delayMicroseconds(10);
    // pinMode(buttonPin, INPUT);
}

void stopProbe()
{
    pinMode(probePin, INPUT);
    pinMode(buttonPin, INPUT);
}

int checkLastFound(int found)
{
    int found2 = 0;
    return found2;
}

void clearLastFound()
{
}

int scanRows(int pin)
{

    int found = -1;
    connectedRows[0] = -1;

    if (checkProbeButton() == 1)
    {
        return -18;
    }

    // pin = ADC1_PIN;

    digitalWrite(RESETPIN, HIGH);
    delayMicroseconds(20);
    digitalWrite(RESETPIN, LOW);
    // delayMicroseconds(20);

    pinMode(probePin, INPUT);
    delayMicroseconds(400);
    int probeRead = readFloatingOrState(probePin, -1);

    if (probeRead == high)
    {
        found = voltageSelection;
        connectedRows[connectedRowsIndex] = found;
        connectedRowsIndex++;
        found = -1;
        // return connectedRows[connectedRowsIndex];
        // Serial.print("high");
        // return found;
    }

    else if (probeRead == low)
    {
        found = GND;
        connectedRows[connectedRowsIndex] = found;
        connectedRowsIndex++;
        // return found;
        found = -1;
        // return connectedRows[connectedRowsIndex];
        // Serial.print(connectedRows[connectedRowsIndex]);

        // return connectedRows[connectedRowsIndex];
    }

    startProbe();
    int chipToConnect = 0;
    int rowBeingScanned = 0;

    int xMapRead = 15;

    if (pin == ADC0_PIN)
    {
        xMapRead = 2;
    }
    else if (pin == ADC1_PIN)
    {
        xMapRead = 3;
    }
    else if (pin == ADC2_PIN)
    {
        xMapRead = 4;
    }
    else if (pin == ADC3_PIN)
    {
        xMapRead = 5;
    }

    for (int chipScan = CHIP_A; chipScan < 8; chipScan++) // scan the breadboard (except the corners)
    {

        sendXYraw(CHIP_L, xMapRead, chipScan, 1);

        for (int yToScan = 1; yToScan < 8; yToScan++)
        {

            sendXYraw(chipScan, 0, 0, 1);
            sendXYraw(chipScan, 0, yToScan, 1);

            rowBeingScanned = ch[chipScan].yMap[yToScan];
            if (readFloatingOrState(pin, rowBeingScanned) == probe)
            {
                found = rowBeingScanned;

                if (found != -1)
                {
                    connectedRows[connectedRowsIndex] = found;
                    connectedRowsIndex++;
                    found = -1;
                    // delayMicroseconds(100);
                    // stopProbe();
                    // break;
                }
            }

            sendXYraw(chipScan, 0, 0, 0);
            sendXYraw(chipScan, 0, yToScan, 0);
        }
        sendXYraw(CHIP_L, 2, chipScan, 0);
    }

    int corners[4] = {1, 30, 31, 60};
    sendXYraw(CHIP_L, xMapRead, 0, 1);
    for (int cornerScan = 0; cornerScan < 4; cornerScan++)
    {

        sendXYraw(CHIP_L, cornerScan + 8, 0, 1);

        rowBeingScanned = corners[cornerScan];
        if (readFloatingOrState(pin, rowBeingScanned) == probe)
        {
            found = rowBeingScanned;
            // if (nextIsSupply)
            // {
            //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
            // }
            // else if (nextIsGnd)
            // {
            //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
            // }
            // else
            // {
            //     leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
            // }
            // showLEDsCore2 = 1;
            if (found != -1)
            {
                connectedRows[connectedRowsIndex] = found;
                connectedRowsIndex++;
                found = -1;
                // stopProbe();
                // break;
            }
        }

        sendXYraw(CHIP_L, cornerScan + 8, 0, 0);
    }
    sendXYraw(CHIP_L, xMapRead, 0, 0);

    for (int chipScan2 = CHIP_I; chipScan2 <= CHIP_J; chipScan2++) // scan the breadboard (except the corners)
    {

        int pinHeader = ADC0_PIN + (chipScan2 - CHIP_I);

        for (int xToScan = 0; xToScan < 12; xToScan++)
        {

            sendXYraw(chipScan2, xToScan, 0, 1);
            sendXYraw(chipScan2, 13, 0, 1);

            // analogRead(ADC0_PIN);

            rowBeingScanned = ch[chipScan2].xMap[xToScan];
            //   Serial.print("rowBeingScanned: ");
            //     Serial.println(rowBeingScanned);
            //     Serial.print("chipScan2: ");
            //     Serial.println(chipScan2);
            //     Serial.print("xToScan: ");
            //     Serial.println(xToScan);

            if (readFloatingOrState(pinHeader, rowBeingScanned) == probe)
            {

                found = rowBeingScanned;

                // if (nextIsSupply)
                // {
                //     //leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
                // }
                // else if (nextIsGnd)
                // {
                //    // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
                // }
                // else
                // {
                //     //leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
                // }
                // //showLEDsCore2 = 1;
                // // leds.show();

                if (found != -1)
                {
                    connectedRows[connectedRowsIndex] = found;
                    connectedRowsIndex++;
                    found = -1;
                    // stopProbe();
                    // break;
                }
            }
            sendXYraw(chipScan2, xToScan, 0, 0);
            sendXYraw(chipScan2, 13, 0, 0);
        }
    }

    // stopProbe();
    // probeTimeout = millis();

    digitalWrite(RESETPIN, HIGH);
    delayMicroseconds(20);
    digitalWrite(RESETPIN, LOW);
    return connectedRows[0];
    // return found;

    // return 0;
}

int readRails(int pin)
{
    int state = -1;

    // Serial.print("adc0 \t");
    // Serial.println(adcReadings[0]);
    // Serial.print("adc1 \t");
    // Serial.println(adcReadings[1]);
    // Serial.print("adc2 \t");
    // Serial.println(adcReadings[2]);
    // Serial.print("adc3 \t");
    // Serial.println(adcReadings[3]);

    return state;
}

// while (Serial.available() == 0 && (millis() - probeTimeout) < 6200)
// {
//     delayMicroseconds(8000);

//     // Serial.println(readRails(ADC0_PIN));

//     row[0] = scanRows(ADC0_PIN);

//     // delayMicroseconds(10);
//     // row[1] = scanRows(0);
//     // delayMicroseconds(10);
//     // row[2] = scanRows(0);
//     // delayMicroseconds(10);

//     if (row[0] != -1 && row[1] != row[0])
//     {
//         row[1] = row[0];

//         if (row[0] == -18 && millis() - probingTimer > 500)
//         {
//             Serial.print("\n\rCommitting paths!\n\r");
//             probingTimer = millis();
//             break;
//         }
//         else if (row[0] == -18)
//         {
//             continue;
//         }

//         // delayMicroseconds(10);

//         // Serial.print("\n\r");

//          int connectedReads = 0;

//         if (row[0] != SUPPLY_5V && row[0] != GND)
//         {

//             for (int i = 0; i < 10; i++)
//             {
//                 delayMicroseconds(50);

//                 int scanForOthers = scanRows(ADC0_PIN);

//                 if (scanForOthers != row[0] && scanForOthers != -1 )
//                 {
//                     lastRow[connectedReads] = scanForOthers;

//                      Serial.print("lastRow[");
//                         Serial.print(connectedReads);
//                         Serial.print("]: ");
//                         Serial.println(lastRow[connectedReads]);

//                         connectedReads++;

//                 }

//             }
//         }
//         // Serial.print("\n\r");
//         // Serial.print("lastFoundIndex: ");
//         // Serial.println (lastFoundIndex);
//         Serial.print("\n\r");
//         Serial.print("node1or2: ");
//         Serial.println(node1or2);

//         // delay(10);
//         lastRow[pokedNumber] = row[0];
//         probedNodes[probedNodesIndex][pokedNumber] = row[0];

//         pokedNumber++;

//         printNodeOrName(row[0]);
//         Serial.print("\r\t");

//         if (connectedReads > 1)
//         {
//             Serial.print("connectedReads: ");
//             Serial.println(connectedReads);

//             // for (int i = 0; i < lastFoundIndex[node1or2]; i++)
//             // {
//             //     Serial.print("lastFound[");
//             //     Serial.print(i);
//             //     Serial.print("][");
//             //     Serial.print(node1or2);
//             //     Serial.print("]: ");
//             //     Serial.print(connectedRows[i]);
//             //     Serial.print("\n\r");
//             // }

//             row[0] = selectFromLastFound();
//             probingTimer = millis();
//             // clearLastFound();
//             //  lastFoundIndex[node1or2] = 0;
//         }

//         if (pokedNumber >= 2)
//         {
//             node1or2 = 0;
//             Serial.print("\r            \r");
//             printNodeOrName(probedNodes[probedNodesIndex][0]);
//             Serial.print(" - ");
//             printNodeOrName(probedNodes[probedNodesIndex][1]);
//             Serial.print("\n\r");

//             Serial.print("\n\r");
//             node1or2 = 0;
//             for (int i = 0; i < probedNodesIndex; i++)
//             {

//                 /// Serial.print("\n\r");

//                 if ((probedNodes[i][0] == probedNodes[probedNodesIndex][0] && probedNodes[i][1] == probedNodes[probedNodesIndex][1]) || (probedNodes[i][0] == probedNodes[probedNodesIndex][1] && probedNodes[i][1] == probedNodes[probedNodesIndex][0]))
//                 {
//                     probedNodes[probedNodesIndex][0] = 0;
//                     probedNodes[probedNodesIndex][1] = 0;

//                     leds.setPixelColor(nodesToPixelMap[probedNodes[i][0]], 0);
//                     leds.setPixelColor(nodesToPixelMap[probedNodes[i][1]], 0);

//                     for (int j = i; j < probedNodesIndex; j++)
//                     {
//                         probedNodes[j][0] = probedNodes[j + 1][0];
//                         probedNodes[j][1] = probedNodes[j + 1][1];
//                     }
//                     // probedNodes[i][0] = -1;
//                     // probedNodes[i][1] = -1;
//                     pokedNumber = 0;

//                     showLEDsCore2 = 1;
//                     probedNodesIndex--;
//                     probedNodesIndex--;
//                     // break;
//                 }
//             }
//             // Serial.print("\n\n\n\r");

//             // Serial.print("\r            \r");
//             // printNodeOrName(probedNodes[probedNodesIndex][0]);
//             // Serial.print(" - ");
//             // printNodeOrName(probedNodes[probedNodesIndex][1]);
//             // Serial.print("\n\r");

//             for (int i = probedNodesIndex; i >= 0; i--)
//             {
//                 // Serial.print ("    ");
//                 // Serial.print (i);
//                 Serial.print("\t");
//                 printNodeOrName(probedNodes[i][0]);
//                 Serial.print(" - ");
//                 printNodeOrName(probedNodes[i][1]);
//                 Serial.print("\n\r");
//             }
//             Serial.print("\n\n\r");

//             // delay(18);
//             pokedNumber = 0;
//             probedNodesIndex++;

//             // clearLEDs();
//             // openNodeFile();
//             // getNodesToConnect();

//             // bridgesToPaths();

//             /// assignNetColors();
//             delay(8);
//             row[0] = -1;
//             // showLEDsCore2 = 1;
//             // delay(18);

//             rainbowIndex++;
//             if (rainbowIndex > 11)
//             {
//                 rainbowIndex = 0;
//             }
//             scanRows(0, true);
//         }
//         else
//         {
//             node1or2 = 1;
//         }
//     }
// }

// for (int i = 0; i < probedNodesIndex; i++)
// {
//     addBridgeToNodeFile(probedNodes[i][0], probedNodes[i][1]);
// }

// for (int i = 0; i < 100; i++)
// {
//     delayMicroseconds(1);
//     row[0] = scanRows(ADC0_PIN);
//     connectedRowsIndex = 0;
//     //row[0] = checkProbeButton();
//     readPercentage[i] = row[0];
// }
// int percent = 0;
// for (int i = 0; i < 100; i++)
// {
//     if (readPercentage[i] != -1)
//     {
//         percent++;
//     }
// }
// Serial.print("percent: ");
// Serial.print(percent);
// Serial.print("%\n\r");
// // delayMicroseconds(10);
// // Serial.println(scanRows(ADC0_PIN));
