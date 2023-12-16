#include "FileParsing.h"
#include <Arduino.h>
#include "LittleFS.h"
#include "MatrixStateRP2040.h"
#include "SafeString.h"
#include "ArduinoJson.h"
#include "NetManager.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include <EEPROM.h>

bool debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
bool debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);



createSafeString(nodeFileString, 1200);

createSafeString(nodeString, 1200);
createSafeString(specialFunctionsString, 800);

char inputBuffer[8000] = {0};

ArduinoJson::StaticJsonDocument<8000> wokwiJson;

String connectionsW[MAX_BRIDGES][5];

File nodeFile;
File wokwiFile;

unsigned long timeToFP = 0;

int numConnsJson = 0;

void savePreformattedNodeFile(int source)
{
    LittleFS.remove("nodeFile.txt");

    nodeFile = LittleFS.open("nodeFile.txt", "w+");

if (source == 0)
{
    while (Serial.available() == 0)
{}

    while (Serial.available() > 0)
    {
        nodeFile.write(Serial.read());
        delay(1);
    }

}
if (source == 1)
{
    while (Serial1.available() == 0)
{}

    //Serial.println("waiting for Arduino to send file");
    while (Serial1.available() > 0)
    {
        nodeFile.write(Serial1.read());
        delayMicroseconds(400);
        //Serial.print(chw);
    }
}


    nodeFile.close();
}
void parseWokwiFileToNodeFile(void)
{

    // delay(3000);
    LittleFS.begin();
    timeToFP = millis();

    wokwiFile = LittleFS.open("wokwi.txt", "w+");
    if (!wokwiFile)
    {
        if (debugFP)
            Serial.println("Failed to open wokwi.txt");
        return;
    }
    else
    {
        if (debugFP)
        {
            Serial.println("\n\ropened wokwi.txt\n\r");
        }
        else
        {
            // Serial.println("\n\r");
        }
    }

    Serial.println("paste Wokwi diagram.json here\n\r");
    while (Serial.available() == 0)
    {
    }

    int numCharsRead = 0;

    char firstChar = Serial.read();

    if (firstChar != '{') // in case you just paste a wokwi file in from the menu, the opening brace will have already been read
    {
        inputBuffer[numCharsRead] = '{';
        numCharsRead++;
    }
    else
    {
        inputBuffer[numCharsRead] = firstChar;
        numCharsRead++;
    }
    /*
        Serial.println(firstChar);
      Serial.println(firstChar);
        Serial.println(firstChar);
       Serial.println(firstChar);
       Serial.println(firstChar);
       Serial.print(firstChar);
    */
    delay(1);
    while (Serial.available() > 0)
    {
        char c = Serial.read();
        inputBuffer[numCharsRead] = c;

        numCharsRead++;

        delayMicroseconds(1000);
    }

    createSafeStringFromCharArray(wokwiFileString, inputBuffer);
    delay(10);
    wokwiFile.write(inputBuffer, numCharsRead);

    delay(10);

    wokwiFile.seek(0);

    if (debugFP)
        Serial.println("\n\n\rwokwiFile\n\n\r");

    /* for (int i = 0; i < numCharsRead; i++)
     {
         Serial.print((char)wokwiFile.read());
     }*/
    if (debugFP)
    {
        Serial.print(wokwiFileString);

        Serial.println("\n\n\rnumCharsRead = ");

        Serial.print(numCharsRead);

        Serial.println("\n\n\r");
    }
    wokwiFile.close();

    deserializeJson(wokwiJson, inputBuffer);

    if (debugFP)
    {

        Serial.println("\n\n\rwokwiJson\n\n\r");

        Serial.println("\n\n\rconnectionsW\n\n\r");
    }

    numConnsJson = wokwiJson["connections"].size();

    copyArray(wokwiJson["connections"], connectionsW);

    // deserializeJson(connectionsW, Serial);
    if (debugFP)
    {
        Serial.println(wokwiJson["connections"].size());

        for (int i = 0; i < MAX_BRIDGES; i++)
        {
            // Serial.println(wokwiJson["connections"].size());
            if (connectionsW[i][0] == "")
            {
                break;
            }
            Serial.print(connectionsW[i][0]);
            Serial.print(",   \t ");

            Serial.print(connectionsW[i][1]);
            Serial.print(",   \t ");

            Serial.print(connectionsW[i][2]);
            Serial.print(",   \t ");

            Serial.print(connectionsW[i][3]);
            Serial.print(",   \t ");

            Serial.print(connectionsW[i][4]);
            Serial.print(",   \t ");

            Serial.println();
        }

        Serial.println("\n\n\rRedefining\n\n\r");
    }

    changeWokwiDefinesToJumperless();

    writeToNodeFile();
    // while(1);
    openNodeFile();
}

void changeWokwiDefinesToJumperless(void)
{

    String connString1 = "                               ";
    String connString2 = "                               ";
    String connStringColor = "                               ";
    String bb = "bb1:";

    int nodeNumber;

    for (int i = 0; i < numConnsJson; i++)
    {
        if (debugFP)
        {
            Serial.println(' ');
        }
        for (int j = 0; j < 2; j++)
        {
            nodeNumber = -1;
            connString1 = connectionsW[i][j];
            if (debugFP)
            {
                Serial.print(connString1);
                Serial.print("   \t\t  ");
            }
            if (connString1.startsWith("bb1:") || connString1.startsWith("bb2:"))
            {
                // Serial.print("bb1 or bb2  ");

                int periodIndex = connString1.indexOf(".");
                connString1 = connString1.substring(4, periodIndex);

                if (connString1.endsWith("b"))
                {
                    nodeNumber = 30;
                    // Serial.println("bottom");
                    connString1.substring(0, connString1.length() - 1);
                    nodeNumber += connString1.toInt();
                }
                else if (connString1.endsWith("t"))
                {
                    nodeNumber = 0;
                    // Serial.println("top");
                    connString1.substring(0, connString1.length() - 1);
                    nodeNumber += connString1.toInt();
                }
                else if (connString1.endsWith("n"))
                {
                    nodeNumber = GND;
                }
                else if (connString1.endsWith("p"))
                {
                    nodeNumber = SUPPLY_5V;
                }
            }
            else if (connString1.startsWith("nano:"))
            {
                // Serial.print("nano\t");
                int periodIndex = connString1.indexOf(".");
                connString1 = connString1.substring(5, periodIndex);

                nodeNumber = NANO_D0;

                if (isDigit(connString1[connString1.length() - 1]))
                {

                    nodeNumber += connString1.toInt();
                }
                else if (connString1.equals("5V"))
                {
                    nodeNumber = SUPPLY_5V;
                }
                else if (connString1.equalsIgnoreCase("AREF"))
                {

                    nodeNumber = NANO_AREF;
                }
                else if (connString1.equalsIgnoreCase("GND"))
                {
                    nodeNumber = GND;
                }
                else if (connString1.equalsIgnoreCase("RESET"))
                {

                    nodeNumber = NANO_RESET;
                }
                else if (connString1.equalsIgnoreCase("3.3V"))
                {
                    nodeNumber = SUPPLY_3V3;
                }
                else if (connString1.startsWith("A"))
                {
                    nodeNumber = NANO_A0;
                    nodeNumber += connString1.toInt();
                }
            }
            else if (connString1.startsWith("vcc1:"))
            {
                // Serial.print("vcc1\t");
                nodeNumber = SUPPLY_5V;
            }
            else if (connString1.startsWith("vcc2:"))
            {
                // Serial.print("vcc2\t");
                nodeNumber = SUPPLY_3V3;
            }
            else if (connString1.startsWith("gnd1:"))
            {
                // Serial.print("gnd1\t");
                nodeNumber = GND;
            }
            else if (connString1.startsWith("gnd2:"))
            {
                // Serial.print("gnd2\t");
                nodeNumber = GND;
            }
            else if (connString1.startsWith("gnd3:"))
            {
                nodeNumber = GND;
            }
            else if (connString1.startsWith("pot1:"))
            {
                nodeNumber = DAC0_5V;
            }
            else
            {

                connectionsW[i][j] = -1;
            }

            // nodeNumber += connString1.toInt();

            connectionsW[i][j] = nodeNumber;
            if (debugFP)
            {
                Serial.print(connectionsW[i][j]);

                Serial.print("   \t ");
            }
        }
    }
}
void clearNodeFile(void)
{
    LittleFS.remove("nodeFile.txt");
}

void writeToNodeFile(void)
{

    LittleFS.remove("nodeFile.txt");
    delay(10);
    nodeFile = LittleFS.open("nodeFile.txt", "w+");
    if (!nodeFile)
    {
        if (debugFP)
            Serial.println("Failed to open nodeFile");
        return;
    }
    else
    {
        if (debugFP)
            Serial.println("\n\rrecreated nodeFile.txt\n\n\rloading bridges from wokwi.txt\n\r");
    }
    nodeFile.print("{\n\r");
    for (int i = 0; i < numConnsJson; i++)
    {
        if (connectionsW[i][0] == "-1" && connectionsW[i][1] != "-1")
        {
            lightUpNode(connectionsW[i][0].toInt());
            continue;
        }
        if (connectionsW[i][1] == "-1" && connectionsW[i][0] != "-1")
        {
            lightUpNode(connectionsW[i][1].toInt());
            continue;
        }
        if (connectionsW[i][0] == connectionsW[i][1])
        {
            lightUpNode(connectionsW[i][0].toInt());
            continue;
        }

        nodeFile.print(connectionsW[i][0]);
        nodeFile.print("-");
        nodeFile.print(connectionsW[i][1]);
        nodeFile.print(",\n\r");
    }
    nodeFile.print("}\n\r");

    if (debugFP)
    {
        Serial.println("wrote to nodeFile.txt");

        Serial.println("nodeFile.txt contents:");
        nodeFile.seek(0);

        while (nodeFile.available())
        {
            Serial.write(nodeFile.read());
        }
        Serial.println("\n\r");
    }
    nodeFile.close();
}

void openNodeFile()
{
    timeToFP = millis();

    nodeFile = LittleFS.open("nodeFile.txt", "r");
    if (!nodeFile)
    {
        if (debugFP)
            Serial.println("Failed to open nodeFile");
        return;
    }
    else
    {
        if (debugFP)
            Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

    nodeFileString.read(nodeFile);

    nodeFile.close();
    splitStringToFields();
    // parseStringToBridges();
}

void splitStringToFields()
{
    int openBraceIndex = 0;
    int closeBraceIndex = 0;

    if (debugFP)
        Serial.println("\n\rraw input file\n\r");
    if (debugFP)
        Serial.println(nodeFileString);
    if (debugFP)
        Serial.println("\n\rsplitting and cleaning up string\n\r");
    if (debugFP)
        Serial.println("_");
    openBraceIndex = nodeFileString.indexOf("{");
    closeBraceIndex = nodeFileString.indexOf("}");
    nodeFileString.substring(specialFunctionsString, openBraceIndex + 1, closeBraceIndex);
    specialFunctionsString.trim();

    if (debugFP)
        Serial.println(specialFunctionsString);

    if (debugFP)
        Serial.println("^\n\r");
    /*
        nodeFileString.remove(0, closeBraceIndex + 1);
        nodeFileString.trim();

        openBraceIndex = nodeFileString.indexOf("{");
        closeBraceIndex = nodeFileString.indexOf("}");
        //nodeFileString.substring(specialFunctionsString, openBraceIndex + 1, closeBraceIndex);
        specialFunctionsString.trim();
        if(debugFP)Serial.println("_");
        if(debugFP)Serial.println(specialFunctionsString);
        if(debugFP)Serial.println("^\n\r");
        */
    replaceSFNamesWithDefinedInts();
}

void replaceSFNamesWithDefinedInts(void)
{
    if (debugFP)
        Serial.println("replacing special function names with defined ints\n\r");

    specialFunctionsString.replace("GND", "100");
    specialFunctionsString.replace("SUPPLY_5V", "105");
    specialFunctionsString.replace("SUPPLY_3V3", "103");
    specialFunctionsString.replace("DAC0_5V", "106");
    specialFunctionsString.replace("DAC1_8V", "107");
    specialFunctionsString.replace("I_N", "109");
    specialFunctionsString.replace("I_P", "108");
    specialFunctionsString.replace("EMPTY_NET", "127");
    specialFunctionsString.replace("ADC0_5V", "110");
    specialFunctionsString.replace("ADC1_5V", "111");
    specialFunctionsString.replace("ADC2_5V", "112");
    specialFunctionsString.replace("ADC3_8V", "113");

    // if(debugFP)Serial.println(specialFunctionsString);
    // if(debugFP)Serial.println("\n\n\r");

    replaceNanoNamesWithDefinedInts();
}

void replaceNanoNamesWithDefinedInts(void) // for dome reason Arduino's String wasn't replacing like 1 or 2 of the names, so I'm using SafeString now and it works
{
    if (debugFP)
        Serial.println("replacing special function names with defined ints\n\r");

    char nanoName[5];

    itoa(NANO_D10, nanoName, 10);
    specialFunctionsString.replace("D10", nanoName);

    itoa(NANO_D11, nanoName, 10);
    specialFunctionsString.replace("D11", nanoName);

    itoa(NANO_D12, nanoName, 10);
    specialFunctionsString.replace("D12", nanoName);

    itoa(NANO_D13, nanoName, 10);
    specialFunctionsString.replace("D13", nanoName);

    itoa(NANO_D0, nanoName, 10);
    specialFunctionsString.replace("D0", nanoName);

    itoa(NANO_D1, nanoName, 10);
    specialFunctionsString.replace("D1", nanoName);

    itoa(NANO_D2, nanoName, 10);
    specialFunctionsString.replace("D2", nanoName);

    itoa(NANO_D3, nanoName, 10);
    specialFunctionsString.replace("D3", nanoName);

    itoa(NANO_D4, nanoName, 10);
    specialFunctionsString.replace("D4", nanoName);

    itoa(NANO_D5, nanoName, 10);
    specialFunctionsString.replace("D5", nanoName);

    itoa(NANO_D6, nanoName, 10);
    specialFunctionsString.replace("D6", nanoName);

    itoa(NANO_D7, nanoName, 10);
    specialFunctionsString.replace("D7", nanoName);

    itoa(NANO_D8, nanoName, 10);
    specialFunctionsString.replace("D8", nanoName);

    itoa(NANO_D9, nanoName, 10);
    specialFunctionsString.replace("D9", nanoName);

    itoa(NANO_RESET, nanoName, 10);
    specialFunctionsString.replace("RESET", nanoName);

    itoa(NANO_AREF, nanoName, 10);
    specialFunctionsString.replace("AREF", nanoName);

    itoa(NANO_A0, nanoName, 10);
    specialFunctionsString.replace("A0", nanoName);

    itoa(NANO_A1, nanoName, 10);
    specialFunctionsString.replace("A1", nanoName);

    itoa(NANO_A2, nanoName, 10);
    specialFunctionsString.replace("A2", nanoName);

    itoa(NANO_A3, nanoName, 10);
    specialFunctionsString.replace("A3", nanoName);

    itoa(NANO_A4, nanoName, 10);
    specialFunctionsString.replace("A4", nanoName);

    itoa(NANO_A5, nanoName, 10);
    specialFunctionsString.replace("A5", nanoName);

    itoa(NANO_A6, nanoName, 10);
    specialFunctionsString.replace("A6", nanoName);

    itoa(NANO_A7, nanoName, 10);
    specialFunctionsString.replace("A7", nanoName);

    // if(debugFP)Serial.println(bridgeString);
    if (debugFP)
        Serial.println(specialFunctionsString);
    if (debugFP)
        Serial.println("\n\n\r");

    parseStringToBridges();
}

void parseStringToBridges(void)
{

    // int bridgeStringLength = bridgeString.length() - 1;

    int specialFunctionsStringLength = specialFunctionsString.length() - 1;

    int readLength = 0;
    int readTotal = specialFunctionsStringLength;

    newBridgeLength = 0;
    newBridgeIndex = 0;

    if (debugFP)
        Serial.println("parsing bridges into array\n\r");

    for (int i = 0; i <= specialFunctionsStringLength; i += readLength)
    {

        sscanf(specialFunctionsString.c_str(), "%i-%i,\n\r%n", &path[newBridgeIndex].node1, &path[newBridgeIndex].node2, &readLength);
        specialFunctionsString.remove(0, readLength);

        readTotal -= readLength;

        // if(debugFP)Serial.print(newBridge[newBridgeIndex][0]);
        // if(debugFP)Serial.print("-");
        // if(debugFP)Serial.println(newBridge[newBridgeIndex][1]);

        newBridgeLength++;
        newBridgeIndex++;

        // delay(500);
    }
    /*
        readTotal = bridgeStringLength;

        for (int i = 0; i <= bridgeStringLength; i += readLength)
        {

            sscanf(bridgeString.c_str(), "%i-%i,\n\r%n", &path[newBridgeIndex].node1, &path[newBridgeIndex].node2, &readLength);
            bridgeString.remove(0, readLength);

            readTotal -= readLength;

            // if(debugFP)Serial.print(newBridge[newBridgeIndex][0]);
            // if(debugFP)Serial.print("-");
            // if(debugFP)Serial.println(newBridge[newBridgeIndex][1]);

            newBridgeLength++;
            newBridgeIndex++;

            // delay(500);
        }*/
    newBridgeIndex = 0;
    if (debugFP)
        for (int i = 0; i < newBridgeLength; i++)
        {
            Serial.print("[");
            Serial.print(path[newBridgeIndex].node1);
            Serial.print("-");
            Serial.print(path[newBridgeIndex].node2);
            Serial.print("],");
            newBridgeIndex++;
        }
    if (debugFP)
        Serial.print("\n\rbridge pairs = ");
    if (debugFP)
        Serial.println(newBridgeLength);

    nodeFileString.clear();

    // if(debugFP)Serial.println(nodeFileString);
    timeToFP = millis() - timeToFP;
    if (debugFPtime)
        Serial.print("\n\rtook ");

    if (debugFPtime)
        Serial.print(timeToFP);
    if (debugFPtime)
        Serial.println("ms to open and parse file\n\r");
}

void debugFlagInit(void)
{

    if (EEPROM.read(FIRSTSTARTUPADDRESS) == 255)
    {
        EEPROM.write(FIRSTSTARTUPADDRESS, 0);
        EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
        EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
        EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
        EEPROM.write(TIME_NETMANAGERADDRESS, 0);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
        EEPROM.write(DEBUG_LEDSADDRESS, 0);
        EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
        EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);
        EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
       
        EEPROM.commit();
        delay(5);
    }

#ifdef EEPROMSTUFF
    debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
    debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);

    debugNM = EEPROM.read(DEBUG_NETMANAGERADDRESS);
    debugNMtime = EEPROM.read(TIME_NETMANAGERADDRESS);

    debugNTCC = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);
    debugNTCC2 = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

    LEDbrightnessRail = EEPROM.read(RAILBRIGHTNESSADDRESS);
    LEDbrightness = EEPROM.read(LEDBRIGHTNESSADDRESS);
    LEDbrightnessSpecial = EEPROM.read(SPECIALBRIGHTNESSADDRESS);

    debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);

#else

    debugFP = 1;
    debugFPtime = 1;

    debugNM = 1;
    debugNMtime = 1;

    debugNTCC = 1;
    debugNTCC2 = 1;

    // debugLEDs = 1;
#endif

    if (debugFP != 0 && debugFP != 1)
        EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

    if (debugFPtime != 0 && debugFPtime != 1)
        EEPROM.write(TIME_FILEPARSINGADDRESS, 0);

    if (debugNM != 0 && debugNM != 1)
        EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);

    if (debugNMtime != 0 && debugNMtime != 1)
        EEPROM.write(TIME_NETMANAGERADDRESS, 0);

    if (debugNTCC != 0 && debugNTCC != 1)
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);

    if (debugNTCC2 != 0 && debugNTCC2 != 1)
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);

    if (debugLEDs != 0 && debugLEDs != 1)
        EEPROM.write(DEBUG_LEDSADDRESS, 0);

    if (LEDbrightnessRail < 0 || LEDbrightnessRail > 200)
    {
        EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);

        LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
    }
    if (LEDbrightness < 0 || LEDbrightness > 200)
    {
        EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
        LEDbrightness = DEFAULTBRIGHTNESS;
    }

    if (LEDbrightnessSpecial < 0 || LEDbrightnessSpecial > 200)
    {
        EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
        LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;
    }

    EEPROM.commit();
    delay(5);
}

void debugFlagSet(int flag)
{
    int flagStatus;
    switch (flag)
    {
    case 1:
    {
        flagStatus = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
        if (flagStatus == 0)
        {
            EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);

            debugFP = true;
        }
        else
        {
            EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

            debugFP = false;
        }

        break;
    }
    case 2:
    {
        flagStatus = EEPROM.read(TIME_FILEPARSINGADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(TIME_FILEPARSINGADDRESS, 1);

            debugFPtime = true;
        }
        else
        {
            EEPROM.write(TIME_FILEPARSINGADDRESS, 0);

            debugFPtime = false;
        }

        break;
    }
    case 3:
    {
        flagStatus = EEPROM.read(DEBUG_NETMANAGERADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);

            debugNM = true;
        }
        else
        {
            EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);

            debugNM = false;
        }
        break;
    }
    case 4:
    {
        flagStatus = EEPROM.read(TIME_NETMANAGERADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(TIME_NETMANAGERADDRESS, 1);

            debugNMtime = true;
        }
        else
        {
            EEPROM.write(TIME_NETMANAGERADDRESS, 0);

            debugNMtime = false;
        }
        break;
    }
    case 5:
    {
        flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);

            debugNTCC = true;
        }
        else
        {
            EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);

            debugNTCC = false;
        }

        break;
    }
    case 6:
    {
        flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);

            debugNTCC2 = true;
        }
        else
        {
            EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);

            debugNTCC2 = false;
        }
        break;
    }

    case 7:
    {
        flagStatus = EEPROM.read(DEBUG_LEDSADDRESS);

        if (flagStatus == 0)
        {
            EEPROM.write(DEBUG_LEDSADDRESS, 1);

            debugLEDs = true;
        }
        else
        {
            EEPROM.write(DEBUG_LEDSADDRESS, 0);

            debugLEDs = false;
        }
        break;
    }

    case 0:
    {
        EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
        EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
        EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
        EEPROM.write(TIME_NETMANAGERADDRESS, 0);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
        EEPROM.write(DEBUG_LEDSADDRESS, 0);
        debugFP = false;
        debugFPtime = false;
        debugNM = false;
        debugNMtime = false;
        debugNTCC = false;
        debugNTCC2 = false;
        debugLEDs = false;
        break;
    }

    case 9:
    {
        EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);
        EEPROM.write(TIME_FILEPARSINGADDRESS, 1);
        EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);
        EEPROM.write(TIME_NETMANAGERADDRESS, 1);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);
        EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);
        EEPROM.write(DEBUG_LEDSADDRESS, 1);
        debugFP = true;
        debugFPtime = true;
        debugNM = true;
        debugNMtime = true;
        debugNTCC = true;
        debugNTCC2 = true;
        debugLEDs = true;
        break;
    }
    }
    delay(4);
    EEPROM.commit();
    delay(8);
    return;
}

void runCommandAfterReset(char command)
{
    if (EEPROM.read(CLEARBEFORECOMMANDADDRESS) == 1)
    {
        return;
    }
    else
    {

        EEPROM.write(CLEARBEFORECOMMANDADDRESS, 1);
        EEPROM.write(LASTCOMMANDADDRESS, command);
        EEPROM.commit();

        digitalWrite(RESETPIN, HIGH);
        delay(1);
        digitalWrite(RESETPIN, LOW);

        AIRCR_Register = 0x5FA0004; // hard reset
    }
}

char lastCommandRead(void)
{

    Serial.print("last command: ");

    Serial.println((char)EEPROM.read(LASTCOMMANDADDRESS));

    return EEPROM.read(LASTCOMMANDADDRESS);
}
void lastCommandWrite(char lastCommand)
{

    EEPROM.write(LASTCOMMANDADDRESS, lastCommand);
}