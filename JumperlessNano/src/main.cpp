

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "NetManager.h"
#include "MatrixStateRP2040.h"

#include "NetsToChipConnections.h"
#include "LEDs.h"
// #include "CH446Q.h"
#include "Peripherals.h"
#include <Wire.h>

// #include <EEPROM.h>
// #include "ArduinoStuff.h"


#ifdef EEPROMSTUFF
#include <EEPROM.h>
#endif



#ifdef PIOSTUFF
#include "CH446Q.h"
#endif



#include "FileParsing.h"

#ifdef FSSTUFF
#include "LittleFS.h"

#endif

// https://wokwi.com/projects/367384677537829889

// nanoStatus nano;
const char *definesToChar(int); // i really need to find a way to not need to forward declare fuctions with this setup, i hate it

void setup()
{
  //
  // initArduino();
debugFlagInit();

#ifdef EEPROMSTUFF
  EEPROM.begin(256);

  
#endif
#ifdef PIOSTUFF
  initCH446Q();
#endif

  initADC();
  initDAC();
  initINA219();
  Serial.begin(115200);

  initLEDs();

#ifdef FSSTUFF
  LittleFS.begin();
#endif

  lightUpRail(-1, 1, 220);

  // if (EEPROM.read(CLEARBEFORECOMMANDADDRESS) == 0)
  //{
#ifdef FSSTUFF5
  if (LittleFS.exists("/nodeFile.txt"))
  {
    delay(20);
    openNodeFile();
    getNodesToConnect();

    Serial.println("\n\n\rnetlist\n\n\r");

    bridgesToPaths();

    listSpecialNets();
    listNets();
    printBridgeArray();
    Serial.print("\n\n\r");
    Serial.print(numberOfNets);

    Serial.print("\n\n\r");
    Serial.print(numberOfPaths);

    assignNetColors();
#ifdef PIOSTUFF
    sendAllPaths();
#endif
  }
  else
  {
    while (Serial.available() > 0)
    {
      Serial.read();
      delay(1);
    }

    delay(20);
  }
  //} else
  // {
  //   delay(20);
  //}


  // parseWokwiFileToNodeFile();
  // openNodeFile();
  // while(1);
  // lastCommandRead();

#endif
}

void loop()
{

  // getNodesToConnect();
  // bridgesToPaths();
  // assignNetColors();

  // sendAllPaths();

  // examples

  // add connection
  char input;
  unsigned long timer = 0;

  // initArduino();
  //
//rainbowy(255,145,100);
menu:

  // arduinoPrint();

  Serial.print("\n\n\r\t\t\tMenu\n\n\r");
  Serial.print("\tn = show netlist\n\r");
  Serial.print("\tb = show bridge array\n\r");
  Serial.print("\tw = waveGen\n\r");
  Serial.print("\tm = measure current/voltage\n\r");
  Serial.print("\tf = load formatted nodeFile\n\r");
  Serial.print("\tu = upload new Wokwi diagram\n\r");
  Serial.print("\tt = reset and load nodeFile.txt\n\r");
  Serial.print("\td = toggle debug flags\n\r");
  Serial.print("\tr = reset\n\r");
  Serial.print("\n\r");

  // if (EEPROM.read(CLEARBEFORECOMMANDADDRESS) == 1)
  //{

  //  input = EEPROM.read(LASTCOMMANDADDRESS);
  //}
  // else
  //{
  while (Serial.available() == 0)
    ;

  input = Serial.read();
  //}
  if (input != 'l')
  {
    // lastCommandWrite(input);
  }

  Serial.print(input);
  Serial.print("\n\r");

  switch (input)
  {
  case 'l':
    // lastCommandRead();
    break;

  case 'n':
    // lastCommandWrite(input);
    Serial.print("\n\n\rnetlist\n\n\r");
    listSpecialNets();
    listNets();
    break;
  case 'b':
    // lastCommandWrite(input);
    printBridgeArray();
    break;
  case 'w':
    // lastCommandWrite(input);
    waveGen();
    break;
  case 'm':
    // measureCurrent();
    break;

  case 'f':
    digitalWrite(RESETPIN, HIGH);

    clearAllNTCC();
    clearLEDs();
    digitalWrite(RESETPIN, LOW);

    timer = millis();
#ifdef FSSTUFF
    clearNodeFile();
    savePreformattedNodeFile();

    openNodeFile();
    getNodesToConnect();
#endif

    bridgesToPaths();
    assignNetColors();

#ifdef PIOSTUFF
    sendAllPaths();
#endif

    Serial.print("\n\n\r");
    Serial.print("took ");
    Serial.print(millis() - timer);
    Serial.print("ms");

    break;

  case 'u':

    // case '{':

    digitalWrite(RESETPIN, HIGH);
    delay(1);
    #ifdef FSSTUFF
    clearNodeFile();
    #endif
    digitalWrite(RESETPIN, LOW);
    clearAllNTCC();
    clearLEDs();

    // EEPROM.write(CLEARBEFORECOMMANDADDRESS, 0);
    // EEPROM.commit();

    timer = millis();
// savePreformattedNodeFile ();
#ifdef FSSTUFF

    parseWokwiFileToNodeFile();
    // lastCommandWrite(input);
    openNodeFile();
    getNodesToConnect();
#endif
    Serial.println("\n\n\rnetlist\n\n\r");
    // listSpecialNets();
    // listNets();
    // printBridgeArray();

    bridgesToPaths();
    assignNetColors();

#ifdef PIOSTUFF
    sendAllPaths();
#endif

    Serial.print("\n\n\r");
    Serial.print("took ");
    Serial.print(millis() - timer);
    Serial.print("ms");

    break;

  case 't':
#ifdef FSSTUFF
    clearNodeFile();
#endif

#ifdef EEPROMSTUFF
    lastCommandWrite(input);

    runCommandAfterReset('t');
#endif

#ifdef FSSTUFF
    openNodeFile();
    getNodesToConnect();
#endif
    Serial.println("\n\n\rnetlist\n\n\r");

    bridgesToPaths();

    listSpecialNets();
    listNets();
    printBridgeArray();
    Serial.print("\n\n\r");
    Serial.print(numberOfNets);

    Serial.print("\n\n\r");
    Serial.print(numberOfPaths);

    assignNetColors();
#ifdef PIOSTUFF
    sendAllPaths();
#endif
    // EEPROM.write(CLEARBEFORECOMMANDADDRESS, 0);
    // EEPROM.commit();
    break;

  case 'r':
    // EEPROM.commit();
    digitalWrite(RESETPIN, HIGH);
    delay(1);
#ifdef FSSSTUFF
    clearNodeFile();
#endif
    digitalWrite(RESETPIN, LOW);
    clearAllNTCC();
    leds.clear();
    leds.show();

    // AIRCR_Register = 0x5FA0004; // this just hardware resets the rp2040, it would be too much of a pain in the ass to reinitialize everything

    break;
    /*
      case '{':

          lastCommandWrite(input);

        runCommandAfterReset('u');

        //lastCommandWrite('{');
        parseWokwiFileToNodeFile();
        getNodesToConnect();

        Serial.println("\n\n\rnetlist\n\r");
        listSpecialNets();
        listNets();
        // printBridgeArray();

        bridgesToPaths();
        assignNetColors();

        sendAllPaths();

        EEPROM.write(CLEARBEFORECOMMANDADDRESS, 0);
        EEPROM.commit();
        break;
    */
  case 'd':
  {
    debugFlagInit();
    // lastCommandWrite(input);

  debugFlags:
    Serial.print("\n\r0.   all off");
    Serial.print("\n\r9.   all on");
    Serial.print("\n\ra-z. exit\n\r");

    Serial.print("\n\r1. file parsing           =    ");
    Serial.print(debugFP);
    Serial.print("\n\r2. file parsing time      =    ");
    Serial.print(debugFPtime);

    Serial.print("\n\r3. net manager            =    ");
    Serial.print(debugNM);
    Serial.print("\n\r4. net manager time       =    ");
    Serial.print(debugNMtime);

    Serial.print("\n\r5. chip connections       =    ");
    Serial.print(debugNTCC);
    Serial.print("\n\r6. chip conns alt paths   =    ");
    Serial.print(debugNTCC2);
    Serial.print("\n\r7. LEDs                   =    ");
    Serial.print(debugLEDs);

    Serial.print("\n\n\r");

    while (Serial.available() == 0)
      ;

    int toggleDebug = Serial.read();
    toggleDebug -= '0';

    if (toggleDebug >= 0 && toggleDebug <= 9)
    {
      #ifdef EEPROMSTUFF
      debugFlagSet(toggleDebug);
      #endif
      goto debugFlags;
    }
    else
    {
      break;
    }
  }

  default:
    while (Serial.available() > 0)
    {
      int f = Serial.read();
      delayMicroseconds(100);
    }

    break;
  }

  goto menu;

  while (1)
  {

    // waveGen();
  }
}
