
#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "MatrixStateRP2040.h"





struct netStruct net[MAX_NETS] = { //these are the special function nets that will always be made
//netNumber,       ,netName          ,memberNodes[]         ,memberBridges[][2]     ,specialFunction        ,intsctNet[] ,doNotIntersectNodes[]                 ,priority
    {     127      ,"Empty Net"      ,{EMPTY_NET}           ,{{}}                   ,EMPTY_NET              ,{}          ,{EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET} , 0},     
    {     1        ,"GND\t"          ,{GND}                 ,{{}}                   ,GND                    ,{}          ,{SUPPLY_3V3,SUPPLY_5V,DAC0_5V,DAC1_8V}    , 1},
    {     2        ,"+5V\t"          ,{SUPPLY_5V}           ,{{}}                   ,SUPPLY_5V              ,{}          ,{GND,SUPPLY_3V3,DAC0_5V,DAC1_8V}          , 1},
    {     3        ,"+3.3V\t"        ,{SUPPLY_3V3}          ,{{}}                   ,SUPPLY_3V3             ,{}          ,{GND,SUPPLY_5V,DAC0_5V,DAC1_8V}           , 1},
    {     4        ,"DAC 0\t"        ,{DAC0_5V}             ,{{}}                   ,DAC0_5V                ,{}          ,{GND,SUPPLY_5V,SUPPLY_3V3,DAC1_8V}        , 1},
    {     5        ,"DAC 1\t"        ,{DAC1_8V}             ,{{}}                   ,DAC1_8V                ,{}          ,{GND,SUPPLY_5V,SUPPLY_3V3,DAC0_5V}        , 1},
    {     6        ,"I Sense +"      ,{CURRENT_SENSE_PLUS}  ,{{}}                   ,CURRENT_SENSE_PLUS     ,{}          ,{CURRENT_SENSE_MINUS}                     , 2},
    {     7        ,"I Sense -"      ,{CURRENT_SENSE_MINUS} ,{{}}                   ,CURRENT_SENSE_MINUS    ,{}          ,{CURRENT_SENSE_PLUS}                      , 2},
};

char *netNameConstants[MAX_NETS] = {(char*)"Net 0",(char*)"Net 1",(char*)"Net 2",(char*)"Net 3",(char*)"Net 4",(char*)"Net 5",(char*)"Net 6",(char*)"Net 7",(char*)"Net 8",(char*)"Net 9",(char*)"Net 10",(char*)"Net 11",(char*)"Net 12",(char*)"Net 13",(char*)"Net 14",(char*)"Net 15",(char*)"Net 16",(char*)"Net 17",(char*)"Net 18",(char*)"Net 19",(char*)"Net 20",(char*)"Net 21",(char*)"Net 22",(char*)"Net 23",(char*)"Net 24",(char*)"Net 25",(char*)"Net 26",(char*)"Net 27",(char*)"Net 28",(char*)"Net 29",(char*)"Net 30",(char*)"Net 31",(char*)"Net 32",(char*)"Net 33",(char*)"Net 34",(char*)"Net 35",(char*)"Net 36",(char*)"Net 37",(char*)"Net 38",(char*)"Net 39",(char*)"Net 40",(char*)"Net 41",(char*)"Net 42",(char*)"Net 43",(char*)"Net 44",(char*)"Net 45",(char*)"Net 46",(char*)"Net 47",(char*)"Net 48",(char*)"Net 49",(char*)"Net 50",(char*)"Net 51",(char*)"Net 52",(char*)"Net 53",(char*)"Net 54",(char*)"Net 55",(char*)"Net 56",(char*)"Net 57",(char*)"Net 58",(char*)"Net 59",(char*)"Net 60",(char*)"Net 61",(char*)"Net 62"};//,{"Net 63",(char*)"Net 64",(char*)"Net 65",(char*)"Net 66",(char*)"Net 67",(char*)"Net 68",(char*)"Net 69",(char*)"Net 70",(char*)"Net 71",(char*)"Net 72",(char*)"Net 73",(char*)"Net 74",(char*)"Net 75",(char*)"Net 76",(char*)"Net 77",(char*)"Net 78",(char*)"Net 79",(char*)"Net 80",(char*)"Net 81",(char*)"Net 82",(char*)"Net 83",(char*)"Net 84",(char*)"Net 85",(char*)"Net 86",(char*)"Net 87",(char*)"Net 88",(char*)"Net 89",(char*)"Net 90",(char*)"Net 91",(char*)"Net 92",(char*)"Net 93",(char*)"Net 94",(char*)"Net 95",(char*)"Net 96",(char*)"Net 97",(char*)"Net 98",(char*)"Net 99",(char*)"Net 100",(char*)"Net 101",(char*)"Net 102",(char*)"Net 103",(char*)"Net 104",(char*)"Net 105",(char*)"Net 106",(char*)"Net 107",(char*)"Net 108",(char*)"Net 109",(char*)"Net 110",(char*)"Net 111",(char*)"Net 112",(char*)"Net 113",(char*)"Net 114",(char*)"Net 115",(char*)"Net 116",(char*)"Net 117",(char*)"Net 118",(char*)"Net 119",(char*)"Net 120",(char*)"Net 121",(char*)"Net 122",(char*)"Net 123",(char*)"Net 124",(char*)"Net 125",(char*)"Net 126",(char*)"Net 127"}};
//char (*netNamesPtr) = netNameConstants[0];


//note that the bottom row is shifted from the schematic by one, so the nodes are what's written on the board
const int bbNodesToChip[62] = {-1,CHIP_L,           //0,1
CHIP_A,CHIP_A,CHIP_A,CHIP_A,CHIP_A,CHIP_A,CHIP_A,  // 2, 3, 4, 5, 6, 7, 8
CHIP_B,CHIP_B,CHIP_B,CHIP_B,CHIP_B,CHIP_B,CHIP_B,  // 9,10,11,12,13,14,15
CHIP_C,CHIP_C,CHIP_C,CHIP_C,CHIP_C,CHIP_C,CHIP_C,  //16,17,18,19,20,21,22
CHIP_D,CHIP_D,CHIP_D,CHIP_D,CHIP_D,CHIP_D,CHIP_D,  //23,24,25,26,27,28,29
CHIP_L,CHIP_L,                                     //30,31
CHIP_E,CHIP_E,CHIP_E,CHIP_E,CHIP_E,CHIP_E,CHIP_E,  //32,33,34,35,36,37,38
CHIP_F,CHIP_F,CHIP_F,CHIP_F,CHIP_F,CHIP_F,CHIP_F,  //39,40,41,42,43,44,45
CHIP_G,CHIP_G,CHIP_G,CHIP_G,CHIP_G,CHIP_G,CHIP_G,  //46,47,48,49,50,51,52
CHIP_H,CHIP_H,CHIP_H,CHIP_H,CHIP_H,CHIP_H,CHIP_H,  //53,54,55,56,57,58,59
CHIP_L};                                           //60




struct chipStatus ch[12] = {
  {0,'A',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_I, CHIP_J, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_K, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},//X MAP constant
  {CHIP_L,  t2,t3, t4, t5, t6, t7, t8}},  // Y MAP constant

  {1,'B',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_I, CHIP_J, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_K, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {CHIP_L,  t9,t10,t11,t12,t13,t14,t15}},//yMap

  {2,'C',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_I, CHIP_J, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_K, CHIP_H, CHIP_H},
  {CHIP_L, t16,t17,t18,t19,t20,t21,t22}},

  {3,'D',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_I, CHIP_J, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_K},
  {CHIP_L, t23,t24,t25,t26,t27,t28,t29}},

  {4,'E',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_K, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_I, CHIP_J, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {CHIP_L,   b2, b3, b4, b5, b6, b7, b8}},

  {5,'F',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_K, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_I, CHIP_J, CHIP_G, CHIP_G, CHIP_H, CHIP_H},
  {CHIP_L,  b9, b10,b11,b12,b13,b14,b15}},

  {6,'G',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_K, CHIP_D, CHIP_D, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_I, CHIP_J, CHIP_H, CHIP_H},
  {CHIP_L,  b16,b17,b18,b19,b20,b21,b22}},

  {7,'H',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_K, CHIP_E, CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_I, CHIP_J},
  {CHIP_L,  b23,b24,b25,b26,b27,b28,b29}},

  {8,'I',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_D1, NANO_A2, NANO_D3, NANO_A4, NANO_D5, NANO_A6, NANO_D7, NANO_D11, NANO_D9, NANO_D13, NANO_RESET, DAC0_5V, ADC0_5V, SUPPLY_3V3, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {9,'J',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_D0, NANO_A1, NANO_D2, NANO_A3, NANO_D4, NANO_A5, NANO_D6, NANO_A7, NANO_D8, NANO_D10, NANO_D12, NANO_AREF, DAC1_8V, ADC1_5V, SUPPLY_5V, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {10,'K',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, ADC2_5V},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {11,'L',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CURRENT_SENSE_MINUS, CURRENT_SENSE_PLUS, ADC0_5V, ADC1_5V, ADC2_5V, ADC3_8V, DAC1_8V, DAC0_5V, t1, t30, b1, b30, NANO_A4, NANO_A5, SUPPLY_5V, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}}
  };

enum nanoPinsToIndex       {     NANO_PIN_D0 ,     NANO_PIN_D1 ,     NANO_PIN_D2 ,     NANO_PIN_D3 ,     NANO_PIN_D4 ,     NANO_PIN_D5 ,     NANO_PIN_D6 ,     NANO_PIN_D7 ,     NANO_PIN_D8 ,     NANO_PIN_D9 ,     NANO_PIN_D10 ,     NANO_PIN_D11 ,      NANO_PIN_D12 ,     NANO_PIN_D13 ,       NANO_PIN_RST ,      NANO_PIN_REF ,     NANO_PIN_A0 ,     NANO_PIN_A1 ,     NANO_PIN_A2 ,     NANO_PIN_A3 ,     NANO_PIN_A4 ,     NANO_PIN_A5 ,     NANO_PIN_A6 ,     NANO_PIN_A7 };


struct nanoStatus nano = {  //there's only one of these so ill declare and initalize together unlike above

//all these arrays should line up (both by index and visually) so one index will give you all this data

//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        | 
                           {   " D0",   " D1",   " D2",   " D3",   " D4",   " D5",   " D6",   " D7",   " D8",   " D9",    "D10",    "D11",     "D12",    "D13",      "RST",     "REF",   " A0",   " A1",   " A2",   " A3",   " A4",   " A5",   " A6",   " A7"},// String with readable name //padded to 3 chars (space comes before chars)
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |                         
                           { NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11,  NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7},//Array index to internal arbitrary #defined number
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
                           {  1     , 1      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2       , 2       ,  2       , 1       , 1         , 1        , 2      , 2      , 2      , 2      , 2      , 2      , 1      , 1      },//Whether this pin has 1 or 2 connections to special function chips    (OR maybe have it be a map like i = 2  j = 3  k = 4  l = 5 if there's 2 it's the product of them ij = 6  ik = 8  il = 10 jk = 12 jl = 15 kl = 20 we're trading minuscule amounts of CPU for RAM)  
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
                           { CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J  , CHIP_I  ,  CHIP_J  , CHIP_I  , CHIP_I    ,  CHIP_J  , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J },//Since there's no overlapping connections between Chip I and J, this holds which of those 2 chips has a connection at that index, if numConns is 1, you only need to check this one
                           { -1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K  , CHIP_K  ,  CHIP_K  , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_L , CHIP_L , -1     , -1     },//Since there's no overlapping connections between Chip K and L, this holds which of those 2 chips has a connection at that index, -1 for no connection
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |   
                           { -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      , -1     , 9      , -1      , 8       ,  -1      , 10      , 11        , -1       , 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     },//holds which X pin is connected to the index on Chip I, -1 for none
                           { -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1      , 0       ,  -1      , 0       , 0         , -1       , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     },//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
                           { 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     , 8      , -1     , 9       , -1      ,  10      , -1      , -1        , 11       , -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      },//holds which X pin is connected to the index on Chip J, -1 for none
                           { 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0       , -1      , 0        , 0       , -1        , 0        , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      },//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
                           { -1     , -1     , 4      , 5      , 6      , 7      , 8      , 9      , 10     , 11     , 12      , 13      ,  14      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , -1     , -1     , -1     , -1     },//holds which X pin is connected to the index on Chip K, -1 for none
                           { -1     , -1     , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0       , 0       ,  0       , -1      , -1        , -1       , 0      , 0      , 0      , 0      , -1     , -1     , -1     , -1     },//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
                           { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 12     , 13     , -1     , -1     },//holds which X pin is connected to the index on Chip L, -1 for none
                           { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 0      , 0      , -1     , -1     },//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here

// mapIJKL[]     will tell you whethher there's a connection from that nano pin to the corresponding special function chip 
// xMapIJKL[]    will tell you the X pin that it's connected to on that sf chip
// xStatusIJKL[] says whether that x pin is being used (this should be the same as mt[8-10].xMap[] if theyre all stacked on top of each other)
//              I haven't decided whether to make this just a flag, or store that signal's destination
 {NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,GND,101,102,SUPPLY_3V3,104,SUPPLY_5V,DAC0_5V,DAC1_8V,CURRENT_SENSE_PLUS,CURRENT_SENSE_MINUS}

};

struct pathStruct path[MAX_BRIDGES]; // node1, node2, net, chip[3], x[3], y[3]

/*

struct nanoStatus nanoConnections = {  //there's only one of these so ill declare and initalize together unlike above

//all these arrays should line up (both by index and visually) so one index will give you all this data

//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        | 
 char *pinNames[24]=  {   " D0",   " D1",   " D2",   " D3",   " D4",   " D5",   " D6",   " D7",   " D8",   " D9",    "D10",    "D11",     "D12",    "D13",      "RST",     "REF",   " A0",   " A1",   " A2",   " A3",   " A4",   " A5",   " A6",   " A7"};// String with readable name //padded to 3 chars (space comes before chars)
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |                         
 int8_t pinMap[24] =  { NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11,  NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7};//Array index to internal arbitrary #defined number
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
 int8_t numConns[24]= {  1     , 1      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2       , 2       ,  2       , 2       , 1         , 1        , 2      , 2      , 2      , 2      , 2      , 2      , 1      , 1      };//Whether this pin has 1 or 2 connections to special function chips    (OR maybe have it be a map like i = 2  j = 3  k = 4  l = 5 if there's 2 it's the product of them ij = 6  ik = 8  il = 10 jk = 12 jl = 15 kl = 20 we're trading minuscule amounts of CPU for RAM)  
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
 int8_t  mapIJ[24] =  { CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J  , CHIP_I  ,  CHIP_J  , CHIP_I  , CHIP_I    ,  CHIP_J  , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J };//Since there's no overlapping connections between Chip I and J, this holds which of those 2 chips has a connection at that index, if numConns is 1, you only need to check this one
 int8_t  mapKL[24] =  { -1     , -1     , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K  , CHIP_K  ,  CHIP_K  , -1      , -1        , -1       , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_L , CHIP_L , -1     , -1     };//Since there's no overlapping connections between Chip K and L, this holds which of those 2 chips has a connection at that index, -1 for no connection
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |   
int8_t xMapI[24]  =  { -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      , -1     , 9      , -1      , 8       ,  -1      , 10      , 11        , -1       , 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     };//holds which X pin is connected to the index on Chip I, -1 for none
   int8_t xStatusI[24]  =  { -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1      , 0       ,  -1      , 0       , 0         , -1       , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
int8_t xMapJ[24]  =  { 0      , -1     , 2      , -1     , 4      , -1     , 6      , -1     , 8      , -1     , 9       , -1      ,  10      , -1      , -1        , 11       , -1     , 1      , -1     , 3      , -1     , 5      , -1     , 7      };//holds which X pin is connected to the index on Chip J, -1 for none
   int8_t xStatusJ[24]  =  { 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0       , -1      , 0        , 0       , -1        , 0        , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0      };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
 int8_t xMapK[24]  =  { -1     , -1     , 4      , 5      , 6      , 7      , 8      , 9      , 10     , 11     , 12      , 13      ,  14      , -1      , -1        , -1       , 0      , 1      , 2      , 3      , -1     , -1     , -1     , -1     };//holds which X pin is connected to the index on Chip K, -1 for none
   int8_t xStatusK[24]  =  { -1     , -1     , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0       , 0       ,  0       , -1      , -1        , -1       , 0      , 0      , 0      , 0      , -1     , -1     , -1     , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        |        |        |        |        |        |         |         |          |         |           |          |        |        |        |        |        |        |        |        |    
int8_t xMapL[24]  =  { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 12     , 13     , -1     , -1     };//holds which X pin is connected to the index on Chip L, -1 for none
   int8_t xStatusL[24]  =  { -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     , -1     , -1     , -1     , 0      , 0      , -1     , -1     };//-1 for not connected to that chip, 0 for available, >0 means it's connected and the netNumber is stored here

// mapIJKL[]     will tell you whethher there's a connection from that nano pin to the corresponding special function chip 
// xMapIJKL[]    will tell you the X pin that it's connected to on that sf chip
// xStatusIJKL[] says whether that x pin is being used (this should be the same as mt[8-10].xMap[] if theyre all stacked on top of each other)
//              I haven't decided whether to make this just a flag, or store that signal's destination
const int8_t reversePinMap[110] = {NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9, NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,GND,101,102,SUPPLY_3V3,104,SUPPLY_5V,DAC0_5V,DAC1_8V,CURRENT_SENSE_PLUS,CURRENT_SENSE_MINUS};

};



//see the comments at the end for a more nicely formatted version that's not in struct initalizers

 struct xy8_t {
  uint8_t x : 4;
  uint8_t y : 4;
};


struct pathStruct{

  int8_t node1 = 0; //these are the rows or nano header pins to connect
  int8_t node2 = 0;
  int pathNumber = 0; // this is just a number to refer to the path by

  uint8_t firstChip = 0xff;
  uint8_t firstXY = 0b11101111;
  //int8_t firstY = -1;

  uint8_t secondChip = 0xff;

  uint8_t secondXY = 0b00000000;
  //int8_t secondY = -1;

 
  int redundantConnection = 0; // if the paths are redundant (for lower resistance) this is the pathNumber of the other one(s)

};
*/
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




