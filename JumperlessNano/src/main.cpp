#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "NetManager.h"
#include "MatrixStateRP2040.h"
#include "LittleFS.h"
#include "FileParsing.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "CH446Q.h"

// nanoStatus nano;
const char *definesToChar(int); // i really need to find a way to not need to forward declare fuctions with this setup, i hate it



void setup()
{

  initCH446Q();

  Serial.begin(115200);

  initLEDs();
  LittleFS.begin();
  delay(3000);
  openNodeFile();

}

void loop()
{

  getNodesToConnect();

  Serial.println("\n\n\rfinal netlist\n\n\r");
  listSpecialNets();
  listNets();
  printBridgeArray();

  bridgesToPaths();
  assignNetColors();
  
  delay(3000);
  while (1)
  {

    sendAllPaths();

    delay(1200);
  }
}
