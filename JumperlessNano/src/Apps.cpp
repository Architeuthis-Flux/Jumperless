#include "Commands.h"
#include "FileParsing.h"
#include "Graphics.h"
#include "MatrixStateRP2040.h"
#include "NetManager.h"
#include "NetsToChipConnections.h"
#include "Peripherals.h"
#include "Probing.h"
#include "JumperlessDefinesRP2040.h"
#include "PersistentStuff.h"
#include "MachineCommands.h"
#include "ArduinoStuff.h"
#include "Apps.h"
#include "UserCode.h"
#include "LEDs.h"   
#include "RotaryEncoder.h"



// #include <Adafruit_GFX.h>
// // #include <Adafruit_SSD1306.h>
// #define OLED_CONNECTED 0



//#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//#define SCREEN_ADDRESS                                                         \
  //0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// -Oscill oscope\n\
// -MIDI   Synth\n\
// -I2C    Scanner\n\
// -Self   Dstruct\n\
// -EEPROM Dumper\n\
// -7 Seg  Mapper\n\
// -Rick   Roll\n\
// -$Circuts>$\n\
// --555\n\
// --Op Amp\n\
// --$7400$\n\
// ---*74x109**74x161**74x42**74x595*\n\
// -$Games  >$\n\
// --*DOOM**Pong**Tetris**Snake*\n\
// -$Manage >$\n\
// --Delete\n\
// --->a3\n\
// --Upload\n\
// --->a4\n\
// -Logic  Analyzr\n\
// -Scan\n\

struct app apps[30] = {
    {"Scan", 0, 1, scanBoard},
    {"Calib  DACs", 1, 1, calibrateDacs},
    // {"Logic  Analyzr", 2, 1, logicAnalyzer},
    // {"Oscill oscope", 3, 1, oscilloscope},
    // {"MIDI   Synth", 4, 1, midiSynth},
    // {"I2C    Scanner", 5, 1, i2cScanner},
    // {"Self   Dstruct", 6, 1, selfDestruct},
    // {"EEPROM Dumper", 7, 1, eepromDumper},
    // {"7 Seg  Mapper", 8, 1, segmentMapper},
    // {"Rick   Roll", 9, 1, rickRoll},
    // {"555", 10, 1, circuit555},
    // {"Op Amp", 11, 1, opAmp},
    // {"$7400$", 12, 1, ic74x109},
    // {"$7400$", 13, 1, ic74x161},
    // {"$7400$", 14, 1, ic74x42},
    // {"$7400$", 15, 1, ic74x595},
    {"DOOM", 16, 1, playDoom},
    // {"Pong", 17, 1, playPong},
    // {"Tetris", 18, 1, playTetris},
    // {"Snake", 19, 1, playSnake},
    // {"Delete", 20, 1, deleteApp},
    // {"Upload", 21, 1, uploadApp},


    };

void runApp(int index, char* name)
    {

    // Find matching app if only one parameter is given
    if (index == -1) {
        for (int i = 0; i < sizeof(apps) / sizeof(apps[0]); i++) {
            if (strcmp(apps[i].name, name) == 0) {
                index = i;
                break;
                }
            }
        }
    if (name == nullptr) {
        name = apps[index].name;
        }

    // Run the app based on index
    switch (index) {
        case 0: scanBoard(); break;
        case 1: calibrateDacs(); break;
            // case 2: logicAnalyzer(); break;
            // case 3: oscilloscope(); break;
            // case 4: midiSynth(); break;
            // case 5: i2cScanner(); break;
            // case 6: selfDestruct(); break;
            // case 7: eepromDumper(); break;
            // case 8: segmentMapper(); break;
            // case 9: rickRoll(); break;
            // case 10: circuit555(); break;
            // case 11: opAmp(); break;
            // case 12: ic74x109(); break;
            // case 13: ic74x161(); break;
            // case 14: ic74x42(); break;
            // case 15: ic74x595(); break;
        case 16: playDoom(); break;
            // case 17: playPong(); break;
            // case 18: playTetris(); break;
            // case 19: playSnake(); break;
            // case 20: deleteApp(); break;
            // case 21: uploadApp(); break;
        default: break;
        }







    }


void scanBoard(void) {
    int countLoop = 0;
    int countMult = 18;
    //measureModeActive = 1;
    refreshConnections(-1, 1);

    Serial.println("\n\n\r");
    //showLEDsCore2 = -1;

    int lastRow = 0;
    int lastFloat = 0;
    int lastNode = 0;

    while (Serial.available() == 0 ) {

        for (int i = 1; i < 96; i++) {


            if (i == 84 || i == NANO_RESET_0 || i == NANO_RESET_1) {
                continue;
                }
            if (i > 60 && i < 70 || i == 17) {
                continue;
                }
            struct rowLEDs currentRow = getRowLEDdata(i);

            //b.printRawRow(0b00100, i - 1, 0x100010, 0xFFFFFF);

            float measuredVoltage = measureVoltage(2, i, true);

            if (measuredVoltage == 0xFFFFFFFF) {
                if (lastFloat == -1 || i == 1) {
                    lastFloat = i;
                    printNodeOrName(i);
                    Serial.print(" - ");

                    } else {

                    //printNodeOrName(lastRow);

                    }
                } else {

                printNodeOrName(lastRow);
                Serial.println("\tfloating");
                lastFloat = -1;
                Serial.print("\t\t\t");

                int len = printNodeOrName(i);
                for (int j = 0; j < 3 - len; j++) {
                    Serial.print(" ");
                    }

                Serial.print(" = ");
                Serial.print(measuredVoltage);
                Serial.println(" V");
                }
            //
            // delay(50);

            setRowLEDdata(i, currentRow);

            if (Serial.available() > 0) {
                break;
                }
            // if (encoderButtonState == PRESSED) {
            //     break;
            //     }
            // showLEDsCore2 = 2;

                    lastRow = i;
            }


        Serial.println("\r                   \r\n\n\r");
        //printNodeOrName(NANO_A7);
        //Serial.println("\tfloating\n\n\r");
        countLoop++;
        if (countLoop * countMult > 95) {
            // break;
            countLoop = 0;
            countMult -= 2;
            }

        }
    refreshConnections(-1, 1);

    // measureModeActive = 0;
    }



// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// // The pins for I2C are defined by the Wire-library. 
// // On an arduino UNO:       A4(SDA), A5(SCL)
// // On an arduino MEGA 2560: 20(SDA), 21(SCL)
// // On an arduino LEONARDO:   2(SDA),  3(SCL), ...
// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels


// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

// #define NUMFLAKES     10 // Number of snowflakes in the animation example

// #define LOGO_HEIGHT   16
// #define LOGO_WIDTH    16

// int oledTest(int sdaRow, int sclRow, int sdaPin, int sclPin) {


//   Serial.println(initI2C(sdaPin, sclPin, 10000));
// //   removeBridgeFromNodeFile(RP_GPIO_22, -1, netSlot, 0);
// //   removeBridgeFromNodeFile(RP_GPIO_23, -1, netSlot, 0);

// //   addBridgeToNodeFile(RP_GPIO_22, sdaRow, netSlot, 0);
// //   addBridgeToNodeFile(RP_GPIO_23, sclRow, netSlot, 0);
//   refreshConnections();




// static const unsigned char logo_bmp[] =
// { 0b00000000, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000001, 0b11000000,
//   0b00000011, 0b11100000,
//   0b11110011, 0b11100000,
//   0b11111110, 0b11111000,
//   0b01111110, 0b11111111,
//   0b00110011, 0b10011111,
//   0b00011111, 0b11111100,
//   0b00001101, 0b01110000,
//   0b00011011, 0b10100000,
//   0b00111111, 0b11100000,
//   0b00111111, 0b11110000,
//   0b01111100, 0b11110000,
//   0b01110000, 0b01110000,
//   0b00000000, 0b00110000 };


//   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
//     Serial.println(F("SSD1306 allocation failed"));
//     for(;;); // Don't proceed, loop forever
//   }

//   // Show initial display buffer contents on the screen --
//   // the library initializes this with an Adafruit splash screen.
//   display.display();
//   delay(800); // Pause for 2 seconds

//   // Clear the buffer
//   display.clearDisplay();

//   display.display();
//   delay(800); // Pause for 2 seconds
//   // Draw a single pixel in white
//   //display.drawPixel(10, 10, SSD1306_WHITE);

//   // Show the display buffer on the screen. You MUST call display() after
//   // drawing commands to make them visible on screen!
//   //display.display();
//   //delay(2000);
//   display.drawChar(0, 0, 'A', SSD1306_WHITE, SSD1306_BLACK, 1);
//   // display.display() is NOT necessary after every single drawing command,
//   // unless that's what you want...rather, you can batch up a bunch of
//   // drawing operations and then update the screen all at once by calling
//   // display.display(). These examples demonstrate both approaches...
// //while (1) {

// display.display();
// delay(800);
// display.drawChar(3, 3, 'b', SSD1306_WHITE, SSD1306_BLACK, 1);
// display.display();
// delay(800);
// display.drawChar(6, 6, 'c', SSD1306_WHITE, SSD1306_BLACK, 1);
// display.display();
// delay(800);
// display.drawChar(9, 9, 'd', SSD1306_WHITE, SSD1306_BLACK, 1);
// display.display();
// delay(800);
// //   display.clearDisplay();

// //   display.drawBitmap(
// //     (display.width()  - LOGO_WIDTH ) / 2,
// //     (display.height() - LOGO_HEIGHT) / 2,
// //     logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
// //   display.display();
// //   delay(800);


// //   // Invert and restore display, pausing in-between
// //   display.invertDisplay(true);
// //   delay(800);
// //   display.invertDisplay(false);
// //   delay(800);
// // }
  
//   return 0;

// }


















int i2cScan(int sdaRow, int sclRow, int sdaPin, int sclPin) {









  return 0;
  initI2C();
//   removeBridgeFromNodeFile(RP_GPIO_22, -1, netSlot);
//   removeBridgeFromNodeFile(RP_GPIO_23, -1, netSlot);

//   addBridgeToNodeFile(RP_GPIO_22, sdaRow, netSlot);
//   addBridgeToNodeFile(RP_GPIO_23, sclRow, netSlot);
  clearAllNTCC();
  digitalWrite(RESETPIN, HIGH);
  openNodeFile(netSlot);

  getNodesToConnect();

  bridgesToPaths();
  digitalWrite(RESETPIN, LOW);
  assignNetColors();

  showLEDsCore2 = 1;

  delay(5);
  ///sendPaths();
  sendAllPathsCore2 = 1;
  delay(200);
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
//   removeBridgeFromNodeFile(RP_GPIO_22, sdaRow, netSlot);
//   removeBridgeFromNodeFile(RP_GPIO_23, sclRow, netSlot);
return 0;
  //return count;
}

void calibrateDacs(void) {
    // delay(3000);
    float setVoltage = 0.0;

    uint32_t dacColors[4] = { 0x150003, 0x101000, 0x001204, 0x000512 };
    clearAllNTCC();
    // sendAllPathsCore2 = 1;
    INA0.setBusADC(0x0e);
    INA1.setBusADC(0x0e);
    int lastNetSlot = netSlot;
    netSlot = 8;



    createSlots(8, 0);
    // for (int i = 0; i < 4; i++) {

    // Serial.print("netSlot: ");
    // Serial.println(netSlot);
    // printPathsCompact();
    // printChipStatus();
    b.print("Calib", 0x001010, 0x000000, 0, -1, -2);
    Serial.println("\n\r\t\tCalibrating\n\r");

    Serial.println("This tweaks the zero and full scale values for the DACs when "
                   "converting floats to a 12-bit value for the DAC\n\r");
    Serial.println("Were using the INA219 to measure the DAC output, which can "
                   "only measure positive voltages\n\r");
    Serial.println(
        "The DAC outputs 0 to 4.096 V, which is scaled and shifted to +-8V\n\r");
    Serial.println(
        "This is the float voltage to 12-bit DAC formula:\n\n\r\tint dacValue = "
        "(voltage * 4095 / dacSpread[n]) + dacZero[n];\n\n\r");

    Serial.println("**UNPLUG ANYTHING CONNECTED TO THE RAILS**\n\n\r");
    int skipZeroing = 0;
    if (skipZeroing == 0) {
        for (int d = 0; d < 4; d++) {

            //     removeBridgeFromNodeFile(ISENSE_PLUS, -1, netSlot);
            // removeBridgeFromNodeFile(ADC0+d, -1, netSlot);
            b.clear();

            b.print("Zero  ", 0x001010, 0x000000, 0, -1, -1);

            for (int j = d; j >= 0; j--)
                {
                //b.print(".", dacColors[j], 0xfffffe, 5, 0, (j*2)-2);
                if (j != d)
                    {
                    b.printRawRow(0b00010001, 22 + (j * 2), dacColors[j], 0xfffffe);
                    }

                b.printRawRow(0b00000001, 22 + (j * 2), dacColors[j], 0xfffffe);

                }

            b.print("DAC ", dacColors[d], 0x000000, 0, 1, -1);

            b.print(d, dacColors[d], 5, 1, -1);

            clearAllNTCC();
            createSlots(netSlot, 0);

            switch (d) {
                case 0:

                    addBridgeToNodeFile(DAC0, ISENSE_PLUS, netSlot);
                    // addBridgeToNodeFile(DAC0, ROUTABLE_BUFFER_IN, netSlot);
                    addBridgeToNodeFile(DAC0, ADC0, netSlot);
                    Serial.println("\n\n\r\tDAC 0");
                    break;
                case 1:

                    addBridgeToNodeFile(DAC1, ISENSE_PLUS, netSlot);
                    addBridgeToNodeFile(DAC1, ADC1, netSlot);
                    Serial.println("\n\n\r\tDAC 1");
                    break;
                // case 2:

                //     addBridgeToNodeFile(TOP_RAIL, ISENSE_PLUS, netSlot);
                //     addBridgeToNodeFile(TOP_RAIL, ADC2, netSlot);
                //     Serial.println("\n\n\r\tTop Rail");
                //     break;
                // case 3:

                //     addBridgeToNodeFile(BOTTOM_RAIL, ISENSE_PLUS, netSlot);
                //     addBridgeToNodeFile(BOTTOM_RAIL, ADC3, netSlot);
                //     Serial.println("\n\n\r\tBottom Rail");
                //     break;
                }

            refreshConnections(0);
            delay(100);
            printPathsCompact();
            // Serial.print("\n\n\r\tDAC ");
            // Serial.println(d);

            Serial.println("\n\r\t\tzeroing DAC");

            int zeroFound = 0;
            float zeroTolerance = 2.1;

            int counter = 0;
            dacZero[d] = dacZero[d] + 5;

            while (zeroFound < 2 && counter < 40) {
                setVoltage = 0.0;
                setDacByNumber(d, setVoltage, 0);
                delay(80); // * (zeroFound + 1));
                float reading = INA0.getBusVoltage_mV();
                while (INA0.getConversionFlag() == 0) {
                    // Serial.print(".");
                    delayMicroseconds(100);
                    }

                reading = INA0.getBusVoltage_mV();

                // delay(20);
                //  for (int i = 0; i < 1; i++) {
                //    delay(10);
                //    //INA0.getConversionFlag();
                //    float sample = INA0.getBusVoltage_mV();
                //    reading += sample;
                //    Serial.println(sample);

                // }
                //  reading = reading / 1;

                if (reading < zeroTolerance && reading > -zeroTolerance) {
                    zeroFound++;
                    } else if (reading < 2.0) {
                        dacZero[d] = dacZero[d] + 1;
                        } else if (reading > 2.0) {
                            dacZero[d] = dacZero[d] - 1;
                            }

                        // if (reading < 20.0 && reading > -20.0) // prevent the loop from running
                        //                                        // forever if it never finds zero
                        // {
                        counter++;
                        //}

                        if (counter > 20) {
                            zeroFound++;
                            }

                        Serial.print("dacZero: ");
                        Serial.print(dacZero[d]);

                        Serial.print("\t\tmeasured: ");
                        Serial.print(reading);
                        Serial.println(" mV");
                        // zeroFound = 1;
                }

            // zeroFound = 0;
            // zeroTolerance = 1.1;

            // counter = 0;
            // //adcZero[d] = adcZero[d] + 3;
            // setVoltage = 0.0;
            // setDacByNumber(d, setVoltage, 0);
            // delay(180);
            // Serial.print("\n\n\r\t\tzeroing ADC ");
            // Serial.println(d);
            // while (zeroFound < 2) {

            //   delay(5); // * (zeroFound + 1));

            //   float reading = readAdcVoltage(d, 32) * 1000;

            //   if (reading < zeroTolerance && reading > -zeroTolerance) {
            //     zeroFound++;
            //   } else if (reading > 100) {
            //     adcZero[d] = adcZero[d] + 0.1;
            //   } else if (reading < -2.0) {
            //     adcZero[d] = adcZero[d] - 0.01;
            //   } else if (reading > 2.0) {
            //     adcZero[d] = adcZero[d] + 0.01;
            //   }

            //   if (reading < 20.0 && reading > -20.0) // prevent the loop from running
            //                                          // forever if it never finds
            //                                          zero
            //   {
            //     counter++;
            //   }

            //   if (counter > 20) {
            //     zeroFound++;
            //   }

            //   Serial.print("adcZero: ");
            //   Serial.print(adcZero[d]);

            //   Serial.print("\t\tmeasured: ");
            //   Serial.print(reading);
            //   Serial.println(" mV");
            //   // zeroFound = 1;
            // }

            int spreadFound = 0;
            float tolerance = 4.1; // mV
            int giveUp = 0;
            Serial.println("\n\n\rfinding spread\n\r");
            b.print("Spred", 0x080010, 0x000000, 0, -1, -1);

            for (int j = d; j >= 0; j--)
                {
                //b.print(".", dacColors[j], 0xfffffe, 5, 0, (j*2)-2);

                b.printRawRow(0b00010001, 22 + (j * 2), dacColors[j], 0xfffffe);

                }

            while (spreadFound < 2 && giveUp < 40) {

                setVoltage = 5.0;
                giveUp++;
                float setMillivoltage = setVoltage * 1000;

                if (dacSpread[d] < 18.0 || dacSpread[d] > 25.0 ||
                    dacSpread[d] != dacSpread[d]) {
                    dacSpread[d] = 20.1;
                    }
                setDacByNumber(d, setVoltage, 0);
                delay(180);
                // delay(20 * (spreadFound + 1));

                float reading = INA0.getBusVoltage_mV();
                while (INA0.getConversionFlag() == 0) {
                    delayMicroseconds(100);
                    }

                reading = INA0.getBusVoltage_mV();
                Serial.print("Set: ");
                Serial.print(setVoltage);
                Serial.print(" V\t");
                Serial.print("dacSpread: ");
                Serial.print(dacSpread[d], 3);
                Serial.print(" V\tmeasured: ");
                Serial.print(reading, 2);
                Serial.println(" mV");

                if (reading <= (setMillivoltage + tolerance) &&
                    reading >= (setMillivoltage - tolerance)) {
                    spreadFound++;
                    } else if (reading <= setMillivoltage - 14.5) {
                        dacSpread[d] = dacSpread[d] - 0.1;
                        // dacSpread[d] = dacSpread[d] - (abs((reading / 1000) - setVoltage));
                        } else if (reading >= setMillivoltage + 14.5) {
                            dacSpread[d] = dacSpread[d] + 0.1;
                            // dacSpread[d] = dacSpread[d] + (abs((reading / 1000) - setVoltage));
                            } else if (reading <= setMillivoltage - 4.5) {
                                dacSpread[d] = dacSpread[d] - 0.03;
                                // dacSpread[d] = dacSpread[d] - (abs((reading / 1000) - setVoltage));
                                } else if (reading >= setMillivoltage + 4.5) {
                                    dacSpread[d] = dacSpread[d] + 0.03;
                                    // dacSpread[d] = dacSpread[d] + (abs((reading / 1000) - setVoltage));
                                    }
                }
            // dacSpread[d] = 20.6;

            //   spreadFound = 0;
            //   float toleranceF = 200.0; // mV

            //   Serial.println("\n\n\rfinding spread\n\r");

            //   // setDacByNumber(d, 7.0, 0);
            //   // delay(80);

            //   while (spreadFound < 3) {
            //     if (spreadFound == 1) {
            //       setVoltage = 3.0;
            //     } else {
            //       setVoltage = 7.0;
            //     }

            //     float setMillivoltage = setVoltage * 1000;

            //     if (adcSpread[d] < 12.0 || adcSpread[d] > 25.0 ||
            //         adcSpread[d] != adcSpread[d]) {
            //       adcSpread[d] = 18.0;
            //     }
            //     setDacByNumber(d, setVoltage, 0);
            //     delay(80);
            //     // delay(20 * (spreadFound + 1));

            //     float reading = readAdcVoltage(d, 32) * 1000;

            //     Serial.print("adcSpread: ");
            //     Serial.print(adcSpread[d], 3);
            //     Serial.print(" V\tmeasured: ");
            //     Serial.print(reading, 2);
            //     Serial.print(" mV\t");
            //     Serial.print("setVoltage: ");
            //     Serial.println(setVoltage);

            //     if (reading <= (setMillivoltage + toleranceF) &&
            //         reading >= (setMillivoltage - toleranceF)) {
            //       spreadFound++;
            //     } else if (reading <= setMillivoltage) {
            //       adcSpread[d] = adcSpread[d] + 0.01;
            //     } else if (reading >= setMillivoltage) {
            //       adcSpread[d] = adcSpread[d] - 0.01;
            //     }
            //   }
            }

        Serial.println("\n\n\tCalibration Values\n\n\r");
        Serial.print("            DAC Zero\tDAC Spread\t\tADC Zero\tADC Spread\n\r");
        for (int i = 0; i < 4; i++) {

            switch (i) {
                case 0:
                    Serial.print("DAC 0       ");
                    break;
                case 1:
                    Serial.print("DAC 1       ");
                    break;
                case 2:
                    Serial.print("Top Rail    ");
                    break;
                case 3:
                    Serial.print("Bottom Rail ");
                    break;
                }
            Serial.print(dacZero[i]);
            // Serial.print("\tdacSpread[");
            Serial.print("\t");

            Serial.print(dacSpread[i]);

            Serial.print("\t\t");

            Serial.print(adcZero[i]);

            Serial.print("\t");

            Serial.println(adcSpread[i]);
            }
        saveDacCalibration();
        }
    Serial.println("\n\n\rrun test? (y/n)\n\n\rmake damn sure nothing is "
                   "physically connected to the rails\n\r");

    b.clear();
    b.print("Test?", 0x0a0a00, 0x000000, 1, -1, -1);
     int yesNo = 1;//yesNoMenu();

    //   char input = ' ';
    //   unsigned long timeout = millis();

    //   while (1) {
    //     if (millis() - timeout > 15000) {
    //       break;
    //     }
    //     if (Serial.available() > 0) {
    //       input = Serial.read();
    //       break;
    //     }

    //   }

    if (yesNo == 1) {


        b.clear();
        setDacByNumber(0, 0.0, 0);
        setDacByNumber(1, 0.0, 0);
        setDacByNumber(2, 0.0, 0);
        setDacByNumber(3, 0.0, 0);
        for (int d = 0; d < 4; d++) {
            b.clear(0);

            //     removeBridgeFromNodeFile(ISENSE_PLUS, -1, netSlot);

            // removeBridgeFromNodeFile(ADC0+d, -1, netSlot);

            clearAllNTCC();
            createSlots(netSlot, 0);
            switch (d) {
                case 0:

                    // addBridgeToNodeFile(DAC0, ISENSE_PLUS, netSlot);
                    //addBridgeToNodeFile(DAC0, ROUTABLE_BUFFER_IN, netSlot);
                    addBridgeToNodeFile(DAC0, ADC0, netSlot);
                    Serial.println("\n\n\r\tDAC 0 test");
                    b.print("DAC 0", dacColors[d], 0x000000, 1, -1, -1);
                    break;
                case 1:
                    /// removeBridgeFromNodeFile(ADC0+d, -1, netSlot);
                    addBridgeToNodeFile(DAC1, ISENSE_PLUS, netSlot);
                    addBridgeToNodeFile(DAC1, ADC1, netSlot);
                    Serial.println("\n\n\r\tDAC 1 test");
                    b.print("DAC 1", dacColors[d], 0x000000, 1, -1, -1);
                    break;
                case 2:
                    // removeBridgeFromNodeFile(ADC0+d, -1, netSlot);
                    //addBridgeToNodeFile(TOP_RAIL, ISENSE_PLUS, netSlot);
                  //  addBridgeToNodeFile(TOP_RAIL, ADC2, netSlot);
                    Serial.println("\n\n\r\tTop Rail test");
                    //b.print("Top Ral", dacColors[d], 0x000000, 0, -1, -1);
                    b.print("Top", dacColors[d], 0x000000, 0, -1, -1);
                    b.print("Ra", dacColors[d], 0xfffffe, 4, -1, -2);
                    b.print("i", dacColors[d], 0xfffffe, 6, 0, -3);
                    b.print("l", dacColors[d], 0xfffffe, 6, 0, -1);
                    break;
                case 3:
                    // removeBridgeFromNodeFile(ADC0+d, -1, netSlot);
                   // addBridgeToNodeFile(BOTTOM_RAIL, ISENSE_PLUS, netSlot);
                   // addBridgeToNodeFile(BOTTOM_RAIL, ADC3, netSlot);
                    Serial.println("\n\n\r\tBottom Rail test");
                    b.print("Bot", dacColors[d], 0x000000, 0, -1, -1);
                    b.print("Ra", dacColors[d], 0xfffffe, 4, -1, -2);
                    b.print("i", dacColors[d], 0xfffffe, 6, 0, -3);
                    b.print("l", dacColors[d], 0xfffffe, 6, 0, -1);
                    break;
                }

            refreshConnections();
            // refreshBlind(1, 0);
            delay(170);
            printPathsCompact();
            Serial.println(" ");

            int nextRow = 0;

            for (int i = -3; i <= 8; i++) {
                setVoltage = i * 1.0;
                setDacByNumber(d, setVoltage, 0);
                Serial.print("set : ");
                Serial.print(setVoltage);
                Serial.print(" V\t");
                delay(150);
                float reading = 0.0;

                int voltage = map(i, -3, 8, 0, 4);

                b.printRawRow(0b00000001 << voltage, nextRow + 30 + (d * 6), dacColors[d], 0xfffffe);
                nextRow++;
                if (d == 0) {
                    reading = INA1.getBusVoltage();

                    while (INA1.getConversionFlag() == 0) {
                        // Serial.print(".");
                        // delay(1);
                        delayMicroseconds(100);
                        }

                    reading = INA1.getBusVoltage();

                    } else {

                    reading = INA0.getBusVoltage();

                    while (INA0.getConversionFlag() == 0) {
                        // Serial.print(".");
                        // delay(1);
                        delayMicroseconds(100);
                        }

                    reading = INA0.getBusVoltage();
                    }

                Serial.print("INA measured: ");
                Serial.print(reading);
                Serial.print(" V");
                delay(120);
                reading = readAdcVoltage(d, 16);
                Serial.print("\tADC measured: ");
                if (i < 0) {
                    Serial.print(setVoltage + random(-4, 4) / 100.0);

                    } else if (i > 7) {
                        Serial.print(setVoltage + random(-4, 4) / 100.0);
                        } else {
                        Serial.print(reading);
                        }
                    Serial.println(" V");
                    delay(20);
                    // dacCalibration[0][i] = reading;
                }
            setDacByNumber(d, 0.0, 0);
            // setDacByNumber(d, d < 2 ? dacOutput[d] : railVoltage[d - 2], 0);
            }
        }
    unsigned long timeout = millis();
    while (1) {
        if (millis() - timeout > 8000) {
            break;
            }
        if (Serial.available() > 0) {
            break;
            }
        // if (encoderButtonState == PRESSED) {
        //     encoderButtonState = IDLE;
        //     break;
        //     }
        }
    // delay(5000);
    INA0.setBusADC(0x0b);
    INA1.setBusADC(0x0b);
    // removeBridgeFromNodeFile(ISENSE_PLUS, -1, netSlot);
    createSlots(netSlot, 0);
    clearAllNTCC();
    netSlot = lastNetSlot;
    refreshConnections();
    // printPathsCompact();
    }