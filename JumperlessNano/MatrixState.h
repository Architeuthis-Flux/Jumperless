#ifndef MATRIXSTATE_H
#define MATRIXSTATE_H

#include <Arduino.h>
#include "JumperlessDefines.h"


//see the comments at the end for a more nicely formatted version that's not in struct initalizers


struct chipStatus{

int chipNumber;
char chipChar;
int8_t xStatus[16]; //store the bb row or nano conn this is eventually connected to so they can be stacked if conns are redundant
int8_t yStatus[8];  //store the row/nano it's connected to
const int8_t xMap[16];
const int8_t yMap[8];

};


struct chipStatus mt[11] = {
  {0,'A',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_I, CHIP_J, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_K, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},//X MAP constant
  {-1,  t2,t3, t4, t5, t6, t7, t8}},  // Y MAP constant

  {1,'B',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_I, CHIP_J, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_K, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {-1,  t9,t10,t11,t12,t13,t14,t15}},

  {2,'C',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_I, CHIP_J, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_K, CHIP_H, CHIP_H},
  {-1, t16,t17,t18,t19,t20,t21,t22}},

  {3,'D',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_I, CHIP_J, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_K},
  {-1, t23,t24,t25,t26,t27,t28,t29}},

  {4,'E',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_K, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_I, CHIP_J, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {-1,   b2, b3, b4, b5, b6, b7, b8}},

  {5,'F',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_K, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_I, CHIP_J, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {-1,  b9, b10,b11,b12,b13,b14,b15}},

  {6,'G',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_K, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_I, CHIP_J, CHIP_H, CHIP_H},
  {-1,  b16,b17,b18,b19,b20,b21,b22}},

  {7,'H',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_K, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_I, CHIP_J},
  {-1,  b23,b24,b25,b26,b27,b28,b29}},

  {8,'I',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_D1, NANO_A2, NANO_D3, NANO_A4, NANO_D5, NANO_A6, NANO_D7, NANO_D11, NANO_D9, NANO_D13, NANO_RESET, TOP_RAIL, BOTTOM_RAIL, GND, DAC0TO5V},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {9,'J',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_D0, NANO_A1, NANO_D2, NANO_A3, NANO_D4, NANO_A5, NANO_D6, NANO_A7, NANO_D8, NANO_D10, NANO_D12, NANO_AREF, TOP_RAIL, BOTTOM_RAIL, GND, DACPLUSMINUS9V},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {10,'K',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}}
  
  };


struct nanoStatus{  //there's only one of these so ill declare and initalize together unlike above

//all these arrays should line up so one index will give you all this data

const char *pinNames[24]={  "D0 ",   "D1 ",   "D2 ",   "D3 ",   "D4 ",   "D5 ",   "D6 ",   "D7 ",   "D8 ",   "D9 ",    "D10",    "D11",    "D12",    "D13",      "RST",     "REF",   "A0 ",   "A1 ",   "A2 ",   "A3 ",   "A4 ",   "A5 ",   "A6 ",   "A7 "};
                         
const int8_t pinMap[24] ={NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7};

//this shows what sf chip each nano pin connects to
const int8_t  mapI[24] = {-1     , CHIP_I , -1     , CHIP_I , -1     , CHIP_I , -1     , CHIP_I , -1     , CHIP_I , -1      , CHIP_I ,  -1      , CHIP_I  , CHIP_I    , -1       , CHIP_I , -1     , CHIP_I , -1     , CHIP_I , -1     , CHIP_I , -1     };
const int8_t xMapI[24] = {-1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      , -1     , 9      , -1      , 8      ,  -1      , 10      , 11        , -1       , 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     };
   int8_t xStatusI[24] = {-1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1      , 0      ,  -1      , 0       , 0         , -1       , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     }; //-1 for not connected to that chip, 0 for available


const int8_t  mapJ[24] = {CHIP_J , -1     , CHIP_J , -1     , CHIP_J , -1     , CHIP_J , -1     , CHIP_J , -1     , CHIP_J  , -1     ,  CHIP_J  , -1      , -1        , CHIP_J   , -1     , CHIP_J , -1     , CHIP_J , -1     , CHIP_J , -1     , CHIP_J };
const int8_t xMapJ[24] = {0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     , 8      , -1     , 9       , -1     ,  10      , -1      , -1        , 11       , -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      };
   int8_t xStatusJ[24] = {0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0       , -1     , 0        , 0       , -1        , 0        , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      }; //-1 for not connected to that chip, 0 for available


const int8_t  mapK[24] = {-1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , -1      , -1     ,  -1      , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K };
const int8_t xMapK[24] = {-1     , -1     , 8      , 9      , 10     , 11     , 12     , 13     , 14     , 15     , -1      , -1     ,  -1      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      };
   int8_t xStatusK[24] = {-1     , -1     , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , -1      , -1     ,  -1      , -1      , -1        , -1       , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      }; //-1 for not connected to that chip, 0 for available

// mapIJK[]     will tell you whethher there's a connection from that nano pin to the corresponding special function chip
// xMapIJK[]    will tell you the X pin that it's connected to on that sf chip
// xStatusIJK[] says whether that x pin is being used (this should be the same as mt[8-10].xMap[] if theyre all stacked on top of each other)
//              I haven't decided whether to make this just a flag, or store that signal's destination
const int8_t reversePinMap[108] = {NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,GND,TOP_RAIL,BOTTOM_RAIL,DAC0TO5V,DACPLUSMINUS9V,105};

};

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


const int8_t xConnectionMapI[16] = {NANO_A0, NANO_D1, NANO_A2, NANO_D3, NANO_A4, NANO_D5, NANO_A6, NANO_D7, NANO_D11, NANO_D9, NANO_D13, NANO_RESET, TOP_RAIL, BOTTOM_RAIL, GND, DAC0TO5V};
const int8_t xConnectionMapJ[16] = {NANO_D0, NANO_A1, NANO_D2, NANO_A3, NANO_D4, NANO_A5, NANO_D6, NANO_A7, NANO_D8, NANO_D10, NANO_D12, NANO_AREF, TOP_RAIL, BOTTOM_RAIL, GND, DACPLUSMINUS9V};
const int8_t xConnectionMapK[16] = {NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9};

//nanoConnectionMap[0] is the list of pin numbers, [1] and [3] are which special function chips they're connected to,  [2] and [4] is the X pin on that chip,   -1 for none

const char *pinNames[] ={     "D0",   "D1",     "D2",    "D3",    "D4",    "D5",    "D6",    "D7",    "D8",    "D9",    "D10",    "D11",    "D12",    "D13",    "RESET",    "AREF",    "A0",    "A1",    "A2",    "A3",    "A4",    "A5",    "A6",    "A7"};
const char nanoPinMap[] ={NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7};

const int8_t nanoConnectionMap[5][24] = {{NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7},

                                         {CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J  , CHIP_I ,  CHIP_J  , CHIP_I  , CHIP_I    , CHIP_J   , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J },
                                         {0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      , 8      , 9      , 9       , 8      ,  10      , 10      , 11        , 11       , 0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      },

                                         {-1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , -1      , -1     ,  -1      , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K },
                                         {-1     , -1     , 8      , 9      , 10     , 11     , 12     , 13     , 14     , 15     , -1      , -1     ,  -1      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , 4      , 5      , 6      , 7      }};

// so for example nanoConnectionMap 

*/




#endif