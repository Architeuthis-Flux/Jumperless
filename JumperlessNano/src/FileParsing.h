// SPDX-License-Identifier: MIT
#ifndef FILEPARSING_H
#define FILEPARSING_H

extern bool debugFP;
extern bool debugFPtime;

extern bool debugNM;
extern bool debugNMtime;

extern bool debugNTCC;
extern bool debugNTCC2;

extern bool debugLEDs;
extern bool debugMM;

// #include "RotaryEncoder.h"


//extern File nodeFile;  
void createSlots(int slot = -1, int addRotaryConnections = 0);
void inputNodeFileList(int addRotaryConnections = 0);
//this just opens the file, takes out all the bullshit, and then populates the newBridge array
void parseWokwiFileToNodeFile();
void changeWokwiDefinesToJumperless ();
void writeToNodeFile(int slot = 0);
void removeBridgeFromNodeFile(int node1, int node2 = -1, int slot = 0);
void addBridgeToNodeFile(int node1, int node2, int slot = 0);
void savePreformattedNodeFile (int source = 0, int slot = 0, int keepEncoder = 1);

void openNodeFile(int slot = 0);

void splitStringToFields();

void replaceSFNamesWithDefinedInts();
void printNodeFile(int slot = 0);
void replaceNanoNamesWithDefinedInts();

void parseStringToBridges();

char lastCommandRead(void);
void lastCommandWrite(char lastCommand);

void runCommandAfterReset(char);

void debugFlagSet(int flag);
void debugFlagInit(void);
void clearNodeFile(int slot = 0);
int lenHelper(int);
int printLen(int);




#endif
