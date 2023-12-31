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
#include "MachineCommands.h"

bool debugMM = false;
// char inputBuffer[INPUTBUFFERLENGTH] = {0};

File nodeFileMachineMode;

createSafeString(nodeString, 1200);

ArduinoJson::DynamicJsonDocument machineModeJson(8000);

enum machineModeInstruction lastReceivedInstruction = unknown;

char machineModeInstructionString[NUMBEROFINSTRUCTIONS][20] = {"unknown", "netlist", "bridgelist", "lightnode", "lightnet", "getmeasurement", "gpio", "uart", "arduinoflash", "setnetcolor", "setnodecolor", "setsupplyswitch"};

enum machineModeInstruction parseMachineInstructions(void)
{

    int doneReading = 0;

    int instructionNumber = 0;

    for (int i = 0; i < INPUTBUFFERLENGTH; i++)
    {
        if (doneReading == 1)
        {
            inputBuffer[i] = '\0'; // eof
            break;
        }
        else
        {
            inputBuffer[i] = Serial.read();
        }

        delayMicroseconds(100);

        if (inputBuffer[i] == ']')
        {
            doneReading = 1;
            // inputBuffer[i] = ' '; //eof
            // break;
        }
    }

    char instructionBuffer[20] = {0};

    for (int i = 0; i < 20; i++)
    {
        if (inputBuffer[i] == '[')
        {
            // inputBuffer[i] = ' ';
            break;
        }
        instructionBuffer[i] = inputBuffer[i];
        inputBuffer[i] = ' ';
    }

    for (int i = 0; i < NUMBEROFINSTRUCTIONS; i++)
    {
        if (strcasecmp(instructionBuffer, machineModeInstructionString[i]) == 0)
        {
            if (debugMM)
            {
                Serial.print("found instruction ");
                Serial.println(instructionBuffer);
                Serial.println(i);
            }

            instructionNumber = i;
            break;
        }

        if (i == NUMBEROFINSTRUCTIONS - 1)
        {
            if (debugMM)
            {
                Serial.println("instruction not found");
            }

            instructionNumber = 0;
            lastReceivedInstruction = static_cast<machineModeInstruction>(instructionNumber);
            
            return lastReceivedInstruction;
        }
    }

    if (debugMM)
    {
        Serial.println("instruction buffer");
        Serial.println(instructionBuffer);
        Serial.println("\n\ninput buffer");
        Serial.println(inputBuffer);
    }

    lastReceivedInstruction = static_cast<machineModeInstruction>(instructionNumber);
    return lastReceivedInstruction;
}

void machineNetlistToNetstruct(void)
{
    char names[MAX_NETS][32] = {0};
    //char *numbers[MAX_NETS] = {0};
    //char *colors[MAX_NETS] = {0};
    //char *nodes[MAX_NETS] = {0};


    if (debugMM)
    {
        Serial.println(inputBuffer);
        Serial.println("\n\n\n");
    }

    deserializeJson(machineModeJson, inputBuffer);

    if (debugMM)
    {
        serializeJsonPretty(machineModeJson, Serial);
        Serial.println("\n\n\n");
    }

    for (int i = 0; i < MAX_NETS; i++)
    {
        int netIndex = 0;
        int nodesIndex = 0;

        if (machineModeJson[i].isNull() == true)
        {
            break;
        }

        netIndex = machineModeJson[i]["index"];



        strcpy(names[i] , machineModeJson[i]["name"]);


       net[netIndex].name = names[i];


        if (debugMM)
        {
            Serial.print("net name = ");
            Serial.println(net[netIndex].name);
        }

        if (machineModeJson[i]["number"].isNull() == false) // makes number an optional value
        {
            net[netIndex].number = machineModeJson[i]["number"];
        }
        else
        {
            net[netIndex].number = netIndex;
        }

        uint32_t rawColor = (uint32_t)removeHexPrefix(machineModeJson[i]["color"]);

        if (debugMM)
        {
            Serial.print("raw color = ");
            Serial.println(rawColor, HEX);
        }

        net[netIndex].rawColor = rawColor;
        net[netIndex].machine = true;

        if (netIndex < 8)
        {
            rawSpecialNetColors[netIndex] = rawColor;

            switch (netIndex)
            {
            case 1:
                for (int i = 0; i < 3; i++)
                {
                    rawRailColors[i][1] = rawColor;
                    rawRailColors[i][3] = rawColor;
                }
                break;
            case 2:
                rawRailColors[1][0] = rawColor;
                rawRailColors[1][2] = rawColor;

                break;
            case 3:
                rawRailColors[0][0] = rawColor;
                rawRailColors[0][2] = rawColor;
                break;
            }
        }

        char *nodeTokens[MAX_NODES] = {0};

        const char delim[5] = ",\"- ";

        char nodesChar[300];

        serializeJson(machineModeJson[i]["nodes"], nodesChar);

        if (debugMM)
            Serial.println(nodeTokens[0]);

        // Serial.println(nodesChar);

        for (int j = 0; j < MAX_NODES; j++)
        {

            if (j == 0)
            {
                nodeTokens[0] = strtok(nodesChar, delim);
            }
            else
            {
                nodeTokens[j] = strtok(NULL, delim);
            }

            if (nodeTokens[j] == NULL)
            {
                // Serial.println("it's null");
                break;
            }

            if (debugMM)
                Serial.println(nodeTokens[j]);

            int nodeToAdd = nodeTokenToInt(nodeTokens[j]);

            if (net[netIndex].nodes[0] == nodeToAdd) // this checks to see if it's trying to add a node that's already defined in the special functions
            {
                continue;
            }

            if (netIndex < 8 && nodesIndex == 0)
            {
                nodesIndex = 1;
            }
            // Serial.println(nodesIndex);

            net[netIndex].nodes[nodesIndex] = nodeToAdd;

            nodesIndex++;
        }
        if (debugMM)
            Serial.println("\n");
    }

    // clearLEDs();
    // assignNetColors();
    // showNets();
    // sendAllPathsCore2 = 1;
}

void populateBridgesFromNodes(void)
{

    for (int i = 1; i < MAX_NETS; i++)
    {
        int addBridgeIndex = 0;

        if (net[i].nodes[0] == -1 || net[i].nodes[0] == 0 || net[i].nodes[1] == -1 || net[i].nodes[1] == 0)
        {
            // continue;
        }
        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[i].nodes[j] == -1 || net[i].nodes[j] == 0)
            {
                break;
            }

            if (net[i].nodes[j] == net[i].nodes[0])
            {
                if (debugMM)
                {
                    Serial.print("net[");
                    Serial.print(i);
                    Serial.print("].nodes[");
                    Serial.print(j);
                    Serial.print("] = ");
                    Serial.print(net[i].nodes[j]);

                    Serial.print("\tnet[");
                    Serial.print(i);
                    Serial.print("].nodes[0] = ");
                    Serial.println(net[i].nodes[0]);
                }

                continue;
            }

            net[i].bridges[addBridgeIndex][0] = net[i].nodes[0];
            net[i].bridges[addBridgeIndex][1] = net[i].nodes[j];

            if (debugMM)
            {
                Serial.print("net[");
                Serial.print(i);
                Serial.print("].bridges[");
                Serial.print(addBridgeIndex);
                Serial.print("][0] = ");
                Serial.println(net[i].bridges[addBridgeIndex][0]);
                Serial.print("net[");
                Serial.print(i);
                Serial.print("].bridges[");
                Serial.print(addBridgeIndex);
                Serial.print("][1] = ");
                Serial.println(net[i].bridges[addBridgeIndex][1]);
                Serial.println();
            }

            addBridgeIndex++;
            // newBridgeIndex++;
            // newBridgeLength++;
        }
    }
}

int removeHexPrefix(const char *str)
{
    if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0)
    {
        const char *hexValue = str + 2;
        return strtol(hexValue, nullptr, 16);
    }
    return atoi(str);
}

int nodeTokenToInt(char *nodeToken)
{
    char uppercaseToken[20];
    strcpy(uppercaseToken, nodeToken);
    for (int i = 0; i < strlen(uppercaseToken); i++)
    {
        uppercaseToken[i] = toupper(uppercaseToken[i]);
    }
    // Serial.print("uppercaseToken = ");
    // Serial.println(uppercaseToken);

    for (int i = 0; i < 90; i++)
    {
        if (strcmp(uppercaseToken, sfMappings[i].name) == 0)
        {
            if (debugMM)
            {
                Serial.print("mapped value = ");
                Serial.println(sfMappings[i].replacement);
            }
            return sfMappings[i].replacement;
        }
    }
    int intToken = atoi(uppercaseToken);
    if (debugMM)
    {
        Serial.print("mapped value = ");
        Serial.println(intToken);
    }
    return intToken;
    // return -1; // Return -1 if no matching name is found
}

void writeNodeFileFromInputBuffer(void)
{
    LittleFS.remove("nodeFile.txt");
    delay(6);

    nodeFileMachineMode = LittleFS.open("nodeFile.txt", "w+");

    if (!nodeFileMachineMode)
    {
        if (debugFP)
            Serial.println("Failed to open nodeFileMachineMode");
        return;
    }
    else
    {
        if (debugFP)
            Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }
    // nodeFile.print("{\n\r");

    int inputBufferIndex = 0;

    for (int i = 0; i < INPUTBUFFERLENGTH; i++)
    {
        if (inputBuffer[i] == '\0')
        {
            break;
        }

        if (inputBuffer[i] == ']')
        {
            if (inputBuffer[i - 1] != ',')
            {
                nodeFileMachineMode.print(",]");
            }
            else
            {
                nodeFileMachineMode.print("]");
            }
            break;
        }
        nodeFileMachineMode.print(inputBuffer[i]);
    }

    // nodeFile.print("}\n\r");

    if (debugMM)
    {
        Serial.println("wrote to nodeFile.txt");

        Serial.println("nodeFile.txt contents:");
        nodeFileMachineMode.seek(0);

        while (nodeFileMachineMode.available())
        {
            Serial.write(nodeFileMachineMode.read());
        }
        Serial.println("\n\r");
    }
    nodeFileMachineMode.close();
}

void lightUpNodesFromInputBuffer(void)
{
    char *bufferPtr = inputBuffer;
    char *token[50];

    token[0] = strtok(bufferPtr, ",:[] ");

    int numTokens = 0;

    for (int i = 0; i < 50; i++)
    {

        if (i > 0)
        {
            token[i] = strtok(NULL, ",:[] ");
        }

        if (token[i] == NULL)
        {
            numTokens = i;
            break;
        }
    }

    uint32_t color = (uint32_t)removeHexPrefix(token[numTokens - 1]);

    // Serial.println( color, HEX);

    for (int i = 0; i < numTokens - 1; i++)
    {
        if (token[i] == NULL)
        {
            break;
        }

        int nodeNumber = nodeTokenToInt(token[i]);

        if (nodeNumber == -1)
        {
            continue;
        }

        lightUpNode(nodesToPixelMap[nodeNumber], color);
    }
}


int setSupplySwitch(void)
{
    
    char *token[2];

    const char *supplySwitchPositionString[] = {"3.3V", "3V3", "+3.3V", "+3V3", "5V", "+5V", "+-8V", "8V"};
    int supplySwitchPositionInt [] = {0,0,0,0,1,1,2,2};
    int supplySwitch = 1;
    token[0] = strtok(inputBuffer, ",:[] \"");

//Serial.println(token[0]);

    //int supplySwitchPosition = atoi(token[0]);
    
    for (int i = 0; i < 8; i++)
    {
        if (strcasecmp(token[0], supplySwitchPositionString[i]) == 0)
        {
            supplySwitch = supplySwitchPositionInt[i];
            break;
        }
    }




    return supplySwitch;


    


}


void lightUpNetsFromInputBuffer(void)
{
    //char *bufferPtr = inputBuffer;
    //char inputBufferCopy[INPUTBUFFERLENGTH]; 
    char *token[50];


    token[0] = strtok(inputBuffer, ",:[] ");

    int numTokens = 0;

    for (int i = 0; i < 50; i++)
    {

        if (i > 0)
        {
            token[i] = strtok(NULL, ",:[] ");
        }

        if (token[i] == nullptr)
        {
            //Serial.println("token is null");
            //Serial.println(i - 1);
            numTokens = i - 1;
            break;
        }

    }

    //     for (int i = 0; i < INPUTBUFFERLENGTH-1; i++)
    // {
    //         inputBuffer[i] = '\0';
    // }

    uint32_t color = (uint32_t)removeHexPrefix(token[numTokens]);
  
    //Serial.print("color = ");
    //Serial.println(color);

    for (int i = 0; i < numTokens; i++)
    {
        if (token[i] == NULL)
        {
            break;
        }

       int netNumber = atoi(token[i]);

        // if (netNumber == 0)
        // {
        //     break;
        // }
       // Serial.print("netNumber = ");   
       // Serial.println(netNumber);

        if (netNumber < 8)
        {
            rawSpecialNetColors[netNumber] = color;
            net[netNumber].rawColor = color;

            switch (netNumber)
            {
            case 1:
                for (int j = 0; j < 3; j++)
                {
                rawRailColors[j][1] = color;
                rawRailColors[j][3] = color;
                }
                break;
            case 2:
                rawRailColors[1][0] = color;
                rawRailColors[1][2] = color;
                break;
            case 3:
                rawRailColors[0][0] = color;
                rawRailColors[0][2] = color;
                break;
            }
        }
        else
        {
            net[netNumber].rawColor = color;
        }

        if (debugMM)
        {
        Serial.print("net[");
        Serial.print(netNumber);
        Serial.print("].rawColor = ");
        Serial.println(net[netNumber].rawColor, HEX);

        Serial.print("net[");
        Serial.print(netNumber);
        Serial.print("].name = ");
        Serial.println(net[netNumber].name);

        Serial.print("net[");
        Serial.print('1');
        Serial.print("].name = ");
        Serial.println(net[1].name);

        Serial.print("inputBuffer = "); 
        Serial.println(inputBuffer);
        }
    //     Serial.print("inputBufferCopy = ");
    //     Serial.println(inputBufferCopy);

    // for (int i = 0; i < INPUTBUFFERLENGTH-1; i++)
    //  {
    //          inputBuffer[i] = '\0';
    //  }

        // lightUpNode(nodesToPixelMap[nodeNumber], color);
    }
}
