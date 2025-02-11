// SPDX-License-Identifier: MIT
#ifndef NETTOCHIPCONNECTIONS_H
#define NETTOCHIPCONNECTIONS_H
// #include "JumperlessDefinesRP2040.h"




extern int numberOfUniqueNets;
extern int numberOfNets;
extern int numberOfPaths;

extern bool debugNTCC;
extern bool debugNTCC2;

extern int powerPriority;
extern int dacPriority;
extern int powerDuplicates;
extern int dacDuplicates;
extern int pathDuplicates;

extern int numberOfUnconnectablePaths;
extern int unconnectablePaths[10][2];


// extern int newBridges[MAX_NETS][MAX_DUPLICATE][2];

void clearAllNTCC(void);

void sortPathsByNet(void);  
void bridgesToPaths(int fillUnused = 1, int allowStacking = 0);

void findStartAndEndChips(int node1, int node2, int net);

void couldntFindPath(int forcePrint = 0);

void resolveChipCandidates();

void printPathArray();

int defToNano(int nanoIndex);

void bbToSfConnections(void);

char chipNumToChar(int);
int printChipNumToChar(int);

void clearChipsOnPathToNegOne(void);

void sortAllChipsLeastToMostCrowded(void);

void sortSFchipsLeastToMostCrowded(void);

int moreAvailableChip (int chip1 , int chip2);

void setChipStatusOtherSide(void);

int xMapForNode(int node, int chip);


int yMapForNode(int node, int chip);


int xMapForChipLane0(int chip, int chip2);
int xMapForChipLane1(int chip, int chip2);


void mergeOverlappingCandidates (int pathIndex);


void assignPathType (int pathIndex);

int printPathType (int pathIndex);

void swapNodes (int);

int ijklPaths(int pathNumber, int allowStacking = 0);

void commitPaths(int allowStacking = 0,int powerOnly = -1);
int checkForOverlappingPaths(void);
void printPathsCompact(int showCullDupes = 1);

void resolveAltPaths(int allowStacking = 0, int powerOnly = -1);

void printChipStatus(void);

void duplicateSFnets (void);

void swapDuplicateNode (int);

void resolveUncommittedHops(int allowStacking = 0, int powerOnly = -1);
void resolveUncommittedHops2(void);

bool freeOrSameNetX(int chip, int x, int net, int allowStacking = 0);
bool freeOrSameNetY(int chip, int x, int net, int allowStacking = 0);
bool frontEnd(int chip, int y = -1, int x = -1);

void fillUnusedPaths(int duplicatePathsOverride = 1, int duplicatePathsPower = 2, int duplicatePathsDac = 1);





#endif
