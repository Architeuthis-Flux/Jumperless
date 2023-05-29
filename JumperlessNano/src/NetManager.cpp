

#include "NetManager.h"
#include <Arduino.h>
#include "LittleFS.h"
#include "MatrixStateRP2040.h"

int8_t newNode1 = -1;
int8_t newNode2 = -1;
int newBridge[MAX_BRIDGES][3]; // node1, node2, net
int newBridgeLength;

int newBridgeIndex = 0;

int foundNode1Net = 0; // netNumbers where that node is, a node can only be in 1 net (except current sense, we'll deal with that separately)
int foundNode2Net = 0; // netNumbers where that node is, a node can only be in 1 net (except current sense, we'll deal with that separately)

// StaticJsonDocument <800> bridgeList;

String nodeFileString;
String bridgeString;
String specialFunctionsString;

File nodeFile;

void openNodeFile()
{

    nodeFile = LittleFS.open("nodeFile.txt", "r");
    if (!nodeFile)
    {
        Serial.println("Failed to open nodeFile");
        return;
    }
    else
    {
        Serial.println("\n\ropened nodeFile.txt\n\n\rloading bridges from file\n\r");
    }

    nodeFileString = nodeFile.readString();

    nodeFile.close();
    splitStringToFields();
    // parseStringToBridges();
}

void splitStringToFields()
{
    int openBraceIndex = 0;
    int closeBraceIndex = 0;

    Serial.println("\n\rraw input file\n\r");
    Serial.println(nodeFileString);
    Serial.println("\n\rsplitting and cleaning up string\n\r");
    Serial.println("_");
    openBraceIndex = nodeFileString.indexOf("{");
    closeBraceIndex = nodeFileString.indexOf("}");
    bridgeString = nodeFileString.substring(openBraceIndex + 1, closeBraceIndex);
    bridgeString.trim();

    Serial.println(bridgeString);

    Serial.println("^\n\r");

    nodeFileString.remove(0, closeBraceIndex + 1);
    nodeFileString.trim();

    openBraceIndex = nodeFileString.indexOf("{");
    closeBraceIndex = nodeFileString.indexOf("}");
    specialFunctionsString = nodeFileString.substring(openBraceIndex + 1, closeBraceIndex);
    specialFunctionsString.trim();
    Serial.println("_");
    Serial.println(specialFunctionsString);
    Serial.println("^\n\r");
    replaceSFNamesWithDefinedInts();
}

void replaceSFNamesWithDefinedInts(void)
{
    Serial.println("replacing special function names with defined ints\n\r");
    specialFunctionsString.replace("GND", "100");
    specialFunctionsString.replace("SUPPLY_5V", "105");
    specialFunctionsString.replace("SUPPLY_3V3", "103");
    specialFunctionsString.replace("DAC0_5V", "106");
    specialFunctionsString.replace("DAC1_8V", "107");
    specialFunctionsString.replace("I_N", "109");
    specialFunctionsString.replace("I_P", "108");

    specialFunctionsString.replace("EMPTY_NET", "127");

    Serial.println(specialFunctionsString);
    Serial.println("\n\n\r");
    parseStringToBridges();
}

void parseStringToBridges(void)
{

    int bridgeStringLength = bridgeString.length() - 1;

    int specialFunctionsStringLength = specialFunctionsString.length() - 1;

    int readLength = 0;
    int readTotal = specialFunctionsStringLength;

    newBridgeLength = 0;
    newBridgeIndex = 0;

     Serial.println("parsing bridges into array\n\r");

    for (int i = 0; i <= specialFunctionsStringLength; i += readLength)
    {
       
        sscanf(specialFunctionsString.c_str(), "%i-%i,\n\r%n", &newBridge[newBridgeIndex][0], &newBridge[newBridgeIndex][1], &readLength);
        specialFunctionsString.remove(0, readLength);

        readTotal -= readLength;

        // Serial.print(newBridge[newBridgeIndex][0]);
        // Serial.print("-");
        // Serial.println(newBridge[newBridgeIndex][1]);

        newBridgeLength++;
        newBridgeIndex++;

        // delay(500);
    }

    readTotal = bridgeStringLength;

    for (int i = 0; i <= bridgeStringLength; i += readLength)
    {

        sscanf(bridgeString.c_str(), "%i-%i,\n\r%n", &newBridge[newBridgeIndex][0], &newBridge[newBridgeIndex][1], &readLength);
        bridgeString.remove(0, readLength);

        readTotal -= readLength;

        // Serial.print(newBridge[newBridgeIndex][0]);
        // Serial.print("-");
        // Serial.println(newBridge[newBridgeIndex][1]);

        newBridgeLength++;
        newBridgeIndex++;

        // delay(500);
    }
        for (int i = 0; i < newBridgeLength; i++)
    {
        Serial.print("[");
        Serial.print(newBridge[i][0]);
        Serial.print("-");
        Serial.print(newBridge[i][1]);
        Serial.print("],");
    }
    Serial.print("\n\rbridge pairs = ");
    Serial.println(newBridgeLength);
    // Serial.println(nodeFileString);
}

void getNodesToConnect() // read in the nodes you'd like to connect
{

    Serial.println("\n\n\rconnecting nodes into nets\n\r");

    for (int i = 0; i < newBridgeLength; i++)
    {
        newNode1 = newBridge[i][0];

        newNode2 = newBridge[i][1];

        newBridgeIndex++;

        printNodeOrName(newNode1);
        Serial.print("-");
        printNodeOrName(newNode2);
        Serial.print("\n\r");

        // do some error checking

        if (newNode1 == 0 || newNode2 == 0)
        {
            // listNets();
            // return ;
        }
        else
        {
            searchExistingNets(newNode1, newNode2);
        }

       // if (i < 7)
       // {
            listSpecialNets();
       // }

            listNets();
      
        
    }
    Serial.println("done");
}

int searchExistingNets(int node1, int node2) // search through existing nets for all nodes that match either one of the new nodes (so it will be added to that net)
{

    foundNode1Net = 0;
    foundNode2Net = 0;

    int foundNode1inSpecialNet = 0;
    int foundNode2inSpecialNet = 0;

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
                if (i > 7)
                {
                    Serial.print("found Node ");
                    printNodeOrName(node1);
                    Serial.print(" in Net ");
                    Serial.println(i);
                }

                if (net[i].specialFunction > 0)
                {
                    foundNode1Net = i;
                    foundNode1inSpecialNet = i;
                }
                else
                {
                    foundNode1Net = i;
                }
            }
            if (net[i].nodes[j] == node2)
            {
                if (i > 7)
                {
                    Serial.print("found Node ");
                    printNodeOrName(node2);
                    Serial.print(" in Net ");
                    Serial.println(i);
                }

                if (net[i].specialFunction > 0)
                {
                    foundNode2Net = i;
                    foundNode2inSpecialNet = i;
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
        addBridgeToNet(foundNode1Net, node1, node2);
        return 1;
    }
    else if ((foundNode1Net > 0 && foundNode2Net > 0) && (foundNode1Net != foundNode2Net)) // if both nodes are in different nets, combine them
    {
        if (checkDoNotIntersectsByNet(foundNode1Net, foundNode2Net) == 1 )
        {
            int swap = 0;
            if ( (foundNode2Net <= 7 && foundNode1Net <= 7))
            {
                    Serial.print("can't combine Special Nets, skipping\n\r"); //maybe have it add a bridge between them if it's allowed?
            } else {

                if (foundNode2Net <= 7)
                {
                    swap = foundNode1Net;
                    foundNode1Net = foundNode2Net;
                    foundNode2Net = swap;
                }

            Serial.print("combining Nets ");
            Serial.print(foundNode1Net);
            Serial.print(" and ");
            Serial.println(foundNode2Net);
            // Serial.println("before");
            // listNets();
            //  Serial.println("after");
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
            for (int i = 0; i < MAX_DNI; i++)
            {
                if (net[foundNode2Net].doNotIntersectNodes[i] == 0)
                {
                    break;
                }

                addNodeToNet(foundNode1Net, net[foundNode2Net].doNotIntersectNodes[i]);
            }

            deleteNet(foundNode2Net);
            }
        }
        else
        {
            //createNewNet();
        }
       

        return 2;
    }
    else if (foundNode1Net > 0 && node2 > 0) // if node1 is in a net and node2 is not, add node2 to node1's net
    {
        if (checkDoNotIntersectsByNode(foundNode1Net, node2) == 1)
        {
            Serial.print("adding Node ");
            printNodeOrName(node2);
            Serial.print(" to Net ");
            Serial.println(foundNode1Net);

            addNodeToNet(foundNode1Net, node2);
            addBridgeToNet(foundNode1Net, node1, node2);
        }
        else
        {
            createNewNet();
        }

        return 3;
    }
    else if (foundNode2Net > 0 && node1 > 0) // if node2 is in a net and node1 is not, add node1 to node2's net
    {
        if (checkDoNotIntersectsByNode(foundNode2Net, node1) == 1)
        {
            Serial.print("adding Node ");
            printNodeOrName(node1);
            Serial.print(" to Net ");
            Serial.println(foundNode2Net);

            addNodeToNet(foundNode2Net, node1);
            addBridgeToNet(foundNode2Net, node1, node2);
        }
        else
        {
            createNewNet();
        }
        return 4;
    }

    else
    {

        createNewNet(); // if neither node is in a net, create a new one

        return 0;
    }
}

int printNodeOrName(int node) // returns number of characters printed (for tabs)
{
    if (node >= 100)
    {
        return Serial.print(definesToChar(node));
    }
    else
    {
        return Serial.print(node);
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
    Serial.print("deleted Net ");
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
    if (net[8].number == 0)
    {
        //Serial.print("No nets to list\n\r");
        //return;
    } else {
    Serial.print("\n\rIndex\tName\t\tNumber\t\tNodes\t\t\tBridges\t\t\t\tDo Not Intersects");

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

            tabs += printNodeOrName(net[i].nodes[j]);

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

        tabs = 0;
        for (int j = 0; j < MAX_BRIDGES; j++)
        {

            tabs += printNodeOrName(net[i].bridges[j][0]);
            tabs += Serial.print("-");
            tabs += printNodeOrName(net[i].bridges[j][1]);
            // Serial.print(",");

            if (net[i].bridges[j + 1][0] == 0)
            {
                break;
            }
            else
            {

                tabs += Serial.print(",");
            }
        }
        tabs += Serial.print("}\t");

        for (int i = 0; i < 3 - (tabs / 8); i++)
        {
            Serial.print("\t");
        }

        for (int j = 0; j < MAX_DNI; j++)
        {

            tabs += printNodeOrName(net[i].doNotIntersectNodes[j]);

            if (net[i].doNotIntersectNodes[j + 1] == 0)
            {
                break;
            }
            else
            {

                tabs += Serial.print(",");
            }
        }
    }
    }
    Serial.print("\n\n\n\r");
}

void listSpecialNets()
{
    Serial.print("\n\rIndex\tName\t\tNumber\t\tNodes\t\t\tBridges\t\t\t\tDo Not Intersects");
    int tabs = 0;
    for (int i = 0; i < 8; i++)
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
        Serial.print("\t");
        Serial.print(net[i].number);
        Serial.print("\t\t");

        tabs = 0;
        for (int j = 0; j < MAX_NODES; j++)
        {
            tabs += printNodeOrName(net[i].nodes[j]);
            // tabs += Serial.print(definesToChar(net[i].nodes[j]));

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

        tabs = 0;
        for (int j = 0; j < MAX_BRIDGES; j++)
        {

            tabs += printNodeOrName(net[i].bridges[j][0]);
            tabs += Serial.print("-");
            tabs += printNodeOrName(net[i].bridges[j][1]);
            // Serial.print(",");

            if (net[i].bridges[j + 1][0] == 0)
            {
                break;
            }
            else
            {

                tabs += Serial.print(",");
            }
        }
        tabs += Serial.print("}\t");

        for (int i = 0; i < 3 - (tabs / 8); i++)
        {
            Serial.print("\t");
        }

        for (int j = 0; j < MAX_DNI; j++)
        {

            tabs += printNodeOrName(net[i].doNotIntersectNodes[j]);

            if (net[i].doNotIntersectNodes[j + 1] == 0 || i == 0)
            {
                break;
            }
            else
            {

                tabs += Serial.print(",");
            }
        }
    }
    Serial.print("\n\r");
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
            Serial.print("found unused Net ");
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
            // Serial.print("found unused bridge ");
            // Serial.println(i);

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

int checkDoNotIntersectsByNet(int netToCheck1, int netToCheck2) //If you're searching DNIs by net, there won't be any valid ways to make a new net with both nodes, so its skipped
{
    int problem = 0;

    for (int i = 0; i <= MAX_DNI; i++)
    {
        if (net[netToCheck1].doNotIntersectNodes[i] == 0)
        {
             break;
        }

        for (int j = 0; j <= MAX_NODES; j++)
        {   //Serial.print (net[netToCheck1].doNotIntersectNodes[i]);
            //Serial.print ("-");
           // Serial.println (net[netToCheck2].nodes[j]);
            
            if (net[netToCheck2].nodes[j] == 0)
            {
                break;
            }

            if (net[netToCheck1].doNotIntersectNodes[i] == net[netToCheck2].nodes[j])
            {
                Serial.print("Net ");
                printNodeOrName(netToCheck2);
                Serial.print(" can't be combined with Net ");
                Serial.print(netToCheck1);
                Serial.print(" due to Do Not Intersect rules, skipping (first net DNI to second net nodes)\n\r"); 
                return 0;
                problem ++;
            }
        }
        //Serial.println (" ");
    }

    for (int i = 0; i <= MAX_DNI; i++)
    {
        if (net[netToCheck2].doNotIntersectNodes[i] == 0)
        {
             break;
        }

        for (int j = 0; j <= MAX_NODES; j++)
        {
            if (net[netToCheck1].nodes[j] == 0)
            {
                break;
            }

            if (net[netToCheck2].doNotIntersectNodes[i] == net[netToCheck1].nodes[j])
            {
                Serial.print("Net ");
                printNodeOrName(netToCheck2);
                Serial.print(" can't be combined with Net ");
                Serial.print(netToCheck1);
                Serial.print(" due to Do Not Intersect rules, skipping(second net DNI to first net nodes)\n\r");
                return 0;
            }
        }
    }

    return 1; // return 1 if it's ok to connect these nets
}

int checkDoNotIntersectsByNode(int netToCheck, int nodeToCheck) // make sure none of the nodes on the net violate doNotIntersect rules, exit and warn
{

    for (int i = 0; i < MAX_DNI; i++)
    {
        if (net[netToCheck].doNotIntersectNodes[i] == 0)
        {
            break;
        }

        if (net[netToCheck].doNotIntersectNodes[i] == nodeToCheck)
        {
            Serial.print("Node ");
            printNodeOrName(nodeToCheck);
            Serial.print(" is not allowed on Net ");
            Serial.print(netToCheck);
            Serial.print(" due to Do Not Intersect rules, a new net will be created\n\r");
            return 0;
        }
    }

    return 1; // return 1 if it's ok to connect these nets
}
/*


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