

#include <Arduino.h>
#include "JumperlessDefinesRP2040.h"
#include "NetManager.h"
#include "MatrixStateRP2040.h"

#include "NetsToChipConnections.h"
#include "LEDs.h"
// #include "CH446Q.h"
#include "Peripherals.h"
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
// #include <EEPROM.h>
#include "ArduinoStuff.h"

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

volatile int sendAllPathsCore2 = 0; // this signals the core 2 to send all the paths to the CH446Q



// https://wokwi.com/projects/367384677537829889

void setup()
{

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  // initArduino();
  // debugFlagInit();

#ifdef EEPROMSTUFF
  EEPROM.begin(256);
  debugFlagInit();

#endif

  initADC();
  delay (1);
  initDAC();
  delay (1);
  initINA219();
  delay (1);
  Serial.begin(115200);
delay (4);
#ifdef FSSTUFF
  LittleFS.begin();
#endif
  setDac0_5Vvoltage(0.0);
  setDac1_8Vvoltage(1.9);

  clearAllNTCC();
  delay (4);
}

void setup1()
{

#ifdef PIOSTUFF
  initCH446Q();
#endif

//delay (4);
  initLEDs();

  

  startupColors();

  lightUpRail();
  showLEDsCore2 = 1;

}

void loop()
{

  char input;
  unsigned long timer = 0;

  //while (1) rainbowBounce(80); //I uncomment this to test the LEDs on a fresh board
//while (1) randomColors(0,90);
menu:

  //showLEDsCore2 = 1;
  Serial.print("\n\n\r\t\t\tMenu\n\n\r");
  Serial.print("\tn = show netlist\n\r");
  Serial.print("\tb = show bridge array\n\r");
  Serial.print("\tw = waveGen\n\r");
  // Serial.print("\tm = measure current/voltage\n\r");
  Serial.print("\tf = load formatted nodeFile\n\r");
  Serial.print("\tp = paste new Wokwi diagram\n\r");
  Serial.print("\tl = LED brightness / test\n\r");
  Serial.print("\td = toggle debug flags\n\r");
  Serial.print("\tr = reset Arduino\n\r");
  Serial.print("\n\r");

  while (Serial.available() == 0)
    ;

  input = Serial.read();

  // Serial.print(input);
  Serial.print("\n\r");

  switch (input)
  {

  case 'n':

    Serial.print("\n\n\rnetlist\n\n\r");
    listSpecialNets();
    listNets();
    break;
  case 'b':

    printBridgeArray();
    break;

  case 'm':

    break;

  case 'w':

    if (waveGen() == 1)
    {
      break;
    }

  case 'a':
  {
    resetArduino(); // reset works
    // uploadArduino(); //this is unwritten
  }

  case 'f':
    digitalWrite(RESETPIN, HIGH);

    clearAllNTCC();
    delay(10);
    
    //showLEDsCore2 = 1;
    digitalWrite(RESETPIN, LOW);
sendAllPathsCore2 = 1;
    timer = millis();
#ifdef FSSTUFF
    clearNodeFile();
    savePreformattedNodeFile();

    openNodeFile();
    getNodesToConnect();
#endif

    bridgesToPaths();
    clearLEDs();
    assignNetColors();
    //showNets();

#ifdef PIOSTUFF
    sendAllPathsCore2 = 1;

#endif

    if (debugNMtime)
    {
      Serial.print("\n\n\r");
      Serial.print("took ");
      Serial.print(millis() - timer);
      Serial.print("ms");
    }


    resetArduino();
    break;

  case '\n':
    goto menu;
    break;

  case 'p':

    // case '{':  //I had this so you could paste a wokwi diagram from the main menu but it kinda makes a mess of other things

    digitalWrite(RESETPIN, HIGH);
    delay(1);
#ifdef FSSTUFF
    clearNodeFile();
#endif
    digitalWrite(RESETPIN, LOW);
    clearAllNTCC();
    clearLEDs();

    timer = millis();

#ifdef FSSTUFF

    parseWokwiFileToNodeFile();

    openNodeFile();
    getNodesToConnect();
#endif
    Serial.println("\n\n\rnetlist\n\n\r");

    bridgesToPaths();
    assignNetColors();

#ifdef PIOSTUFF

    sendAllPaths();
#endif

    if (debugNMtime)
    {
      Serial.print("\n\n\r");
      Serial.print("took ");
      Serial.print(millis() - timer);
      Serial.print("ms");
    }
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

    break;

  case 'l':
    if (LEDbrightnessMenu() == '!')
    {
      clearLEDs();
      delayMicroseconds(9200);
      sendAllPathsCore2 = 1;
    }
    break;
  

  case 'r':
    resetArduino();
    break;

  case 'd':
  {
    debugFlagInit();

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
    Serial.write(toggleDebug);
    toggleDebug -= '0';

    if (toggleDebug >= 0 && toggleDebug <= 9)
    {

#ifdef EEPROMSTUFF
      debugFlagSet(toggleDebug);
#endif

      delay(10);

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
      delayMicroseconds(50);
    }

    break;
  }

  goto menu;
}
unsigned long logoFlashTimer = 0;


void loop1() // core 2 handles the LEDs and the CH446Q8
{

//while (1) rainbowBounce(50); //I uncomment this to test the LEDs on a fresh board
  if (showLEDsCore2 >= 1)
  {
    int rails = showLEDsCore2;
   
    //showNets();
    if (rails == 1)
    {
 lightUpRail();

    } 
    if (rails > 3)
    {
      Serial.print("\n\r");
      Serial.print(rails);
    }
   delayMicroseconds(5200);
    

    leds.show();
    delayMicroseconds(9200);
    showLEDsCore2 = 0;
  }

  if (sendAllPathsCore2 == 1)
  {
    delayMicroseconds(12200);
    sendAllPaths();
    delayMicroseconds(4200);
    showNets();
    delayMicroseconds(9200);
    sendAllPathsCore2 = 0;
  }



  if (logoFlash == 2)
  {
    logoFlashTimer = millis();
    logoFlash = 1;
    
  } 

  if (logoFlash == 1 && logoFlashTimer != 0 && millis() - logoFlashTimer > 600)
  {
    logoFlash = 0;
    logoFlashTimer = 0;
    //lightUpRail();
    leds.setPixelColor(110, 0x550008);
    leds.show();
  }


}


