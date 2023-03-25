#ifndef JMATRIXCONTROL_H  // so this doesnt get defined twice
#define JMATRIXCONTROL_H

#include <Arduino.h> //move this eventually


class jMatrixControl{

public:
jMatrixControl();

int connectDumbMode(int x,int y, int chip, int connectDisconnect = 1); //X address, Y Address, Chip (Char,Int, or Int ascii), connect (1) or disconnect (0)
int connectDumbMode(int x,int y, char chip, int connectDisconnect = 1); //X address, Y Address, Chip (Char,Int, or Int ascii), connect (1) or disconnect (0)
void clearAllConnections(void);
char chipIntToChar(int chipChar);    //returns ' ' (space) if invalid
int  chipCharToInt(char);   //returns -1 if invalid
int8_t bottomRowTranslation (int8_t rowToTranslate);//returns -1 if invalid  -  works both ways

private:

void setAddress(int, int);
void strobeItIn(int connectDisconnect);
int selectChip(int);        //returns 1 if it's a valid chip number 
void deselectChip(void);

};


#endif