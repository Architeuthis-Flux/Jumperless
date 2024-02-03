// SPDX-License-Identifier: MIT

#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
//#include "MatrixStateRP2040.h"

extern int newBridge[MAX_BRIDGES][3]; // node1, node2, net
extern int newBridgeLength;
extern int newBridgeIndex;
extern bool debugNM;
extern bool debugNMtime;


void writeJSONtoFile();

int printNodeOrName(int node, int longOrShort = 0); //0 = short, 1 = long

void getNodesToConnect(); //read in the nodes you'd like to connect

int searchExistingNets(int,int); //search through existing nets for all nodes that match either one of the new nodes (so it will be added to that net)

int findFirstUnusedNetIndex(); //search for a free net[] 

int findFirstUnusedBridgeIndex(int netNumber);

int findFirstUnusedNodeIndex(int netNumber);

void createNewNet(); //add those nodes to a new net

void nameNet(int);

void addNodeToNet(int, int); //just add those nodes to the net

void addBridgeToNet(uint8_t, int8_t, int8_t); //just add those nodes to the net

int checkDoNotIntersectsByNet(int,int); //make sure none of the nodes on the net violate doNotIntersect rules, exit and warn

int checkDoNotIntersectsByNode(int,int); //make sure none of the nodes on the net violate doNotIntersect rules, exit and warn

void combineNets(int,int); //combine those 2 nets into a single net, probably call addNodesToNet and deleteNet and just expand the lower numbered one. Should we shift nets down? or just reuse the newly emply space for the next net

void deleteNet(int); //make sure to check special function nets and clear connections to it

int shiftNets(int); //returns last net number to be deleted

void deleteBridge();

void deleteNode(); //disconnects everything connected to that one node

const char* definesToChar (int defined, int longOrShort = 0); //0 = short, 1 = long

void printBridgeArray();



void checkIfNodesAreABridge(); //if both of those nodes make up a memberBridge[][] pair. if not, warn and exit

void deleteBridgeAndShift(); //shift the remaining bridges over so they're left justified and we don't need to search the entire memberBridges[] every time

void deleteNodesAndShift(); //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

void deleteAllBridgesConnectedToNode(); //search bridges for node and delete any that contain it

void deleteNodesAndShift(); //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

void checkForSplitNets(); //if the newly deleted nodes wold split a net into 2 or more non-intersecting nets, we'll need to split them up. return numberOfNewNets check memberBridges[][] https://www.geeksforgeeks.org/check-removing-given-edge-disconnects-given-graph/#

void copySplitNetIntoNewNet(); //find which nodes and bridges belong in a new net

void deleteNodesAndShift(); //delete the nodes and bridges that were copied from the original net

void leftShiftNodesBridgesNets();

void listNets();

void listSpecialNets();


#endif





/*
functions needed to deal with nets ( Now we're doing   Nets > Bridges > Nodes)

getNodesToConnect() //read in the nodes you'd like to connect

searchExistingNets() //search through existing nets for all nodes that match either one of the new nodes (so it will be added to that net)
{
  if (no nets share a node)

    findFirstUnusedNet() //search for a free net[] 

    createNewNet() //add those nodes to a new net


  if (exactly 1 existing net shares a node)

    addNodesToNet() //just add those nodes to the net


  if (nodes touch separate nets (and priority matches))

    checkDoNotIntersects() //make sure none of the nodes on the net violate doNotIntersect rules, exit and warn

    combineNets() //combine those 2 nets into a single net, probably call addNodesToNet and deleteNet and just expand the lower numbered one. Should we shift nets down? or just reuse the newly emply space for the next net
}


deleteNet() //make sure to check special function nets and clear connections to it


deleteBridge()
{
    checkIfNodesAreABridge() //if both of those nodes make up a memberBridge[][] pair. if not, warn and exit

    deleteBridgeAndShift() //shift the remaining bridges over so they're left justified and we don't need to search the entire memberBridges[] every time

    deleteNodesAndShift() //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

}


deleteNode() //disconnects everything connected to that one node
{
  searchExistingNets() //find where that node is

  if (it's one of only 2 nodes in a net)

    deleteNet()

  else 

    deleteAllBridgesConnectedToNode() //search bridges for node and delete any that contain it

    deleteNodesAndShift() //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

}


checkForSplitNets() //if the newly deleted nodes wold split a net into 2 or more non-intersecting nets, we'll need to split them up. return numberOfNewNets check memberBridges[][] https://www.geeksforgeeks.org/check-removing-given-edge-disconnects-given-graph/#
{
  while (numberOfNewNets > 0)

    findFirstUnusedNet() //search for a free net[] 

    createNewNet() //add those nodes to a new net

    copySplitNetIntoNewNet() //find which nodes and bridges belong in a new net

    deleteNodesAndShift() //delete the nodes and bridges that were copied from the original net

    numberOfNewNets--;

}

eventually send data to crosspoints (after we convert the net data into actual X Y CHIP data)

Now that we're recalculating nets at every step, 



*/
