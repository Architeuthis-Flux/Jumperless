#include "FileParsing.h"
#include <Arduino.h>
#include "LittleFS.h"
#include "MatrixStateRP2040.h"
#include "SafeString.h"
#include "NetManager.h"

static bool debugFP;
static bool debugFPtime;

createSafeString(nodeFileString, 1200);
createSafeString(bridgeString, 800);
createSafeString(specialFunctionsString, 400);

File nodeFile;

unsigned long timeToFP = 0;


void openNodeFile()
{
    timeToFP = millis();
    if(DEBUG_FILEPARSING == 1) debugFP = true; //yeah we're using runtime debug flags so it can be toggled from commands
    else debugFP = false;
    if(TIME_FILEPARSING == 1) debugFPtime = true;
    else debugFPtime = false;

    nodeFile = LittleFS.open("nodeFile.txt", "r");
    if (!nodeFile)
    {
        if(debugFP)Serial.println("Failed to open nodeFile");
        return;
    }
    else
    {
        if(debugFP)Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
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

    if(debugFP)Serial.println("\n\rraw input file\n\r");
    if(debugFP)Serial.println(nodeFileString);
    if(debugFP)Serial.println("\n\rsplitting and cleaning up string\n\r");
    if(debugFP)Serial.println("_");
    openBraceIndex = nodeFileString.indexOf("{");
    closeBraceIndex = nodeFileString.indexOf("}");
    nodeFileString.substring(bridgeString ,openBraceIndex + 1, closeBraceIndex);
    bridgeString.trim();

    if(debugFP)Serial.println(bridgeString);

    if(debugFP)Serial.println("^\n\r");

    nodeFileString.remove(0, closeBraceIndex + 1);
    nodeFileString.trim();

    openBraceIndex = nodeFileString.indexOf("{");
    closeBraceIndex = nodeFileString.indexOf("}");
    nodeFileString.substring(specialFunctionsString, openBraceIndex + 1, closeBraceIndex);
    specialFunctionsString.trim();
    if(debugFP)Serial.println("_");
    if(debugFP)Serial.println(specialFunctionsString);
    if(debugFP)Serial.println("^\n\r");
    replaceSFNamesWithDefinedInts();
}

void replaceSFNamesWithDefinedInts(void)
{
    if(debugFP)Serial.println("replacing special function names with defined ints\n\r");
    specialFunctionsString.replace("GND", "100");
    specialFunctionsString.replace("SUPPLY_5V", "105");
    specialFunctionsString.replace("SUPPLY_3V3", "103");
    specialFunctionsString.replace("DAC0_5V", "106");
    specialFunctionsString.replace("DAC1_8V", "107");
    specialFunctionsString.replace("I_N", "109");
    specialFunctionsString.replace("I_P", "108");
    specialFunctionsString.replace("EMPTY_NET", "127");

    if(debugFP)Serial.println(specialFunctionsString);
    if(debugFP)Serial.println("\n\n\r");

   replaceNanoNamesWithDefinedInts();
}

void replaceNanoNamesWithDefinedInts(void)//for dome reason Arduino's String wasn't replacing like 1 or 2 of the names, so I'm using SafeString now and it works 
{
    if(debugFP)Serial.println("replacing special function names with defined ints\n\r");
    char nanoName[5];
   

    itoa(NANO_D10, nanoName, 10);
    specialFunctionsString.replace("D10", nanoName);
    bridgeString.replace("D10", nanoName);
    itoa(NANO_D11, nanoName, 10);
    specialFunctionsString.replace("D11", nanoName);
    bridgeString.replace("D11", nanoName);
    itoa(NANO_D12, nanoName, 10);
    specialFunctionsString.replace("D12", nanoName);
    bridgeString.replace("D12", nanoName);
    itoa(NANO_D13, nanoName, 10);
    specialFunctionsString.replace("D13", nanoName);
    bridgeString.replace("D13", nanoName);
    itoa(NANO_D0, nanoName, 10);
    specialFunctionsString.replace("D0", nanoName);
    bridgeString.replace("D0", nanoName);
    itoa(NANO_D1, nanoName, 10);
    specialFunctionsString.replace("D1", nanoName);
    bridgeString.replace("D1", nanoName);
    itoa(NANO_D2, nanoName, 10);
    specialFunctionsString.replace("D2", nanoName);
    bridgeString.replace("D2", nanoName);
    itoa(NANO_D3, nanoName, 10);
    specialFunctionsString.replace("D3", nanoName);
    bridgeString.replace("D3", nanoName);
    itoa(NANO_D4, nanoName, 10);
    specialFunctionsString.replace("D4", nanoName);
    bridgeString.replace("D4", nanoName);
    itoa(NANO_D5, nanoName, 10);
    specialFunctionsString.replace("D5", nanoName);
    bridgeString.replace("D5", nanoName);
    itoa(NANO_D6, nanoName, 10);
    specialFunctionsString.replace("D6", nanoName);
    bridgeString.replace("D6", nanoName);
    itoa(NANO_D7, nanoName, 10);
    specialFunctionsString.replace("D7", nanoName);
    bridgeString.replace("D7", nanoName);
    itoa(NANO_D8, nanoName, 10);
    specialFunctionsString.replace("D8", nanoName);
    bridgeString.replace("D8", nanoName);
    itoa(NANO_D9, nanoName, 10);
    specialFunctionsString.replace("D9", nanoName);
    bridgeString.replace("D9", nanoName);
    itoa(NANO_RESET, nanoName, 10);
    specialFunctionsString.replace("RESET", nanoName);
    bridgeString.replace("RESET", nanoName);
    itoa(NANO_AREF, nanoName, 10);
    specialFunctionsString.replace("AREF", nanoName);
    bridgeString.replace("AREF", nanoName);
    itoa(NANO_A0, nanoName, 10);
    specialFunctionsString.replace("A0", nanoName);
    bridgeString.replace("A0", nanoName);
    itoa(NANO_A1, nanoName, 10);
    specialFunctionsString.replace("A1", nanoName);
    bridgeString.replace("A1", nanoName);
    itoa(NANO_A2, nanoName, 10);
    specialFunctionsString.replace("A2", nanoName);
    bridgeString.replace("A2", nanoName);
    itoa(NANO_A3, nanoName, 10);
    specialFunctionsString.replace("A3", nanoName);
    bridgeString.replace("A3", nanoName);
    itoa(NANO_A4, nanoName, 10);
    specialFunctionsString.replace("A4", nanoName);
    bridgeString.replace("A4", nanoName);
    itoa(NANO_A5, nanoName, 10);
    specialFunctionsString.replace("A5", nanoName);
    bridgeString.replace("A5", nanoName);
    itoa(NANO_A6, nanoName, 10);
    specialFunctionsString.replace("A6", nanoName);
    bridgeString.replace("A6", nanoName);
    itoa(NANO_A7, nanoName, 10);
    specialFunctionsString.replace("A7", nanoName);
    bridgeString.replace("A7", nanoName);


    if(debugFP)Serial.println(bridgeString);
    if(debugFP)Serial.println(specialFunctionsString);
    if(debugFP)Serial.println("\n\n\r");

 parseStringToBridges();
}

void parseStringToBridges(void)
{

    int bridgeStringLength = bridgeString.length() - 1;

    int specialFunctionsStringLength = specialFunctionsString.length() - 1;

    int readLength = 0;
    int readTotal = specialFunctionsStringLength;

    newBridgeLength = 0;
    newBridgeIndex = 0;

     if(debugFP)Serial.println("parsing bridges into array\n\r");

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
    }
    newBridgeIndex = 0;
        if(debugFP) for (int i = 0; i < newBridgeLength; i++)
    {
        Serial.print("[");
        Serial.print(path[newBridgeIndex].node1);
        Serial.print("-");
        Serial.print(path[newBridgeIndex].node2);
        Serial.print("],");
        newBridgeIndex++;
    }
    if(debugFP)Serial.print("\n\rbridge pairs = ");
    if(debugFP)Serial.println(newBridgeLength);

    nodeFileString.clear();


    // if(debugFP)Serial.println(nodeFileString);
    timeToFP = millis() - timeToFP;
    if(debugFP)Serial.print("\n\rtook ");

    if(debugFPtime)Serial.print(timeToFP);
    if(debugFPtime)Serial.println("ms to open and parse file\n\r");
}

