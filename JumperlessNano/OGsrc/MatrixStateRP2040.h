// SPDX-License-Identifier: MIT
#ifndef MATRIXSTATE_H
#define MATRIXSTATE_H

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"


struct netStruct{ 

uint8_t number; //nets are uint8_t, nodes are int8_t

const char *name; // human readable "Net 3"

int8_t nodes[MAX_NODES] = {};//maybe make this smaller and allow nets to just stay connected currently 64x64 is 4 Kb

int8_t bridges[MAX_NODES][2]; //either store them here or in one long array that references the net

int8_t specialFunction = -1; // store #defined number for that special function -1 for regular net

uint8_t intersections[8]; //if this net shares a node with another net, store this here. If it's a regular net, we'll need a function to just merge them into one new net. special functions can intersect though (except Power and Ground), 0x7f is a reserved empty net that nothing and intersect

int8_t doNotIntersectNodes[8]; //if the net tries to share a node with a net that contains any #defined nodes here, it won't connect and throw an error (SUPPLY to GND)

uint8_t priority = 0; //this isn't implemented - priority = 1 means it will move connections to take the most direct path, priority = 2 means connections will be doubled up when possible, priority = 3 means both

rgbColor color; //color of the net in hex

uint32_t rawColor; //color of the net in hex (for the machine)

char *colorName; //name of the color

bool machine = false; //whether this net was created by the machine or by the user
};

extern struct netStruct net[MAX_NETS];

extern char *netNameConstants[MAX_NETS];

extern const int bbNodesToChip[62];

struct chipStatus{

int chipNumber;

char chipChar;

int8_t xStatus[16]; //store the bb row or nano conn this is eventually connected to so they can be stacked if conns are redundant

int8_t yStatus[8];  //store the row/nano it's connected to

const int8_t xMap[16];

const int8_t yMap[8];

};

extern struct chipStatus ch[12];

struct nanoStatus {  //there's only one of these so ill declare and initalize together unlike above

//all these arrays should line up (both by index and visually) so one index will give you all this data

//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        | 
const char *pinNames[24];//=  {   " D0",   " D1",   " D2",   " D3",   " D4",   " D5",   " D6",   " D7",   " D8",   " D9",    "D10",    "D11",     "D12",    "D13",      "RST",     "REF",   " A0",   " A1",   " A2",   " A3",   " A4",   " A5",   " A6",   " A7"};// String with readable name //padded to 3 chars (space comes before chars)
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |                         
const int8_t pinMap[24];// =  { NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11,  NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7};//Array index to internal arbitrary #defined number
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
const int8_t numConns[24];//= {  1     , 1      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2       , 2       ,  2       , 2       , 1         , 1        , 2      , 2      , 2      , 2      , 2      , 2      , 1      , 1      };//Whether this pin has 1 or 2 connections to special function chips    (OR maybe have it be a map like i = 2  j = 3  k = 4  l = 5 if there's 2 it's the product of them ij = 6  ik = 8  il = 10 jk = 12 jl = 15 kl = 20 we're trading minuscule amounts of CPU for RAM)  
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
const int8_t  mapIJ[24];// =  { CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J  , CHIP_I  ,  CHIP_J  , CHIP_I  , CHIP_I    ,  CHIP_J  , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J };//Since there's no overlapping connections between Chip I and J, this holds which of those 2 chips has a connection at that index, if numConns is 1, you only need to check this one
const int8_t  mapKL[24];// =  { -1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K  , CHIP_K  ,  CHIP_K  , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_L , CHIP_L , -1     , -1     };//Since there's no overlapping connections between Chip K and L, this holds which of those 2 chips has a connection at that index, -1 for no connection
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |   
const int8_t xMapI[24];//  =  { -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      , -1     , 9      , -1      , 8       ,  -1      , 10      , 11        , -1       , 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     };//holds which X pin is connected to the index on Chip I, -1 for none
   int8_t xStatusI[24];//  =  { -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1      , 0       ,  -1      , 0       , 0         , -1       , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
const int8_t xMapJ[24];//  =  { 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     , 8      , -1     , 9       , -1      ,  10      , -1      , -1        , 11       , -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      };//holds which X pin is connected to the index on Chip J, -1 for none
   int8_t xStatusJ[24];//  =  { 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0       , -1      , 0        , 0       , -1        , 0        , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
const int8_t xMapK[24];//  =  { -1     , -1     , 4      , 5      , 6      , 7      , 8      , 9      , 10     , 11     , 12      , 13      ,  14      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , -1     , -1     , -1     , -1     };//holds which X pin is connected to the index on Chip K, -1 for none
   int8_t xStatusK[24];//  =  { -1     , -1     , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0       , 0       ,  0       , -1      , -1        , -1       , 0      , 0      , 0      , 0      , -1     , -1     , -1     , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                            |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
const int8_t xMapL[24];//  =  { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 12     , 13     , -1     , -1     };//holds which X pin is connected to the index on Chip L, -1 for none
   int8_t xStatusL[24];//  =  { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 0      , 0      , -1     , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here

// mapIJKL[]     will tell you whethher there's a connection from that nano pin to the corresponding special function chip 
// xMapIJKL[]    will tell you the X pin that it's connected to on that sf chip
// xStatusIJKL[] says whether that x pin is being used (this should be the same as mt[8-10].xMap[] if theyre all stacked on top of each other)
//              I haven't decided whether to make this just a flag, or store that signal's destination
const int8_t reversePinMap[110];// = {NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,GND,101,102,SUPPLY_3V3,104,SUPPLY_5V,DAC0_5V,DAC1_8V,CURRENT_SENSE_PLUS,CURRENT_SENSE_MINUS};

};
extern struct nanoStatus nano;

const int duplucateSFnodes[26][4] = {  // [] [sf chip1,  x pin1, sf chip2, x pin2]
{CHIP_I,0,CHIP_K,0},
{CHIP_J,1,CHIP_K,1},
{CHIP_I,2,CHIP_K,2},
{CHIP_J,2,CHIP_K,4},
{CHIP_I,3,CHIP_K,5},
{CHIP_J,3,CHIP_K,3},
{CHIP_I,4,CHIP_L,12},
{CHIP_J,4,CHIP_K,6},
{CHIP_I,5,CHIP_K,7},
{CHIP_J,5,CHIP_L,13},
{CHIP_J,6,CHIP_K,8},
{CHIP_I,7,CHIP_K,9},
{CHIP_I,8,CHIP_K,13},
{CHIP_J,8,CHIP_K,10},
{CHIP_I,9,CHIP_K,11},
{CHIP_J,9,CHIP_K,12},
{CHIP_J,10,CHIP_K,14},
{CHIP_I,12,CHIP_L,7},
{CHIP_J,12,CHIP_L,6},
{CHIP_I,13,CHIP_L,2},
{CHIP_J,13,CHIP_L,3},
{CHIP_J,14,CHIP_L,14},
{CHIP_I,15,CHIP_J,15},
//{CHIP_I,15,CHIP_L,15},  //rev2
///{CHIP_J,15,CHIP_L,15},
{CHIP_K,15,CHIP_L,4},
};

//see the comments at the end for a more nicely formatted version that's not in struct initalizers
enum pathType {BBtoBB, BBtoNANO, NANOtoNANO, BBtoSF, NANOtoSF, BBtoBBL, NANOtoBBL, SFtoSF, SFtoBBL, BBLtoBBL};

enum nodeType {BB, NANO, SF, BBL};

struct pathStruct{

  int node1; //these are the rows or nano header pins to connect
  int node2;
  int net; 

  int chip[4];
  int x[6];
  int y[6];
  int candidates[3][3]; //[node][candidate]
  int altPathNeeded;
  enum pathType pathType;
  enum nodeType nodeType[3];
  bool sameChip;
  bool Lchip;
  bool skip = false;

};



extern struct pathStruct path[MAX_BRIDGES]; //this is the array of paths 

 struct SFmapPair {
        const char* name;
        int replacement;
    };

extern struct SFmapPair sfMappings[100];




/*
uint8_t connectionsMatrix[85][85];

connectionsMatrix [1-60][1-60] are breadboard connections 
connectionsMatrix [60-85][60-85] are Nano connections

value stored is row it's connected to, so we can use the value as the index for the next connection to see which nets are connected

or we can do a 64 bit number for the breadboard and just store a 1 or 0 in that bit location

uint64_t bbConns[64];

uint64_t nanoConns[25];

each set bit in the 64bit value correspond to which bb rows that nano pin is connected to

should nano conns be handled separately?

the problem with all this is that it says nothing about the path taken

*/
/*
//this is all defined in the struct initalizer now but leaving this here for reference

const int8_t sfyConnectionMap[8] = {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}; //chips


const int8_t xConnectionMapA[16] = {CHIP_I, CHIP_J, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_K, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H};
const int8_t xConnectionMapB[16] = {CHIP_A, CHIP_A, CHIP_I, CHIP_J, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_K, CHIP_G, CHIP_G, CHIP_H, CHIP_H};
const int8_t xConnectionMapC[16] = {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_I, CHIP_J, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_K, CHIP_H, CHIP_H};
const int8_t xConnectionMapD[16] = {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_I, CHIP_J, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_K};
const int8_t xConnectionMapE[16] = {CHIP_A, CHIP_K, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_I, CHIP_J, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H};
const int8_t xConnectionMapF[16] = {CHIP_A, CHIP_A, CHIP_B, CHIP_K, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_I, CHIP_J, CHIP_G, CHIP_G, CHIP_H, CHIP_H};
const int8_t xConnectionMapG[16] = {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_K, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_I, CHIP_J, CHIP_H, CHIP_H};
const int8_t xConnectionMapH[16] = {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_K, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_I, CHIP_J};

const int8_t yConnectionMapA[8] = {-1,  t2,t3, t4, t5, t6, t7, t8};
const int8_t yConnectionMapB[8] = {-1,  t9,t10,t11,t12,t13,t14,t15};
const int8_t yConnectionMapC[8] = {-1, t16,t17,t18,t19,t20,t21,t22};
const int8_t yConnectionMapD[8] = {-1, t23,t24,t25,t26,t27,t28,t29}; //make an alt_jumper version later

const int8_t yConnectionMapE[8] = {-1,   b2, b3, b4, b5, b6, b7, b8};
const int8_t yConnectionMapF[8] = {-1,  b9, b10,b11,b12,b13,b14,b15};
const int8_t yConnectionMapG[8] = {-1,  b16,b17,b18,b19,b20,b21,b22};
const int8_t yConnectionMapH[8] = {-1,  b23,b24,b25,b26,b27,b28,b29}; //make an alt_jumper version later


const int8_t yConnectionMapE[8] = {-1, 33, 34, 35, 36, 37, 38, 39}; // if you want to match them up with the schematic
const int8_t yConnectionMapF[8] = {-1, 40, 41, 42, 43, 44, 45, 46}; // use this for reference (it's the same thing as above)
const int8_t yConnectionMapG[8] = {-1, 47, 48, 49, 50, 51, 52, 53}; // 
const int8_t yConnectionMapH[8] = {-1, 54, 55, 56, 57, 58, 59, 60}; //


const int8_t xConnectionMapI[16] = {NANO_A0, NANO_D1, NANO_A2, NANO_D3, NANO_A4, NANO_D5, NANO_A6, NANO_D7, NANO_D11, NANO_D9, NANO_D13, NANO_RESET, DAC0_5V, ADC0_5V, SUPPLY_3V3, GND};
const int8_t xConnectionMapJ[16] = {NANO_D0, NANO_A1, NANO_D2, NANO_A3, NANO_D4, NANO_A5, NANO_D6, NANO_A7, NANO_D8, NANO_D10, NANO_D12, NANO_AREF, DAC1_8V, ADC1_5V, SUPPLY_5V, GND};
const int8_t xConnectionMapK[16] = {NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9};
const int8_t xConnectionMapK[16] = {CURRENT_SENSE_MINUS, CURRENT_SENSE_PLUS, ADC0_5V, ADC1_5V, ADC2_5V, ADC3_8V, DAC1_8V, DAC0_5V, t1, t30, b1, b30, NANO_A4, NANO_A5, SUPPLY_5V, GND};

//nanoConnectionMap[0] is the list of pin numbers, [1] and [3] are which special function chips they're connected to,  [2] and [4] is the X pin on that chip,   -1 for none

const char *pinNames[] ={     "D0",   "D1",     "D2",    "D3",    "D4",    "D5",    "D6",    "D7",    "D8",    "D9",    "D10",    "D11",    "D12",    "D13",    "RESET",    "AREF",    "A0",    "A1",    "A2",    "A3",    "A4",    "A5",    "A6",    "A7"};
const char nanoPinMap[] ={NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7};

const int8_t nanoConnectionMap[5][24] = {{NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7},

                                         {CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J  , CHIP_I ,  CHIP_J  , CHIP_I  , CHIP_I    , CHIP_J   , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J },
                                         {0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      , 8      , 9      , 9       , 8      ,  10      , 10      , 11        , 11       , 0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      },

                                         {-1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , -1      , -1     ,  -1      , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K },
                                         {-1     , -1     , 8      , 9      , 10     , 11     , 12     , 13     , 14     , 15     , -1      , -1     ,  -1      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      }};

// so for example nanoConnectionMap 





      _________________________________________________________________
     /                                                                 \
    |                  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0                    |
    |                                                                   |
    |                  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0                    |
    |                                                                   |
    |   ------------------------------0--31--------------------------   |
    |   ------------------------------0---0--------------------------   |
    |                                                                   |
    |                                                                   | 
    |   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0     |
    |                                                                   |
    |   1 2 3 4 5 6 7 8 9 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3     |
    |                     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0     |
    |                                                                   |
    |                                                                   |
    |                                                                   |
    |   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0     |
    |                                                                   |
    |   3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6     |
    |   2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1     |
    |                                                                   |
    |   ------------------------------0--63--------------------------   |
    |   ------------------------------0--32--------------------------   |
    |                                                                   |
    |___________________________________________________________________|


    we have ~2MB on the pico se we don't need as much fuckery to store a map like we did with 32Kb

connections[90][90]?     8Kb

chip[A-L][8][16]?        1.5Kb
+ const chip[A-L][8][16] 1.5Kb for the map



use the littleFS to make a text file with connections and just update it

maybe copy kicad's schematic format 
*/



#endif
