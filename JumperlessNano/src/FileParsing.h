#ifndef FILEPARSING_H
#define FILEPARSING_H

extern bool debugFP;
extern bool debugFPtime;

extern bool debugNM;
extern bool debugNMtime;

extern bool debugNTCC;
extern bool debugNTCC2;

extern bool debugLEDs;



//this just opens the file, takes out all the bullshit, and then populates the newBridge array
void parseWokwiFileToNodeFile();
void changeWokwiDefinesToJumperless ();
void writeToNodeFile(void);

void savePreformattedNodeFile (int source = 0);

void openNodeFile();

void splitStringToFields();

void replaceSFNamesWithDefinedInts();

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