#ifndef NETTOCHIPCONNECTIONS_H
#define NETTOCHIPCONNECTIONS_H





extern int numberOfUniqueNets;
extern int numberOfNets;
extern int numberOfPaths;

void sortPathsByNet(void);  
void bridgesToPaths(void);

void findStartAndEndChips(int node1, int node2, int net);

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



int xMapForNode(int node, int chip);


int yMapForNode(int node, int chip);


int xMapForChipLane0(int chip, int chip2);
int xMapForChipLane1(int chip, int chip2);


void mergeOverlappingCandidates (int pathIndex);


void assignPathType (int pathIndex);

int printPathType (int pathIndex);

void swapNodes (int);

void commitPaths(void);

void printPathsCompact(void);

void resolveAltPaths(void);

void printChipStatus(void);

void duplicateSFnets (void);

void swapDuplicateNode (int);

void resolveUncommittedHops(void);










#endif