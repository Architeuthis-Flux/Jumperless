
#include "PersistentStuff.h"
#include "FileParsing.h"
#include "JumperlessDefinesRP2040.h"
#include "LEDs.h"
#include "NetManager.h"
#include "Probing.h"
#include "Peripherals.h"
#include <EEPROM.h>
#include "Graphics.h"
void debugFlagInit(int forceDefaults) {

  if (EEPROM.read(FIRSTSTARTUPADDRESS) != 0xAA || forceDefaults == 1) {
    EEPROM.write(FIRSTSTARTUPADDRESS, 0xAA);
    EEPROM.write(REVISIONADDRESS, REV);
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
    EEPROM.write(DEBUG_LEDSADDRESS, 0);
    EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
    EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);
    EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
    EEPROM.write(PROBESWAPADDRESS, 0);
    EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);
    EEPROM.write(DISPLAYMODE_ADDRESS, 1);
    EEPROM.write(NETCOLORMODE_ADDRESS, 0);
    EEPROM.write(MENUBRIGHTNESS_ADDRESS, 100);
    EEPROM.write(PATH_DUPLICATE_ADDRESS, 2);
    EEPROM.write(DAC_DUPLICATE_ADDRESS, 0);
    EEPROM.write(POWER_DUPLICATE_ADDRESS, 2);
    EEPROM.write(DAC_PRIORITY_ADDRESS, 1);
    EEPROM.write(POWER_PRIORITY_ADDRESS, 1);
    EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 0);

    saveVoltages(0.0f, 0.0f, 3.33f, 0.0f);

    EEPROM.commit();
    delay(5);
  }


  debugFP = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
  debugFPtime = EEPROM.read(TIME_FILEPARSINGADDRESS);

  debugNM = EEPROM.read(DEBUG_NETMANAGERADDRESS);
  debugNMtime = EEPROM.read(TIME_NETMANAGERADDRESS);

   debugNTCC = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);
   debugNTCC2 = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

  LEDbrightnessRail = EEPROM.read(RAILBRIGHTNESSADDRESS);
  LEDbrightness = EEPROM.read(LEDBRIGHTNESSADDRESS);
  LEDbrightnessSpecial = EEPROM.read(SPECIALBRIGHTNESSADDRESS);

  debugLEDs = EEPROM.read(DEBUG_LEDSADDRESS);

  rotaryEncoderMode = EEPROM.read(ROTARYENCODER_MODE_ADDRESS);

  displayMode = EEPROM.read(DISPLAYMODE_ADDRESS);

  probeSwap = EEPROM.read(PROBESWAPADDRESS);
  netColorMode = EEPROM.read(NETCOLORMODE_ADDRESS);

  revisionNumber = EEPROM.read(REVISIONADDRESS);

  pathDuplicates = EEPROM.read(PATH_DUPLICATE_ADDRESS);
  dacDuplicates = EEPROM.read(DAC_DUPLICATE_ADDRESS);
  powerDuplicates = EEPROM.read(POWER_DUPLICATE_ADDRESS);
  dacPriority = EEPROM.read(DAC_PRIORITY_ADDRESS);
  powerPriority = EEPROM.read(POWER_PRIORITY_ADDRESS);

  showProbeCurrent = EEPROM.read(SHOW_PROBE_CURRENT_ADDRESS);



menuBrightnessSetting = EEPROM.read(MENUBRIGHTNESS_ADDRESS) - 100;

  dacSpread[1] = EEPROM.get(DAC0_SPREAD_ADDRESS, dacSpread[0]);
  dacSpread[1] = EEPROM.get(DAC1_SPREAD_ADDRESS, dacSpread[1]);
  dacSpread[2] = EEPROM.get(TOP_RAIL_SPREAD_ADDRESS, dacSpread[2]);
  dacSpread[3] = EEPROM.get(BOTTOM_RAIL_SPREAD_ADDRESS, dacSpread[3]);

  dacZero[0] = EEPROM.get(DAC0_ZERO_ADDRESS, dacZero[0]);
  dacZero[1] = EEPROM.get(DAC1_ZERO_ADDRESS, dacZero[1]);
  dacZero[2] = EEPROM.get(TOP_RAIL_ZERO_ADDRESS, dacZero[2]);
  dacZero[3] = EEPROM.get(BOTTOM_RAIL_ZERO_ADDRESS,dacZero[3]);

  for (int i = 0; i < 4; i++) {
    if (dacSpread[i] < 12.0 || dacSpread[i] > 28.0 || dacSpread[i] != dacSpread[i]) {
      // delay(2000);
      
      // Serial.print("dacSpread[");
      // Serial.print(i);
      // Serial.print("] out of range = ");
      // Serial.println(dacSpread[i]);
      dacSpread[i] = 21.0;
      EEPROM.put(DAC0_SPREAD_ADDRESS + (i * 8), dacSpread[i]);
      //EEPROM.put(CALIBRATED_ADDRESS, 0);
    }
    if (dacZero[i] < 1000 || dacZero[i] > 2000) {

      // Serial.print("dacZero[");
      // Serial.print(i);
      // Serial.print("] out of range = ");
      // Serial.println(dacZero[i]);
      dacZero[i] = 1630;
      EEPROM.put(DAC0_ZERO_ADDRESS + (i * 4), 1630);
     // EEPROM.put(CALIBRATED_ADDRESS, 0);
    }
  }
if (showProbeCurrent != 0 && showProbeCurrent != 1)
  {
    EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 0);
    showProbeCurrent = 0;
  }
// delay(3000);
// Serial.print("pathDuplicates: ");
// Serial.println(pathDuplicates);
// Serial.print("dacDuplicates: ");
// Serial.println(dacDuplicates);
// Serial.print("powerDuplicates: ");
// Serial.println(powerDuplicates);
// Serial.print("dacPriority: ");
// Serial.println(dacPriority);
// Serial.print("powerPriority: ");
// Serial.println(powerPriority);

if (pathDuplicates < 0 || pathDuplicates > 20) {
  Serial.print("pathDuplicates out of range (");
  Serial.print(pathDuplicates);
  Serial.println("), setting to 3");
    EEPROM.write(PATH_DUPLICATE_ADDRESS, 3);
    pathDuplicates = 3;
  }
  if (dacDuplicates < 0 || dacDuplicates > 20) {
    Serial.print("dacDuplicates out of range (");
    Serial.print(dacDuplicates);
    Serial.println("), setting to 0");

    EEPROM.write(DAC_DUPLICATE_ADDRESS, 0);
    dacDuplicates = 0;
  }
  if (powerDuplicates < 0 || powerDuplicates > 20) {
    Serial.print("powerDuplicates out of range (");
    Serial.print(powerDuplicates);
    Serial.println("), setting to 3");

    
    EEPROM.write(POWER_DUPLICATE_ADDRESS, 3);
    powerDuplicates = 3;
  }
  if (dacPriority < 1 || dacPriority > 10) {

    Serial.print("dacPriority out of range (");
    Serial.print(dacPriority);
    Serial.println("), setting to 1");

    EEPROM.write(DAC_PRIORITY_ADDRESS, 1);
    dacPriority = 1;
  }
  if (powerPriority < 1 || powerPriority > 10) {

    Serial.print("powerPriority out of range (");
    Serial.print(powerPriority);
    Serial.println("), setting to 1");

    EEPROM.write(POWER_PRIORITY_ADDRESS, 1);
    powerPriority = 1;
  }


  if (revisionNumber <= 0 || revisionNumber > 10) {


    //delay(5000);
    Serial.print("Revision number out of range (");
    Serial.print(revisionNumber);
    Serial.print("), setting to revision ");
    EEPROM.write(REVISIONADDRESS, REV);
    revisionNumber = REV;
    
    Serial.println(revisionNumber);
  }


  if (debugFP != 0 && debugFP != 1)
  {
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

debugFP = false;
  }
  if (debugFPtime != 0 && debugFPtime != 1)
  {
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
    debugFPtime = false;
  }

  if (debugNM != 0 && debugNM != 1)
  {
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
    debugNM = false;
  }

  if (debugNMtime != 0 && debugNMtime != 1)
  {
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);
    debugNMtime = false;
  }

  if (debugNTCC != 0 && debugNTCC != 1)
  {
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
    debugNTCC = false;
  }

  if (debugNTCC2 != 0 && debugNTCC2 != 1)
  {
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
    debugNTCC2 = false;
  }

  if (debugLEDs != 0 && debugLEDs != 1)
  {

    EEPROM.write(DEBUG_LEDSADDRESS, 0);
    debugLEDs = false;
  }

  if (LEDbrightnessRail < 0 || LEDbrightnessRail > 200) {
    EEPROM.write(RAILBRIGHTNESSADDRESS, DEFAULTRAILBRIGHTNESS);

    LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
  }
  if (LEDbrightness < 0 || LEDbrightness > 200) {
    EEPROM.write(LEDBRIGHTNESSADDRESS, DEFAULTBRIGHTNESS);
    LEDbrightness = DEFAULTBRIGHTNESS;
  }

  if (LEDbrightnessSpecial < 0 || LEDbrightnessSpecial > 200) {
    EEPROM.write(SPECIALBRIGHTNESSADDRESS, DEFAULTSPECIALNETBRIGHTNESS);
    LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;
  }
    // delay(3000);
    // Serial.print("menuBrightnessSetting out of range = ");
    // Serial.println(menuBrightnessSetting);
  if (menuBrightnessSetting < -100 || menuBrightnessSetting > 100) {


    EEPROM.write(MENUBRIGHTNESS_ADDRESS, 100);
    menuBrightnessSetting = 0;
  }

  
  if (rotaryEncoderMode != 0 && rotaryEncoderMode != 1) {
    EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);
    rotaryEncoderMode = 0;
  }
  if (displayMode != 0 && displayMode != 1) {
    EEPROM.write(DISPLAYMODE_ADDRESS, 1);
    displayMode = 0;
  }
  if (netColorMode != 0 && netColorMode != 1) {
    EEPROM.write(NETCOLORMODE_ADDRESS, 0);
    netColorMode = 0;
  }


readVoltages();
readLogoBindings();
  EEPROM.commit();
  delay(2);
}

void saveDacCalibration(void)
{

  EEPROM.put(DAC0_SPREAD_ADDRESS, dacSpread[0]);
  EEPROM.put(DAC1_SPREAD_ADDRESS, dacSpread[1]);
  EEPROM.put(TOP_RAIL_SPREAD_ADDRESS, dacSpread[2]);
  EEPROM.put(BOTTOM_RAIL_SPREAD_ADDRESS, dacSpread[3]);

  EEPROM.put(DAC0_ZERO_ADDRESS, dacZero[0]);
  EEPROM.put(DAC1_ZERO_ADDRESS, dacZero[1]);
  EEPROM.put(TOP_RAIL_ZERO_ADDRESS, dacZero[2]);
  EEPROM.put(BOTTOM_RAIL_ZERO_ADDRESS, dacZero[3]);

  EEPROM.put(CALIBRATED_ADDRESS, 0x55);

  EEPROM.commit();
  delay(2);



}


void debugFlagSet(int flag) {
  int flagStatus;
  switch (flag) {
  case 1: {
    flagStatus = EEPROM.read(DEBUG_FILEPARSINGADDRESS);
    if (flagStatus == 0) {
      EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);

      debugFP = true;
    } else {
      EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);

      debugFP = false;
    }

    break;
  }

  case 2: {
    flagStatus = EEPROM.read(DEBUG_NETMANAGERADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);

      debugNM = true;
    } else {
      EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);

      debugNM = false;
    }
    break;
  }

  case 3: {
    flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);

      debugNTCC = true;
    } else {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);

      debugNTCC = false;
    }

    break;
  }
  case 4: {
    flagStatus = EEPROM.read(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);

      debugNTCC2 = true;
    } else {
      EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);

      debugNTCC2 = false;
    }
    break;
  }

  case 5: {
    flagStatus = EEPROM.read(DEBUG_LEDSADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(DEBUG_LEDSADDRESS, 1);

      debugLEDs = true;
    } else {
      EEPROM.write(DEBUG_LEDSADDRESS, 0);

      debugLEDs = false;
    }
    break;
  }

  // case 6: {
  //   flagStatus = EEPROM.read(PROBESWAPADDRESS);

  //   if (flagStatus == 0) {
  //     EEPROM.write(PROBESWAPADDRESS, 1);

  //     probeSwap = true;
  //   } else {
  //     EEPROM.write(PROBESWAPADDRESS, 0);

  //     probeSwap = false;
  //   }
  //   break;
  // }
  case 6: {
    flagStatus = EEPROM.read(SHOW_PROBE_CURRENT_ADDRESS);

    if (flagStatus == 0) {
      EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 1);

      showProbeCurrent = 1;
    } else {
      EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 0);

      showProbeCurrent = 0;
    }
    break;
  }

  case 0: {
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 0);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 0);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 0);
    EEPROM.write(TIME_NETMANAGERADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 0);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 0);
    EEPROM.write(DEBUG_LEDSADDRESS, 0);
    EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 0);

    debugFP = false;
    debugFPtime = false;
    debugNM = false;
    debugNMtime = false;
    debugNTCC = false;
    debugNTCC2 = false;
    debugLEDs = false;
    showProbeCurrent = 0;

    break;
  }

  case 9: {
    EEPROM.write(DEBUG_FILEPARSINGADDRESS, 1);
    EEPROM.write(TIME_FILEPARSINGADDRESS, 1);
    EEPROM.write(DEBUG_NETMANAGERADDRESS, 1);
    EEPROM.write(TIME_NETMANAGERADDRESS, 1);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSADDRESS, 1);
    EEPROM.write(DEBUG_NETTOCHIPCONNECTIONSALTADDRESS, 1);
    EEPROM.write(DEBUG_LEDSADDRESS, 1);
    EEPROM.write(SHOW_PROBE_CURRENT_ADDRESS, 1);
    debugFP = true;
    debugFPtime = true;
    debugNM = true;
    debugNMtime = true;
    debugNTCC = true;
    debugNTCC2 = true;
    debugLEDs = true;
    showProbeCurrent = 1;
    break;
  }
  case 10: {
    {
      EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 0);

      rotaryEncoderMode = 0;
    }
    break;
  }
  case 11: {
    {
      EEPROM.write(ROTARYENCODER_MODE_ADDRESS, 1);

      rotaryEncoderMode = 1;
    }
    break;
  }
  case 12: {
    
      EEPROM.write(DISPLAYMODE_ADDRESS, displayMode);

      
    
    break;
  }
  case 13: 
    {
      EEPROM.write(NETCOLORMODE_ADDRESS, netColorMode);

      
    }
    break;
  }
  delay(4);
  EEPROM.commit();
  delay(8);
  return;
}

void saveVoltages(float top, float bot, float dac0, float dac1) {
  // Serial.print("saving voltages: ");
  // Serial.print(top);
  // Serial.print(" ");
  // Serial.print(bot);
  // Serial.print(" ");
  // Serial.print(dac0);
  // Serial.print(" ");
  // Serial.println(dac1);


  EEPROM.put(TOP_RAIL_ADDRESS0, top);
  EEPROM.put(BOTTOM_RAIL_ADDRESS0, bot);
  EEPROM.put(DAC0_ADDRESS0, dac0);
  EEPROM.put(DAC1_ADDRESS0, dac1);
  EEPROM.commit();
  delay(1);


}

void saveDuplicateSettings(int forceDefaults) {
  if (forceDefaults == 1) {
    EEPROM.write(PATH_DUPLICATE_ADDRESS, 2);
    EEPROM.write(DAC_DUPLICATE_ADDRESS, 0);
    EEPROM.write(POWER_DUPLICATE_ADDRESS, 3);
    EEPROM.write(DAC_PRIORITY_ADDRESS, 1);
    EEPROM.write(POWER_PRIORITY_ADDRESS, 1);
    EEPROM.commit();
    delay(2);
    return;
  }
  EEPROM.write(PATH_DUPLICATE_ADDRESS, pathDuplicates);
  EEPROM.write(DAC_DUPLICATE_ADDRESS, dacDuplicates);
  EEPROM.write(POWER_DUPLICATE_ADDRESS, powerDuplicates);
  EEPROM.write(DAC_PRIORITY_ADDRESS, dacPriority);
  EEPROM.write(POWER_PRIORITY_ADDRESS, powerPriority);
  EEPROM.commit();
  delay(2);
}

void readVoltages(void) {


//delay(1000);

 delayMicroseconds(200);
   EEPROM.get(TOP_RAIL_ADDRESS0, railVoltage[0]);


   EEPROM.get(BOTTOM_RAIL_ADDRESS0, railVoltage[1]);
   EEPROM.get(DAC0_ADDRESS0, dacOutput[0]);
   EEPROM.get(DAC1_ADDRESS0, dacOutput[1]);
   delayMicroseconds(200);

int needsInit = 0;
if (railVoltage[0] > 8.0f || railVoltage[0] < -8.0f ){//|| (uint32_t)railVoltage[0] == 0x00000000 || (uint32_t)railVoltage[0] == 0xFFFFFFFF) {
Serial.println(railVoltage[0]);

    railVoltage[0] = 0.0f;
    needsInit = 1;
    
    //Serial.println("rail voltage 0 out of range");
  }
  if (railVoltage[1] > 8.0f || railVoltage[1] < -8.0f){// || (uint32_t)railVoltage[1] == 0x00000000 || (uint32_t)railVoltage[1] == 0xFFFFFFFF) {
    railVoltage[1] = 0.0f;
    needsInit = 1;
    //Serial.println("rail voltage 1 out of range");
  }
  if (dacOutput[0] > 5.0f || dacOutput[0] < 0.0f){// || (uint32_t)dacOutput[0] == 0x00000000 || (uint32_t)dacOutput[0] == 0xFFFFFFFF) {
    dacOutput[0] = 0.0f;
    needsInit = 1;
    //Serial.println("dac 0 out of range");
  }
  if (dacOutput[1] > 8.0f || dacOutput[1] < -8.0f){// || (uint32_t)dacOutput[1] == 0x00000000 || (uint32_t)dacOutput[1] == 0xFFFFFFFF) {
    dacOutput[1] = 0.0f;
    needsInit = 1;
    //Serial.println("dac 1 out of range");
  }
  if (needsInit == 1)
  {
    //Serial.println("needs init");
saveVoltages(railVoltage[0],railVoltage[1],dacOutput[0],dacOutput[1]);
  }
//Serial.println(sizeof(float));

  // Serial.print("top rail: ");
  // Serial.println((float)railVoltage[0]);
  // Serial.print("bot rail: ");
  // Serial.println(railVoltage[1],BIN);
  // Serial.print("dac0: ");
  // Serial.println(dacOutput[0],BIN);
  // Serial.print("dac1: ");
  // Serial.println(dacOutput[1],BIN);


// setTopRail(railVoltage[0]);
// setBotRail(railVoltage[1]);
// setDac0_5Vvoltage(dacOutput[0]);
// setDac1_8Vvoltage(dacOutput[1]);
  return;
}

void saveLogoBindings(void) {
  EEPROM.put(LOGO_TOP_ADDRESS0, logoTopSetting[0]);
  EEPROM.put(LOGO_TOP_ADDRESS1, logoTopSetting[1]);
  EEPROM.put(LOGO_BOTTOM_ADDRESS0, logoBottomSetting[0]);
  EEPROM.put(LOGO_BOTTOM_ADDRESS1, logoBottomSetting[1]);
  EEPROM.put(BUILDING_TOP_ADDRESS0, buildingTopSetting[0]);
  EEPROM.put(BUILDING_TOP_ADDRESS1, buildingTopSetting[1]);
  EEPROM.put(BUILDING_BOTTOM_ADDRESS0, buildingBottomSetting[0]);
  EEPROM.put(BUILDING_BOTTOM_ADDRESS1, buildingBottomSetting[1]);
  EEPROM.commit();
  delay(2);
}

void readLogoBindings(void) {
  EEPROM.get(LOGO_TOP_ADDRESS0, logoTopSetting[0]);

  EEPROM.get(LOGO_TOP_ADDRESS1, logoTopSetting[1]);
  if (logoTopSetting[0] == 2) {
    //gpioState[logoTopSetting[1]] = 0;
  }
  EEPROM.get(LOGO_BOTTOM_ADDRESS0, logoBottomSetting[0]);
  EEPROM.get(LOGO_BOTTOM_ADDRESS1, logoBottomSetting[1]);
  if (logoBottomSetting[0] == 2) {
   // gpioState[logoBottomSetting[1]] = 0;
  }
  EEPROM.get(BUILDING_TOP_ADDRESS0, buildingTopSetting[0]);
  EEPROM.get(BUILDING_TOP_ADDRESS1, buildingTopSetting[1]);

  if (buildingTopSetting[0] == 2) {
    //gpioState[buildingTopSetting[1]] = 0;
  }
  EEPROM.get(BUILDING_BOTTOM_ADDRESS0, buildingBottomSetting[0]);
  EEPROM.get(BUILDING_BOTTOM_ADDRESS1, buildingBottomSetting[1]);
  if (buildingBottomSetting[0] == 2) {
    //gpioState[buildingBottomSetting[1]] = 0;
  }
  return;
} 

void saveLEDbrightness(int forceDefaults) {
  if (forceDefaults == 1) {
    LEDbrightness = DEFAULTBRIGHTNESS;
    LEDbrightnessRail = DEFAULTRAILBRIGHTNESS;
    LEDbrightnessSpecial = DEFAULTSPECIALNETBRIGHTNESS;
    menuBrightnessSetting = 0;
    
  }


    EEPROM.write(MENUBRIGHTNESS_ADDRESS, menuBrightnessSetting+100);
    EEPROM.write(LEDBRIGHTNESSADDRESS, LEDbrightness);
    EEPROM.write(RAILBRIGHTNESSADDRESS, LEDbrightnessRail);
    EEPROM.write(SPECIALBRIGHTNESSADDRESS, LEDbrightnessSpecial);
    EEPROM.commit();
    delay(2);
  
}




void runCommandAfterReset(char command) {
  if (EEPROM.read(CLEARBEFORECOMMANDADDRESS) == 1) {
    return;
  } else {

    EEPROM.write(CLEARBEFORECOMMANDADDRESS, 1);
    EEPROM.write(LASTCOMMANDADDRESS, command);
    EEPROM.commit();

    digitalWrite(RESETPIN, HIGH);
    delay(1);
    digitalWrite(RESETPIN, LOW);

    AIRCR_Register = 0x5FA0004; // hard reset
  }
}

char lastCommandRead(void) {

  Serial.print("last command: ");

  Serial.println((char)EEPROM.read(LASTCOMMANDADDRESS));

  return EEPROM.read(LASTCOMMANDADDRESS);
}
void lastCommandWrite(char lastCommand) {

  EEPROM.write(LASTCOMMANDADDRESS, lastCommand);
}
