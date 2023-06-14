#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "NetManager.h"
#include "MatrixStateRP2040.h"
#include "LittleFS.h"
#include "FileParsing.h"
#include "NetsToChipConnections.h"
#include "LEDs.h"
#include "CH446Q.h"
#include "Peripherals.h"
#include <Wire.h>
#include <Adafruit_MCP4725.h>

// nanoStatus nano;
const char *definesToChar(int); // i really need to find a way to not need to forward declare fuctions with this setup, i hate it



void setup()
{


  initCH446Q();
  initADC();
   initDAC();
   initINA219();
  Serial.begin(115200);

  initLEDs();
  LittleFS.begin();
  //delay(3000);
  //rainbowy(255,253,100);
  parseWokwiFileToNodeFile();
  openNodeFile();
  //while(1);

}

void loop()
{

  getNodesToConnect();

  Serial.println("\n\n\rfinal netlist\n\n\r");
  listSpecialNets();
  listNets();
  printBridgeArray();
//rainbowy(255,30,100);
  bridgesToPaths();
  assignNetColors();
  //showNets();
 
  //delay(3000);
    sendAllPaths();
   
   int i = 0;
  while (1)
  {
    //sendAllPaths();

 //dacSine(1);

    //setDac0_5V(3.3);
waveGen();
    //
    //dacTriangle();
    //delay(1200);
  }
}
