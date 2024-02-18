// SPDX-License-Identifier: MIT
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

char machineModeInstructionString[NUMBEROFINSTRUCTIONS][20] = {"unknown", "netlist", "getnetlist", "bridgelist", "getbridgelist", "lightnode", "lightnet", "getmeasurement", "gpio", "uart", "arduinoflash", "setnetcolor", "setnodecolor", "setsupplyswitch", "getsupplyswitch", "getchipstatus", "getunconnectedpaths"};

enum machineModeInstruction parseMachineInstructions(int *sequenceNumber)
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

        if (inputBuffer[i] == ']')
        {
            doneReading = 1;
            // inputBuffer[i] = ' '; //eof
            // break;
        }

        delayMicroseconds(300);
    }

    char instructionBuffer[20] = {0};

    int seqNumberSeparatorPos = -1;

    for (int i = 0; i < 20; i++)
    {
        if (inputBuffer[i] == '[')
        {
            // inputBuffer[i] = ' ';
            break;
        }
        if (inputBuffer[i] == ':')
        {
            seqNumberSeparatorPos = i;
        }
        instructionBuffer[i] = inputBuffer[i];
        inputBuffer[i] = ' ';
    }

    if (seqNumberSeparatorPos > 0)
    {
        instructionBuffer[seqNumberSeparatorPos] = 0;
        *sequenceNumber = atoi(instructionBuffer + seqNumberSeparatorPos + 1);
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

void machineModeRespond(int sequenceNumber, bool ok)
{
    Serial.print(ok ? "::ok" : "::error");
    if (sequenceNumber >= 0)
    {
        Serial.print(":");
        Serial.print(sequenceNumber);
    }
    Serial.println("");
}

void getUnconnectedPaths(void)
{
    if (numberOfUnconnectablePaths == 0)
    {

        return;
    }
    
    Serial.print("::unconnectedpaths[");
    for (int i = 0; i < numberOfUnconnectablePaths; i++)
    {
        if (i > 0)
        {
            Serial.print(",");
        }

        printNodeOrName(unconnectablePaths[i][0]);
        Serial.print("-");
        printNodeOrName(unconnectablePaths[i][1]);
    }
    Serial.println("]");
    
}

void machineNetlistToNetstruct(void)
{
    char names[MAX_NETS][32] = {0};
    // char *numbers[MAX_NETS] = {0};
    // char *colors[MAX_NETS] = {0};
    // char *nodes[MAX_NETS] = {0};
    int netIndex = 0;

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

        int nodesIndex = 0;

        if (machineModeJson[i].isNull() == true)
        {
            continue;
            // break;
        }
        //netIndex++;
         netIndex = machineModeJson[i]["index"];

        strcpy(names[i], machineModeJson[i]["name"]);

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

         //Serial.println(nodesChar);

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
    //clearNodeFile();

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
                continue;
                // break;
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

            //addBridgeToNodeFile(net[i].bridges[addBridgeIndex][0], net[i].bridges[addBridgeIndex][1]);

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
    if (str[0] == '#')
    {
        const char *hexValue = str + 1;
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
    delayMicroseconds(60);

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

void printSupplySwitch(int supplySwitchPos)
{
    const char *positionString[] = {"3.3V", "5V", "8V"};
    Serial.print("::supplyswitch[");
    Serial.print(positionString[supplySwitchPos]);
    Serial.println("]");
}

int setSupplySwitch(void)
{

    char *token[2];

    const char *supplySwitchPositionString[] = {"3.3V", "3V3", "+3.3V", "+3V3", "5V", "+5V", "+-8V", "8V"};
    int supplySwitchPositionInt[] = {0, 0, 0, 0, 1, 1, 2, 2};
    int supplySwitch = 1;
    token[0] = strtok(inputBuffer, ",:[] \"");

    // Serial.println(token[0]);

    // int supplySwitchPosition = atoi(token[0]);

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
    const int numberOfNRNs = 10; // how many commands there are below
    const char *notReallyNets[] = {"headerglow", "glow", "hg", "+8v", "8v", "-8v", "logo", "status", "logoflash", "statusflash"};
    int notReallyNetsInt[] = {0, 0, 0, 3, 3, 4, 1, 1, 2, 2}; // these correspond to an index in rawOtherColors[]
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
            // Serial.println("token is null");
            // Serial.println(i - 1);
            numTokens = i - 1;
            break;
        }
    }

    //     for (int i = 0; i < INPUTBUFFERLENGTH-1; i++)
    // {
    //         inputBuffer[i] = '\0';
    // }

    uint32_t color = (uint32_t)removeHexPrefix(token[numTokens]);

    // Serial.print("color = ");
    // Serial.println(color);

    for (int i = 0; i < numTokens; i++)
    {
        if (token[i] == NULL)
        {
            break;
        }

        int wasNRN = 0;

        for (int j = 0; j < numberOfNRNs; j++)
        {
            // Serial.print("token[i] = ");
            // Serial.println(token[i]);
            // Serial.print("notReallyNets[j] = ");
            // Serial.println(notReallyNets[j]);
            if (strcasecmp(token[i], notReallyNets[j]) == 0)
            {
                rawOtherColors[notReallyNetsInt[j]] = color;

                switch (notReallyNetsInt[j])
                {
                case 3:
                    rawRailColors[2][0] = color;
                    break;

                case 4:
                    rawRailColors[2][2] = color;
                    break;
                }

                wasNRN = 1;
                break;
            }
        }

        if (wasNRN == 1)
        {
            continue;
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

/**
 * Lists the nets, in a machine-readable, line-based format.
 *
 * Example:
 *   ::netlist-begin
 *   ::net[1,1,GND,true,001c04,false,GND]
 *   ::net[2,2,5V,true,1c0702,false,+5V]
 *   ::net[3,3,3V3,true,1c0107,false,+3.3V]
 *   ::net[4,4,DAC_0,true,231111,false,DAC 0]
 *   ::net[5,5,DAC_1,true,230913,false,DAC 1]
 *   ::net[6,6,I_POS,true,232323,false,I Sense +]
 *   ::net[7,7,I_NEG,true,232323,false,I Sense -]
 *   ::netlist-end
 *
 * Grammar:
 *   netlist      = "::netlist-begin" CRLF net* "::netlist-end" CRLF
 *   net          = "::net[" index "," number "," nodes "," special "," color
 *                  "," machine "," name "]" CRLF
 *   index        = [0-9]+
 *   number       = [0-9]+
 *   nodes        = node
 *                | node ";" nodes
 *   node         = special-node
 *                | numeric-node
 *   special-node = "GND"
 *                | ... # (imagine a complete list of special nodes here)
 *   numeric-node = [1-9][0-9]*
 *   special      = "true"
 *                | "false"
 *   color        = [0-9a-f]{6}
 *   machine      = "true"
 *                | "false"
 *   name         = [^\]\r\n]+
 */
void listNetsMachine(void)
{
    Serial.println("\n\r::netlist-begin");

    // start with 1, to ignore empty net.
    for (int i = 1; i < MAX_NETS; i++)
    {
        struct netStruct *n = &net[i];

        if (n->number == 0 || n->nodes[0] == -1)
        {
            // net not allocated yet
            continue; // this allows us to delete nets and have jumperlab work
            // break;
        }

        Serial.print("::net[");
        // INDEX
        Serial.print(i);
        Serial.print(',');

        // NUMBER
        Serial.print(n->number);
        Serial.print(',');

        // NODES
        for (int j = 0; j < MAX_NODES; j++)
        {
            if (n->nodes[j] == 0)
            {
                break;
            }
            if (j > 0)
            {
                Serial.print(';');
            }
            printNodeOrName(n->nodes[j], 1);
        }
        Serial.print(',');

        // SPECIAL
        Serial.print(n->specialFunction ? "true," : "false,");

        // COLOR

        rgbColor color = unpackRgb(scaleUpBrightness(n->rawColor));
        char buf[8];
        snprintf(buf, 8, "%.2x%.2x%.2x,", color.r, color.g, color.b);
        Serial.print(buf);

        // MACHINE
        Serial.print(n->machine ? "true," : "false,");

        // NAME
        Serial.print(n->name);

        Serial.println("]");
    }

    Serial.println("::netlist-end");
}

void listBridgesMachine(void)
{
    Serial.print("::bridgelist[");
    bool started = false;
    for (int i = 1; i < MAX_NETS; i++)
    {
        struct netStruct *n = &net[i];

        if (n->number == 0 || n->nodes[0] == -1)
        {
            // net not allocated yet
            break;
        }

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (n->bridges[j][0] <= 0)
            {
                continue;
            }
            if (started)
            {
                Serial.print(",");
            }
            else
            {
                started = true;
            }
            printNodeOrName(n->bridges[j][0], 1);
            Serial.print("-");
            printNodeOrName(n->bridges[j][1], 1);
        }
    }
    Serial.println("]");
}

void printChipStatusMachine()
{
    Serial.println("::chipstatus-begin");
    for (int i = 0; i < 12; i++)
    {
        Serial.print("::chipstatus[");
        Serial.print(chipNumToChar(i));
        Serial.print(",");
        for (int j = 0; j < 16; j++)
        {
            Serial.print(ch[i].xStatus[j]);
            Serial.print(",");
        }
        for (int j = 0; j < 8; j++)
        {
            Serial.print(ch[i].yStatus[j]);
            if (j != 7)
            {
                Serial.print(",");
            }
        }
        Serial.println("]");
    }
    Serial.println("::chipstatus-end");
}
