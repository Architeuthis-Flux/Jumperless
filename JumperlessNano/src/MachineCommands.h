// SPDX-License-Identifier: MIT
#ifndef MACHINECOMMANDS_H
#define MACHINECOMMANDS_H

#define NUMBEROFINSTRUCTIONS 18

enum machineModeInstruction
{
    unknown = 0,
    netlist,
    getnetlist,
    bridgelist,
    getbridgelist,
    lightnode,
    lightnet,
    getmeasurement,
    gpio,
    uart,
    arduinoflash,
    setnetcolor,
    setnodecolor,
    setsupplyswitch,
    getsupplyswitch,
    getchipstatus,
    getunconnectedpaths
};

extern char inputBuffer[INPUTBUFFERLENGTH];
extern char machineModeInstructionString[NUMBEROFINSTRUCTIONS][20];

enum machineModeInstruction parseMachineInstructions(int *sequenceNumber);
void machineModeRespond(int sequenceNumber, bool ok);
void getUnconnectedPaths(void);
void machineNetlistToNetstruct(void);
void populateBridgesFromNodes(void);
int nodeTokenToInt(char *);
int findReplacement(char *name);
int removeHexPrefix(const char *);

void populateBridgesFromNodes(void);

void writeNodeFileFromInputBuffer(void);

void lightUpNodesFromInputBuffer(void);

void lightUpNetsFromInputBuffer(void);

void printSupplySwitch(int supplySwitchPos);
int setSupplySwitch(void);

void listNetsMachine(void);
void listBridgesMachine(void);

void printChipStatusMachine();

#endif
