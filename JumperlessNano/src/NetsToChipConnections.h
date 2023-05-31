#ifndef NETTOCHIPCONNECTIONS_H
#define NETTOCHIPCONNECTIONS_H







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

void sortSFchipsMostToLeastCrowded(void);

int moreAvailableChip (int chip1 , int chip2);























#endif