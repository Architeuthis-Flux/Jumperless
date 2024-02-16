

#include "CH446Q.h"
#include "MatrixStateRP2040.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "Peripherals.h"
#include "JumperlessDefinesRP2040.h"
#include "FileParsing.h"
#include "NetManager.h"
#include "Probing.h"

int probeHalfPeriodus = 10;

unsigned long probingTimer = 0;
long probeFrequency = 50000;

int probeMode(int pin)
{

    int lastRow[10];
    int pokedNumber = 0;
    Serial.print("Press any key to exit and commit paths (or touch probe to gpio 18)\n\n\r");
    rawOtherColors[1] = 0x3500A8;
    // Serial.print(numberOfNets);

    if (numberOfNets == 0)
    {
        clearNodeFile();
    }
    clearAllNTCC();
    openNodeFile();
    getNodesToConnect();

    bridgesToPaths();
    // clearLEDs();
    assignNetColors();
    delay(18);
    showLEDsCore2 = 1;
    delay(28);
    int probedNodes[40][2];
    int probedNodesIndex = 0;

    int row = 0;
    while (Serial.available() == 0)
    {
        delayMicroseconds(1700);
        row = scanRows(0);

        if (row != -1)
        {
            if (row == -18 && millis() - probingTimer > 500)
            {
                Serial.print("\n\rCommitting paths!\n\r");
                probingTimer = millis();
                break;
            }
            else if (row == -18)
            {
                continue;
            }
            delay(10);
            lastRow[pokedNumber] = row;
            probedNodes[probedNodesIndex][pokedNumber] = row;

            pokedNumber++;

            printNodeOrName(row);
            Serial.print("\r\t");

            if (pokedNumber >= 2)
            {
                Serial.print("\r            \r");
                printNodeOrName(probedNodes[probedNodesIndex][0]);
                Serial.print(" - ");
                printNodeOrName(probedNodes[probedNodesIndex][1]);
                Serial.print("\n\r");

                Serial.print("\n\r");

                for (int i = 0; i < probedNodesIndex; i++)
                {

                    /// Serial.print("\n\r");

                    if ((probedNodes[i][0] == probedNodes[probedNodesIndex][0] && probedNodes[i][1] == probedNodes[probedNodesIndex][1]) || (probedNodes[i][0] == probedNodes[probedNodesIndex][1] && probedNodes[i][1] == probedNodes[probedNodesIndex][0]))
                    {
                        probedNodes[probedNodesIndex][0] = 0;
                        probedNodes[probedNodesIndex][1] = 0;

                        leds.setPixelColor(nodesToPixelMap[probedNodes[i][0]], 0);
                        leds.setPixelColor(nodesToPixelMap[probedNodes[i][1]], 0);

                        for (int j = i; j < probedNodesIndex; j++)
                        {
                            probedNodes[j][0] = probedNodes[j + 1][0];
                            probedNodes[j][1] = probedNodes[j + 1][1];
                        }
                        // probedNodes[i][0] = -1;
                        // probedNodes[i][1] = -1;
                        pokedNumber = 0;

                        showLEDsCore2 = 1;
                        probedNodesIndex--;
                        probedNodesIndex--;
                        break;
                    }
                }
                // Serial.print("\n\n\n\r");

                // Serial.print("\r            \r");
                // printNodeOrName(probedNodes[probedNodesIndex][0]);
                // Serial.print(" - ");
                // printNodeOrName(probedNodes[probedNodesIndex][1]);
                // Serial.print("\n\r");

                for (int i = probedNodesIndex; i >= 0; i--)
                {
                    // Serial.print ("    ");
                    // Serial.print (i);
                    Serial.print("\t");
                    printNodeOrName(probedNodes[i][0]);
                    Serial.print(" - ");
                    printNodeOrName(probedNodes[i][1]);
                    Serial.print("\n\r");
                }
                Serial.print("\n\n\r");

                // delay(18);
                pokedNumber = 0;
                probedNodesIndex++;

                // clearLEDs();
                // openNodeFile();
                // getNodesToConnect();

                // bridgesToPaths();

                /// assignNetColors();
                delay(8);
                // showLEDsCore2 = 1;
                // delay(18);
                scanRows(0, true);
            }
        }
    }

    for (int i = 0; i < probedNodesIndex; i++)
    {
        addBridgeToNodeFile(probedNodes[i][0], probedNodes[i][1]);
    }

    clearAllNTCC();
    openNodeFile();
    getNodesToConnect();

    bridgesToPaths();
    // clearLEDs();
    assignNetColors();
    // Serial.print("bridgesToPaths\n\r");
    delay(18);
    // showNets();
    rawOtherColors[1] = 0x550004;
    sendAllPathsCore2 = 1;
    delay(25);
    pinMode(19, INPUT);
    delay(300);

    return 1;
}
int lastProbeButtonState = 0;
int lastProbeButtonState2 = 0;

int checkProbeButton(void)
{
    int buttonState = 0;
    int probeButton[3] = {-1, -1, -1};

    startProbe();
    delayMicroseconds(probeHalfPeriodus * 10);

    probeButton[0] = readFloatingOrState(18);
    delayMicroseconds(probeHalfPeriodus);
    probeButton[1] = readFloatingOrState(18);
    delayMicroseconds(probeHalfPeriodus);
    probeButton[2] = readFloatingOrState(18);

    if (probeButton[0] == probeButton[1] && probeButton[1] == probeButton[2] && probeButton[0] == probeButton[2])
    {
        if (probeButton[0] == probe)
        {
            buttonState = 1;
        }
        else
        {
            buttonState = 0;
        }
    }
    else
    {
        // Serial.print("periodus: ");
        // Serial.print(probeHalfPeriodus);
        // Serial.print("\t");
        // Serial.print(probeButton[0]);
        // Serial.print("\t");
        // Serial.print(probeButton[1]);
        // Serial.print("\t");
        // Serial.print(probeButton[2]);
        // Serial.print("\n\r");
    }

    // pinMode(19, INPUT);
    stopProbe();
    return buttonState;
}

int readFloatingOrState(int pin, int rowBeingScanned)
{

    enum measuredState state = floating;
    int readingPullup = 0;
    int readingPullup2 = 0;
    int readingPullup3 = 0;

    int readingPulldown = 0;
    int readingPulldown2 = 0;
    int readingPulldown3 = 0;

    pinMode(pin, INPUT_PULLUP);

    delayMicroseconds(probeHalfPeriodus * 32);

    readingPullup = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 2);
    readingPullup2 = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus);
    readingPullup3 = digitalRead(pin);

    pinMode(pin, INPUT_PULLDOWN);

    delayMicroseconds(probeHalfPeriodus * 32);

    readingPulldown = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus * 2);
    readingPulldown2 = digitalRead(pin);
    delayMicroseconds(probeHalfPeriodus);
    readingPulldown3 = digitalRead(pin);

    if (readingPullup != readingPullup2 || readingPullup2 != readingPullup3 && rowBeingScanned != -1)
    {
        if (readingPulldown != readingPulldown2 || readingPulldown2 != readingPulldown3)
        {
            state = probe;

            // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);

            // Serial.print("probe");
            //
        }
    }
    else
    {

        // Serial.print(readingPulldown);
        // Serial.print("\t");
        if (readingPullup == 1 && readingPulldown == 0)
        {
            // Serial.print("floating");
            // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 0, 0, 3);
            state = floating;
        }
        else if (readingPullup == 1 && readingPulldown == 1)
        {
            // Serial.print("HIGH");
            // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 45, 0, 0);
            state = high;
        }
        else if (readingPullup == 0 && readingPulldown == 0)
        {
            // Serial.print("LOW");
            // leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 0, 45, 0);
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
    probeFrequency = probeSpeed;
    probeHalfPeriodus = 1000000 / probeSpeed / 2;
    pinMode(19, OUTPUT);
    analogWriteFreq(probeSpeed);
    analogWrite(19, 128);
    // delayMicroseconds(10);
    // pinMode(18, INPUT);
}

void stopProbe()
{
    pinMode(19, INPUT);
    pinMode(18, INPUT);
}

int rainbowList[12][3] = {
    {45, 35, 8},
    {10, 45, 30},
    {30, 15, 45},
    {8, 27, 45},
    {45, 18, 19},
    {35, 42, 5},
    {02, 45, 35},
    {18, 25, 45},
    {40, 12, 45},
    {10, 32, 45},
    {18, 5, 43},
    {45, 28, 13}};
int rainbowIndex = 0;
int lastFound[5] = {-1, -1, -1, -1, -1};
int nextIsSupply = 0;
int nextIsGnd = 0;
int justCleared = 1;

int scanRows(int pin, bool clearLastFound)
{

    int found = -1;

    if (clearLastFound)
    {

        rainbowIndex++;
        if (rainbowIndex > 11)
        {
            rainbowIndex = 0;
        }

        justCleared = 1;
        nextIsGnd = 0;
        nextIsSupply = 0;
        return -1;
    }

    if (checkProbeButton() == 1)
    {
        return -18;
    }

    pin = ADC1_PIN;

    digitalWrite(RESETPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(RESETPIN, LOW);
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

    // Serial.print("xMapRead: ");
    // Serial.println(xMapRead);

    pinMode(pin, INPUT);

    for (int chipScan = CHIP_A; chipScan < 8; chipScan++) // scan the breadboard (except the corners)
    {

        sendXYraw(CHIP_L, xMapRead, chipScan, 1);

        for (int yToScan = 1; yToScan < 8; yToScan++)
        {

            sendXYraw(chipScan, 0, 0, 1);
            sendXYraw(chipScan, 0, yToScan, 1);

            // analogRead(ADC0_PIN);

            rowBeingScanned = ch[chipScan].yMap[yToScan];
            if (readFloatingOrState(pin, rowBeingScanned) == probe && rowBeingScanned != lastFound[0])
            {
                found = rowBeingScanned;
                if (nextIsSupply)
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
                }
                else if (nextIsGnd)
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
                }
                else
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
                }
                showLEDsCore2 = 1;
                // leds.show();
                for (int i = 4; i > 0; i--)
                {
                    lastFound[i] = lastFound[i - 1];
                }
                lastFound[0] = found;
            }

            sendXYraw(chipScan, 0, 0, 0);
            sendXYraw(chipScan, 0, yToScan, 0);

            if (found != -1)
            {
                stopProbe();
                break;
            }
        }
        sendXYraw(CHIP_L, 2, chipScan, 0);
    }

    int corners[4] = {1, 30, 31, 60};
    sendXYraw(CHIP_L, xMapRead, 0, 1);
    for (int cornerScan = 0; cornerScan < 4; cornerScan++)
    {

        sendXYraw(CHIP_L, cornerScan + 8, 0, 1);

        // analogRead(ADC0_PIN);

        rowBeingScanned = corners[cornerScan];
        if (readFloatingOrState(pin, rowBeingScanned) == probe && rowBeingScanned != lastFound[0])
        {
            found = rowBeingScanned;
            if (nextIsSupply)
            {
                leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
            }
            else if (nextIsGnd)
            {
                leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
            }
            else
            {
                leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
            }
            showLEDsCore2 = 1;
            // leds.show();
            for (int i = 4; i > 0; i--)
            {
                lastFound[i] = lastFound[i - 1];
            }
            lastFound[0] = found;
        }

        sendXYraw(CHIP_L, cornerScan + 8, 0, 0);

        if (found != -1)
        {
            stopProbe();
            break;
        }
    }
    sendXYraw(CHIP_L, xMapRead, 0, 0);

    for (int chipScan2 = CHIP_I; chipScan2 <= CHIP_J; chipScan2++) // scan the breadboard (except the corners)
    {
        // Serial.print("pin: ");
        // Serial.println(pin);
        int pinHeader = ADC0_PIN + (chipScan2 - CHIP_I);
        // Serial.print("pinHeader: ");
        // Serial.println(pinHeader);
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

            if (readFloatingOrState(pinHeader, rowBeingScanned) == probe && rowBeingScanned != lastFound[0])
            {
                Serial.print("rowBeingScanned: ");
                Serial.println(rowBeingScanned);
                found = rowBeingScanned;

                if (nextIsSupply)
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 65, 10, 10);
                }
                else if (nextIsGnd)
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], 10, 65, 10);
                }
                else
                {
                    leds.setPixelColor(nodesToPixelMap[rowBeingScanned], rainbowList[rainbowIndex][0], rainbowList[rainbowIndex][1], rainbowList[rainbowIndex][2]);
                }
                showLEDsCore2 = 1;
                // leds.show();
                for (int i = 4; i > 0; i--)
                {
                    lastFound[i] = lastFound[i - 1];
                }
                lastFound[0] = found;
            }
            sendXYraw(chipScan2, xToScan, 0, 0);
            sendXYraw(chipScan2, 13, 0, 0);

            if (found != -1)
            {
                stopProbe();

                break;
            }
        }
    }

    pinMode(19, INPUT);
    delayMicroseconds(900);
    int probeRead = readFloatingOrState(19, -1);

    if (probeRead == high && ((lastFound[0] != SUPPLY_3V3)))
    {
        found = SUPPLY_3V3;
        if (justCleared)
        {
            nextIsSupply = 1;
            // justCleared = 0;
        }
        else
        {
            leds.setPixelColor(nodesToPixelMap[lastFound[0]], 65, 10, 10);
            showLEDsCore2 = 1;
            nextIsSupply = 0;
        }

        for (int i = 4; i > 0; i--)
        {
            lastFound[i] = lastFound[i - 1];
        }
        lastFound[0] = found;
    }

    else if (probeRead == low && ((lastFound[0] != GND)))
    {
        found = GND;
        if (justCleared)
        {
            // leds.setPixelColor(nodesToPixelMap[lastFound[0]], 0, 0, 0);
            nextIsGnd = 1;
            // justCleared = 0;
        }
        else
        {
            leds.setPixelColor(nodesToPixelMap[lastFound[0]], 10, 65, 10);
            showLEDsCore2 = 1;
            nextIsGnd = 0;
        }

        for (int i = 4; i > 0; i--)
        {
            lastFound[i] = lastFound[i - 1];
        }
        lastFound[0] = found;
    }

    if (justCleared && found != -1)
    {
        // Serial.print("\n\rjustCleared: ");
        // Serial.println(justCleared);
        // Serial.print("nextIsSupply: ");
        // Serial.println(nextIsSupply);
        // Serial.print("nextIsGnd: ");
        // Serial.println(nextIsGnd);

        justCleared = 0;
    }
    stopProbe();
    return found;

    // return 0;
}