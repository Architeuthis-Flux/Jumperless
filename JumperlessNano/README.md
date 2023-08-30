I'm going to do a high-level explanation of what the code in the Jumperless is actually doing. There's a lot going on and it's in a bunch of separate files so I think it will be helpful for people who what to understand or improve upon it.

Table of Contents

- General terms - the names I've decided to call things
- What's being stored - how the overall state is stored
- File Parsing - how we fill in those arrays
- Routing - how we find valid paths for each connection
- Controlling the crosspoint switches - how we send that data to the CH446Qs
- LEDs - how we choose unique colors for each net
- The Wokwi bridge app - how we scrape the Wokwi page for updates
  
This is going to be really long and so I'll be filling out parts as I finish them in no particular order.

## General terms
I've made up terms for things here that may or may not be the formal definition, so I should probably let you know what I chose.


**Breadboard Chips** - This refers to the 8 CH446Q crosspoint switches (labeled A-H) that have their Y pins connected to the breadboard. This excludes Chip L which actually has it's X pins connected to the 4 corners of the board (rows 1, 30, 31 (b1), 60 (b30))

**Special Function Chips** - This refers to the 4 crosspoints (labeled I-L) that connect to everything else; the Nano header, power supplies, DACs, GPIO from the RP2040, etc... 

![ChipNames](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/34ed6646-fa15-432e-bc27-61237f822a87)

**Nodes** - Also used synonymously with **Row** especially when it's on the breadboard or Nano Header. And end point to a bridge

**Bridges** - Just 2 nodes that should be connected

**Nets** - Groups of nodes and bridges that should all be electrically connected to each other

**Bounce** - Sometimes there won't be an available path directly from one node to another, so in this case it will pick another chip with a free path to the start and end chips and "bounce" through it.

**Paths** - Similar to a **bridge**, except that it contains data for *how* that bridge is connected. So it will have which chips and which X and X pins are needed to make the connection

### Defines
You'll see a lot of these, they're all in JumperlessDefinesRP2040.h. But the point of this whole this is so you don't have to dig through the code, so I'll put the abridged version here:
```
#define CHIP_A 0
...
#define CHIP_L 11


#define t1  1
...
#define t30 30

#define b1  31
...
#define b30 60


#define NANO_D0  70 //these are completely arbitrary
...
#define NANO_A7  93


#define GND  100 
#define SUPPLY_3V3  103
#define SUPPLY_5V  105

#define DAC0_5V  106 
#define DAC1_8V  107

#define CURRENT_SENSE_PLUS  108
#define CURRENT_SENSE_MINUS  109

#define ADC0_5V 110
#define ADC1_5V 111
#define ADC2_5V 112
#define ADC3_8V 113

#define EMPTY_NET 127

```


## What's being stored

There are a few big arrays of structs that store the overall state of everything. Here are the main ones:

### chipStatus
This stores the actual hardware layout of each of the 12 crosspoint switches, like what is physically connected where and whether that path is being used. In the code it's and array called ch[12] and it's in MatrixStateRP2040.h

```
//the struct
struct chipStatus{
int chipNumber;
char chipChar;
int8_t xStatus[16]; //store the bb row or nano conn this is eventually connected to so they can be stacked if conns are redundant
int8_t yStatus[8];  //store the row/nano it's connected to
const int8_t xMap[16];
const int8_t yMap[8];
};

//the array of those structs
struct chipStatus ch[12] = {
  {0,'A',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_I, CHIP_J, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E, CHIP_K, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},//X MAP constant
  {CHIP_L,  t2,t3, t4, t5, t6, t7, t8}},  // Y MAP constant
...
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
```
In general, I use -1 to mean the path is availale to be connected to something. As the pathfinding algorithm runs, it will fill up xStatus and yStatus with the net they're connected to.

### Paths
Each path is also stored as an array of structs, this also gets filled out as the pathfinding stuff runs. There are also a couple enums to store the type of path it is, which becomes important for pathfinding because they all are sort of dealt with differently. Note that chip L is kind of a special case because it's kind of the special function chip among special function chips. Most notably, it's Y pins are actually connected to the Y pins on the breadboard chips instead of the X pins like the rest of the special function chips.

```
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

};

extern struct pathStruct path[MAX_BRIDGES]; //this is the array of paths 
```

### Nets
This is where it stores all the info about the nets, this is filled in early on in this whole process during input parsing.
```
struct netStruct{ 
uint8_t number;                  //nets are uint8_t, nodes are int8_t
const char *name;                // human readable "Net 3"
int8_t nodes[MAX_NODES] = {};    //maybe make this smaller and allow nets to just stay connected currently 64x64 is 4 Kb
int8_t bridges[MAX_NODES][2];    //either store them here or in one long array that references the net
int8_t specialFunction = -1;     // store #defined number for that special function -1 for regular net
uint8_t intersections[8];        //if this net shares a node with another net, store this here. If it's a regular net, we'll need a function to just merge them into one new net. special functions can intersect though (except Power and Ground), 0x7f is a reserved empty net that nothing and intersect
int8_t doNotIntersectNodes[8];   //if the net tries to share a node with a net that contains any #defined nodes here, it won't connect and throw an error (SUPPLY to GND)
rgbColor color;                  //color of the net in hex
};

extern struct netStruct net[MAX_NETS];

//The first 8 nets are the Special Function Nets so they're always filled

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

char *netNameConstants[MAX_NETS] = {(char*)"Net 0",(char*)"Net 1",(char*)"Net 2" ... (char*)"Net 62"}; //Thanks Copilot

```

Cool, so those 3 arrays are basically the important ones you'll need to be aware of when I go through the rest of how this all works

## File Parsing
I guess "file" is a bit of a misnomer here, the main way of using this thing right now is via the JumperlessWokwiBridge app and that just sends data over serial where it's directly stored into the **Nets** array. There is support for loading a file using LittleFS but that's not the main way I use this thing anymore because it's slower. But the format is exactly the same.

There are actually 2 indentical file parsers, one in the Wokwi Bridge app and another on the Jumperless, they do the same thing. Really all they do is go through the list formatted with human readable names and replace them with the #defined numbers above. So D0 is replaced with 70 and DAC1_8V is replaced with 107.

Extra formatting is an "opening curly brace" "newline", then "dashes" between nodes and "comma newline" between bridges. 
```
{
45-GND,
15-SUPPLY_5V,
23-16,
17-46,
42-47,
51-23,
53-52,
48-SUPPLY_3V,
59-46,
DAC1_8V-57,
17-2,
DAC0_5V-26,
ADC3_8V-57,
29-SUPPLY_5V,
GND-34,
A3-D10,
A2-12,
A0-10,
35-SUPPLY_3V,
11-D8,
A5-20,
9-8,
}
```
Gets parsed into
```
{
45-100,
15-105,
23-16,
17-46,
42-47,
51-23,
53-52,
48-103,
59-46,
107-57,
17-2,
106-26,
113-57,
29-105,
100-34,
79-80,
88-12,
86-10,
35-103,
11-78,
91-20,
9-8,
}
```
The Wokwi bridge app does that conversion just to send less data over serial, but even if it didn't, it would just parse it on the Jumperless.

### Actually sorting it into nets (real parsing)
Now the real fun begins in NetManager.cpp. At a high level, this is what it does:


- Take in a pair of nodes
- Search the existing nets for either of those nodes
  - If it finds *one* of those nodes in a net, add both nodes to that net
  - If it each of those nodes in 2 different nets, check the doNotIntersects, if that's okay, then combine those 2 nets into one net
  - Else create a new net with both of those nodes at the first unused net index
  
After all that runs, you'll end up with a netlist that look like this
```
Index	Name		Number		Nodes			Bridges
0	Empty Net	127		EMPTY_NET		{0-0}				
1	GND		1		GND,45,34		{45-GND,GND-34}			
2	+5V		2		5V,15,29		{15-5V,29-5V}			
3	+3.3V		3		3V3,48,35		{48-3V3,35-3V3}			
4	DAC 0		4		DAC_0,26		{DAC_0-26}			
5	DAC 1		5		DAC_1,57,ADC_3		{DAC_1-57,ADC_3-57}		
6	I Sense +	6		I_POS			{0-0}				
7	I Sense -	7		I_NEG			{0-0}				

Index	Name		Number		Nodes			Bridges
8	Net 8		8		23,16,51		{23-16,51-23}			
9	Net 9		9		17,46,59,2		{17-46,59-46,17-2}		
10	Net 10		10		42,47			{42-47}				
11	Net 11		11		53,52			{53-52}				
12	Net 12		12		D9,D10			{D9-D10}			
13	Net 13		13		A2,12			{A2-12}				
14	Net 14		14		A0,10			{A0-10}				
15	Net 15		15		11,D8			{11-D8}				
16	Net 16		16		A5,20			{A5-20}				
17	Net 17		17		9,8			{9-8}	
```

# Pathfinding
This is the *really* tricky part. I probably wrote all this code about 4 times, trashed it and started over from scratch with only the lessons learned from the last attempt.
Earlier versions would add connections one at a time, but you'd end up in weird states because it has no knowledge of what other paths it needs to make room for. So the final version here clears the connections from the last update, takes in all the connections to be made, and finds paths for the whole board every time you add a new wire. All the old connections usually follow the same paths as last time unless they need to be nudged over to make room for some other path, and the actual connection won't be interrupted at all.


![Untitled](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/0dbe9d22-85e1-45a0-96ac-68855ffd41c4)
Here's the schematic of just the crosspoints, the Nano header, and the breadboard. 

If you look closely, you'll see that there generally 2 connections (I'm calling these 2 connections **Lanes** between each breadboard chip, except for the chip that is across from it on the breadboard. And every breadboard chip has one connection to each of the special function chips. The pins on the Nano header has a connection to 2 special function chips (interleaved to make it easier to connect continuous pairs of pins to the breadboard).


Here's the high level outline of what [NetsToChipConnections.cpp](src/NetsToChipConnections.cpp) is doing

- Sorts all the paths by Net
This sort of sets priority, lower net numbers (so all the special function nets) will be picked first and are more likely to have direct connections chosen

- For each path, find the start and end chips
  - If there are multiple candidates (Nano header pins will have 2) store both
  - If both start and end chips have candidates in common, choose that chip (this would make it a bounce)
  - Assign node and path types (BB to BB, NANO to BB, BB to SF, etc...)
  - Sort a list of chips from least to most crowded (by how many connections that chip has used)
  - Resolve the candidate chips by going down the sorted list of chips and picking the less crowded chip
  - Search for a direct path between those 2 chips
    - If there isn't one, swap to the other candidate chips and search again
    - If one of the nodes is a special function with multiple options swap the nodes with their equivalents and search again
    - If there still isn't a direct path, set the altPathNeeded flag and move on
At this point, any paths that have a simple direct connection should be done, now we need to deal with the ones that don't

- Resolve alt paths, if the altPathNeeded flag is set
  - Search through all the other chips until you find one that has a direct connection to both the start and end chips
    - If one chip is connected to the X inputs and the other the Y inputs, set that connection on chip[2] and x[2] y[2]
    - If they're both on X or both on Y, set the sameChip flag and the x[3] or y[3] as -2 to denote that that connection is a bounce and it doesn't matter which pin is chosen, as long as it's available
      
- Resolve uncommitted hops, anything set as -2 gets a random unconnected pin assigned to it at the very end so it doesn't take up connection space


There's a lot more subtlety to this but if I go into any more detail you might as well just read the code itself. It will all be in the file [NetsToChipConnections.cpp](src/NetsToChipConnections.cpp) , and if you're running it on a Jumperless or just an RP2040, you can set the Chip Connections and Chip Connections Alt debug flags and it will show you everything it's doing in a somewhat nicely formatted way. There are comments in the code but there are a lot of nested array things that can get pretty confusing, if you need help understanding what's going on in a particular function, let me know and I'd be happy to walk you through it.


Here's the output with the debug flags on (with some redundant bits removed because it's really long)

```
sorting paths by net

number of nets: 20
path[0] net: 1
path[1] net: 8
path[2] net: 9
path[3] net: 10
path[4] net: 11
path[5] net: 12
path[6] net: 13
path[7] net: 14
path[8] net: 15
path[9] net: 16
path[10] net: 17
path[11] net: 18
path[12] net: 19
number unique of nets: 26
pathIndex: 13
numberOfPaths: 13

0  [24,GND,Net 1],      1  [D4,14,Net 8],       2  [10,36,Net 9],       3  [D6,40,Net 10],      
4  [D7,39,Net 11],      5  [D8,38,Net 12],      6  [D9,6,Net 13],       7  [D10,7,Net 14],      
8  [D11,9,Net 15],      9  [D12,8,Net 16],      10 [D3,25,Net 17],      11 [D2,23,Net 18],      
12 [D1,45,Net 19],      


finding start and end chips 

path[0]
nodes [24-100]

finding chips for nodes: 24-GND

node: 1
 
chip: D

node: 2
 
special function candidate chips: I J L  
Path 0 type: BB to SF
Node 1: 24 Node 2: 100
Chip 1: 3 Chip 2: -1


path[1]
nodes [74-14]

finding chips for nodes: D4-14

node: 1
 
nano candidate chips: J K

node: 2
 
chip: B
Path 1 type: BB to NANO
Node 1: 14 Node 2: 74
Chip 1: 1 Chip 2: -1


... [removed for space]


path[11]
nodes [72-23]

finding chips for nodes: D2-23

node: 1
 
nano candidate chips: J K

node: 2
 
chip: D
Path 11 type: BB to NANO
Node 1: 23 Node 2: 72
Chip 1: 3 Chip 2: -1


path[12]
nodes [71-45]

finding chips for nodes: D1-45

node: 1
 
nano chip: I

node: 2
 
chip: F
Path 12 type: BB to NANO
Node 1: 45 Node 2: 71
Chip 1: 5 Chip 2: 8


paths with candidates:
0,1,3,4,5,6,7,8,9,10,11,

resolving candidates

chips least to most crowded:
C: 0
G: 0
H: 0
J: 0
K: 0
L: 0
I: 1
E: 2
A: 3
B: 3
D: 3
F: 3


sf connections: I1
sf connections: J0
sf connections: K0
sf connections: L0
C: 0
G: 0
H: 0
J: 0
K: 0
L: 0
I: 1
E: 2
A: 3
B: 3
D: 3
F: 3
path[0] chip from D to chip J chosen

sf connections: I1
sf connections: J1
sf connections: K0
sf connections: L0
C: 0
G: 0
H: 0
K: 0
L: 0
I: 1
J: 1
E: 2
A: 3
B: 3
D: 3
F: 3
path[1] chip from B to chip K chosen


... [removed for space]


sf connections: I4
sf connections: J4
sf connections: K3
sf connections: L0
C: 0
G: 0
H: 0
L: 0
E: 2
A: 3
B: 3
D: 3
F: 3
K: 3
I: 4
J: 4
path[11] chip from D to chip K chosen


path[0] net: 1        24 to GND     chip[0]: D  x[0]: 7  y[0]: 2      chip[1]: J  x[1]: 15  y[1]: 3      1      1      

path[1] net: 8        14 to D4     chip[0]: B  x[0]: 11  y[0]: 6      chip[1]: K  x[1]: 6  y[1]: 1      8      8      

path[2] net: 9        10 to 36     chip[0]: B  x[0]: 8  y[0]: 2      chip[1]: E  x[1]: 2  y[1]: 5      9      9      

path[3] net: 10        40 to D6     chip[0]: F  x[0]: 11  y[0]: 2      chip[1]: J  x[1]: 6  y[1]: 5      10      10      

path[4] net: 11        39 to D7     chip[0]: F  x[0]: 10  y[0]: 1      chip[1]: I  x[1]: 7  y[1]: 5      11      11      

path[5] net: 12        38 to D8     chip[0]: E  x[0]: 1  y[0]: 7      chip[1]: K  x[1]: 10  y[1]: 4      12      12      

path[6] net: 13        6 to D9     chip[0]: A  x[0]: 0  y[0]: 5      chip[1]: I  x[1]: 9  y[1]: 0      13      13      

path[7] net: 14        7 to D10     chip[0]: A  x[0]: 1  y[0]: 6      chip[1]: J  x[1]: 9  y[1]: 0      14      14      

path[8] net: 15        9 to D11    no direct path, setting altPathNeeded flag (BBtoSF)

path[9] net: 16        8 to D12    no direct path, setting altPathNeeded flag (BBtoSF)

path[10] net: 17        25 to D3     chip[0]: D  x[0]: 6  y[0]: 3      chip[1]: I  x[1]: 3  y[1]: 3      17      17      

path[11] net: 18        23 to D2     chip[0]: D  x[0]: 15  y[0]: 1      chip[1]: K  x[1]: 4  y[1]: 3      18      18      

path[12] net: 19        45 to D1    no direct path, setting altPathNeeded flag (BBtoSF)


alt paths 

BBtoSF    path: 8
      bb: A        sfChip: K        xMapBB: 0        yMapSF: 0        xStatus: 13  
      bb: B        sfChip: K        xMapBB: -1        yMapSF: 1        xStatus: 65  
      bb: C        sfChip: K        xMapBB: 4        yMapSF: 2        xStatus: -1  
      bb: C        sfChip: K        xMapBB: 4        yMapSF: 2        xStatus: -1  
xMapL0c0: 4      xMapL1c0: 2      xMapL1c1: 5      xMapL0c1: 3

8     chip[0]: B  x[0]: 4  y[0]: 1      chip[1]: K  x[1]: 13  y[1]: 2      15      -1     
Found Path!

BBtoSF    path: 9
      bb: A        sfChip: J        xMapBB: -1        yMapSF: 0        xStatus: 65  
      bb: B        sfChip: J        xMapBB: 2        yMapSF: 1        xStatus: -1  
      bb: B        sfChip: J        xMapBB: 2        yMapSF: 1        xStatus: -1  
xMapL0c0: 2      xMapL1c0: 0      xMapL1c1: 3      xMapL0c1: 1

9     chip[0]: A  x[0]: 2  y[0]: 7      chip[1]: J  x[1]: 10  y[1]: 1      16      -1     
Found Path!

BBtoSF    path: 12
      bb: A        sfChip: I        xMapBB: 0        yMapSF: 0        xStatus: 13  
      bb: B        sfChip: I        xMapBB: 2        yMapSF: 1        xStatus: 16  
      bb: C        sfChip: I        xMapBB: 4        yMapSF: 2        xStatus: -1  
      bb: C        sfChip: I        xMapBB: 4        yMapSF: 2        xStatus: 15  
xMapL0c0: 4      xMapL1c0: 10      xMapL1c1: 5      xMapL0c1: 11

12     chip[0]: F  x[0]: 4  y[0]: 7      chip[1]: I  x[1]: 1  y[1]: 2      19      -1     
Found Path!

resolving uncommitted hops
path    net    node1    type    chip0    x0    y0    node2    type    chip1    x1    y1    altPath    sameChp    pathType    chipL    chip2    x2    y2    x3    y3

0    1    24    0    D    7    2    GND    2    J    15    3    0    0    BB to SF    0 
1    8    14    0    B    11    6    D4    1    K    6    1    0    0    BB to NANO    0 
2    9    10    0    B    8    2    36    0    E    2    5    0    0    BB to BB    0 
3    10    40    0    F    11    2    D6    1    J    6    5    0    0    BB to NANO    0 
4    11    39    0    F    10    1    D7    1    I    7    5    0    0    BB to NANO    0 
5    12    38    0    E    1    7    D8    1    K    10    4    0    0    BB to NANO    0 
6    13    6    0    A    0    5    D9    1    I    9    0    0    0    BB to NANO    0 
7    14    7    0    A    1    6    D10    1    J    9    0    0    0    BB to NANO    0 
8    15    9    0    B    4    1    D11    1    K    13    2    0    1    BB to NANO    0     C     2     -2     13     -1 
9    16    8    0    A    2    7    D12    1    J    10    1    0    1    BB to NANO    0     B     0     -2     3     -1 
10    17    25    0    D    6    3    D3    1    I    3    3    0    0    BB to NANO    0 
11    18    23    0    D    15    1    D2    1    K    4    3    0    0    BB to NANO    0 
12    19    45    0    F    4    7    D1    1    I    1    2    0    1    BB to NANO    0     C     10     -2     4     -1 

path    net    node1    type    chip0    x0    y0    node2    type    chip1    x1    y1    altPath    sameChp    pathType    chipL    chip2    x2    y2    x3    y3


taken connections (-1 is free)
chip    0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15        0    1    2    3    4    5    6    7
A    13   14   16   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1       -1   -1   -1   -1   -1   13   14   16    
B    16   -1   16   -1   15   -1   -1   -1   9    -1   -1   8    -1   -1   -1   -1       -1   15   9    -1   -1   -1   8    -1    
C    -1   -1   15   -1   19   -1   -1   -1   -1   -1   19   -1   -1   -1   -1   -1       -1   -1   -1   -1   -1   -1   -1   -1    
D    -1   -1   -1   -1   -1   -1   17   1    -1   -1   -1   -1   -1   -1   -1   18       -1   18   1    17   -1   -1   -1   -1    
E    -1   12   9    -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1       -1   -1   -1   -1   -1   9    -1   12    
F    -1   -1   -1   -1   19   -1   -1   -1   -1   -1   11   10   -1   -1   -1   -1       -1   11   10   -1   -1   -1   -1   19    
G    -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1       -1   -1   -1   -1   -1   -1   -1   -1    
H    -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1       -1   -1   -1   -1   -1   -1   -1   -1   

chip    0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15        0    1    2    3    4    5    6    7 
I    -1   19   -1   17   -1   -1   -1   11   15   13   -1   -1   -1   -1   -1   1        13   -1   19   17   -1   11   -1   -1    
J    -1   -1   18   -1   8    -1   10   -1   12   14   16   -1   -1   -1   -1   1        14   16   -1   1    -1   10   -1   -1    
K    -1   -1   -1   -1   18   17   8    -1   10   11   12   13   14   15   16   -1       -1   8    15   18   12   -1   -1   -1    
L    -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   -1   1        -1   -1   -1   -1   -1   -1   -1   -1    
8    same chips: @, @, @, @,     @, @, C, C,  
9    same chips: @, @, @, @,     @, @, B, B,  
12    same chips: @, @, @, @,     @, @, C, C,  

final paths
path    net    node1    type    chip0    x0    y0    node2    type    chip1    x1    y1    altPath    sameChp    pathType    chipL    chip2    x2    y2    x3    y3

0    1    24    0    D    7    2    GND    2    J    15    3    0    0    BB to SF    0 
1    8    14    0    B    11    6    D4    1    K    6    1    0    0    BB to NANO    0 
2    9    10    0    B    8    2    36    0    E    2    5    0    0    BB to BB    0 
3    10    40    0    F    11    2    D6    1    J    6    5    0    0    BB to NANO    0 
4    11    39    0    F    10    1    D7    1    I    7    5    0    0    BB to NANO    0 
5    12    38    0    E    1    7    D8    1    K    10    4    0    0    BB to NANO    0 
6    13    6    0    A    0    5    D9    1    I    9    0    0    0    BB to NANO    0 
7    14    7    0    A    1    6    D10    1    J    9    0    0    0    BB to NANO    0 
8    15    9    0    B    4    1    D11    1    K    13    2    0    1    BB to NANO    0     C     2     0     13     0 
9    16    8    0    A    2    7    D12    1    J    10    1    0    1    BB to NANO    0     B     0     0     3     0 
10    17    25    0    D    6    3    D3    1    I    3    3    0    0    BB to NANO    0 
11    18    23    0    D    15    1    D2    1    K    4    3    0    0    BB to NANO    0 
12    19    45    0    F    4    7    D1    1    I    1    2    0    1    BB to NANO    0     C     10     1     4     1 

path    net    node1    type    chip0    x0    y0    node2    type    chip1    x1    y1    altPath    sameChp    pathType    chipL    chip2    x2    y2    x3    y3

```

# Controlling the Crosspoint Switches


## What crosspoint switches crave
Okay, so now we have all our paths filled out with what chips need to have which X and Y inputs connected to make the magic happen.

The CH446Qs are basically clones of the MT8816, except for one important difference, they accept serial addressing. The datasheet is kinda vague about how, but it turns out it's just a kinda weird version of SPI. 
![Screenshot 2023-08-29 at 7 18 05 PM](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/b2d8d286-f4ab-4744-9115-da5f1b8976bd)


Basically, all the chips see the same data signal, and whichever one sees a pulse on the STB when the last bit comes in will connect or disconnect the selected X and Y inputs. The state of the DAT line when the Strobe comes in determines whether it's connecting or disconnecting. That stretched out clock line shows that it doesn't care about real time, which comes in handy.

## PIO State Machine
So I have to do something that's kinda like SPI but not quite, this looks like a job for the RP2040 PIO State Machines.

Even knowing assembly, the learning curve for writing a PIO program is steep. The documentation is really hit-or-miss, the examples are uncommented and written like they're playing code golf, insanely terse. Like these people do realize you can name variables after what they do, right? And these are the Official examples in the datasheet. Anyway after a few days of staring at what looks like gibberish, it starts to click.

I copied the SPI.pio example and edited it from there. Let me try to explain some of the things I learned to hopefully make it easier for you to write a PIO program in the future. 

I'm just compiling this with the online pioasm compiler and then pasting the compiled code into spi.pio.h

https://wokwi.com/tools/pioasm

Here's where we are:
![start](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7fc8357f-2b50-4341-9832-10d94d4cb595)



;this is basically spi but it sets a system IRQ on the last bit to allow the chip select pulse to happen


```
.program spi_ch446_multi_cs
.side_set 1

.wrap_target
bitloop:

    out pins, 1        side 0x0 [2]
    
    nop                side 0x1 [2]

    jmp x-- bitloop    side 0x1

    out pins, 1        side 0x1

    mov x, y           side 0x1

    irq  0             side 0x1
    
    wait 0 irq 0 rel   side 0x1
    
    jmp !osre bitloop  side 0x0

public entry_point:                 ; Must set X,Y to n-2 before starting!


    pull ifempty       side 0x0 [1] ; Block with CSn high (minimum 2 cycles)
    
    nop                side 0x0 [1]; CSn front porch


.wrap

```
What wasn't explained well is what the hell a sideset pin is. Basically you do your normal-ish assembly code on the left, and then each operation also affects the sideset pin on the right. It's kind of a hack to allow you to control 2 pins in a single clock cycle. In this case, the sideset pin is attached to the CLK, and pins, 1 is DAT.

So, whats going on is that in the regular code, I'm sending a byte to the sm register with this line

pio_sm_put(pio, sm, chAddress);
(the last bit of chAddress is set to 1 or 0 depending if I want to connect or disconnect)

and that pull ifempty will pull in a byte to the working register and send it out one bit at a time while toggling the clock. When it's out of data to send, it triggers a system interrupt request that can be seen outside of the PIO state machine and I deal with it in an ISR in CH446Q.cpp

At this point, here's where we are in the timing diagram:

![IRG](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/92b8b1f4-fae2-453b-b174-3a69154230d1)

Now we need to select the correct CS line to make the right chip make the connection
```
void isrFromPio(void)
{
  switch (chipSelect)
  {
  case CHIP_A:
  {
    digitalWriteFast(CS_A, HIGH);
    break;
  }
  case CHIP_B:
  {
    digitalWriteFast(CS_B, HIGH);
    break;
  }
  case CHIP_C:
  {
    digitalWriteFast(CS_C, HIGH);
    break;
  }
  case CHIP_D:
  {
    digitalWriteFast(CS_D, HIGH);
    break;
  }
  case CHIP_E:
  {
    digitalWriteFast(CS_E, HIGH);
    break;
  }
  case CHIP_F:
  {
    digitalWriteFast(CS_F, HIGH);
    break;
  }
  case CHIP_G:
  {
    digitalWriteFast(CS_G, HIGH);
    break;
  }
  case CHIP_H:
  {
    digitalWriteFast(CS_H, HIGH);
    break;
  }
  case CHIP_I:
  {
    digitalWriteFast(CS_I, HIGH);
    break;
  }
  case CHIP_J:
  {
    digitalWriteFast(CS_J, HIGH);
    break;
  }
  case CHIP_K:
  {
    digitalWriteFast(CS_K, HIGH);
    break;
  }
  case CHIP_L:
  {
    digitalWriteFast(CS_L, HIGH);
    break;
  }
  }


```
![csHigh](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/95b52b1f-7d4a-4ab9-9eb4-9ff492b6118f)

```
delayMicroseconds(1);
```
![csLow](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/a4529b4c-401f-464a-a9dd-03701ef6b916)
```
  digitalWriteFast(CS_A, LOW);
  digitalWriteFast(CS_B, LOW);
  digitalWriteFast(CS_C, LOW);
  digitalWriteFast(CS_D, LOW);
  digitalWriteFast(CS_E, LOW);
  digitalWriteFast(CS_F, LOW);
  digitalWriteFast(CS_G, LOW);

  digitalWriteFast(CS_H, LOW);
  digitalWriteFast(CS_I, LOW);
  digitalWriteFast(CS_J, LOW);
  digitalWriteFast(CS_K, LOW);
  digitalWriteFast(CS_L, LOW);

  irq_flags = pio0_hw->irq; 
  pio_interrupt_clear(pio, PIO0_IRQ_0);
  hw_clear_bits(&pio0_hw->irq, irq_flags);//clears the IRQ
  
}
```
The reason I had to do it in an external interrupt instead of in the PIO code is because there's a limit to how many pins can be attached to a single state machine, 8. And this is just way easier to do.

## The C
This all runs on the second core just so it can stay somewhat timing sensitive while not worrying about what's going on elsewhere. How this process is triggered is that when the pathfinding algorithm is finished running in core 0, it sets 
```
volatile int sendAllPathsCore2 = 1; // this signals the core 2 to send all the paths to the CH446Q
```
Then in loop1, is just constantly checks if that's a 1 and will send the paths and set it back to 0. Just a reminder that the cores on an RP2040 share global variables, because it's very useful. 

Here's what the SendAllPaths() functions look like. 
```
void sendAllPaths(void) // should we sort them by chip? for now, no
{

  for (int i = 0; i < numberOfPaths; i++)
  {
    sendPath(i, 1);
  }

}

void sendPath(int i, int setOrClear)
{

  uint32_t chAddress = 0;

  int chipToConnect = 0;
  int chYdata = 0;
  int chXdata = 0;

  for (int chip = 0; chip < 4; chip++)
  {
    if (path[i].chip[chip] != -1)
    {
      chipSelect = path[i].chip[chip];

      chipToConnect = path[i].chip[chip];

        chYdata = path[i].y[chip];
        chXdata = path[i].x[chip];

        chYdata = chYdata << 5;
        chYdata = chYdata & 0b11100000;

        chXdata = chXdata << 1;
        chXdata = chXdata & 0b00011110;

        chAddress = chYdata | chXdata;

        if (setOrClear == 1)
        {
          chAddress = chAddress | 0b00000001; // this last bit determines whether we set or unset the path
        }

        chAddress = chAddress << 24;

        // delayMicroseconds(50);

        delayMicroseconds(30);

        pio_sm_put(pio, sm, chAddress);

        delayMicroseconds(40);
      //}
    }
  }
}
```
The whole process to connect the whole board takes a couple milliseconds at most. But you can shave down these delays if you have some reason to go faster.

## To be continued in part 4 - LEDs





























































