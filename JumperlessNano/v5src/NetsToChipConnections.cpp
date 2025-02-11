// SPDX-License-Identifier: MIT

#include "NetsToChipConnections.h"

#include <Arduino.h>
#include <EEPROM.h>

#include "Commands.h"
#include "JumperlessDefinesRP2040.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "Peripherals.h"
// don't try to understand this, it's still a mess
bool debugNTCC5 = false;
int startEndChip[2] = { -1, -1 };
int bothNodes[2] = { -1, -1 };
int endChip = -1;
int chipCandidates[2][4] = {
    {-1, -1, -1, -1},
    {-1, -1, -1,
     -1} }; // nano and sf nodes have multiple possible chips they could be
// connected to, so we need to store them all and check them all

int chipsLeastToMostCrowded[12] = {
    0, 1, 2, 3, 4,  5,
    6, 7, 8, 9, 10, 11 }; // this will be sorted from most to least crowded, and
// will be used to determine which chip to use for
// each node
int sfChipsLeastToMostCrowded[4] = {
    8, 9, 10, 11 }; // this will be sorted from most to least crowded, and will
// be used to determine which chip to use for each node

int numberOfUniqueNets = 0;
int numberOfNets = 0;
int numberOfPaths = 0;

int pathsWithCandidates[MAX_BRIDGES] = { 0 };
int pathsWithCandidatesIndex = 0;

int numberOfUnconnectablePaths = 0;
int unconnectablePaths[10][2] = {
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1},
  };
int newBridges[MAX_NETS][MAX_DUPLICATE][2] = { 0 };
unsigned long timeToSort = 0;

bool debugNTCC = 0; // EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);

bool debugNTCC2 = 0; // EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

bool debugNTCC3 = false;
int pathIndex = 0;

int powerDuplicates = 2;
int dacDuplicates = 0;
int pathDuplicates = 2;
int powerPriority = 1;
int dacPriority = 1;

// Or maybe a more useful way to default to: run a set number of connections
// (like 2-4) for power, then 2 for every regular jumper, then fill in the rest
// of with more power connections.

void clearAllNTCC(void) {
  startEndChip[0] = -1;
  startEndChip[1] = -1;
  bothNodes[0] = -1;

  bothNodes[1] = -1;

  numberOfUniqueNets = 0;
  numberOfNets = 0;
  numberOfPaths = 0;

  pathsWithCandidatesIndex = 0;
  pathIndex = 0;

  for (int i = 0; i < 12; i++) {
    chipsLeastToMostCrowded[i] = i;
    }
  for (int i = 0; i < 4; i++) {
    chipCandidates[0][i] = -1;
    chipCandidates[1][i] = -1;

    sfChipsLeastToMostCrowded[i] = i + 8;
    }
  // for (int g = 0; g < 10; g++) {
  // gpioNet[g] = -1;
  //   }
  for (int i = 0; i < MAX_BRIDGES; i++) {
    pathsWithCandidates[i] = 0;
    path[i].net = 0;
    path[i].node1 = 0;
    path[i].node2 = 0;
    path[i].altPathNeeded = false;
    path[i].sameChip = false;
    path[i].skip = false;

    for (int j = 0; j < 4; j++) {
      path[i].chip[j] = 0;
      }

    for (int j = 0; j < 6; j++) {
      path[i].x[j] = 0;
      path[i].y[j] = 0;
      }

    for (int j = 0; j < 3; j++) {
      path[i].nodeType[j] = BB;
      for (int k = 0; k < 3; k++) {
        path[i].candidates[j][k] = -1;
        }
      }
    }
  //clang-format off
  // struct netStruct net[MAX_NETS] = { //these are the special function nets
  // that will always be made
  // //netNumber,       ,netName          ,memberNodes[] ,memberBridges[][2]
  // ,specialFunction        ,intsctNet[] ,doNotIntersectNodes[] ,priority
  // (unused)
  //     {     127      ,"Empty Net"      ,{EMPTY_NET}           ,{{}}
  //     ,EMPTY_NET              ,{}
  //     ,{EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET}
  //     , 0}, {     1        ,"GND"            ,{GND}                 ,{{}}
  //     ,GND                    ,{}          ,{SUPPLY_3V3,SUPPLY_5V,DAC0,DAC1}
  //     , 1}, {     2        ,"Top Rail"       ,{TOP_RAIL}            ,{{}}
  //     ,TOP_RAIL               ,{}          ,{GND} , 1}, {     3 ,"Bottom
  //     Rail"    ,{BOTTOM_RAIL}         ,{{}}                   ,BOTTOM_RAIL
  //     ,{}          ,{GND}                               , 1}, {     4 ,"DAC
  //     0"          ,{DAC0}                ,{{}}                   ,DAC0 ,{}
  //     ,{GND}                               , 1}, {     5        ,"DAC 1"
  //     ,{DAC1}                ,{{}}                   ,DAC1 ,{} ,{GND} , 1},
  //     {     6        ,"I Sense +"      ,{ISENSE_PLUS}         ,{{}}
  //     ,ISENSE_PLUS            ,{}          ,{ISENSE_MINUS} , 2}, {     7 ,"I
  //     Sense -"      ,{ISENSE_MINUS}        ,{{}} ,ISENSE_MINUS           ,{}
  //     ,{ISENSE_PLUS}                       , 2},
  // };
  net[0] = { 127,
            "Empty Net",
            {EMPTY_NET},
            {{}},
            EMPTY_NET,
            {},
            {EMPTY_NET, EMPTY_NET, EMPTY_NET, EMPTY_NET, EMPTY_NET, EMPTY_NET,
             EMPTY_NET},
            0 };
  net[1] = { 1, "GND", {GND}, {{}}, GND, {}, {SUPPLY_3V3, SUPPLY_5V, DAC0, DAC1},
            1 };
  net[2] = { 2, "Top Rail", {TOP_RAIL}, {{}}, TOP_RAIL, {}, {GND}, 1 };
  net[3] = { 3, "Bottom Rail", {BOTTOM_RAIL}, {{}}, BOTTOM_RAIL, {}, {GND}, 1 };
  net[4] = { 4, "DAC 0", {DAC0}, {{}}, DAC0, {}, {GND}, 1 };
  net[5] = { 5, "DAC 1", {DAC1}, {{}}, DAC1, {}, {GND}, 1 };

  //clang-format on

  net[1].rawColor = rawSpecialNetColors[1];
  net[2].rawColor = rawSpecialNetColors[2];
  net[3].rawColor = rawSpecialNetColors[3];
  net[4].rawColor = rawSpecialNetColors[4];
  net[5].rawColor = rawSpecialNetColors[5];

  net[1].machine = false;
  net[2].machine = false;
  net[3].machine = false;
  net[4].machine = false;
  net[5].machine = false;

  net[1].priority = powerPriority;
  net[2].priority = powerPriority;
  net[3].priority = powerPriority;
  net[4].priority = dacPriority;
  net[5].priority = dacPriority;

  for (int i = 6; i < MAX_NETS; i++) {
    net[i] = { 0, " ", {}, {{}}, 0, {}, {}, 0, 0, 0, 0, false };
    }

  for (int i = 0; i < 12; i++) {
    ch[i].uncommittedHops = 0;
    for (int j = 0; j < 16; j++) {
      ch[i].xStatus[j] = -1;
      }

    for (int j = 0; j < 8; j++) {
      ch[i].yStatus[j] = -1;
      }
    }
  for (int i = 0; i < 10; i++) {
    unconnectablePaths[i][0] = -1;
    unconnectablePaths[i][1] = -1;
    }
  numberOfUnconnectablePaths = 0;
  // printPathsCompact();
  // printChipStatus();
  }

void sortPathsByNet(
    void) // not actually sorting, just copying the bridges and nets back from
  // netStruct so they're both in the same order
  {
  if (debugNTCC) {
    Serial.println("sortPathsByNet()");
    }
  timeToSort = micros();
  numberOfPaths = 0;
  pathIndex = 0;

  if (debugNTCC) {
    printBridgeArray();
    }

  numberOfNets = 0;
  for (int i = 0; i < MAX_NETS - 1; i++) {
    if (net[i].number != 0 && net[i].number != -1) {
      numberOfNets++;
      // break;
      } else {
      break;
      }
    }



  for (int i = 0; i < MAX_BRIDGES; i++) {
    if ((path[i].node1 != 0 && path[i].node2 != 0) && (path[i].node1 != -1 && path[i].node2 != -1)) {
      numberOfPaths++;

      // Serial.print("path[");
      // Serial.print(i);
      // Serial.print("] ");
      // Serial.print("node1: ");
      // Serial.print(path[i].node1);
      // Serial.print("  node2: ");
      // Serial.println(path[i].node2);

      // break;
      } else if (path[i].node1 == 0 || path[i].node2 == 0) {
        break;
        }
    }

  // Serial.print("number of paths: ");
  // Serial.println(numberOfPaths);
  // printPathArray();
  // if (debugNTCC)
  // {
  // Serial.print("number of paths: ");
  // Serial.println(numberOfPaths);
  // }


  int routableBufferPowerFound = -1;

  int lastPowerPath = -1;
  numberOfUniqueNets = 0;
  numberOfShownNets = 0;

  for (int j = 1; j <= MAX_NETS; j++) {
    if (net[j].number == 0) {
      break;
      // continue;
      }

    for (int k = 0; k < MAX_NODES; k++) {
      if (net[j].bridges[k][0] == 0) {
        break;
        // continue;
        } else {
        path[pathIndex].net = net[j].number;
        path[pathIndex].node1 = net[j].bridges[k][0];
        path[pathIndex].node2 = net[j].bridges[k][1];
        path[pathIndex].duplicate = 0;

        if ((path[pathIndex].node1 == ROUTABLE_BUFFER_IN && path[pathIndex].node2 == DAC0) ||
            (path[pathIndex].node1 == DAC0 && path[pathIndex].node2 == ROUTABLE_BUFFER_IN)) {
          routableBufferPowerFound = pathIndex;
          }

        if (path[pathIndex].net <= 5) {
          lastPowerPath = pathIndex;
          }

        if (path[pathIndex].net == path[pathIndex - 1].net) {
          } else {
          numberOfUniqueNets++;
          if (path[pathIndex].net >= 6) {
            if ((path[pathIndex].node1 <= 60 || (path[pathIndex].node1 >= NANO_D0 &&
                                                 path[pathIndex].node1 <= NANO_RESET_1)) || (path[pathIndex].node2 <= 60 ||
                                                                                             (path[pathIndex].node2 >= NANO_D0 && path[pathIndex].node2 <= NANO_RESET_1))) {
              net[j].visible = 1;
              numberOfShownNets++;
              // Serial.print("path  ");
              // Serial.print(pathIndex);
              // Serial.print("   net ");
              // Serial.print(j);
              // Serial.println(" is visible\n\r");

              } else {

              net[j].visible = 0;
              // Serial.print("path  ");
              // Serial.print(pathIndex);
              // Serial.print("   net ");
              // Serial.print(j);
              // Serial.print(" is not visible\n\r");
              // Serial.print("node1: ");
              // Serial.print(path[pathIndex].node1);
              // Serial.print("  node2: ");
              // Serial.println(path[pathIndex].node2);
              }
            // numberOfShownNets++;
            }
          }

        // if (debugNTCC) {
      // Serial.print("path[");
      // Serial.print(pathIndex);
      // Serial.print("] net: ");
      // Serial.println(path[pathIndex].net);
      //}
        pathIndex++;
        }
      }
    }

  if (routableBufferPowerFound > 0) {

    pathStruct tempPath = path[routableBufferPowerFound];

    //printPathsCompact();

    //shift all paths up one and put routableBufferPower path at the beginning
    for (int i = routableBufferPowerFound; i > 0; i--) {
      path[i] = path[i - 1];
      }
    path[0] = tempPath;

    //printPathsCompact();


    }
  // Serial.print("Routable Buffer Power Found: ");
  // Serial.println(routableBufferPowerFound);

  // Serial.print("Last Power Path: ");
  // Serial.println(lastPowerPath);

  newBridgeLength = numberOfPaths;
  numberOfPaths = pathIndex;

  // for (int i = 0; i < numberOfNets; i++) {

  //   }

  if (debugNTCC) {
    Serial.print("number unique of nets: ");
    Serial.println(numberOfUniqueNets);
    Serial.print("pathIndex: ");
    Serial.println(pathIndex);
    Serial.print("numberOfPaths: ");
    Serial.println(numberOfPaths);
    }
  // numberOfShownNets = numberOfUniqueNets;
  //  printPathArray();
  clearChipsOnPathToNegOne(); // clear chips and all trailing paths to -1{if
  // there are bridges that weren't made due to DNI
  // rules, there will be fewer paths now because
  // they were skipped}

  if (debugNTCC) {
    Serial.println("cleared trailing paths");
    // delay(10);
    printBridgeArray();
    // delay(10);
    Serial.println("\n\r");
    timeToSort = micros() - timeToSort;
    Serial.print("time to sort: ");
    Serial.print(timeToSort);
    Serial.println("us\n\r");
    }
  }

void bridgesToPaths(int fillUnused, int allowStacking) {
  if (debugNTCC5) {
    Serial.println("bridgesToPaths()");
    }

  for (int i = 0; i < MAX_BRIDGES; i++) {
    pathsWithCandidates[i] = 0;
    }

  // allowStacking = 0;
  sortPathsByNet();
  //   Serial.print("number of paths: ");
  // Serial.println(numberOfPaths);
  // Serial.print("number of shown paths: ");
  // Serial.println(numberOfShownNets);

  if (fillUnused == 1) {
    fillUnusedPaths(powerDuplicates, dacDuplicates, pathDuplicates);
    }
  // Serial.print("number of paths: ");
  // Serial.println(numberOfPaths);
  // Serial.print("number of shown paths: ");
  // Serial.println(numberOfShownNets);
  // sortPathsByNet();

  for (int i = 0; i < numberOfPaths; i++) {
    if (debugNTCC5) {
      delay(10);
      Serial.print("path[");
      Serial.print(i);
      Serial.print("]\n\rnodes [");
      Serial.print(path[i].node1);
      Serial.print("-");
      Serial.print(path[i].node2);
      Serial.println("]\n\r");
      }

    findStartAndEndChips(path[i].node1, path[i].node2, i);

    if (debugNTCC5) {
      delay(10);
      Serial.print("startEndChip[0]: ");
      Serial.print(startEndChip[0]);
      Serial.print("  startEndChip[1]: ");
      Serial.println(startEndChip[1]);
      }

    mergeOverlappingCandidates(i);
    if (debugNTCC5) {
      delay(10);
      Serial.println("mergeOverlappingCandidates done");
      }

    assignPathType(i);

    if (debugNTCC5) {
      delay(10);
      Serial.println("assignPathType done");
      Serial.println("\n\n\r");
      }
    }

  if (debugNTCC5) {
    Serial.println("paths with candidates:");
    }

  if (debugNTCC5) {
    delay(10);
    for (int i = 0; i < pathsWithCandidatesIndex; i++) {
      Serial.print(pathsWithCandidates[i]);
      Serial.print(",");
      }
    Serial.println("\n\r");
    // printPathArray();
    }

  sortAllChipsLeastToMostCrowded();

  resolveChipCandidates();

  commitPaths(allowStacking, -1);


  resolveAltPaths(allowStacking, -1);

  resolveUncommittedHops(allowStacking, -1);

  resolveAltPaths(1);

  resolveUncommittedHops(1, -1);
  couldntFindPath(0);
  // couldntFindPath();
  checkForOverlappingPaths();
  // printPathsCompact();
  // printChipStatus();
  if (debugNTCC2) {
    delay(10);
    printPathsCompact();
    delay(10);
    printChipStatus();
    delay(10);
    }


  }

void fillUnusedPaths(int duplicatePathsOverride, int duplicatePathsPower,
                     int duplicatePathsDac) {
  /// return;

  int duplicatePathIndex = 0;

  uint8_t nodeCount[MAX_NETS] = { 0 };
  uint8_t bridgeCount[MAX_NETS] = { 0 };

  for (int i = 0; i < MAX_NETS; i++) {
    for (int j = 0; j < MAX_DUPLICATE; j++) {
      for (int k = 0; k < 2; k++) {
        newBridges[i][j][k] = 0;
        }
      }
    }

  for (int n = 0; n < numberOfNets; n++) {
    // Serial.print("net[");
    // Serial.print(n);
    // Serial.print("] \n\rnumber: ");
    // Serial.println(net[n].number);
    for (int i = 0; i < MAX_NODES; i++) {
      if (net[n].nodes[i] == 0) {
        break;
        }
      nodeCount[n]++;
      // Serial.print(" \n\rnode: ");
      // Serial.println(net[n].nodes[i]);
      }

    for (int i = 0; i < MAX_BRIDGES; i++) {
      if (net[n].bridges[i][0] == 0) {
        break;
        }
      bridgeCount[n]++;
      // Serial.print(" \n\rbridges: ");
      // Serial.print(net[n].bridges[i][0]);
      // Serial.print("-");
      // Serial.println(net[n].bridges[i][1]);
      }
    // Serial.println("\n\r");
    }

  int duplindex = 0;
  // first figure out which paths need duplicates
  for (int i = 0; i < numberOfPaths; i++) {
    if (bridgeCount[path[i].net] > 0) {
      // path[i].duplicate = 1;
      // net[path[i].net].duplicatePaths[duplindex] = i;
      if (path[i].net <= 3) {
        net[path[i].net].numberOfDuplicates = powerDuplicates;
        } else if (path[i].net == 4 || path[i].net == 5) {
          net[path[i].net].numberOfDuplicates = dacDuplicates;
          } else {
          net[path[i].net].numberOfDuplicates = pathDuplicates;
          }
      }
    }

  // get the nodes in the net and cycle them, so if the bridges are A-B, B-C,
  // the duplicate paths will start with A-C

  //  A-B, B-C                        -> A-C
  //  A-B, B-C, C-D                   -> A-C, A-D, B-D
  //  A-B, B-C, C-D, D-E              -> A-C, A-D, A-E, B-D, B-E,
  //  A-B, B-C, C-D, D-E, E-F         -> A-C, A-D, A-E, A-F, B-D, B-E, B-F, C-E,
  //  C-F, D-F A-B, B-C, C-D, D-E, E-F, F-G    -> A-C, A-D, A-E, A-F, A-G, B-D,
  //  B-E, B-F, B-G, C-E, C-F, C-G, D-F, D-G, E-G A-B, B-C, C-D, D-E, E-F, F-G,
  //  G-H -> A-C, A-D, A-E, A-F, A-G, A-H, B-D, B-E, B-F, B-G, B-H, C-E, C-F,
  //  C-G, C-H, D-F, D-G, D-H, E-G, E-H, F-H

  // int bridgeLUT[MAX_DUPLICATE] = {1, 1, 3, 5, 10, 15, 21, 28, 36, 45, 55, 66,
  // 78, 91, 105, 120, 136, 153, 171, 190, 210, 231, 253, 276, 300};

  // int16_t tempNodes[MAX_NETS][MAX_NODES] = {0};

  for (int i = 1; i < numberOfNets; i++) {
    if (net[i].numberOfDuplicates == 0) {
      continue;
      }

    // int16_t tempNodes[MAX_NODES];
    //  Serial.print("net[");
    //  Serial.print(i);
    //  Serial.print("]  nodes[");

    for (int j = 0; j < nodeCount[i]; j++) {
      // tempNodes[j] = net[i].nodes[j];
      //   Serial.print(tempNodes[j]);
      // Serial.print(net[i].nodes[j]);
      // Serial.print(", ");
      }
    // Serial.println("]\t\t");

    int targetBridgeCount = net[i].numberOfDuplicates;
    int skip = 1;

    int unique = 0;

    int testCounter0 = 0;
    int testCounter1 = 1; // nodeCount[i] / 2;
    int testBridge[2] = { -1, -1 };

    int bridge0 = 0;
    int bridge1 = 1;

    for (int j = 0; j <= targetBridgeCount; j++) {
      if (nodeCount[i] >= 3) {
        for (int l = 0; l < MAX_DUPLICATE; l++) {
          if (unique == -1) {
            bridge1++;
            if (bridge1 >= nodeCount[i]) {
              bridge0++;
              if (bridge0 >= nodeCount[i]) {
                bridge0 = 0;
                }
              bridge1 = bridge0 + 1;
              }
            unique = 0;
            }
          if (net[i].nodes[bridge0] == 0 || net[i].nodes[bridge1] == 0) {
            break;
            }
          if (net[i].nodes[bridge0] == net[i].nodes[bridge1]) {
            unique = -1;
            continue;
            }
          if (net[i].nodes[bridge0] == net[i].bridges[l][0] &&
              net[i].nodes[bridge1] == net[i].bridges[l][1]) {
            unique = -1;
            continue;
            }
          if (net[i].nodes[bridge0] == net[i].bridges[l][1] &&
              net[i].nodes[bridge1] == net[i].bridges[l][0]) {
            unique = -1;
            continue;
            }
          unique = 1;
          // Serial.print(net[i].nodes[bridge0]);
          // Serial.print("-");
          // Serial.print(net[i].nodes[bridge1]);
          // Serial.println("]\t\t");
          // Serial.print("net[");

          break;
          }
        }
      newBridges[i][j][0] = net[i].nodes[bridge0];
      newBridges[i][j][1] = net[i].nodes[bridge1];

      net[i].bridges[j][0] = newBridges[i][j][0];
      net[i].bridges[j][1] = newBridges[i][j][1];

      bridge1++;

      if (bridge1 >= nodeCount[i]) {
        bridge0++;
        if (bridge0 >= nodeCount[i]) {
          bridge0 = 0;
          }
        bridge1 = bridge0 + 1;
        }

      if (newBridges[i][j][0] == newBridges[i][j][1] ||
          newBridges[i][j][0] == 0 || newBridges[i][j][1] == 0) {
        // Serial.print("skipping ");
        // Serial.println(j);
        j--;
        continue;
        } else {
        duplicatePathIndex++;
        }
      }
    }
  // int maxxed = 0;
  int priorities[MAX_NETS] = { 0 };
  int maxp = 0;

  for (int j = 0; j < MAX_DUPLICATE; j++) {
    for (int i = 0; i < numberOfNets; i++) {
      priorities[i] = net[i].priority;
      if (i < 6 && net[i].priority > maxp) {
        maxp = net[i].priority;
        }
      }
    for (int k = 0; k < maxp; k++) {
      for (int i = 0; i < 6; i++) {
        // for (int p = 0; p < net[i].priority; p++) {

        if (net[i].numberOfDuplicates == 0) {
          continue;
          }

        if (newBridges[i][j][0] >= 110 && newBridges[i][j][0] <= 115 ||
            newBridges[i][j][1] >= 110 && newBridges[i][j][1] <= 115) {
          continue;
          }

        if (priorities[i] <= 0) {
          continue;
          }

        if (priorities[i] > 0) {
          priorities[i]--;
          }

        //! make it add the the priority so the connections are mixed

        if (newBridges[i][j][0] == ROUTABLE_BUFFER_IN &&
                newBridges[i][j][1] == DAC0 ||
            newBridges[i][j][0] == DAC0 &&
                newBridges[i][j][1] == ROUTABLE_BUFFER_IN) {
          continue;
          }

        if (newBridges[i][j][0] != 0 || newBridges[i][j][1] != 0) {
          path[numberOfPaths].net = i;
          path[numberOfPaths].node1 = newBridges[i][j][0];
          path[numberOfPaths].node2 = newBridges[i][j][1];
          path[numberOfPaths].altPathNeeded = false;
          path[numberOfPaths].sameChip = false;
          path[numberOfPaths].skip = false;
          path[numberOfPaths].duplicate = 1;
          numberOfPaths++;
          if (numberOfPaths >= MAX_BRIDGES - 1) {
            // maxxed = 1;
            return;
            break;
            }
          }
        // }
        // Serial.print("\n\r");
        }
      }

    // for (int i = 0; i < 6; i++) {
    //   priorities[i] = net[i].priority;
    // }
    for (int i = 5; i < numberOfNets; i++) {
      if (net[i].numberOfDuplicates == 0) {
        continue;
        }

      if (newBridges[i][j][0] >= 110 && newBridges[i][j][0] <= 115 ||
          newBridges[i][j][1] >= 110 && newBridges[i][j][1] <= 115) {
        continue;
        }

      if (priorities[i] <= 0) {
        continue;
        }

      if (priorities[i] > 0) {
        priorities[i]--;
        }

      if (newBridges[i][j][0] != 0 && newBridges[i][j][1] != 0) {
        ///
        net[i].bridges[bridgeCount[i]][0] = newBridges[i][j][0];
        net[i].bridges[bridgeCount[i]][1] = newBridges[i][j][1];
        bridgeCount[i]++; ///!why is this incrementing bridgeCount[0]?

        path[numberOfPaths].net = i;
        path[numberOfPaths].node1 = newBridges[i][j][0];
        path[numberOfPaths].node2 = newBridges[i][j][1];
        path[numberOfPaths].altPathNeeded = false;
        path[numberOfPaths].sameChip = false;
        path[numberOfPaths].skip = false;
        path[numberOfPaths].duplicate = 1;
        numberOfPaths++;
        if (numberOfPaths >= MAX_BRIDGES - 1) {
          // maxxed = 1;
          return;
          break;
          }
        }
      // }
      // Serial.print("\n\r");
      }
    }
  // Serial.print("done filling unused paths\n\r");
  }

void commitPaths(int allowStacking, int powerOnly) {
  if (debugNTCC2) {
    Serial.println("commitPaths()\n\r");
    }

  for (int i = 0; i < numberOfPaths; i++) {
    // duplicateSFnets();
    // Serial.print(i);
    // Serial.print(" \t");

    if (debugNTCC == true) {
      Serial.print("\n\rpath[");
      Serial.print(i);
      Serial.print("] net: ");
      Serial.print(path[i].net);
      Serial.print("   \t ");

      printNodeOrName(path[i].node1);
      Serial.print(" to ");
      printNodeOrName(path[i].node2);
      // Serial.print("\n\r");
      }
    // if (path[i].altPathNeeded == true)
    // {
    //     // if (debugNTCC2 == true)
    //     // {
    //     //     Serial.println("\taltPathNeeded flag already set\n\r");
    //     // }

    //     continue;
    // }

    if (powerOnly == 1 && path[i].net > 5) {
      continue;
      }
    if (powerOnly == 1 && path[i].duplicate == 1) {
      continue;
      }

    switch (path[i].pathType) {
      case BBtoBB: {
      // Serial.print("BBtoBB\t");
      int freeLane = -1;
      int xMapL0c0 = xMapForChipLane0(path[i].chip[0], path[i].chip[1]);
      int xMapL1c0 = xMapForChipLane1(path[i].chip[0], path[i].chip[1]);

      int xMapL0c1 = xMapForChipLane0(path[i].chip[1], path[i].chip[0]);
      int xMapL1c1 = xMapForChipLane1(path[i].chip[1], path[i].chip[0]);

      if (path[i].chip[0] ==
          path[i].chip[1]) { // && (ch[path[i].chip[0]].yStatus[0] == -1)) {
        // if (path[i].sameChip == true ){
        //  Serial.print("same chip  ");
        path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
        path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[0]);
        ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
        ch[path[i].chip[0]].yStatus[path[i].y[1]] = path[i].net;
        path[i].x[0] = -2;
        path[i].x[1] = -2;

        if (debugNTCC == true) {
          Serial.print("path [");
          Serial.print(i);
          Serial.print("]  ");

          Serial.print(" \tchip[0]: ");
          Serial.print(chipNumToChar(path[i].chip[0]));

          Serial.print("  x[0]: ");
          Serial.print(path[i].x[0]);

          Serial.print("  y[0]: ");
          Serial.print(path[i].y[0]);

          Serial.print("\t  chip[1]: ");
          Serial.print(chipNumToChar(path[i].chip[1]));

          Serial.print("  x[1]: ");
          Serial.print(path[i].x[1]);

          Serial.print("  y[1]: ");
          Serial.print(path[i].y[1]);
          }

        break;
        }
      if (debugNTCC2 == true) {
        Serial.print("xMapL0c0: ");
        Serial.println(xMapL0c0);
        Serial.print("xMapL0c1: ");

        Serial.println(xMapL0c1);
        Serial.print("xMapL1c0: ");
        Serial.println(xMapL1c0);
        Serial.print("xMapL1c1: ");
        Serial.println(xMapL1c1);
        }
      // changed to not stack paths for redundant connections
      // if ((xMapL1c0 != -1) &&
      //     ch[path[i].chip[0]].xStatus[xMapL1c0] ==
      //         path[i].net) // check if lane 1 shares a net first so it
      //         should
      //                      // prefer sharing lanes
      // {
      //   freeLane = 1;
      // } else

      if (freeOrSameNetX(path[i].chip[0], xMapL0c0, path[i].net,
                         allowStacking) == true) {
        freeLane = 0;
        } else if ((xMapL1c0 != -1) &&
                   freeOrSameNetX(path[i].chip[0], xMapL1c0, path[i].net,
                                  allowStacking) ==
                       true) // || ch[path[i].chip[0]].xStatus[xMapL1c0] ==
          // path[i].net))
          {
          freeLane = 1;
          } else {
          path[i].altPathNeeded = true;

          if (debugNTCC2 == true) {
            Serial.print("\tno free lanes for path, setting altPathNeeded flag");
            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[0]].xStatus[xMapL0c0]);
            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[0]].xStatus[xMapL1c0]);
            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[1]].xStatus[xMapL0c1]);
            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[1]].xStatus[xMapL1c1]);
            Serial.println(" \t ");
            }
          break;
          }

        if (freeLane == 0) {
          ch[path[i].chip[0]].xStatus[xMapL0c0] = path[i].net;
          ch[path[i].chip[1]].xStatus[xMapL0c1] = path[i].net;
          path[i].x[0] = xMapL0c0;
          path[i].x[1] = xMapL0c1;
          } else if (freeLane == 1) {
            ch[path[i].chip[0]].xStatus[xMapL1c0] = path[i].net;
            ch[path[i].chip[1]].xStatus[xMapL1c1] = path[i].net;

            path[i].x[0] = xMapL1c0;
            path[i].x[1] = xMapL1c1;
            }

          path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
          path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[1]);
          ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
          ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;

          if (debugNTCC2 == true) {
            Serial.print(" \tchip[0]: ");
            Serial.print(chipNumToChar(path[i].chip[0]));

            Serial.print("  x[0]: ");
            Serial.print(path[i].x[0]);

            Serial.print("  y[0]: ");
            Serial.print(path[i].y[0]);

            Serial.print("\t  chip[1]: ");
            Serial.print(chipNumToChar(path[i].chip[1]));

            Serial.print("  x[1]: ");
            Serial.print(path[i].x[1]);

            Serial.print("  y[1]: ");
            Serial.print(path[i].y[1]);

            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[0]].xStatus[xMapL0c0]);

            Serial.print(" \t ");
            Serial.print(ch[path[i].chip[1]].xStatus[xMapL0c1]);
            Serial.print(" \t ");
            }
          break;
      }

      case BBtoNANO:
      case BBtoSF: // nodes should always be in order of the enum, so node1 is
        // BB and node2 is SF
      {
      int xMapBBc0 = xMapForChipLane0(
          path[i].chip[0], path[i].chip[1]); // find x connection to sf chip

      int xMapSFc1 = xMapForNode(path[i].node2, path[i].chip[1]);
      int yMapSFc1 = path[i].chip[0];

      // if ((ch[path[i].chip[0]].xStatus[xMapBBc0] == path[i].net ||
      //      ch[path[i].chip[0]].xStatus[xMapBBc0] == -1) &&
      //     (ch[path[i].chip[1]].yStatus[yMapSFc1] == path[i].net ||
      //      ch[path[i].chip[1]].yStatus[yMapSFc1] ==
      //          -1)) // how's that for a fuckin if statement
      // {

      if (freeOrSameNetX(path[i].chip[0], xMapBBc0, path[i].net,
                         allowStacking) == true &&
          freeOrSameNetY(path[i].chip[1], yMapSFc1, path[i].net,
                         allowStacking) == true) {
        path[i].x[0] = xMapBBc0;
        path[i].x[1] = xMapSFc1;

        path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);

        path[i].y[1] =
          path[i].chip[0]; // bb to sf connections are always in chip order,
        // so chip A is always connected to sf y 0

        ch[path[i].chip[0]].xStatus[xMapBBc0] = path[i].net;
        ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;

        ch[path[i].chip[1]].xStatus[path[i].x[1]] = path[i].net;

        ch[path[i].chip[1]].yStatus[path[i].chip[0]] = path[i].net;

        if (debugNTCC2 == true) {
          // delay(10);

          Serial.print(" \t\n\rchip[0]: ");
          Serial.print(chipNumToChar(path[i].chip[0]));

          Serial.print("  x[0]: ");
          Serial.print(path[i].x[0]);

          Serial.print("  y[0]: ");
          Serial.print(path[i].y[0]);

          Serial.print("\t  chip[1]: ");
          Serial.print(chipNumToChar(path[i].chip[1]));

          Serial.print("  x[1]: ");
          Serial.print(path[i].x[1]);

          Serial.print("  y[1]: ");
          Serial.print(path[i].y[1]);

          Serial.print(" \t ");
          Serial.print(ch[path[i].chip[0]].xStatus[xMapBBc0]);

          Serial.print(" \t ");
          Serial.print(ch[path[i].chip[1]].xStatus[xMapSFc1]);
          Serial.print(" \t ");

          Serial.println("  ");
          }
        } else {
        path[i].altPathNeeded = true;

        if (debugNTCC2) {
          // delay(10);
          Serial.print("\tno direct path, setting altPathNeeded flag (BBtoSF)");
          }
        break;
        }
      break;
      }
      case NANOtoSF:
      case NANOtoNANO: {
      // Serial.print(" NANOtoNANO  ");
      int xMapNANOC0 = xMapForNode(path[i].node1, path[i].chip[0]);
      int xMapNANOC1 = xMapForNode(path[i].node2, path[i].chip[1]);

      if (path[i].chip[0] == path[i].chip[1]) {
        if (freeOrSameNetX(path[i].chip[0], xMapNANOC0, path[i].net,
                           allowStacking) == true) {
          if (freeOrSameNetX(path[i].chip[0], xMapNANOC1, path[i].net,
                             allowStacking) == true) {
            ch[path[i].chip[0]].xStatus[xMapNANOC0] = path[i].net;
            ch[path[i].chip[1]].xStatus[xMapNANOC1] = path[i].net;

            path[i].x[0] = xMapNANOC0;
            path[i].x[1] = xMapNANOC1;

            path[i].y[0] = -2;
            path[i].y[1] = -2;

            path[i].sameChip = true;
            // Serial.print(" ?????????");
            if (debugNTCC2) {
              Serial.print(" \t\t\tchip[0]: ");
              Serial.print(chipNumToChar(path[i].chip[0]));

              Serial.print("  x[0]: ");
              Serial.print(path[i].x[0]);

              Serial.print("  y[0]: ");
              Serial.print(path[i].y[0]);

              Serial.print("\t  chip[1]: ");
              Serial.print(chipNumToChar(path[i].chip[1]));

              Serial.print("  x[1]: ");
              Serial.print(path[i].x[1]);

              Serial.print("  y[1]: ");
              Serial.print(path[i].y[1]);
              }
            }
          }
        } else {
        path[i].altPathNeeded = true;
        if (debugNTCC2) {
          Serial.print(
              "\n\rno direct path, setting altPathNeeded flag (NANOtoNANO)");
          Serial.print(" \t ");
          Serial.println(i);
          printPathsCompact();
          }
        }
      }
                     // case BBtoNANO:
      }
    // if (debugNTCC2)
    // {
    //     Serial.println("\n\r");
    // }
    }
  // duplicateSFnets();
  //    printPathsCompact();
  //     printChipStatus();

  // duplicateSFnets();
  }

void duplicateSFnets(void) {
  // if (debugNTCC2)
  // {
  //     Serial.println("duplicateSFnets()");
  // }
  for (int i = 0; i < 26; i++) {
    if (ch[duplucateSFnodes[i][0]].xStatus[duplucateSFnodes[i][1]] > 0) {
      if (ch[duplucateSFnodes[i][2]].xStatus[duplucateSFnodes[i][3]] == -1) {
        ch[duplucateSFnodes[i][2]].xStatus[duplucateSFnodes[i][3]] =
          ch[duplucateSFnodes[i][0]].xStatus[duplucateSFnodes[i][1]];
        }
      }

    if (ch[duplucateSFnodes[i][2]].xStatus[duplucateSFnodes[i][3]] > 0) {
      if (ch[duplucateSFnodes[i][0]].xStatus[duplucateSFnodes[i][1]] == -1) {
        ch[duplucateSFnodes[i][0]].xStatus[duplucateSFnodes[i][1]] =
          ch[duplucateSFnodes[i][2]].xStatus[duplucateSFnodes[i][3]];
        }
      }
    }
  }

int ijklPaths(int pathNumber, int allowStacking) {
  // return 0;
  int chip0 = path[pathNumber].chip[0];
  int chip1 = path[pathNumber].chip[1];
  // int chip2 = path[pathNumber].chip[2];
  // int chip3 = path[pathNumber].chip[3];

  if (chip0 == chip1) {
    return 0;
    }
  if (chip0 < 8 || chip1 < 8) { // allow it to find a hop here
    return 0;
    }

  int x0 = -1;
  int x1 = -1;
  //  printPathsCompact();
  //  printChipStatus();
  for (int i = 12; i < 15; i++) {
    if (ch[chip0].xMap[i] == chip1) {
      x0 = i;
      }
    if (ch[chip1].xMap[i] == chip0) {
      x1 = i;
      }
    }
  // if ((ch[chip0].xStatus[x0] == -1 ||
  //      ch[chip0].xStatus[x0] == path[pathNumber].net) &&
  //     (ch[chip1].xStatus[x1] == -1 ||
  //      ch[chip1].xStatus[x1] == path[pathNumber].net)) {

  if (freeOrSameNetX(chip0, x0, path[pathNumber].net, allowStacking) == true &&
      freeOrSameNetX(chip1, x1, path[pathNumber].net, allowStacking) == true) {
    ch[chip0].xStatus[x0] = path[pathNumber].net;
    ch[chip1].xStatus[x1] = path[pathNumber].net;
    path[pathNumber].x[0] = xMapForNode(path[pathNumber].node1, chip0);
    path[pathNumber].x[1] = xMapForNode(path[pathNumber].node2, chip1);

    if (path[pathNumber].x[0] != -1 && path[pathNumber].x[1] != -1) {
      ch[chip0].xStatus[path[pathNumber].x[0]] = path[pathNumber].net;
      ch[chip1].xStatus[path[pathNumber].x[1]] = path[pathNumber].net;
      }

    ch[path[pathNumber].chip[0]].uncommittedHops++;
    ch[path[pathNumber].chip[1]].uncommittedHops++;
    path[pathNumber].y[0] = -2;
    path[pathNumber].y[1] = -2;
    path[pathNumber].sameChip = true;
    path[pathNumber].altPathNeeded = false;

    path[pathNumber].chip[2] = chip0;
    path[pathNumber].chip[3] = chip1;
    path[pathNumber].x[2] = x0;
    path[pathNumber].x[3] = x1;

    path[pathNumber].y[2] = -2;
    path[pathNumber].y[3] = -2;
    //  printPathsCompact();
    //  printChipStatus();
    // Serial.print("pathNumber: ");
    // Serial.println(pathNumber);
    return 1;
    } else {
    return 0;
    }
  }

void resolveAltPaths(int allowStacking, int powerOnly) {
  // if (debugNTCC5) {
  //  Serial.println("\n\rresolveAltPaths()");
  //  Serial.println(" ");
  //  Serial.print("numberOfPaths: ");
  //  Serial.println(numberOfPaths);
  //}
  // return;
  int couldFindPath = -1;

  for (int i = 0; i <= numberOfPaths; i++) {
    if (powerOnly == 1 && path[i].net > 5) {
      continue;
      }
    if (powerOnly == 1 && path[i].duplicate == 1) {
      continue;
      }

    if (path[i].altPathNeeded == true) {
      //   Serial.print("\n\n\rPATH: ");
      //   Serial.println(i);
      // Serial.print("path[i].altPathNeeded = ");
      // Serial.println(path[i].altPathNeeded);
      // Serial.print("numberOfPaths = ");
      // Serial.println(numberOfPaths);

      if (ijklPaths(i) == 1) {
        continue;
        }

      switch (path[i].pathType) {
        case BBtoBB: {
        int foundPath = 0;
        if (debugNTCC2) {
          Serial.println("BBtoBB");
          }

        int yNode1 = yMapForNode(path[i].node1, path[i].chip[0]);
        int yNode2 = yMapForNode(path[i].node2, path[i].chip[1]);

        ch[path[i].chip[0]].yStatus[yNode1] = path[i].net;
        ch[path[i].chip[1]].yStatus[yNode2] = path[i].net;

        if (foundPath == 1) {
          couldFindPath = i;
          break;
          }
        int giveUpOnL = 0;
        int swapped = 0;

        int chipsWithFreeY0[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
        int numberOfChipsWithFreeY0 = 0;

        for (int chipFreeY = 0; chipFreeY < 8; chipFreeY++) {
          // if (ch[chipFreeY].yStatus[0] == -1 ||
          //     ch[chipFreeY].yStatus[0] == path[i].net) {

          if (freeOrSameNetY(chipFreeY, 0, path[i].net, allowStacking) ==
              true) {
            chipsWithFreeY0[chipFreeY] = chipFreeY;
            numberOfChipsWithFreeY0++;
            }
          }

        for (int chipFreeY = 0; chipFreeY < 8; chipFreeY++) {
          if (debugNTCC2) {
            Serial.print("\n\r");
            Serial.print("path: ");
            Serial.print(i);
            Serial.print("\tindex: ");
            Serial.print(chipFreeY);
            Serial.print("  chip: ");
            printChipNumToChar(chipsWithFreeY0[chipFreeY]);
            Serial.print("\n\r");
            }
          }

        for (int bb = 0; bb < 8; bb++) // this is a long winded way to do this
          // but it's at least slightly readable
          {
          if (chipsWithFreeY0[bb] == -1) {
            // Serial.print("chipsWithFreeY0[");
            // Serial.print(bb);
            // Serial.print("] == -1\n\r");
            continue;
            }

          int xMapL0c0 = xMapForChipLane0(bb, path[i].chip[0]);
          int xMapL0c1 = xMapForChipLane0(bb, path[i].chip[1]);

          int xMapL1c0 = xMapForChipLane1(bb, path[i].chip[0]);
          int xMapL1c1 = xMapForChipLane1(bb, path[i].chip[1]);

          // if (bb == 7 && giveUpOnL == 0 && swapped == 0) {
          //   bb = 0;
          //   giveUpOnL = 0;
          //   swapped = 1;
          //   // Serial.println("\t\t\tt\t\t\tt\t\tswapped");
          //   // swapDuplicateNode(i);
          // } else if (bb == 7 && giveUpOnL == 0 && swapped == 1) {
          //   bb = 0;
          //   giveUpOnL = 1;
          // }

          //   if ((ch[CHIP_L].yStatus[bb] != -1 &&
          //        ch[CHIP_L].yStatus[bb] != path[i].net) &&
          //       giveUpOnL == 0) {

          //     continue;
          //   }

          if (path[i].chip[0] == bb || path[i].chip[1] == bb) {
            continue;
            }

          // if (path[i].chip[0] == path[i].chip[1]) {
          //   continue;
          // }

          // if (ch[bb].xStatus[] == path[i].net ||
          //     ch[bb].xStatus[xMapL0c0] == -1)

          if (freeOrSameNetX(bb, xMapL0c0, path[i].net, allowStacking) ==
              true) {
            // were going through each bb chip to see if it has a
            // connection to both chips free

            // if (ch[bb].xStatus[xMapL0c1] == path[i].net ||
            //     ch[bb].xStatus[xMapL0c1] == -1) // lanes 0 0
            if (freeOrSameNetX(bb, xMapL0c1, path[i].net, allowStacking) ==
                true) {
              // if (ch[bb].yStatus[0] == -1) {
              ch[bb].xStatus[xMapL0c0] = path[i].net;
              ch[bb].xStatus[xMapL0c1] = path[i].net;

              //   if (giveUpOnL == 0) {
              //     ch[CHIP_L].yStatus[bb] = path[i].net;
              //     ch[bb].yStatus[0] = path[i].net;
              //     path[i].y[2] = 0;
              //     path[i].y[3] = 0;
              //   } else {
              // if (debugNTCC2) {
              //     Serial.println("Gave up on L  ");
              // Serial.println(bb);
              //  // }

              path[i].y[2] = 0;
              path[i].y[3] = 0;
              ch[bb].yStatus[0] = path[i].net;
              //}

              path[i].sameChip = true;

              path[i].chip[2] = bb;
              path[i].chip[3] = bb;
              path[i].x[2] = xMapL0c0;
              path[i].x[3] = xMapL0c1;

              ch[bb].xStatus[xMapL0c0] = path[i].net;
              ch[bb].xStatus[xMapL0c1] = path[i].net;

              // Serial.print("!!!!3 bb: ");
              // Serial.println(bb);
              // Serial.print("chip[3]: ");
              // Serial.println(path[i].chip[3]);

              path[i].altPathNeeded = false;

              path[i].x[0] = xMapForChipLane0(path[i].chip[0], bb);
              path[i].x[1] = xMapForChipLane0(path[i].chip[1], bb);

              ch[path[i].chip[0]]
                .xStatus[xMapForChipLane0(path[i].chip[0], bb)] = path[i].net;
              ch[path[i].chip[1]]
                .xStatus[xMapForChipLane0(path[i].chip[1], bb)] = path[i].net;

              path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
              path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[1]);

              ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
              ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;

              if (debugNTCC2) {
                Serial.print("\n\r");
                Serial.print(i);
                Serial.print("  chip[2]: ");
                Serial.print(chipNumToChar(path[i].chip[2]));

                Serial.print("  x[2]: ");
                Serial.print(path[i].x[2]);

                Serial.print("  x[3]: ");
                Serial.print(path[i].x[3]);

                Serial.print(" \n\r");
                }
              // printPathsCompact();
              //}
              // continue;
              break;
              }
            }
          if (freeOrSameNetX(bb, xMapL1c0, path[i].net, allowStacking) ==
              true) {
            if (freeOrSameNetX(bb, xMapL1c1, path[i].net, allowStacking) ==
                true) {
              ch[bb].xStatus[xMapL1c0] = path[i].net;
              ch[bb].xStatus[xMapL1c1] = path[i].net;

              //   if (giveUpOnL == 0) {
              //     // Serial.print("Give up on L?");
              //     ch[CHIP_L].yStatus[bb] = path[i].net;
              //     ch[bb].yStatus[0] = path[i].net;
              //     path[i].y[2] = 0;
              //     path[i].y[3] = 0;
              //   } else {
              // if (debugNTCC2) {
              // Serial.println("Gave up on L");
              //}
              path[i].y[2] = 0;
              path[i].y[3] = 0;
              ch[bb].yStatus[0] = path[i].net;
              /// }

              path[i].chip[2] = bb;
              path[i].chip[3] = bb;
              path[i].x[2] = xMapL1c0;
              path[i].x[3] = xMapL1c1;
              path[i].sameChip = true;
              path[i].altPathNeeded = false;

              ch[bb].xStatus[xMapL1c0] = path[i].net;
              ch[bb].xStatus[xMapL1c1] = path[i].net;

              path[i].x[0] = xMapForChipLane1(path[i].chip[0], bb);
              path[i].x[1] = xMapForChipLane1(path[i].chip[1], bb);

              ch[path[i].chip[0]]
                .xStatus[xMapForChipLane1(path[i].chip[0], bb)] = path[i].net;
              ch[path[i].chip[1]]
                .xStatus[xMapForChipLane1(path[i].chip[1], bb)] = path[i].net;

              path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
              path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[1]);

              ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
              ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;

              if (debugNTCC2) {
                Serial.print("\n\r");
                Serial.print(i);
                Serial.print("  chip[2]: ");
                Serial.print(chipNumToChar(path[i].chip[2]));

                Serial.print("  x[2]: ");
                Serial.print(path[i].x[2]);

                Serial.print("  x[3]: ");
                Serial.print(path[i].x[3]);

                Serial.print(" \n\r");
                }
              // continue;
              break;
              }
            }
          if (freeOrSameNetX(bb, xMapL0c0, path[i].net, allowStacking) ==
                  true &&
              false) {
            if (freeOrSameNetX(bb, xMapL1c1, path[i].net, allowStacking) ==
                true) {
              //   if (giveUpOnL == 0) {
              //     ch[CHIP_L].yStatus[bb] = path[i].net;
              //     ch[bb].yStatus[0] = path[i].net;
              //     path[i].y[2] = 0;
              //     path[i].y[3] = 0;
              //   } else {
              if (debugNTCC2) {
                Serial.println("Gave up on L");
                }
              path[i].y[2] = 0;
              path[i].y[3] = 0;
              ch[bb].yStatus[0] = path[i].net;
              //}

              ch[bb].xStatus[xMapL0c0] = path[i].net;
              ch[bb].xStatus[xMapL1c1] = path[i].net;

              path[i].chip[2] = bb;
              path[i].chip[3] = bb;
              path[i].x[2] = xMapL0c0;
              path[i].x[3] = xMapL1c1;

              path[i].altPathNeeded = false;

              path[i].x[0] = xMapForChipLane0(path[i].chip[0], bb);
              path[i].x[1] = xMapForChipLane1(path[i].chip[1], bb);

              ch[path[i].chip[0]]
                .xStatus[xMapForChipLane0(path[i].chip[0], bb)] = path[i].net;
              ch[path[i].chip[1]]
                .xStatus[xMapForChipLane1(path[i].chip[1], bb)] = path[i].net;

              path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
              path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[1]);

              ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
              ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;

              path[i].sameChip = true;
              if (debugNTCC2) {
                Serial.print("\n\r");
                Serial.print(i);
                Serial.print("  chip[2]: ");
                Serial.print(chipNumToChar(path[i].chip[2]));

                Serial.print("  x[2]: ");
                Serial.print(path[i].x[2]);

                Serial.print("  x[3]: ");
                Serial.print(path[i].x[3]);

                Serial.print(" \n\r");
                }
              couldFindPath = i;
              // continue;
              break;
              }
            }
          if (freeOrSameNetX(bb, xMapL1c0, path[i].net, allowStacking) ==
                  true &&
              false) {
            if (freeOrSameNetX(bb, xMapL0c1, path[i].net, allowStacking) ==
                true) {
              //   if (giveUpOnL == 0) {
              //     ch[CHIP_L].yStatus[bb] = path[i].net;
              //     ch[bb].yStatus[0] = path[i].net;
              //     path[i].y[2] = 0;
              //     path[i].y[3] = 0;
              //   } else {
              //     if (debugNTCC2) {
              //       Serial.println("Gave up on L");
              //     }
              path[i].y[2] = 0;
              path[i].y[3] = 0;
              ch[bb].yStatus[0] = path[i].net;
              //}

              ch[bb].xStatus[xMapL0c1] = path[i].net;
              ch[bb].xStatus[xMapL1c0] = path[i].net;

              path[i].chip[2] = bb;
              path[i].chip[3] = bb;
              path[i].x[2] = xMapL0c1;
              path[i].x[3] = xMapL1c0;
              // path[i].y[2] = -2;
              // path[i].y[3] = -2;
              path[i].altPathNeeded = false;
              path[i].sameChip = true;
              path[i].x[0] = xMapForChipLane1(path[i].chip[0], bb);
              path[i].x[1] = xMapForChipLane0(path[i].chip[1], bb);

              ch[path[i].chip[0]]
                .xStatus[xMapForChipLane1(path[i].chip[0], bb)] = path[i].net;
              ch[path[i].chip[1]]
                .xStatus[xMapForChipLane0(path[i].chip[1], bb)] = path[i].net;

              path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
              path[i].y[1] = yMapForNode(path[i].node2, path[i].chip[1]);

              ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;
              ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;

              if (debugNTCC2) {
                Serial.print("\n\r");
                Serial.print(i);
                Serial.print(" chip[2]: ");
                Serial.print(chipNumToChar(path[i].chip[2]));

                Serial.print("  x[2]: ");
                Serial.print(path[i].x[2]);

                Serial.print("  x[3]: ");
                Serial.print(path[i].x[3]);

                Serial.print(" \n\r");
                }
              couldFindPath = i;
              // continue;
              break;
              }

            if (debugNTCC2) {
              Serial.print("\n\r");
              Serial.print(i);
              Serial.print("  chip[2]: ");
              Serial.print(chipNumToChar(path[i].chip[2]));

              Serial.print("  x[2]: ");
              Serial.print(path[i].x[2]);

              Serial.print("  x[3]: ");
              Serial.print(path[i].x[3]);

              Serial.print(" \n\r");
              }
            }
          //}
          }
        // continue;
        break;
        }

        case NANOtoSF:
        case NANOtoNANO: {
        if (debugNTCC2) {
          Serial.println("   NANOtoNANO");
          }
        int foundHop = 0;
        int giveUpOnL = 0;
        int swapped = 0;
        // duplicateSFnets();
        ////printPathsCompact();
        // printChipStatus();
        giveUpOnL = 0;

        // Serial.print(i);
        // Serial.println("   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        for (int bb = 0; bb < 8; bb++) // this is a long winded way to do this
          // but it's at least slightly readable
          {
          int sfChip1 = path[i].chip[0];
          int sfChip2 = path[i].chip[1];

          int chip1Lane = xMapForNode(sfChip1, bb);
          int chip2Lane = xMapForNode(sfChip2, bb);

          if (bb == 7 && giveUpOnL == 0 && swapped == 0) {
            bb = 0;
            giveUpOnL = 0;
            swapped = 1;
            // swapDuplicateNode(i);
            } else if (bb == 7 && giveUpOnL == 0 && swapped == 1) {
              // bb = 0;
              giveUpOnL = 1;
              }

            // if ((ch[CHIP_L].yStatus[bb] != -1 &&
            //      ch[CHIP_L].yStatus[bb] != path[i].net) &&
            //     giveUpOnL == 0) {

            //   continue;
            // }
            // if ((ch[path[i].chip[0]].yStatus[bb] != -1 &&
            //      ch[path[i].chip[0]].yStatus[bb] != path[i].net) ||
            //     (ch[path[i].chip[1]].yStatus[bb] != -1 &&
            //      ch[path[i].chip[1]].yStatus[bb] != path[i].net) ||
            //     (ch[bb].xStatus[chip1Lane] != -1 &&
            //      ch[bb].xStatus[chip1Lane] != path[i].net) ||
            //     (ch[bb].xStatus[chip2Lane] != -1 &&
            //      ch[bb].xStatus[chip2Lane] != path[i].net) ||
            //     (ch[bb].yStatus[0] != -1 && ch[bb].yStatus[0] !=
            //     path[i].net))
            //     {

            if (freeOrSameNetY(path[i].chip[0], bb, path[i].net, allowStacking) ==
                    false ||
                freeOrSameNetY(path[i].chip[1], bb, path[i].net, allowStacking) ==
                    false ||
                freeOrSameNetX(bb, chip1Lane, path[i].net, allowStacking) ==
                    false ||
                freeOrSameNetX(bb, chip2Lane, path[i].net, allowStacking) ==
                    false ||
                freeOrSameNetY(bb, 0, path[i].net, allowStacking) == false) {
              //                 Serial.print("bb:\t");
              // Serial.println(bb);
              // Serial.print("xStatus:\t");
              // Serial.println(ch[bb].xStatus[chip1Lane]);
              // Serial.print("xStatus:\t");
              // Serial.println(ch[bb].xStatus[chip2Lane]);
              // Serial.println(" ");
              // Serial.print("path: ");
              // Serial.println(i);
              //    printPathsCompact();
              //   printChipStatus();
              //   Serial.print("!!!!!!!!!!!!!!!!!!!!!!!!\n\r");
              // continue;
              } else {
              // if (ch[bb].xStatus[chip1Lane] == path[i].net ||
              //     ch[bb].xStatus[chip1Lane] == -1) {
              if (freeOrSameNetX(bb, chip1Lane, path[i].net, allowStacking) ==
                  true) {
                // if (ch[bb].xStatus[chip2Lane] == path[i].net ||
                //     ch[bb].xStatus[chip2Lane] == -1) {
                if (freeOrSameNetX(bb, chip2Lane, path[i].net, allowStacking) ==
                    true) {
                  // printPathsCompact();
                  // printChipStatus();

                  if (giveUpOnL == 1) {
                    if (debugNTCC2) {
                      // Serial.println("Gave up on L");
                      // Serial.print("path :");
                      // Serial.println(i);
                      }
                    continue;
                    // break;
                    }

                  path[i].sameChip = true;

                  ch[bb].xStatus[chip1Lane] = path[i].net;
                  ch[bb].xStatus[chip2Lane] = path[i].net;

                  if (path[i].chip[0] != path[i].chip[1]) {
                    //                 Serial.println("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV");
                    //                 int pathAddress = (int)&path[i];
                    // Serial.print(pathAddress);
                    // Serial.print("\tpath: ");
                    // Serial.println(i);
                    // Serial.print("bb:\t");
                    // Serial.print(bb);

                    path[i].chip[2] = bb;
                    path[i].chip[3] = bb;
                    path[i].y[2] = 0;
                    path[i].y[3] = 0;
                    ch[bb].yStatus[0] = path[i].net;

                    path[i].x[2] = chip1Lane;
                    path[i].x[3] = chip2Lane;
                    }

                  path[i].altPathNeeded = false;

                  path[i].x[0] = xMapForNode(path[i].node1, path[i].chip[0]);
                  path[i].x[1] = xMapForNode(path[i].node2, path[i].chip[1]);
                  ch[path[i].chip[0]]
                    .xStatus[xMapForNode(path[i].node1, path[i].chip[0])] =
                    path[i].net;
                  ch[path[i].chip[1]]
                    .xStatus[xMapForNode(path[i].node2, path[i].chip[1])] =
                    path[i].net;

                  path[i].y[0] = bb;
                  path[i].y[1] = bb;

                  //            Serial.print(">>>> path ");
                  // Serial.println(i);

                  ch[path[i].chip[0]].yStatus[bb] = path[i].net;
                  ch[path[i].chip[1]].yStatus[bb] = path[i].net;

                  if (debugNTCC2) {
                    Serial.println("\n\r");
                    Serial.print(i);
                    Serial.print("  chip[2]: ");
                    Serial.print(chipNumToChar(path[i].chip[2]));

                    Serial.print("  y[2]: ");
                    Serial.print(path[i].y[2]);

                    Serial.print("  y[3]: ");
                    Serial.print(path[i].y[3]);

                    Serial.println(" \n\r");
                    }

                  foundHop = 1;
                  couldFindPath = i;
                  // printPathsCompact();
                  // printChipStatus();
                  // Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
                  //  printPathsCompact();
                  //  printChipStatus();

                  // Serial.print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                  // continue;
                  break;
                  }
                }
              }
          }

        for (int bb = 0; bb < 8;
             bb++) // this will connect to a random breadboard row, add a
          // test to make sure nothing is connected
          {
          int sfChip1 = path[i].chip[0];
          int sfChip2 = path[i].chip[1];

          int chip1Lane = xMapForNode(sfChip1, bb);
          int chip2Lane = xMapForNode(sfChip2, bb);

          // Serial.print("bb:\t");
          // Serial.println(bb);
          // Serial.print("xStatus:\t");
          // Serial.println(ch[bb].xStatus[chip1Lane]);
          // Serial.print("xStatus:\t");
          // Serial.println(ch[bb].xStatus[chip2Lane]);
          // Serial.println(" ");
          // Serial.print("path: ");
          // Serial.println(i);
          // if ((ch[path[i].chip[0]].yStatus[bb] != -1 &&
          //      ch[path[i].chip[0]].yStatus[bb] != path[i].net) ||
          //     (ch[path[i].chip[1]].yStatus[bb] != -1 &&
          //      ch[path[i].chip[1]].yStatus[bb] != path[i].net) ||
          //     (ch[bb].xStatus[chip1Lane] != -1 &&
          //      ch[bb].xStatus[chip1Lane] != path[i].net) ||
          //     (ch[bb].xStatus[chip2Lane] != -1 &&
          //      ch[bb].xStatus[chip2Lane] != path[i].net) ||
          //     (ch[bb].yStatus[0] != -1 && ch[bb].yStatus[0] !=
          //     path[i].net))
          //     {

          if (freeOrSameNetY(path[i].chip[0], bb, path[i].net, allowStacking) ==
                  true ||
              freeOrSameNetY(path[i].chip[1], bb, path[i].net, allowStacking) ==
                  true ||
              freeOrSameNetX(bb, chip1Lane, path[i].net, allowStacking) ==
                  true ||
              freeOrSameNetX(bb, chip2Lane, path[i].net, allowStacking) ==
                  true ||
              freeOrSameNetY(bb, 0, path[i].net, allowStacking) == true) {
            //                 Serial.print("bb:\t");
            // Serial.println(bb);
            // Serial.print("xStatus:\t");
            // Serial.println(ch[bb].xStatus[chip1Lane]);
            // Serial.print("xStatus:\t");
            // Serial.println(ch[bb].xStatus[chip2Lane]);
            // Serial.println(" ");
            // Serial.print("path: ");
            // Serial.println(i);
            //    printPathsCompact();
            //   printChipStatus();
            //   Serial.print("?????????????????????\n\r");
            //   // continue;
            } else {
            // Serial.print("?????????????????????\n\r");
            // if ((ch[bb].xStatus[chip1Lane] == path[i].net ||
            //      ch[bb].xStatus[chip1Lane] == -1) &&
            //     foundHop == 0) {
            if (freeOrSameNetX(bb, chip1Lane, path[i].net, allowStacking) ==
                    true &&
                foundHop == 0) {
              if (freeOrSameNetX(bb, chip2Lane, path[i].net, allowStacking) ==
                  true) {
                // Serial.print("path :");
                // Serial.println(i);
                //  printPathsCompact();
                ch[bb].xStatus[chip1Lane] = path[i].net;
                ch[bb].xStatus[chip2Lane] = path[i].net;
                ch[bb].yStatus[0] = path[i].net;
                if (path[i].chip[0] !=
                    path[i].chip[1]) // this makes it not try to find a third
                  // chip if it doesn't need to
                  {
                  path[i].chip[2] = bb;
                  path[i].x[2] = chip1Lane;
                  path[i].x[3] = chip2Lane;

                  path[i].y[2] = 0;
                  path[i].y[3] = 0;
                  }

                path[i].sameChip = true;
                path[i].altPathNeeded = false;

                path[i].x[0] = xMapForNode(path[i].node1, path[i].chip[0]);
                path[i].x[1] = xMapForNode(path[i].node2, path[i].chip[1]);
                ch[path[i].chip[0]]
                  .xStatus[xMapForNode(path[i].node1, path[i].chip[0])] =
                  path[i].net;
                ch[path[i].chip[1]]
                  .xStatus[xMapForNode(path[i].node2, path[i].chip[1])] =
                  path[i].net;
                // Serial.print(">>>> path ");
                // Serial.println(i);

                path[i].y[0] = bb;
                path[i].y[1] = bb;
                ch[path[i].chip[0]].yStatus[bb] = path[i].net;
                ch[path[i].chip[1]].yStatus[bb] = path[i].net;

                if (debugNTCC2) {
                  Serial.print("\n\r");
                  Serial.print(i);
                  Serial.print("  chip[2]: ");
                  Serial.print(chipNumToChar(path[i].chip[2]));
                  // Serial.print("  y[2]: ");

                  Serial.print("  y[2]: ");
                  Serial.print(path[i].y[2]);

                  Serial.print("  y[3]: ");
                  Serial.print(path[i].y[3]);

                  Serial.print(" \n\r");
                  }
                foundHop = 1;
                couldFindPath = i;
                // printPathsCompact();
                // printChipStatus();
                // continue;
                // printPathsCompact();
                // printChipStatus();
                break;
                }
              }
            }
          }
        // couldntFindPath(i);
        }
        case BBtoSF: {
        // Serial.print("path[");
        // Serial.print(i);
        // Serial.println("] ");

        // duplicateSFnets();
        if (path[i].pathType == BBtoSF ||
            path[i].pathType == BBtoNANO) // do bb to sf first because these
          // are hardest to find
          {
          int foundPath = 0;

          if (debugNTCC2) {
            Serial.print("\n\rBBtoSF\tpath: ");
            Serial.println(i);
            }

          int saveUncommittedHops = ch[path[i].chip[0]].uncommittedHops;
          int saveUncommittedHops1 = ch[path[i].chip[1]].uncommittedHops;
          // Serial.print("saveUncommittedHops1: ");
          // Serial.println(saveUncommittedHops1);

          // Serial.print("saveUncommittedHops: ");
          // Serial.println(saveUncommittedHops);

          for (int bb = 0; bb < (8 - saveUncommittedHops1);
               bb++) // check if any other chips have free paths to both the
            // sf chip and target chip
            {
            // tryAfterSwap:

            if (foundPath == 1) {
              if (debugNTCC2) {
                Serial.print("Found Path!\n\r");
                }
              couldFindPath = i;

              break;
              }

            int xMapBB = xMapForChipLane0(path[i].chip[0], bb);
            if (xMapBB == -1) {
              // Serial.print("xMapBB == -1");

              continue; // don't bother checking if there's no connection
              }
            // if (xMapForChipLane1(path[i].chip[0], bb) == -1)
            // {
            //     //Serial.print("xMapForChipLane1(path[i].chip[0], bb) !=
            //     -1");

            //     continue; // don't bother checking if there's no connection
            // }
            // Serial.print("           fuck         ");
            int yMapSF = bb; // always

            int sfChip = path[i].chip[1];

            // not chip L
            if (debugNTCC2) {
              Serial.print("\n\r");
              Serial.print("      bb: ");
              printChipNumToChar(bb);
              Serial.print("  \t  sfChip: ");
              printChipNumToChar(sfChip);
              Serial.print("  \t  xMapBB: ");
              Serial.print(xMapBB);
              Serial.print("  \t  yMapSF: ");
              Serial.print(yMapSF);
              Serial.print("  \t  xStatus: ");
              Serial.print(ch[bb].xStatus[xMapBB]);
              Serial.print("  \n\r");
              }

            if (freeOrSameNetX(bb, xMapBB, path[i].net, allowStacking) ==
                    true &&
                freeOrSameNetY(bb, 0, path[i].net, allowStacking) == true) {
              // were going through each bb chip to see if it has a
              // connection to both chips free

              int xMapL0c0 = xMapForChipLane0(path[i].chip[0], bb);
              int xMapL1c0 = xMapForChipLane1(path[i].chip[0], bb);

              int xMapL0c1 = xMapForChipLane0(bb, path[i].chip[0]);
              int xMapL1c1 = xMapForChipLane1(bb, path[i].chip[0]);

              if (debugNTCC2) {
                Serial.print("\n\r");
                Serial.print("      bb: ");
                printChipNumToChar(bb);
                Serial.print("  \t  sfChip: ");
                printChipNumToChar(sfChip);
                Serial.print("  \t  xMapBB: ");
                Serial.print(xMapBB);
                Serial.print("  \t  yMapSF: ");
                Serial.print(yMapSF);
                Serial.print("  \t  xStatus: ");
                Serial.print(ch[bb].xStatus[xMapBB]);
                Serial.print("  \n\r");

                Serial.print("xMapL0c0: ");
                Serial.print(xMapL0c0);
                Serial.print("  \txMapL1c0: ");

                Serial.print(xMapL0c1);
                Serial.print("  \txMapL1c1: ");

                Serial.print(xMapL1c0);
                Serial.print("  \txMapL0c1: ");
                Serial.print(xMapL1c1);
                Serial.print("\n\n\r");
                }
              int freeLane = -1;
              // Serial.print("\t");
              // Serial.print(bb);

              // if ((xMapL1c0 != -1) &&
              //     ch[path[i].chip[0]].xStatus[xMapL1c0] ==
              //         path[i].net) // check if lane 1 shares a net first so
              //         it
              //                      // should prefer sharing lanes
              // {
              //   freeLane = 1;
              //} else

              // if ((ch[path[i].chip[0]].xStatus[xMapL0c0] == -1) ||
              //            ch[path[i].chip[0]].xStatus[xMapL0c0] ==
              //                path[i].net) // lanes will alway be taken
              //                together,
              //                             // so only check chip 1
              // {
              if (freeOrSameNetX(path[i].chip[0], xMapL0c0, path[i].net,
                                 allowStacking) == true) {
                freeLane = 0;
                // } else if ((xMapL1c0 != -1) &&
                //            ((ch[path[i].chip[0]].xStatus[xMapL1c0] == -1)
                //            ||
                //             ch[path[i].chip[0]].xStatus[xMapL1c0] ==
                //             path[i].net)) {
                } else if (freeOrSameNetX(path[i].chip[0], xMapL1c0, path[i].net,
                                          allowStacking) == true) {
                  freeLane = 1;
                  } else {
                  continue;
                  }

                // if (ch[sfChip].yStatus[yMapSF] != -1 &&
                //     ch[sfChip].yStatus[yMapSF] != path[i].net) {
                if (freeOrSameNetY(sfChip, yMapSF, path[i].net, allowStacking) ==
                    false) {
                  continue;
                  }

                path[i].altPathNeeded = false;

                int SFnode = xMapForNode(path[i].node2, path[i].chip[1]);
                // Serial.print("\n\r\t\t\t\tSFnode: ");
                // Serial.println(SFnode);
                // Serial.print("\n\r\t\t\t\tFree Lane: ");
                // Serial.println(freeLane);

                if (freeLane == 0) {
                  path[i].chip[2] = bb;
                  path[i].chip[3] = bb;
                  ch[path[i].chip[0]].xStatus[xMapL0c0] = path[i].net;
                  ch[path[i].chip[1]].xStatus[SFnode] = path[i].net;

                  ch[path[i].chip[2]].xStatus[xMapL0c1] = path[i].net;
                  ch[path[i].chip[2]].xStatus[xMapBB] = path[i].net;

                  path[i].x[0] = xMapL0c0;
                  path[i].x[1] = SFnode;

                  path[i].x[2] = xMapL0c1;
                  // Serial.print("\n\r\t\t\t\txBB: ");
                  // Serial.println(bb);

                  xMapBB = xMapForChipLane0(path[i].chip[2], path[i].chip[1]);
                  // Serial.println(xMapBB);
                  path[i].chip[3] = path[i].chip[2];

                  path[i].x[3] = xMapBB;

                  path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
                  path[i].y[1] = yMapSF;
                  path[i].y[2] = 0;
                  path[i].y[3] = 0;
                  ch[bb].yStatus[0] = path[i].net;

                  ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;

                  ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;
                  ch[path[i].chip[2]].yStatus[0] = path[i].net;

                  path[i].sameChip = true;
                  } else if (freeLane == 1) {
                    path[i].chip[2] = bb;
                    path[i].chip[3] = bb;
                    ch[path[i].chip[0]].xStatus[xMapL1c0] = path[i].net;
                    ch[path[i].chip[1]].xStatus[SFnode] = path[i].net;

                    ch[path[i].chip[2]].xStatus[xMapL1c1] = path[i].net;
                    ch[path[i].chip[2]].xStatus[xMapBB] = path[i].net;

                    path[i].x[0] = xMapL1c0;
                    path[i].x[1] = SFnode;

                    path[i].x[2] = xMapL1c1;
                    xMapBB = xMapForChipLane0(path[i].chip[2], path[i].chip[1]);
                    path[i].x[3] = xMapBB;

                    path[i].chip[3] = path[i].chip[2];

                    path[i].y[0] = yMapForNode(path[i].node1, path[i].chip[0]);
                    path[i].y[1] = yMapSF;
                    path[i].y[2] = 0;
                    path[i].y[3] = 0;
                    ch[bb].yStatus[0] = path[i].net;

                    ch[path[i].chip[0]].yStatus[path[i].y[0]] = path[i].net;

                    ch[path[i].chip[1]].yStatus[path[i].y[1]] = path[i].net;
                    ch[path[i].chip[2]].yStatus[0] = path[i].net;
                    }

                  foundPath = 1;
                  couldFindPath = i;

                  if (debugNTCC2 == true) {
                    Serial.print("\n\rFreelane = ");
                    Serial.print(freeLane);
                    Serial.print("\t path: ");
                    Serial.print(i);
                    Serial.print(" \n\rchip[0]: ");
                    Serial.print(chipNumToChar(path[i].chip[0]));

                    Serial.print("  x[0]: ");
                    Serial.print(path[i].x[0]);

                    Serial.print("  y[0]: ");
                    Serial.print(path[i].y[0]);

                    Serial.print("\t  chip[1]: ");
                    Serial.print(chipNumToChar(path[i].chip[1]));

                    Serial.print("  x[1]: ");
                    Serial.print(path[i].x[1]);

                    Serial.print("  y[1]: ");
                    Serial.print(path[i].y[1]);

                    Serial.print("   ch[path[i].chip[0]].xStatus[");
                    Serial.print(xMapL0c0);
                    Serial.print("]: ");
                    Serial.print(ch[path[i].chip[0]].xStatus[xMapL0c0]);

                    Serial.print("   ch[path[i].chip[1]].xStatus[ ");
                    Serial.print(xMapL0c1);
                    Serial.print("]: ");
                    Serial.print(ch[path[i].chip[1]].xStatus[xMapL0c1]);
                    Serial.print(" \t\n\r");
                    // printChipStatus();
                    }
                  // break;
              }

            // if (foundPath == 0 && swapped == 0 && bb == 7) {
            //   //swapped = 1;
            //   // if (debugNTCC2 == true)
            //   //  Serial.print("\n\rtrying again with swapped nodes\n\r");

            //   // path[i].x[0] = xMapForNode(path[i].node2,
            //   path[i].chip[0]);
            //   // swapDuplicateNode(i);
            //   bb = 0;
            //   // goto tryAfterSwap;
            // }
            }
          }
        break;
        }
        }

      // break;
      }
    }
  }

bool freeOrSameNetX(int chip, int x, int net, int allowStacking) {
  if (ch[chip].xStatus[x] == -1 ||
      (ch[chip].xStatus[x] == net && allowStacking == 1)) {
    return true;
    } else {
    return false;
    }
  }

bool freeOrSameNetY(int chip, int y, int net, int allowStacking) {
  if (ch[chip].yStatus[y] == -1 ||
      (ch[chip].yStatus[y] == net && allowStacking == 1)) {
    return true;
    } else {
    return false;
    }
  }

bool frontEnd(int chip, int y, int x) { // is this an externally facing node
  if (chip < 8) {
    if (y == 0) // bounce node
      {
      return false;
      } else {
      return true;
      }
    }
  if (chip >= 8) {
    if (x >= 12 && x <= 14) // ijkl
      return false;
    } else {
    return true;
    }

  return false;
  }

// Serial.print("path");
// Serial.print(i);

// resolveUncommittedHops();

// printPathsCompact();
// printChipStatus();

void couldntFindPath(int forcePrint) {
  if (debugNTCC2 == true || forcePrint == 1 || debugNTCC5 == true) {
    // Serial.print("\n\r");
    }
  numberOfUnconnectablePaths = 0;
  for (int i = 0; i < numberOfPaths; i++) {
    if (debugNTCC5) {
      Serial.print("path ");
      Serial.println(i);
      }
    int foundNegative = 0;
    for (int j = 0; j < 3; j++) {
      if (path[i].chip[j] == -1 && j >= 2) {
        continue;
        }

      if (path[i].x[j] < 0 || path[i].y[j] < 0) {
        foundNegative = 1;
        }
      }

    if (foundNegative == 1 && path[i].duplicate == 0) {

      if (debugNTCC2 == true || forcePrint == 1) {
        Serial.print("\n\rCouldn't find a path for ");
        printNodeOrName(path[i].node1);
        Serial.print(" to ");
        printNodeOrName(path[i].node2);
        Serial.println("\n\r");
        }

      unconnectablePaths[numberOfUnconnectablePaths][0] = path[i].node1;
      unconnectablePaths[numberOfUnconnectablePaths][1] = path[i].node2;
      numberOfUnconnectablePaths++;
      path[i].skip = true;
      }
    }
  if (debugNTCC2 == true || forcePrint == 1 || debugNTCC5 == true) {
    // Serial.print("\n\r");
    }
  }

void resolveUncommittedHops2(void) { }

void resolveUncommittedHops(int allowStacking, int powerOnly) { /// fix this
  // return;
  if (debugNTCC2) {
    Serial.print("\n\r");
    Serial.print("resolveUncommittedHops()");
    Serial.print("\n\r");
    }
  // return;
  /// return;

  // printPathsCompact();
  // printChipStatus();

  int freeXSearchOrder[12][16] = {

    // this disallows bounces from sf x pins that would cause problems (5V,
    // GND, etc.)
    {-1, -1, 2, 3, 4, 5, 6, 7, 8, -1, 10, 11, 12, -1, 14, 15},        // a
    {0, 1, -1, -1, 4, 5, 6, 7, 8, 9, 10, -1, 12, 13, 14, -1},         // b
    {0, 1, 2, 3, -1, -1, 6, 7, 8, -1, 10, 11, 12, -1, 14, 15},        // c
    {0, 1, 2, 3, 4, 5, -1, -1, 8, 9, 10, -1, 12, 13, 14, -1},         // d
    {0, -1, 2, 3, 4, -1, 6, 7, -1, -1, 10, 11, 12, 13, 14, 15},       // e
    {0, 1, 2, -1, 4, 5, 6, -1, 8, 9, -1, -1, 12, 13, 14, 15},         // f
    {0, -1, 2, 3, 4, -1, 6, 7, 8, 9, 10, 11, -1, -1, 14, 15},         // g
    {0, 1, 2, -1, 4, 5, 6, -1, 8, 9, 10, 11, 12, 13, -1, -1},         // h
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, 12, 13, 14, -1}, // i
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1}, // j
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1}, // k
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1}, // l
    };

  int pathsWithSameXChips[numberOfPaths + 2];
  int pathsWithSameYChips[numberOfPaths + 2];

  for (int i = 0; i <= numberOfPaths; i++) {
    if (powerOnly == 1 && path[i].net > 5) {
      if (path[i].duplicate == 1) {
        }

      continue;
      }
    if (powerOnly == 1 && path[i].duplicate == 1) {
      continue;
      }
    // printPathsCompact();
    // printChipStatus();
    pathsWithSameXChips[i] = -1;
    pathsWithSameYChips[i] = -1;
    int sameChips[2][4] = {//[x,y][chip]
                           {-1, -1, -1, -1},
                           {-1, -1, -1, -1} };
    int xFlag = -1;
    int yFlag = -1;

    for (int chip = 0; chip < 4; chip++) {
      if (path[i].chip[chip] != -1) {
        for (int chip2 = 0; chip2 < 4; chip2++) {
          if ((path[i].chip[chip] == path[i].chip[chip2] && chip != chip2) ||
              (chip == 2 &&
               path[i].chip[2] !=
               -1)) //(path[i].x[3] != -1 || path[i].y[3] != -1)) )
            {
            for (int xy = 0; xy < 6; xy++) {
              if (path[i].x[xy] == -2) {
                xFlag = 1;
                }
              if (path[i].y[xy] == -2) {
                yFlag = 1;
                }
              }

            if (xFlag == 1) {
              sameChips[0][chip] = path[i].chip[chip];
              //  Serial.print("sameChips[0][chip]: ");
              //  Serial.println(sameChips[0][chip]);

              pathsWithSameXChips[i] = 1;
              }
            if (yFlag == 1) {
              sameChips[1][chip] = path[i].chip[chip];
              //  Serial.print("sameChips[1][chip]: ");
              //  Serial.println(sameChips[1][chip]);

              pathsWithSameYChips[i] = 1;
              }
            }
          }
        }
      }

    // for (int t = 0; t < 4; t++)
    // {
    //     Serial.print(sameChips[0][t]);
    //     Serial.print(", ");
    //     Serial.print(sameChips[1][t]);
    //     Serial.println("  ");
    // }
    // Serial.println("  \n\r");

    if (pathsWithSameXChips[i] == 1 || pathsWithSameYChips[i] == 1) {
      if (debugNTCC2) {
        Serial.print("\n\r");
        Serial.print(i);
        Serial.print("\tsame chips: x");
        }
      for (int chip = 0; chip < 4; chip++) {
        if (debugNTCC2) {
          Serial.print(chipNumToChar(sameChips[0][chip]));
          Serial.print(", ");
          }
        }
      if (debugNTCC2) {
        Serial.print("\ty");
        }
      for (int chip = 0; chip < 4; chip++) {
        if (debugNTCC2) {
          Serial.print(chipNumToChar(sameChips[1][chip]));
          Serial.print(", ");
          }
        }
      if (debugNTCC2) {
        Serial.println(" ");
        }
      }

    if (pathsWithSameXChips[i] == 1 || pathsWithSameYChips[i] == 1) {
      if (pathsWithSameXChips[i] == 1) {
        int freeX = -1;
        int otherChip = -1;
        int otherChipX = -1;
        int lastFreeX = -1;

        int foundChipXs[2][3] = { {-1, -1, -1}, {-1, -1, -1} };

        for (int chip = 0; chip < 3; chip++) {
          if (sameChips[0][chip] != -1) // && sameChips[0][chip] > 8)
            {
            for (int freeXidx = 0; freeXidx < 16; freeXidx++) {
              int otherChipXStatus = 99;

              if (freeXSearchOrder[sameChips[0][chip]][freeXidx] == -1) {
                continue;
                }

              // if (ch[sameChips[0][chip]].xStatus
              //             [freeXSearchOrder[sameChips[0][chip]][freeXidx]] ==
              //         -1 ||
              //     ch[sameChips[0][chip]].xStatus
              //             [freeXSearchOrder[sameChips[0][chip]][freeXidx]] ==
              //         path[i].net) {

              if (freeOrSameNetX(sameChips[0][chip],
                                 freeXSearchOrder[sameChips[0][chip]][freeXidx],
                                 path[i].net, allowStacking) == true) {
                otherChip =
                  ch[sameChips[0][chip]]
                  .xMap[freeXSearchOrder[sameChips[0][chip]][freeXidx]];
                int thisChip = sameChips[0][chip];
                // Serial.print("other Chip = ");
                // Serial.println(chipNumToChar(otherChip));
                int otherChipFree = 0;
                otherChipX = -1;

                for (int xOtherCheck = 0; xOtherCheck < 16; xOtherCheck++) {
                  //             int lane = -1;
                  int lane = -1;

                  if (sameChips[0][chip] < 8) {
                    otherChip =
                      ch[thisChip].xMap[freeXSearchOrder[thisChip][freeXidx]];

                    if (freeXidx % 2 == 0) {
                      lane = 0;
                      } else {
                      lane = 1;
                      }

                    if (lane == 1) {
                      otherChipX = xMapForChipLane1(otherChip, thisChip);
                      } else if (lane == 0) {
                        otherChipX = xMapForChipLane0(otherChip, thisChip);
                        }
                      // Serial.print("otherChipX = ");
                      // Serial.println(otherChipX);

                      otherChipXStatus = ch[otherChip].xStatus[otherChipX];
                    }

                  // if (ch[otherChip].xMap[xOtherCheck] ==
                  // sameChips[0][chip])
                  // {
                  //     if (ch[otherChip].xStatus[xOtherCheck] == -1 ||
                  //     ch[otherChip].xStatus[xOtherCheck] == path[i].net)
                  //     {

                  //     }
                  // }
                  }
                if ((otherChipXStatus == -1 ||
                     (otherChipXStatus == path[i].net && allowStacking == 1))) {
                  freeX = freeXSearchOrder[sameChips[0][chip]][freeXidx];
                  // lastFreeX = freeX;
                  //  ch[otherChip].xStatus[otherChipX] = path[i].net;
                  break;
                  }
                }
              }

            // if (chip >= 2)
            // {
            //     path[i].x[2] = freeX;
            //     path[i].x[3] = freeX;
            //     ch[sameChips[0][chip]].xStatus[freeX] = path[i].net;
            // }
            // else
            // {
            // Serial.print("freeX = ");
            /// Serial.println(freeX);
            path[i].x[chip] = freeX;

            foundChipXs[0][0] = sameChips[0][chip];
            foundChipXs[0][1] = freeX;

            foundChipXs[1][0] = otherChip;
            foundChipXs[1][1] = otherChipX;

            // ch[otherChip].xStatus[otherChipX] = path[i].net;

            // ch[sameChips[0][chip]].xStatus[freeX] = path[i].net;

            // Serial.print("path: ");
            // Serial.print(i);
            // Serial.print("\nfreeX: ");
            // Serial.print(freeX);
            // Serial.print("  status: ");
            // Serial.print(ch[sameChips[0][chip]].xStatus[freeX]);
            // Serial.println(" ");
            //}
            }
          }

        // for (int i = 0; i < 2; i++) {
        //   Serial.print("foundChipXs[");
        //   Serial.print(i);
        //   Serial.print("][0]: ");
        //   Serial.println(foundChipXs[i][0]);
        //   Serial.print("foundChipXs[");
        //   Serial.print(i);
        //   Serial.print("][1]: ");
        //   Serial.println(foundChipXs[i][1]);
        // }

        ch[foundChipXs[0][0]].xStatus[foundChipXs[0][1]] = path[i].net;

        ch[foundChipXs[1][0]].xStatus[foundChipXs[1][1]] = path[i].net;
        }

      // continue; /// the fucked part is in the y
      if (pathsWithSameYChips[i] == 1) {
        // int chipPairs[2] =  {-1, -1};
        // for (int t = 0; t < 4; t++)
        // {
        //     Serial.print("\n\n\r");
        //     Serial.print(sameChips[0][t]);
        //     Serial.print(", ");
        //     Serial.print(sameChips[1][t]);
        //     Serial.println("  ");

        //     if (sameChips[0][t] != -1 && sameChips[1][t] != -1)
        //     {
        //         if (sameChips[0][t]!= chipPairs[0] )
        //         {
        //             chipPairs[0] = sameChips[0][t];
        //             chipPairs[1] = sameChips[1][t];
        //             break;
        //         }

        //     }
        int chipPairs[2] = { -1, -1 };
        for (int t = 0; t < 4; t++) {
          if (sameChips[1][t] != -1) {
            bool isUnique = true;
            for (int i = 0; i < 2; i++) {
              if (sameChips[1][t] == chipPairs[i]) {
                isUnique = false;
                break;
                }
              }
            if (isUnique) {
              for (int i = 0; i < 2; i++) {
                if (chipPairs[i] == -1) {
                  chipPairs[i] = sameChips[1][t];
                  break;
                  }
                }
              }
            }
          }

        // Serial.print("\n\n\rchipPairs[0]: ");
        // Serial.println(chipPairs[0]);
        // Serial.print("chipPairs[1]: ");
        // Serial.println(chipPairs[1]);
        // Serial.print("sameChips[1][0]: ");
        // Serial.println(sameChips[1][0]);
        // Serial.print("sameChips[1][1]: ");
        // Serial.println(sameChips[1][1]);
        // Serial.print("sameChips[1][2]: ");
        // Serial.println(sameChips[1][2]);
        // Serial.print("sameChips[1][3]: ");
        // Serial.println(sameChips[1][3]);

        // Serial.println("  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\r");
        // printPathsCompact();
        // printChipStatus();
        for (int pairs = 0; pairs < 2; pairs++) {
          int freeY = -1;
          for (int chip = 0; chip < 4; chip++) {
            if (sameChips[1][chip] != -1 &&
                sameChips[1][chip] == chipPairs[pairs]) {
              for (int freeYsearch = 0; freeYsearch < 8;
                   freeYsearch++) ////////// make it search l chip connections
                /// first
                {
                if (sameChips[1][chip] < 8 && freeYsearch != 0) {
                  // Serial.print("continued chip: ");
                  // Serial.print(sameChips[1][chip]);
                  // Serial.print("\tfreeYsearch: ");
                  // Serial.println(freeYsearch);

                  continue;
                  }

                if ((ch[sameChips[1][chip]].yStatus[freeYsearch] == -1 ||
                     ch[sameChips[1][chip]].yStatus[freeYsearch] ==
                     path[i].net)) {
                  // if (freeOrSameNetY(sameChips[1][chip], freeYsearch,
                  // path[i].net,
                  //                    allowStacking) == true) {
                  // Serial.print("pairs: ");
                  // Serial.println(pairs);
                  // Serial.print("chiploop: ");
                  // Serial.println(chip);
                  // Serial.print("chip: ");
                  // Serial.println(sameChips[1][chip]);
                  // Serial.print("ch[sameChips[1][");
                  // Serial.print(chip);
                  // Serial.print("]].yStatus[");
                  // Serial.print(freeYsearch);
                  // Serial.print("]: ");
                  // Serial.println(ch[sameChips[1][chip]].yStatus[freeYsearch]);

                  freeY = freeYsearch;
                  //  Serial.print("freeY: ");
                  //  Serial.println(freeY);

                  if (path[i].y[chip] == -2) {
                    // Serial.print("path: ");
                    // Serial.print(i);
                    // Serial.print("\n\rchip: ");
                    // printChipNumToChar(chip);
                    // Serial.print("\n\rfreeYsearch: ");
                    // Serial.print(freeYsearch);
                    // Serial.print("\n\rstatus: ");
                    // Serial.print(ch[sameChips[1][chip]].yStatus[freeYsearch]);
                    // Serial.println(" \n\n\r");
                    // Serial.print("\n\n\rfreeY After search : ");
                    // Serial.println(freeY);

                    path[i].y[chip] = freeY;
                    ch[sameChips[1][chip]].yStatus[freeY] = path[i].net;

                    int otherChipX =
                      xMapForChipLane0(freeY, sameChips[1][chip]);
                    // Serial.print("otherChipX: ");
                    // Serial.println(otherChipX);
                    ch[freeY].xStatus[otherChipX] = path[i].net;

                    // ch[path[i].chip[0]].yStatus[freeY] = path[i].net;
                    // ch[path[i].chip[1]].yStatus[freeY] = path[i].net;

                    // Serial.print("\t ");
                    // Serial.print(path[i].y[chip]);

                    // Serial.print("\t ");
                    // Serial.print("sameChips[1][chip]: ");
                    // printChipNumToChar(sameChips[1][chip]);
                    break;
                    }

                  /// Serial.println(" \r\n");
                  continue;
                  // break;
                  }
                }
              }
            }
          }
        }
      }
    }
  //  printPathsCompact();
  // printChipStatus();
  }

void swapDuplicateNode(int pathIndex) {
  for (int i = 0; i < 26; i++) {
    if ((duplucateSFnodes[i][0] == path[pathIndex].chip[1]) &&
        (duplucateSFnodes[i][1] ==
         xMapForNode(path[pathIndex].node2, path[pathIndex].chip[1]))) {
      if (debugNTCC2) {
        Serial.print("swapping ");
        printChipNumToChar(path[pathIndex].chip[1]);
        Serial.print(" with ");
        printChipNumToChar(duplucateSFnodes[i][2]);
        }

      path[pathIndex].chip[1] = duplucateSFnodes[i][2];
      break;

      // path[pathIndex].x[1] = duplucateSFnodes[i][3];
      }
    }
  }

int checkForOverlappingPaths() {
  int found = 0;

  for (int i = 0; i <= numberOfPaths; i++) {
    int fchip[4] = { path[i].chip[0], path[i].chip[1], path[i].chip[2],
                    path[i].chip[3] };

    for (int j = 0; j < numberOfPaths; j++) {
      if (i == j) {
        continue;
        }
      if (path[i].net == path[j].net) {
        continue;
        }
      int schip[4] = { path[j].chip[0], path[j].chip[1], path[j].chip[2],
                      path[j].chip[3] };

      for (int f = 0; f < 4; f++) {
        for (int s = 0; s < 4; s++) {
          if (fchip[f] == schip[s] && fchip[f] != -1) {
            if (path[i].x[f] == -1 || path[j].x[s] == -1) {
              continue;
              }
            if (path[i].x[f] == path[j].x[s]) {
              // if (debugNTCC3) {
              Serial.print("Path ");
              Serial.print(i);
              Serial.print(" and ");
              Serial.print(j);
              Serial.print(" overlap at x ");
              Serial.print(path[i].x[f]);
              Serial.print(" on chip ");
              Serial.print(chipNumToChar(fchip[f]));
              Serial.print("   nets ");
              Serial.print(path[i].net);
              Serial.print(" and ");
              Serial.println(path[j].net);

              // printPathsCompact();
              // printChipStatus();
              // }
              // return 1;
              found++;
              } else if (path[i].y[f] == path[j].y[s]) {
                // if (debugNTCC3) {
                Serial.print("Path ");
                Serial.print(i);
                Serial.print(" and ");
                Serial.print(j);
                Serial.print(" overlap at y ");
                Serial.print(path[i].y[f]);
                Serial.print(" on chip ");
                Serial.print(chipNumToChar(fchip[f]));
                Serial.print("   nets ");
                Serial.print(path[i].net);
                Serial.print(" and ");
                Serial.println(path[j].net);

                // printPathsCompact();
                // printChipStatus();
                // }
                // return 1;
                found++;
                }
            }
          }
        }
      }
    }
  return found;
  }

int printNetOrNumber(int net) {
  int spaces = 0;
  switch (net) {
    case 0:
      spaces = Serial.print("E");
      break;
    case 1:
      spaces = Serial.print("Gn");
      break;
    case 2:
      spaces = Serial.print("T");
      break;
    case 3:
      spaces = Serial.print("B");
      break;
    case 4:
      spaces = Serial.print("d0");
      break;
    case 5:
      spaces = Serial.print("d1");
      break;
    default:
      spaces = Serial.print(net);
      break;
    }
  return spaces;
  }

void printPathsCompact(int showCullDupes) {
  // Serial.println(" ");
  // Serial.println(checkForOverlappingPaths());

  int lastDuplicate = 0;
  Serial.println(
      "\n\rpath\tnet\tnode1\tchip0\tx0\ty0\tnode2\tchip1\tx1\ty1\ta"
      "ltPath\tsameChp\tdup\tpathType\tchip2\tx2\ty2"); //\tx3\ty3\n\r");
  for (int i = 0; i < numberOfPaths; i++) {
    switch (showCullDupes) {
      case 0:
        if (path[i].duplicate > 0) {
          continue;
          }
        break;
      case 1:

        if (path[i].duplicate > 0 && path[i].x[0] == -1 && path[i].x[1] == -1) {
          continue;
          }
        break;
      }

    if (lastDuplicate == 0 && path[i].duplicate == 1) {
      Serial.println("\n\rduplicates");
      }

    lastDuplicate = path[i].duplicate;

    Serial.print(i);
    Serial.print("\t");

    // Serial.print(path[i].net);

    printNetOrNumber(path[i].net);
    Serial.print("\t");
    printNodeOrName(path[i].node1);
    // Serial.print("\t");
    // Serial.print(path[i].nodeType[0]);
    Serial.print("\t");
    Serial.print(chipNumToChar(path[i].chip[0]));
    Serial.print("\t");
    Serial.print(path[i].x[0]);
    Serial.print("\t");
    Serial.print(path[i].y[0]);
    Serial.print("\t");
    printNodeOrName(path[i].node2);
    // Serial.print("\t");
    // Serial.print(path[i].nodeType[1]);
    Serial.print("\t");
    Serial.print(chipNumToChar(path[i].chip[1]));
    Serial.print("\t");
    Serial.print(path[i].x[1]);
    Serial.print("\t");
    Serial.print(path[i].y[1]);
    Serial.print("\t");
    Serial.print(path[i].altPathNeeded);
    Serial.print("\t");
    Serial.print(path[i].sameChip);
    Serial.print("\t");
    Serial.print(path[i].duplicate);
    Serial.print("\t");
    printPathType(i);

    if (path[i].chip[2] != -1) {
      Serial.print(" \t");
      Serial.print(chipNumToChar(path[i].chip[2]));
      Serial.print(" \t");
      Serial.print(path[i].x[2]);
      Serial.print(" \t");
      Serial.print(path[i].y[2]);
      Serial.print(" \t");
      Serial.print(path[i].x[3]);
      Serial.print(" \t");
      Serial.print(path[i].y[3]);
      }
    if (1) {
      if (path[i].chip[3] != -1) {
        Serial.print(" \t");
        Serial.print(chipNumToChar(path[i].chip[3]));
        Serial.print(" \t");
        }
      }

    Serial.println(" ");
    }
  // Serial.println(
  //     "\n\rpath\tnet\tnode1\tchip0\tx0\ty0\tnode2\tchip1\tx1\ty1\ta"
  //     "ltPath\tsameChp\tpathType\tchipL\tchip2\tx2\ty2\n\r");
  }

void printChipStatus(void) {
  Serial.println(
      "\n\rchip\t0    1    2    3    4    5    6    7    8    9    10   "
      "11   "
      "12   13   14   15\t\t0    1    2    3    4    5    6    7");
  for (int i = 0; i < 12; i++) {
    int spaces = 0;
    Serial.print(chipNumToChar(i));
    Serial.print("\t");
    for (int j = 0; j < 16; j++) {
      if (ch[i].xStatus[j] == -1) {
        spaces += Serial.print(".");
        } else {
        spaces += printNetOrNumber(ch[i].xStatus[j]);
        }
      for (int k = 0; k < 4 - spaces; k++) {
        Serial.print(" ");
        }
      Serial.print(" ");
      spaces = 0;
      }
    Serial.print("\t");
    for (int j = 0; j < 8; j++) {
      if (ch[i].yStatus[j] == -1) {
        spaces += Serial.print(".");
        } else {
        spaces += printNetOrNumber(ch[i].yStatus[j]);
        }

      for (int k = 0; k < 4 - spaces; k++) {
        Serial.print(" ");
        }
      Serial.print(" ");
      spaces = 0;
      }
    if (i == 7) {
      Serial.print("\n\n\rchip\t0    1    2    3    4    5    6    7    "
                   "8    9    10   "
                   "11   12   13   14   15\t\t0    1    2    3    4    5 "
                   "   6    7");
      }
    Serial.println(" ");
    }
  }

void findStartAndEndChips(int node1, int node2, int pathIdx) {
  if (debugNTCC2) {
    Serial.print("findStartAndEndChips()\n\r");
    }
  bothNodes[0] = node1;
  bothNodes[1] = node2;
  startEndChip[0] = -1;
  startEndChip[1] = -1;

  if (debugNTCC) {
    Serial.print("finding chips for nodes: ");
    Serial.print(definesToChar(node1));
    Serial.print("-");
    Serial.println(definesToChar(node2));
    }

  for (int twice = 0; twice < 2; twice++) // first run gets node1 and start
    // chip, second is node2 and end
    {
    if (debugNTCC) {
      Serial.print("\n\rnode: ");
      Serial.println(twice + 1);
      Serial.println(" ");
      }
    int candidatesFound = 0;

    switch (bothNodes[twice]) {
      case 30:
      case 60: {
      path[pathIdx].chip[twice] = CHIP_L;
      if (debugNTCC) {
        Serial.print("chip: ");
        Serial.println(chipNumToChar(path[pathIdx].chip[twice]));
        }
      break;
      }

      case 29:
      case 59: {
      path[pathIdx].chip[twice] = CHIP_K;
      if (debugNTCC) {
        Serial.print("chip: ");
        Serial.println(chipNumToChar(path[pathIdx].chip[twice]));
        }
      break;
      }

      case 1 ... 28: // on the breadboard
      case 31 ... 58: {
      path[pathIdx].chip[twice] = bbNodesToChip[bothNodes[twice]];
      if (debugNTCC) {
        Serial.print("chip: ");
        Serial.println(chipNumToChar(path[pathIdx].chip[twice]));
        }
      break;
      }
      case NANO_D0 ... NANO_A7: // on the nano
      {
      int nanoIndex = defToNano(bothNodes[twice]);

      if (nano.numConns[nanoIndex] == 1) {
        path[pathIdx].chip[twice] = nano.mapIJ[nanoIndex];
        if (debugNTCC) {
          Serial.print("nano chip: ");
          Serial.println(chipNumToChar(path[pathIdx].chip[twice]));
          }
        } else {
        if (debugNTCC) {
          Serial.print("nano candidate chips: ");
          }
        chipCandidates[twice][0] = nano.mapIJ[nanoIndex];
        path[pathIdx].candidates[twice][0] = chipCandidates[twice][0];
        // Serial.print(candidatesFound);
        if (debugNTCC) {
          Serial.print(chipNumToChar(path[pathIdx].candidates[twice][0]));
          }
        candidatesFound++;
        chipCandidates[twice][1] = nano.mapKL[nanoIndex];
        Serial.print(candidatesFound);
        path[pathIdx].candidates[twice][1] = chipCandidates[twice][1];
        candidatesFound++;
        if (debugNTCC) {
          Serial.print(" ");
          Serial.println(chipNumToChar(path[pathIdx].candidates[twice][1]));
          }
        }
      break;
      }
      case GND ... 141: {
      if (debugNTCC5) {
        Serial.print("special function candidate chips: ");
        }
      for (int i = 8; i < 12; i++) {
        for (int j = 0; j < 16; j++) {
          if (ch[i].xMap[j] == bothNodes[twice]) {
            chipCandidates[twice][candidatesFound] = i;
            path[pathIdx].candidates[twice][candidatesFound] =
              chipCandidates[twice][candidatesFound];
            candidatesFound++;
            if (debugNTCC5) {
              Serial.print(chipNumToChar(i));
              Serial.print(" ");
              }
            }
          }
        }

      if (candidatesFound == 1) {
        path[pathIdx].chip[twice] = chipCandidates[twice][0];

        path[pathIdx].candidates[twice][0] = -1;
        if (debugNTCC5) {
          Serial.print("\n\rchip: ");
          Serial.println(chipNumToChar(path[pathIdx].chip[twice]));
          }
        }
      if (debugNTCC5) {
        Serial.println(" ");
        }
      break;
      }
      }
    }
  }

void mergeOverlappingCandidates(
    int pathIndex) // also sets altPathNeeded flag if theyre on different
  // sf chips (there are no direct connections between
  // them)
  {
  // Serial.print("\t 0 \t");
  int foundOverlap = 0;

  if ((path[pathIndex].candidates[0][0] != -1 &&
       path[pathIndex].candidates[1][0] != -1)) // if both nodes have candidates
    {
    /// Serial.print("\t1");
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 3; j++) {
        if (path[pathIndex].candidates[0][i] ==
            path[pathIndex].candidates[1][j]) {
          // Serial.print("! \t");
          path[pathIndex].chip[0] = path[pathIndex].candidates[0][i];
          path[pathIndex].chip[1] = path[pathIndex].candidates[0][i];
          foundOverlap = 1;
          break;
          }
        }
      }
    if (foundOverlap == 0) {
      pathsWithCandidates[pathsWithCandidatesIndex] = pathIndex;
      pathsWithCandidatesIndex++;
      }
    } else if (path[pathIndex].candidates[0][0] != -1) // if node 1 has candidates
      {
      // Serial.print("\t2");

      for (int j = 0; j < 3; j++) {
        if (path[pathIndex].chip[1] == path[pathIndex].candidates[0][j]) {
          // Serial.print("! \t");
          path[pathIndex].chip[0] = path[pathIndex].candidates[0][j];

          foundOverlap = 1;

          break;
          }
        }
      if (foundOverlap == 0) {
        pathsWithCandidates[pathsWithCandidatesIndex] = pathIndex;
        pathsWithCandidatesIndex++;
        }

      // path[pathIndex].altPathNeeded = 1;
      } else if (path[pathIndex].candidates[1][0] != -1) // if node 2 has candidates
        {
        // Serial.print(" \t3");

        for (int j = 0; j < 3; j++) {
          if (path[pathIndex].chip[0] == path[pathIndex].candidates[1][j]) {
            // Serial.print("! \t");

            path[pathIndex].chip[1] = path[pathIndex].candidates[1][j];
            foundOverlap = 1;
            break;
            }
          }
        if (foundOverlap == 0) {
          pathsWithCandidates[pathsWithCandidatesIndex] = pathIndex;
          pathsWithCandidatesIndex++;
          }

        // path[pathIndex].altPathNeeded = 1;
        }

      if (foundOverlap == 1) {
        path[pathIndex].candidates[0][0] = -1;
        path[pathIndex].candidates[0][1] = -1;
        path[pathIndex].candidates[0][2] = -1;
        path[pathIndex].candidates[1][0] = -1;
        path[pathIndex].candidates[1][1] = -1;
        path[pathIndex].candidates[1][2] = -1;
        } else {
        }

      //   if (path[pathIndex].chip[0] >= CHIP_I && path[pathIndex].chip[1] >=
      //   CHIP_I) {
      //     if (path[pathIndex].chip[0] != path[pathIndex].chip[1]) {

      //       path[pathIndex].altPathNeeded = 1;
      //     }
      //   }
  }

void assignPathType(int pathIndex) {
  if (path[pathIndex].chip[0] == path[pathIndex].chip[1]) {
    path[pathIndex].sameChip = true;
    } else {
    path[pathIndex].sameChip = false;
    }

  if ((path[pathIndex].node1 == 29 || path[pathIndex].node1 == 59 ||
       path[pathIndex].node1 == 30 || path[pathIndex].node1 == 60) ||
      path[pathIndex].node1 == 114 || path[pathIndex].node1 == 116 ||
      path[pathIndex].node1 == 117) {
    // Serial.print("\n\n\rthis should be a bb to sf connection\n\n\n\r
    // ");
    //  path[pathIndex].altPathNeeded = true;
    // Serial.print("path ");
    // Serial.print(pathIndex);
    // Serial.print(" is a bb to sf connection, swapping\n\r");
    // Serial.print("node1: ");
    // Serial.print(path[pathIndex].node1);
    // Serial.print("\tnode2: ");
    // Serial.print(path[pathIndex].node2);
    // Serial.println("\n\r");
    swapNodes(pathIndex);
    //     Serial.print("path ");
    // Serial.print(pathIndex);
    // Serial.print(" is a bb to sf connection, swapping\n\r");
    // Serial.print("node1: ");
    // Serial.print(path[pathIndex].node1);
    // Serial.print("\tnode2: ");
    // Serial.print(path[pathIndex].node2);
    // Serial.println("\n\r");
    // path[pathIndex].Lchip = true;

    path[pathIndex].nodeType[0] = SF; // maybe have a separate type for ChipL
    // connected nodes, but not now
    }

  if ((path[pathIndex].node1 >= 1 && path[pathIndex].node1 <= 28) ||
      (path[pathIndex].node1 >= 31 && path[pathIndex].node1 <= 58)) {
    path[pathIndex].nodeType[0] = BB;
    } else if (path[pathIndex].node1 >= NANO_D0 &&
               path[pathIndex].node1 <= NANO_A7) {
      path[pathIndex].nodeType[0] = NANO;
      } else if (path[pathIndex].node1 >= GND && path[pathIndex].node1 <= 141) {
        path[pathIndex].nodeType[0] = SF;
        }

      if ((path[pathIndex].node2 == 29 || path[pathIndex].node2 == 59 ||
           path[pathIndex].node2 == 30 || path[pathIndex].node2 == 60) ||
          path[pathIndex].node2 == 114 || path[pathIndex].node2 == 116 ||
          path[pathIndex].node2 == 117 || path[pathIndex].chip[1] == CHIP_K) {
        // Serial.print("\n\n\rthis should be a bb to sf connection 2\n\n\n\r
        // "); path[pathIndex].altPathNeeded = true; path[pathIndex].Lchip =
        // true;
        path[pathIndex].nodeType[1] = SF;
        } else if ((path[pathIndex].node2 >= 1 && path[pathIndex].node2 <= 28) ||
                   (path[pathIndex].node2 >= 31 && path[pathIndex].node2 <= 58)) {
          path[pathIndex].nodeType[1] = BB;
          } else if (path[pathIndex].node2 >= NANO_D0 &&
                     path[pathIndex].node2 <= NANO_A7) {
            path[pathIndex].nodeType[1] = NANO;
            } else if (path[pathIndex].node2 >= GND && path[pathIndex].node2 <= 141) {
              path[pathIndex].nodeType[1] = SF;
              }

            if ((path[pathIndex].nodeType[0] == NANO &&
                 path[pathIndex].nodeType[1] == SF)) {
              path[pathIndex].pathType = NANOtoSF;
              if (path[pathIndex].chip[0] != path[pathIndex].chip[1]) {
                path[pathIndex].altPathNeeded = true;
                }
              } else if ((path[pathIndex].nodeType[0] == SF &&
                          path[pathIndex].nodeType[1] == SF)) {
                path[pathIndex].pathType =
                  NANOtoSF; // SFtoSF is dealt with the same as NANOtoSF
                } else if ((path[pathIndex].nodeType[0] == SF &&
                            path[pathIndex].nodeType[1] == NANO)) {
                  // swapNodes(pathIndex);
                  path[pathIndex].pathType = NANOtoSF;
                  if (path[pathIndex].chip[0] != path[pathIndex].chip[1]) {
                    path[pathIndex].altPathNeeded = true;
                    }

                  // path[pathIndex].altPathNeeded = true;
                  } else if ((path[pathIndex].nodeType[0] == BB &&
                              path[pathIndex].nodeType[1] == SF)) {
                    path[pathIndex].pathType = BBtoSF;
                    } else if ((path[pathIndex].nodeType[0] == SF &&
                                path[pathIndex].nodeType[1] == BB)) {
                      swapNodes(pathIndex);
                      path[pathIndex].pathType = BBtoSF;
                      } else if ((path[pathIndex].nodeType[0] == BB &&
                                  path[pathIndex].nodeType[1] == NANO)) {
                        path[pathIndex].pathType = BBtoNANO;
                        } else if (path[pathIndex].nodeType[0] == NANO &&
                                   path[pathIndex].nodeType[1] ==
                                       BB) // swtich node order so BB always comes first
                          {
                          swapNodes(pathIndex);
                          path[pathIndex].pathType = BBtoNANO;
                          } else if (path[pathIndex].nodeType[0] == BB &&
                                     path[pathIndex].nodeType[1] == BB) {
                            path[pathIndex].pathType = BBtoBB;
                            } else if (path[pathIndex].nodeType[0] == NANO &&
                                       path[pathIndex].nodeType[1] == NANO) {
                              path[pathIndex].pathType = NANOtoNANO;
                              }
                            if (debugNTCC) {
                              Serial.print("Path ");
                              Serial.print(pathIndex);
                              Serial.print(" type: ");
                              printPathType(pathIndex);
                              Serial.print("\n\r");

                              Serial.print("  Node 1: ");
                              Serial.print(path[pathIndex].node1);
                              Serial.print("\tNode 2: ");
                              Serial.print(path[pathIndex].node2);
                              Serial.print("\n\r");

                              Serial.print("  Chip 1: ");
                              Serial.print(path[pathIndex].chip[0]);
                              Serial.print("\tChip 2: ");
                              Serial.print(path[pathIndex].chip[1]);
                              Serial.print("\n\n\r");
                              }
  }

void swapNodes(int pathIndex) {
  int temp = 0;
  temp = path[pathIndex].node1;
  path[pathIndex].node1 = path[pathIndex].node2;
  path[pathIndex].node2 = temp;

  temp = path[pathIndex].chip[0];
  path[pathIndex].chip[0] = path[pathIndex].chip[1];
  path[pathIndex].chip[1] = temp;

  temp = path[pathIndex].candidates[0][0];
  path[pathIndex].candidates[0][0] = path[pathIndex].candidates[1][0];
  path[pathIndex].candidates[1][0] = temp;

  temp = path[pathIndex].candidates[0][1];
  path[pathIndex].candidates[0][1] = path[pathIndex].candidates[1][1];
  path[pathIndex].candidates[1][1] = temp;

  temp = path[pathIndex].candidates[0][2];
  path[pathIndex].candidates[0][2] = path[pathIndex].candidates[1][2];
  path[pathIndex].candidates[1][2] = temp;

  enum nodeType tempNT = path[pathIndex].nodeType[0];
  path[pathIndex].nodeType[0] = path[pathIndex].nodeType[1];
  path[pathIndex].nodeType[1] = tempNT;

  temp = path[pathIndex].x[0];
  path[pathIndex].x[0] = path[pathIndex].x[1];
  path[pathIndex].x[1] = temp;

  temp = path[pathIndex].y[0];
  path[pathIndex].y[0] = path[pathIndex].y[1];
  path[pathIndex].y[1] = temp;
  }

int xMapForNode(int node, int chip) {
  int nodeFound = -1;
  for (int i = 0; i < 16; i++) {
    if (ch[chip].xMap[i] == node) {
      nodeFound = i;
      break;
      }
    }
  if (nodeFound == -1) {
    if (debugNTCC) {
      Serial.print("xMapForNode: \n\rnode ");
      Serial.print(node);
      Serial.print(" not found on chip ");
      Serial.println(chipNumToChar(chip));
      }
    }

  return nodeFound;
  }

int yMapForNode(int node, int chip) {
  int nodeFound = -1;
  for (int i = 1; i < 8; i++) {
    if (ch[chip].yMap[i] == node) {
      nodeFound = i;
      break;
      }
    }
  return nodeFound;
  }

int xMapForChipLane0(int chip1, int chip2) {
  int nodeFound = -1;
  for (int i = 0; i < 16; i++) {
    if (ch[chip1].xMap[i] == chip2) {
      nodeFound = i;
      break;
      }
    }
  return nodeFound;
  }
int xMapForChipLane1(int chip1, int chip2) {
  int nodeFound = -1;
  for (int i = 0; i < 16; i++) {
    if (ch[chip1].xMap[i] == chip2) {
      if (ch[chip1].xMap[i + 1] == chip2) {
        nodeFound = i + 1;
        break;
        }
      }
    }

  if (nodeFound == -1) {
    if (debugNTCC) {
      Serial.print("nodeNotFound lane 1: ");
      Serial.print(chipNumToChar(chip1));
      Serial.print(" ");
      Serial.println(chipNumToChar(chip2));
      }
    }

  return nodeFound;
  }

void resolveChipCandidates(void) {
  int nodesToResolve[2] = {
      0, 0 }; // {node1,node2} 0 = already found, 1 = needs resolving

  for (int pathIndex = 0; pathIndex < numberOfPaths; pathIndex++) {
    nodesToResolve[0] = 0;
    nodesToResolve[1] = 0;

    if (path[pathIndex].chip[0] == -1) {
      nodesToResolve[0] = 1;
      } else {
      nodesToResolve[0] = 0;
      }

    if (path[pathIndex].chip[1] == -1) {
      nodesToResolve[1] = 1;
      } else {
      nodesToResolve[1] = 0;
      }

    for (int nodeOneOrTwo = 0; nodeOneOrTwo < 2; nodeOneOrTwo++) {
      if (nodesToResolve[nodeOneOrTwo] == 1) {
        path[pathIndex].chip[nodeOneOrTwo] =
          moreAvailableChip(path[pathIndex].candidates[nodeOneOrTwo][0],
                            path[pathIndex].candidates[nodeOneOrTwo][1]);
        if (debugNTCC) {
          Serial.print("path[");
          Serial.print(pathIndex);
          Serial.print("] chip from ");
          Serial.print(
              chipNumToChar(path[pathIndex].chip[(1 + nodeOneOrTwo) % 2]));
          Serial.print(" to chip ");
          Serial.print(chipNumToChar(path[pathIndex].chip[nodeOneOrTwo]));
          Serial.print(" chosen\n\n\r");
          }
        }
      }
    }
  }

int moreAvailableChip(int chip1, int chip2) {
  int chipChosen = -1;
  sortSFchipsLeastToMostCrowded();
  sortAllChipsLeastToMostCrowded();

  for (int i = 0; i < 12; i++) {
    if (chipsLeastToMostCrowded[i] == chip1 ||
        chipsLeastToMostCrowded[i] == chip2) {
      chipChosen = chipsLeastToMostCrowded[i];
      break;
      }
    }
  return chipChosen;
  }

void sortSFchipsLeastToMostCrowded(void) {
  bool tempDebug = debugNTCC;
  // debugNTCC = false;
  int numberOfConnectionsPerSFchip[4] = { 0, 0, 0, 0 };

  for (int i = 0; i < numberOfPaths; i++) {
    for (int j = 0; j < 2; j++) {
      if (path[i].chip[j] > 7) {
        numberOfConnectionsPerSFchip[path[i].chip[j] - 8]++;
        }
      }
    }

  if (debugNTCC) {
    for (int i = 0; i < 4; i++) {
      Serial.print("sf connections: ");
      Serial.print(chipNumToChar(i + 8));
      Serial.print(numberOfConnectionsPerSFchip[i]);
      Serial.print("\n\r");
      }
    }
  // debugNTCC = tempDebug;
  }

void sortAllChipsLeastToMostCrowded(void) {
  // bool tempDebug = debugNTCC;
  // debugNTCC = false;

  int numberOfConnectionsPerChip[12] = {
      0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0 }; // this will be used to determine which chip is most
  // crowded

  for (int i = 0; i < 12; i++) {
    chipsLeastToMostCrowded[i] = i;
    }

  if (debugNTCC) {
    // Serial.println("\n\r");
    }
  for (int i = 0; i < numberOfPaths; i++) {
    for (int j = 0; j < 2; j++) {
      if (path[i].chip[j] != -1) {
        numberOfConnectionsPerChip[path[i].chip[j]]++;
        }
      }
    }

  // debugNTCC = false;
  if (debugNTCC5) {
    for (int i = 0; i < 12; i++) {
      Serial.print(chipNumToChar(i));
      Serial.print(": ");
      Serial.println(numberOfConnectionsPerChip[i]);
      }

    Serial.println("\n\r");
    }

  int temp = 0;

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 11; j++) {
      if (numberOfConnectionsPerChip[j] > numberOfConnectionsPerChip[j + 1]) {
        temp = numberOfConnectionsPerChip[j];
        // chipsLeastToMostCrowded[j] = chipsLeastToMostCrowded[j + 1];
        numberOfConnectionsPerChip[j] = numberOfConnectionsPerChip[j + 1];
        numberOfConnectionsPerChip[j + 1] = temp;

        temp = chipsLeastToMostCrowded[j];
        chipsLeastToMostCrowded[j] = chipsLeastToMostCrowded[j + 1];
        chipsLeastToMostCrowded[j + 1] = temp;
        }
      }
    }

  for (int i = 0; i < 12; i++) {
    if (debugNTCC5) {
      Serial.print(chipNumToChar(chipsLeastToMostCrowded[i]));
      Serial.print(": ");
      Serial.println(numberOfConnectionsPerChip[i]);
      }
    }

  /*
      if (debugNTCC == true)
      {
          for (int i = 0; i < 4; i++)
          {
              Serial.print("\n\r");
              Serial.print(chipNumToChar(sfChipsLeastToMostCrowded[i]));
              Serial.print(": ");

              Serial.print("\n\r");
          }
      }
  */
  // debugNTCC = tempDebug;
  //  bbToSfConnections();
  }

void printPathArray(void) // this also prints candidates and x y
  {
  // Serial.print("\n\n\r");
  // Serial.print("newBridgeIndex = ");
  // Serial.println(newBridgeIndex);
  Serial.print("\n\r");
  int tabs = 0;
  int lineCount = 0;
  for (int i = 0; i < numberOfPaths; i++) {
    Serial.print("\n\r");
    tabs += Serial.print(i);
    Serial.print("  ");
    if (i < 10) {
      tabs += Serial.print(" ");
      }
    if (i < 100) {
      tabs += Serial.print(" ");
      }
    tabs += Serial.print("[");
    tabs += printNodeOrName(path[i].node1);
    tabs += Serial.print("-");
    tabs += printNodeOrName(path[i].node2);
    tabs += Serial.print("]\tNet ");
    tabs += printNodeOrName(path[i].net);
    tabs += Serial.println(" ");
    tabs += Serial.print("\n\rnode1 chip:  ");
    tabs += printChipNumToChar(path[i].chip[0]);
    tabs += Serial.print("\n\rnode2 chip:  ");
    tabs += printChipNumToChar(path[i].chip[1]);
    // tabs += Serial.print("\n\n\rnode1 candidates: ");
    // for (int j = 0; j < 3; j++) {
    //   printChipNumToChar(path[i].candidates[0][j]);
    //   tabs += Serial.print(" ");
    // }
    // tabs += Serial.print("\n\rnode2 candidates: ");
    // for (int j = 0; j < 3; j++) {
    //   printChipNumToChar(path[i].candidates[1][j]);
    //   tabs += Serial.print(" ");
    // }
    tabs += Serial.print("\n\rpath type: ");
    tabs += printPathType(i);

    if (path[i].altPathNeeded == true) {
      tabs += Serial.print("\n\ralt path needed");
      } else {
      }
    tabs += Serial.println("\n\n\r");

    /// Serial.print(tabs);
    for (int i = 0; i < 24 - (tabs); i++) {
      Serial.print(" ");
      }
    tabs = 0;
    }
  }

int printPathType(int pathIndex) {
  switch (path[pathIndex].pathType) {
    case 0:
      return Serial.print("BB to BB");
      break;
    case 1:
      return Serial.print("BB to NANO");
      break;
    case 2:
      return Serial.print("NANO to NANO");
      break;
    case 3:
      return Serial.print("BB to SF");
      break;
    case 4:
      return Serial.print("NANO to SF");
      break;
    default:
      return Serial.print("Not Assigned");
      break;
    }
  }

int defToNano(int nanoIndex) { return nanoIndex - NANO_D0; }

char chipNumToChar(int chipNumber) { return chipNumber + 'A'; }

int printChipNumToChar(int chipNumber) {
  return Serial.print(chipNumber);
  if (chipNumber == -1) {
    return Serial.print("-1");
    } else {
    return Serial.print((char)(chipNumber + 'A'));
    }
  }

void clearChipsOnPathToNegOne(void) {
  for (int i = 0; i < MAX_BRIDGES - 1; i++) {
    if (i >= numberOfPaths) {
      path[i].node1 = 0; // i know i can just do {-1,-1,-1} but
      path[i].node2 = 0;
      path[i].net = 0;
      // Serial.println(i);
      }
    for (int c = 0; c < 4; c++) {
      path[i].chip[c] = -1;
      }

    for (int c = 0; c < 6; c++) {
      path[i].x[c] = -1;
      path[i].y[c] = -1;
      }

    for (int c = 0; c < 3; c++) {
      path[i].candidates[c][0] = -1;
      path[i].candidates[c][1] = -1;
      path[i].candidates[c][2] =
        -1; // CEEEEEEEE!!!!!! i had this set to 3 and it was clearing
      // everything, but no im not using rust
      }
    }
  }
/*
So the nets are all made, now we need to figure out which chip connections
need to be made to make that phycically happen

start with the special function nets, they're the highest priority

maybe its simpler to make an array of every possible connection


start at net 1 and go up

find start and end chip

bb chips
sf chips
nano chips


things that store x and y valuse for paths
chipStatus.xStatus[]
chipStatus.yStatus[]
nanoStatus.xStatusIJKL[]


struct nanoStatus {  //there's only one of these so ill declare and initalize
together unlike above

//all these arrays should line up (both by index and visually) so one index
will give you all this data

//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const char *pinNames[24]=  {
" D0",   " D1",   " D2",   " D3",   " D4",   " D5",   " D6",   " D7",   " D8",
" D9",    "D10",    "D11",     "D12",    "D13",      "RST",     "REF",   "
A0", " A1",   " A2",   " A3",   " A4",   " A5",   " A6",   " A7"};// String
with readable name //padded to 3 chars (space comes before chars)
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t pinMap[24] =  {
NANO_D0, NANO_D1, NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7,
NANO_D8, NANO_D9, NANO_D10, NANO_D11,  NANO_D12, NANO_D13, NANO_RESET,
NANO_AREF, NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6,
NANO_A7};//Array index to internal arbitrary #defined number
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t numConns[24]= {
1 , 1      , 2      , 2      , 2      , 2      , 2      , 2      , 2      , 2
, 2 , 2       ,  2       , 2       , 1         , 1        , 2      , 2      ,
2 , 2 , 2      , 2      , 1      , 1      };//Whether this pin has 1 or 2
connections to special function chips    (OR maybe have it be a map like i = 2
j = 3  k = 4 l = 5 if there's 2 it's the product of them ij = 6  ik = 8  il =
10 jk = 12 jl = 15 kl = 20 we're trading minuscule amounts of CPU for RAM)
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t  mapIJ[24] =  {
CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J
, CHIP_I , CHIP_J  , CHIP_I  ,  CHIP_J  , CHIP_I  , CHIP_I    ,  CHIP_J  ,
CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J , CHIP_I , CHIP_J
};//Since there's no overlapping connections between Chip I and J, this holds
which of those 2 chips has a connection at that index, if numConns is 1, you
only need to check this one const int8_t  mapKL[24] =  { -1     , -1     ,
CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K , CHIP_K
, CHIP_K  ,  CHIP_K  , -1 , -1        , -1       , CHIP_K , CHIP_K , CHIP_K ,
CHIP_K , CHIP_L , CHIP_L , -1     , -1     };//Since there's no overlapping
connections between Chip K and L, this holds which of those 2 chips has a
connection at that index, -1 for no connection
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t xMapI[24]  =  {
-1 , 1      , -1     , 3      , -1     , 5      , -1     , 7      , -1     , 9
, -1 , 8       ,  -1      , 10      , 11        , -1       , 0      , -1     ,
2 , -1 , 4      , -1     , 6      , -1     };//holds which X pin is connected
to the index on Chip I, -1 for none int8_t xStatusI[24]  =  { -1     , 0 , -1
, 0 , -1     , 0      , -1     , 0      , -1     , 0      , -1      , 0 ,  -1
, 0       , 0         , -1       , 0      , -1     , 0      , -1     , 0 , -1
, 0      , -1     };//-1 for not connected to that chip, 0 for available, >0
means it's connected and the netNumber is stored here
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t xMapJ[24]  =  {
0 , -1     , 2      , -1     , 4      , -1     , 6      , -1     , 8      , -1
, 9       , -1      ,  10      , -1      , -1        , 11       , -1     , 1 ,
-1 , 3      , -1     , 5      , -1     , 7      };//holds which X pin is
connected to the index on Chip J, -1 for none int8_t xStatusJ[24]  =  { 0 , -1
, 0      , -1     , 0      , -1     , 0      , -1     , 0      , -1     , 0 ,
-1 , 0        , 0       , -1        , 0        , -1     , 0      , -1     , 0
, -1 , 0      , -1     , 0      };//-1 for not connected to that chip, 0 for
available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t xMapK[24]  =  {
-1 , -1     , 4      , 5      , 6      , 7      , 8      , 9      , 10     ,
11 , 12      , 13      ,  14      , -1      , -1        , -1       , 0      ,
1 , 2 , 3      , -1     , -1     , -1     , -1     };//holds which X pin is
connected to the index on Chip K, -1 for none int8_t xStatusK[24]  =  { -1 ,
-1     , 0      , 0      , 0      , 0      , 0      , 0      , 0      , 0 , 0
, 0 , 0       , -1      , -1        , -1       , 0      , 0      , 0      , 0
, -1     , -1     , -1     , -1     };//-1 for not connected to that chip, 0
for available, >0 means it's connected and the netNumber is stored here
//                         |        |        |        |        |        | | |
| |        |         |         |          |         |           |          | |
| |        |        |        |        |        | const int8_t xMapL[24]  =  {
-1 , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     ,
-1 , -1      , -1      ,  -1      , -1      , -1        , -1       , -1     ,
-1 , -1 , -1     , 12     , 13     , -1     , -1     };//holds which X pin is
connected to the index on Chip L, -1 for none int8_t xStatusL[24]  =  { -1 ,
-1     , -1     , -1     , -1     , -1     , -1     , -1     , -1     , -1 ,
-1 , -1 ,  -1      , -1      , -1        , -1       , -1     , -1     , -1 ,
-1 , 0 , 0      , -1     , -1     };//-1 for not connected to that chip, 0 for
available, >0 means it's connected and the netNumber is stored here

// mapIJKL[]     will tell you whethher there's a connection from that nano
pin to the corresponding special function chip
// xMapIJKL[]    will tell you the X pin that it's connected to on that sf
chip
// xStatusIJKL[] says whether that x pin is being used (this should be the
same as mt[8-10].xMap[] if theyre all stacked on top of each other)
//              I haven't decided whether to make this just a flag, or store
that signal's destination const int8_t reversePinMap[110] = {NANO_D0, NANO_D1,
NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9,
NANO_D10, NANO_D11, NANO_D12, NANO_D13, NANO_RESET, NANO_AREF, NANO_A0,
NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6,
NANO_A7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,GND,101,102,SUPPLY_3V3,104,SUPPLY_5V,DAC0,DAC1_8V,ISENSE_PLUS,ISENSE_MINUS};

};

struct netStruct net[MAX_NETS] = { //these are the special function nets that
will always be made
//netNumber,       ,netName          ,memberNodes[] ,memberBridges[][2]
,specialFunction        ,intsctNet[] ,doNotIntersectNodes[] ,priority { 127
,"Empty Net"      ,{EMPTY_NET}           ,{{}}                   ,EMPTY_NET
,{}
,{EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET,EMPTY_NET} , 0},
    {     1        ,"GND\t"          ,{GND}                 ,{{}} ,GND ,{}
,{SUPPLY_3V3,SUPPLY_5V,DAC0,DAC1_8V}    , 1}, {     2        ,"+5V\t"
,{SUPPLY_5V}           ,{{}}                   ,SUPPLY_5V              ,{}
,{GND,SUPPLY_3V3,DAC0,DAC1_8V}          , 1}, {     3        ,"+3.3V\t"
,{SUPPLY_3V3}          ,{{}}                   ,SUPPLY_3V3             ,{}
,{GND,SUPPLY_5V,DAC0,DAC1_8V}           , 1}, {     4        ,"DAC 0\t"
,{DAC0}
,{{}}                   ,DAC0                ,{}
,{GND,SUPPLY_5V,SUPPLY_3V3,DAC1_8V}        , 1}, {     5        ,"DAC 1\t"
,{DAC1_8V}             ,{{}}                   ,DAC1_8V                ,{}
,{GND,SUPPLY_5V,SUPPLY_3V3,DAC0}        , 1}, {     6        ,"I Sense +"
,{ISENSE_PLUS}  ,{{}}                   ,ISENSE_PLUS     ,{} ,{ISENSE_MINUS} ,
2}, {     7        ,"I Sense -"      ,{ISENSE_MINUS} ,{{}} ,ISENSE_MINUS ,{}
,{ISENSE_PLUS}                      , 2},
};



Index   Name            Number          Nodes                   Bridges Do Not
Intersects 0       Empty Net       127             EMPTY_NET {0-0} EMPTY_NET 1
GND             1               GND,1,2,D0,3,4 {1-GND,1-2,D0-1,2-3,3-4}
3V3,5V,DAC_0,DAC_1 2       +5V             2 5V,11,12,10,9
{11-5V,11-12,10-11,9-10}        GND,3V3,DAC_0,DAC_1 3 +3.3V           3
3V3,D10,D11,D12 {D10-3V3,D10-D11,D11-D12} GND,5V,DAC_0,DAC_1 4       DAC 0 4
DAC_0 {0-0} GND,5V,3V3,DAC_1 5       DAC 1           5               DAC_1
{0-0} GND,5V,3V3,DAC_0 6       I Sense +       6 I_POS,6,5,A1,AREF
{6-I_POS,5-6,A1-5,AREF-A1}      I_NEG 7       I Sense -       7 I_NEG {0-0}
I_POS

Index   Name            Number          Nodes                   Bridges Do Not
Intersects 8       Net 8           8               7,8 {7-8} 0 9       Net 9
9               D13,D1,A7 {D13-D1,D13-A7} 0




struct chipStatus{

int chipNumber;
char chipChar;
int8_t xStatus[16]; //store the bb row or nano conn this is eventually
connected to so they can be stacked if conns are redundant int8_t yStatus[8];
//store the row/nano it's connected to const int8_t xMap[16]; const int8_t
yMap[8];

};



struct chipStatus ch[12] = {
  {0,'A',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_I, CHIP_J, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E,
CHIP_K, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H},//X MAP constant
  {CHIP_L,  t2,t3, t4, t5, t6, t7, t8}},  // Y MAP constant

  {1,'B',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_I, CHIP_J, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E,
CHIP_E, CHIP_F, CHIP_K, CHIP_G, CHIP_G, CHIP_H, CHIP_H}, {CHIP_L,
t9,t10,t11,t12,t13,t14,t15}},

  {2,'C',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_I, CHIP_J, CHIP_D, CHIP_D, CHIP_E,
CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_K, CHIP_H, CHIP_H}, {CHIP_L,
t16,t17,t18,t19,t20,t21,t22}},

  {3,'D',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_I, CHIP_J, CHIP_E,
CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_K}, {CHIP_L,
t23,t24,t25,t26,t27,t28,t29}},

  {4,'E',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_K, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_I,
CHIP_J, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_H, CHIP_H}, {CHIP_L,   b2, b3,
b4, b5, b6, b7, b8}},

  {5,'F',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_K, CHIP_C, CHIP_C, CHIP_D, CHIP_D, CHIP_E,
CHIP_E, CHIP_I, CHIP_J, CHIP_G, CHIP_G, CHIP_H, CHIP_H}, {CHIP_L,  b9,
b10,b11,b12,b13,b14,b15}},

  {6,'G',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_K, CHIP_D, CHIP_D, CHIP_E,
CHIP_E, CHIP_F, CHIP_F, CHIP_I, CHIP_J, CHIP_H, CHIP_H}, {CHIP_L,
b16,b17,b18,b19,b20,b21,b22}},

  {7,'H',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {CHIP_A, CHIP_A, CHIP_B, CHIP_B, CHIP_C, CHIP_C, CHIP_D, CHIP_K, CHIP_E,
CHIP_E, CHIP_F, CHIP_F, CHIP_G, CHIP_G, CHIP_I, CHIP_J}, {CHIP_L,
b23,b24,b25,b26,b27,b28,b29}},

  {8,'I',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_D1, NANO_A2, NANO_D3, NANO_A4, NANO_D5, NANO_A6, NANO_D7,
NANO_D11, NANO_D9, NANO_D13, NANO_RESET, DAC0, ADC0, SUPPLY_3V3, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {9,'J',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_D0, NANO_A1, NANO_D2, NANO_A3, NANO_D4, NANO_A5, NANO_D6, NANO_A7,
NANO_D8, NANO_D10, NANO_D12, NANO_AREF, DAC1_8V, ADC1_5V, SUPPLY_5V, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {10,'K',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {NANO_A0, NANO_A1, NANO_A2, NANO_A3, NANO_A4, NANO_A5, NANO_A6, NANO_A7,
NANO_D2, NANO_D3, NANO_D4, NANO_D5, NANO_D6, NANO_D7, NANO_D8, NANO_D9},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}},

  {11,'L',
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // x status
  {-1,-1,-1,-1,-1,-1,-1,-1}, //y status
  {ISENSE_MINUS, ISENSE_PLUS, ADC0, ADC1_5V, ADC2_5V, ADC3_8V, DAC1_8V, DAC0,
t1, t30, b1, b30, NANO_A4, NANO_A5, SUPPLY_5V, GND},
  {CHIP_A,CHIP_B,CHIP_C,CHIP_D,CHIP_E,CHIP_F,CHIP_G,CHIP_H}}
  };

enum nanoPinsToIndex       {     NANO_PIN_D0 ,     NANO_PIN_D1 , NANO_PIN_D2
,     NANO_PIN_D3 ,     NANO_PIN_D4 ,     NANO_PIN_D5 ,     NANO_PIN_D6 ,
NANO_PIN_D7 ,     NANO_PIN_D8 ,     NANO_PIN_D9 ,     NANO_PIN_D10 ,
NANO_PIN_D11 ,      NANO_PIN_D12 ,     NANO_PIN_D13 ,       NANO_PIN_RST ,
NANO_PIN_REF ,     NANO_PIN_A0 ,     NANO_PIN_A1 ,     NANO_PIN_A2 ,
NANO_PIN_A3 ,     NANO_PIN_A4 ,     NANO_PIN_A5 ,     NANO_PIN_A6 ,
NANO_PIN_A7 };

extern struct nanoStatus nano;


struct pathStruct{

  int node1; //these are the rows or nano header pins to connect
  int node2;
  int net;

  int chip[3];
  int x[3];
  int y[3];
  int candidates[3][3];

};

*/
