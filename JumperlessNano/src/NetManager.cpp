

#include "NetManager.h"
#include <Arduino.h>
// #include "JumperlessDefinesRP2040.h"
// #include "nodeFile.txt"
#include "LittleFS.h"

#include "MatrixStateRP2040.h"

int8_t newNode1 = -1;
int8_t newNode2 = -1;
int8_t newBridge[MAX_BRIDGES][3]; // node1, node2, net

FILE *nodeFile;

int nodePairIndex = 0;

int foundNode1Net = 0; // netNumbers where that node is, a node can only be in 1 net (except current sense, we'll deal with that separately)
int foundNode2Net = 0; // netNumbers where that node is, a node can only be in 1 net (except current sense, we'll deal with that separately)

void getNodesToConnect() // read in the nodes you'd like to connect
{

    char readBuffer1[8];
    char readBuffer2[8];

    LittleFS.begin();

    File f = LittleFS.open("nodeFile.txt", "r");
    if (f.available())
    {
        for (int i = 0; i < nodePairIndex; i++)
        {
            for (int k = 0; k < 8; k++)
            {
                readBuffer1[k] = ' ';
                readBuffer2[k] = ' ';
            }

            f.readBytesUntil('-', readBuffer1, 4);
            f.readBytesUntil(',', readBuffer2, 4);
        }

        newNode1 = atoi(readBuffer1);
        newNode2 = atoi(readBuffer2);

        Serial.print(newNode1);
        Serial.print("-");
        Serial.println(newNode2);

        // do some error checking

        if (newNode1 == 0 || newNode2 == 0)
        {
            listNets();
            // return ;
        }
        searchExistingNets(newNode1, newNode2);
        nodePairIndex++;
    }
    else
    {
        LittleFS.end();
        listNets();
        nodePairIndex = 0;
    }
}

int searchExistingNets(int node1, int node2) // search through existing nets for all nodes that match either one of the new nodes (so it will be added to that net)
{

    foundNode1Net = 0;
    foundNode2Net = 0;

    int foundNode1OnSpecialNet = 0;
    int foundNode2OnSpecialNet = 0;

    for (int i = 1; i < MAX_NETS; i++)
    {
        if (net[i].number == 0) // stops searching if it gets to an unallocated net
        {
            break;
        }

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[i].nodes[j] == 0)
            {
                break;
            }

            if (net[i].nodes[j] == node1)
            {
                Serial.print("found node 1 in net ");
                Serial.println(i);

                if (net[i].specialFunction > 0)
                {
                    foundNode1OnSpecialNet = i;
                }
                else
                {
                    foundNode1Net = i;
                }
            }
            if (net[i].nodes[j] == node2)
            {
                Serial.print("found node 2 in net ");
                Serial.println(i);

                if (net[i].specialFunction > 0)
                {
                    foundNode2OnSpecialNet = i;
                }
                else
                {
                    foundNode2Net = i;
                }
            }
        }
    }

    if (foundNode1Net == foundNode2Net && foundNode1Net > 0) // if both nodes are in the same net, do nothing
    {

        addNodeToNet(foundNode1Net, node1); // note that they both connect to node1's net
        addNodeToNet(foundNode1Net, node2);
        return 1;
    }
    else if ((foundNode1Net > 0 && foundNode2Net > 0) && (foundNode1Net != foundNode2Net)) // if both nodes are in different nets, combine them
    {
        Serial.print("combining nets ");
        Serial.print(foundNode1Net);
        Serial.print(" and ");
        Serial.println(foundNode2Net);
        listNets();
        for (int i = 0; i < MAX_NODES; i++)
        {
            if (net[foundNode2Net].nodes[i] == 0)
            {
                break;
            }

            addNodeToNet(foundNode1Net, net[foundNode2Net].nodes[i]);
        }

        for (int i = 0; i < MAX_BRIDGES; i++)
        {
            if (net[foundNode2Net].bridges[i][0] == 0)
            {
                break;
            }

            addBridgeToNet(foundNode1Net, net[foundNode2Net].bridges[i][0], net[foundNode2Net].bridges[i][1]);
        }

        deleteNet(foundNode2Net);

        // net[foundNode2Net].number = 0; // clear the old net //make this a function

        return 2;
    }
    else if (foundNode1Net > 0 && node2 > 0) // if node1 is in a net and node2 is not, add node2 to node1's net
    {
        Serial.print("adding node2 to net ");
        Serial.println(foundNode1Net);

        addNodeToNet(foundNode1Net, node2);

        return 3;
    }
    else if (foundNode2Net > 0 && node1 > 0) // if node2 is in a net and node1 is not, add node1 to node2's net
    {
        Serial.print("adding node1 to net ");
        Serial.println(foundNode2Net);

        addNodeToNet(foundNode2Net, node1);

        return 4;
    }

    else
    {

        createNewNet(); // if neither node is in a net, create a new one

        return 0;
    }
}

void deleteNet(int netNumber) // make sure to check special function nets and clear connections to it
{
    shiftNets(netNumber);
}

int shiftNets(int deletedNet) // why in the ever-loving fuck does this work? there's no recursion but somehow it moves all the nets
{
    int lastNet;

    for (int i = MAX_NETS - 2; i > 0; i--)
    {
        if (net[i].number != 0)
        {
            lastNet = i;
            // Serial.print("last net = ");
            // Serial.println(lastNet);
            break;
        }
    }
    Serial.print("deleted net = ");
    Serial.println(deletedNet);

    for (int i = deletedNet; i < lastNet; i++)
    {
        net[i] = net[i + 1];
        net[i].name = netNameConstants[i];
        net[i].number = i;
    }

    net[lastNet].number = 0;
    net[lastNet].name = "       "; // netNameConstants[lastNet];
    net[lastNet].priority = 0;
    net[lastNet].specialFunction = -1;

    for (int i = 0; i < 8; i++)
    {
        net[lastNet].intersections[i] = 0;
        net[lastNet].doNotIntersectNodes[i] = 0;
    }
    for (int j = 0; j < MAX_NODES; j++)
    {
        if (net[lastNet].nodes[j] == 0)
        {
            break;
        }

        net[lastNet].nodes[j] = 0;
    }

    for (int j = 0; j < MAX_BRIDGES; j++)
    {
        if (net[lastNet].bridges[j][0] == 0)
        {
            break;
        }

        net[lastNet].bridges[j][0] = 0;
        net[lastNet].bridges[j][1] = 0;
    }
    return lastNet;
}

void checkCurrentSense(void)
{
    return; // I might deal with current sense in a different way because this is dumb
}

void listNets(void)
{
    Serial.print("\n\rIndex\tName\t\tNumber\t\tNodes\t\t\tBridges\n\r");
    int tabs = 0;
    for (int i = 8; i < MAX_NETS; i++)
    {
        if (net[i].number == 0) // stops searching if it gets to an unallocated net
        {
            // Serial.print("Done listing nets");
            break;
        }

        Serial.print("\n\r");
        Serial.print(i);
        Serial.print("\t");
        Serial.print(net[i].name);
        Serial.print("\t\t");
        Serial.print(net[i].number);
        Serial.print("\t\t");

        tabs = 0;
        for (int j = 0; j < MAX_NODES; j++)
        {

            tabs += Serial.print(net[i].nodes[j]);

            if (net[i].nodes[j + 1] == 0)
            {
                break;
            }
            else
            {

                tabs += Serial.print(",");
            }
        }

        for (int i = 0; i < 3 - (tabs / 8); i++)
        {
            Serial.print("\t");
        }

        Serial.print("{");

        for (int j = 0; j < MAX_BRIDGES; j++)
        {

            Serial.print(net[i].bridges[j][0]);
            Serial.print("-");
            Serial.print(net[i].bridges[j][1]);
            // Serial.print(",");

            if (net[i].bridges[j + 1][0] == 0)
            {
                break;
            }
            else
            {

                Serial.print(",");
            }
        }
        Serial.print("}\t");
    }
    Serial.print("\n\n\r");
}

void listSpecialNets()
{
    for (int i = 0; i < MAX_NETS; i++)
    {
        if (net[i].number == 0) // stops searching if it gets to an unallocated net
        {
            // Serial.print("Done listing nets");
            break;
        }

        if (net[i].specialFunction > 0)
        {
            Serial.print("\n\r");
            Serial.print(net[i].name), DEC;
            Serial.print("\t");
            Serial.print(net[i].number);
            Serial.print("\t");
            Serial.print(definesToChar(net[i].specialFunction));
            if (i == 1)
                Serial.print("\t"); // padding for "GND"
            Serial.print("\t{");

            for (int k = 0; k < 8; k++)
            {
                if (net[i].doNotIntersectNodes[k] != 0)
                {

                    Serial.print(definesToChar(net[i].doNotIntersectNodes[k]));
                    Serial.print(",");
                }
            }
            Serial.print("}\t\t\t");

            for (int j = 0; j < MAX_NODES; j++)
            {
                if (net[i].nodes[j] == 0)
                {
                    break;
                }

                Serial.print(definesToChar(net[i].nodes[j]));
                Serial.print(",");

                Serial.print(net[i].nodes[j]);
                Serial.print(",");
            }

            Serial.print("\t{");

            for (int j = 0; j < MAX_BRIDGES; j++)
            {
                if (net[i].bridges[j][0] == 0)
                {
                    break;
                }

                Serial.print(definesToChar(net[i].bridges[j][0]));
                Serial.print("-");
                Serial.print(definesToChar(net[i].bridges[j][1]));
                Serial.print(",");
            }
            Serial.print("}\t");
        }
    }
}

void createNewNet() // add those nodes to a new net
{
    int newNetNumber = findFirstUnusedNetIndex();
    net[newNetNumber].number = newNetNumber;

    net[newNetNumber].name = netNameConstants[newNetNumber]; // dont need a function for this anymore

    net[newNetNumber].specialFunction = -1;

    addNodeToNet(newNetNumber, newNode1);

    addNodeToNet(newNetNumber, newNode2);

    addBridgeToNet(newNetNumber, newNode1, newNode2);
}


void addBridgeToNet(uint8_t netToAddBridge, int8_t node1, int8_t node2) // just add those nodes to the net
{
    int newBridgeIndex = findFirstUnusedBridgeIndex(netToAddBridge);
    net[netToAddBridge].bridges[newBridgeIndex][0] = node1;
    net[netToAddBridge].bridges[newBridgeIndex][1] = node2;
}

void addNodeToNet(int netToAddNode, int node)
{
    int newNodeIndex = findFirstUnusedNodeIndex(netToAddNode); // using a function lets us add more error checking later and maybe shift the nodes down so they're left justified
    net[netToAddNode].nodes[newNodeIndex] = node;
}

int findFirstUnusedNetIndex() // search for a free net[]
{
    for (int i = 0; i < MAX_NETS; i++)
    {
        if (net[i].nodes[0] == 0)
        {
            //Serial.print("found unused net ");
            Serial.println(i);

            return i;
            break;
        }
    }
    return 0x7f;
}

int findFirstUnusedBridgeIndex(int netNumber)
{
    for (int i = 0; i < MAX_BRIDGES; i++)
    {
        if (net[netNumber].bridges[i][0] == 0)
        {
            //Serial.print("found unused bridge ");
            Serial.println(i);

            return i;
            break;
        }
    }
    return 0x7f;
}

int findFirstUnusedNodeIndex(int netNumber) // search for a free net[]
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (net[netNumber].nodes[i] == 0)
        {
            // Serial.printf("found unused node index net[%d]. node[%d]\n\r", netNumber, i);
            //  Serial.println(i);

            return i;
            break;
        }
    }
    return 0x7f;
}

void checkDoNotIntersects() // make sure none of the nodes on the net violate doNotIntersect rules, exit and warn
{
    for (int i = 0; i < MAX_NETS; i++)
    {
        if (net[i].number == 0) // stops searching if it gets to an unallocated net
        {
            break;
        }

        for (int j = 0; j < MAX_NODES; j++)
        {
            if (net[i].nodes[j] == 0)
            {
                break;
            }

            for (int k = 0; k < 8; k++)
            {
                if (net[i].doNotIntersectNodes[k] == 0)
                {
                    break;
                }

                if (net[i].nodes[j] == net[i].doNotIntersectNodes[k])
                {
                    Serial.print("you tried to connect a node that is not allowed on this net, skipping");
                    return;
                }
            }
        }
    }
}

/*
void addNodesToNet(); //just add those nodes to the net

void checkDoNotIntersects(); //make sure none of the nodes on the net violate doNotIntersect rules, exit and warn

void combineNets(); //combine those 2 nets into a single net, probably call addNodesToNet and deleteNet and just expand the lower numbered one. Should we shift nets down? or just reuse the newly emply space for the next net

void deleteNet(); //make sure to check special function nets and clear connections to it

void deleteBridge();

void deleteNode(); //disconnects everything connected to that one node

void checkIfNodesAreABridge(); //if both of those nodes make up a memberBridge[][] pair. if not, warn and exit

void deleteBridgeAndShift(); //shift the remaining bridges over so they're left justified and we don't need to search the entire memberBridges[] every time

void deleteNodesAndShift(); //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

void deleteAllBridgesConnectedToNode(); //search bridges for node and delete any that contain it

void deleteNodesAndShift(); //shift the remaining nodes over so they're left justified and we don't need to search the entire memberNodes[MAX_NODES] every time

void checkForSplitNets(); //if the newly deleted nodes wold split a net into 2 or more non-intersecting nets, we'll need to split them up. return numberOfNewNets check memberBridges[][] https://www.geeksforgeeks.org/check-removing-given-edge-disconnects-given-graph/#

void copySplitNetIntoNewNet(); //find which nodes and bridges belong in a new net

void deleteNodesAndShift(); //delete the nodes and bridges that were copied from the original net

void leftShiftNodesBridgesNets();*/