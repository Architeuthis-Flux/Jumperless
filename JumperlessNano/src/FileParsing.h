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


//extern File nodeFile;  


//this just opens the file, takes out all the bullshit, and then populates the newBridge array
void parseWokwiFileToNodeFile();
void changeWokwiDefinesToJumperless ();
void writeToNodeFile(void);
void removeBridgeFromNodeFile(int node1, int node2 = -1);
void addBridgeToNodeFile(int node1, int node2);
void savePreformattedNodeFile (int source = 0);

void openNodeFile();

void splitStringToFields();

void replaceSFNamesWithDefinedInts();
void printNodeFile(void);
void replaceNanoNamesWithDefinedInts();

void parseStringToBridges();

char lastCommandRead(void);
void lastCommandWrite(char lastCommand);

void runCommandAfterReset(char);

void debugFlagSet(int flag);
void debugFlagInit(void);
void clearNodeFile(void);
int lenHelper(int);
int printLen(int);




#endif
