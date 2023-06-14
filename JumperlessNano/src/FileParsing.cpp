#include "FileParsing.h"
#include <Arduino.h>
#include "LittleFS.h"
#include "MatrixStateRP2040.h"
#include "SafeString.h"
#include "ArduinoJson.h"
#include "NetManager.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"


static bool debugFP;
static bool debugFPtime;

createSafeString(nodeFileString, 1200);

createSafeString(nodeString, 1200);
createSafeString(specialFunctionsString, 800);

char inputBuffer[5000] = {0};

ArduinoJson::StaticJsonDocument<5000> wokwiJson;

String connectionsW[MAX_BRIDGES][5];

File nodeFile;
File wokwiFile;

unsigned long timeToFP = 0;

int numConnsJson = 0;

void parseWokwiFileToNodeFile(void)
{

    delay(3000);
    LittleFS.begin();
    timeToFP = millis();
    if (DEBUG_FILEPARSING == 1)
        debugFP = true; // yeah we're using runtime debug flags so it can be toggled from commands
    else
        debugFP = false;
    if (TIME_FILEPARSING == 1)
        debugFPtime = true;
    else
        debugFPtime = false;

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
            Serial.println("\n\ropened wokwi.txt\n\n\n\r");
    }

    Serial.println("paste Wokwi diagram.json here");
    while (Serial.available() == 0)
    {
    }

    int numCharsRead = 0;

    while (Serial.available() > 0)
    {
        char c = Serial.read();
        inputBuffer[numCharsRead] = c;

        numCharsRead++;
        delay(1);
    }

    createSafeStringFromCharArray(wokwiFileString, inputBuffer);
delay(10);
    wokwiFile.write(inputBuffer, numCharsRead);

    delay(10);

    wokwiFile.seek(0);

   
    Serial.println("\n\n\rwokwiFile\n\n\r");

   /* for (int i = 0; i < numCharsRead; i++)
    {
        Serial.print((char)wokwiFile.read());
    }*/

Serial.print(wokwiFileString);

    Serial.println("\n\n\rnumCharsRead\n\n\r");

    Serial.print(numCharsRead);

    wokwiFile.close();

deserializeJson(wokwiJson, inputBuffer);

Serial.println("\n\n\rwokwiJson\n\n\r");

//serializeJsonPretty(wokwiJson, Serial);
Serial.println("\n\n\rconnectionsW\n\n\r");

numConnsJson = wokwiJson["connections"].size();



copyArray(wokwiJson["connections"], connectionsW);



//deserializeJson(connectionsW, Serial);
 Serial.println(wokwiJson["connections"].size());


for (int i = 0; i < MAX_BRIDGES; i++)
{
    //Serial.println(wokwiJson["connections"].size());
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


changeWokwiDefinesToJumperless();



writeToNodeFile();
//while(1);

   
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
    Serial.println(' ');

    for (int j = 0; j < 2; j++)
{
nodeNumber = -1;
connString1  = connectionsW[i][j];
Serial.print(connString1);
Serial.print("   \t\t  ");

if (connString1.startsWith("bb1:") || connString1.startsWith("bb2:"))
{
    //Serial.print("bb1 or bb2  ");
    
    int periodIndex = connString1.indexOf(".");
    connString1 = connString1.substring(4,periodIndex);

    if (connString1.endsWith("b"))
    {
        nodeNumber = 30;
        //Serial.println("bottom");
        connString1.substring(0,connString1.length()-1);
        nodeNumber += connString1.toInt();
    }
    else if (connString1.endsWith("t"))
    {
        nodeNumber = 0;
        //Serial.println("top");
        connString1.substring(0,connString1.length()-1);
        nodeNumber += connString1.toInt();
    } else if (connString1.endsWith("n"))
    {
        nodeNumber = GND;
    } else if (connString1.endsWith("p"))
    {
        nodeNumber = SUPPLY_5V;
    }

} else if (connString1.startsWith("nano:"))
{
    //Serial.print("nano\t");
        int periodIndex = connString1.indexOf(".");
    connString1 = connString1.substring(5,periodIndex);

    nodeNumber = NANO_D0;

    if (isDigit(connString1[connString1.length()-1]))
    {

        nodeNumber += connString1.toInt();

    } else if (connString1.equals("5V"))
    {
        nodeNumber = SUPPLY_5V;
    } else if (connString1.equalsIgnoreCase("AREF"))
    {

    nodeNumber = NANO_AREF;
    } else if (connString1.equalsIgnoreCase("GND"))
    {
        nodeNumber = GND;
    } else if (connString1.equalsIgnoreCase("RESET"))
    {

    nodeNumber = NANO_RESET;
    } else if (connString1.equalsIgnoreCase("3.3V"))
    {
        nodeNumber = SUPPLY_3V3;
    } else if (connString1.startsWith ("A"))
    {
        nodeNumber = NANO_A0;
        nodeNumber += connString1.toInt();
    }




} else if (connString1.startsWith("vcc1:"))
{
    //Serial.print("vcc1\t");
    nodeNumber = SUPPLY_5V;

}else if (connString1.startsWith("vcc2:"))
{
    //Serial.print("vcc2\t");
    nodeNumber = SUPPLY_3V3;

}  else if (connString1.startsWith("gnd1:"))
{
    //Serial.print("gnd1\t");
    nodeNumber = GND;
} else if (connString1.startsWith("gnd2:"))
{
    //Serial.print("gnd2\t");
    nodeNumber = GND;
} else if (connString1.startsWith("gnd3:"))
{
    nodeNumber = GND;
} else {


    connectionsW[i][j] = -1;



}




    //nodeNumber += connString1.toInt();

    connectionsW[i][j] = nodeNumber;
    Serial.print(connectionsW[i][j]);
   
    //connectionsW[i][0] = connString1;

    Serial.print("   \t ");

    //Serial.println(connString1);

//Serial.println(connString1);
}

}

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
        if (connectionsW[i][0] ==  connectionsW[i][1])
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

Serial.println("wrote to nodeFile.txt");

Serial.println("nodeFile.txt contents:");
nodeFile.seek(0);
while (nodeFile.available())
{
    Serial.write(nodeFile.read());
}
Serial.println("\n\r");


    nodeFile.close();

}





void openNodeFile()
{
    timeToFP = millis();
    if (DEBUG_FILEPARSING == 1)
        debugFP = true; // yeah we're using runtime debug flags so it can be toggled from commands
    else
        debugFP = false;
    if (TIME_FILEPARSING == 1)
        debugFPtime = true;
    else
        debugFPtime = false;

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
    if (debugFP)
        Serial.print("\n\rtook ");

    if (debugFPtime)
        Serial.print(timeToFP);
    if (debugFPtime)
        Serial.println("ms to open and parse file\n\r");
}
