#ifndef FILEPARSING_H
#define FILEPARSING_H


//this just opens the file, takes out all the bullshit, and then populates the newBridge array
void parseWokwiFileToNodeFile();
void changeWokwiDefinesToJumperless ();
void writeToNodeFile(void);

void openNodeFile();

void splitStringToFields();

void replaceSFNamesWithDefinedInts();

void replaceNanoNamesWithDefinedInts();

void parseStringToBridges();



#endif