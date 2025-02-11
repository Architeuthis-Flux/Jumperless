// #ifdef USE_FATFS


// #include "USBfs.h" 

// #include <FatFS.h>
// #include <FatFSUSB.h>
// #include "FileParsing.h"

// volatile bool updated = false;
// volatile bool driveConnected = false;
// volatile bool inPrinting = false;

// void printDirectory(String dirName, int numTabs);

// // Called by FatFSUSB when the drive is released.  We note this, restart FatFS, and tell the main loop to rescan.
// void unplug(uint32_t i) {
//   (void) i;
//   driveConnected = false;
//   updated = true;
//   FatFS.begin();
// }

// // Called by FatFSUSB when the drive is mounted by the PC.  Have to stop FatFS, since the drive data can change, note it, and continue.
// void plug(uint32_t i) {
//   (void) i;
//   driveConnected = true;
//   FatFS.end();
// }

// // Called by FatFSUSB to determine if it is safe to let the PC mount the USB drive.  If we're accessing the FS in any way, have any Files open, etc. then it's not safe to let the PC mount the drive.
// bool mountable(uint32_t i) {
//   (void) i;
//   return !inPrinting;
// }

// void usbFSbegin() {
//   //Serial.begin(115200);
//   closeAllFiles();
  
//   uint8_t vendor_id[8] = { 'A', 'd', 'a', 'f', 'r', 'u', 'i', 't' };
//     uint8_t product_id[16] = { 'C', 'i', 'r', 'c', 'u', 'i', 't', 'P', 'l', 'a', 'y', ' ', 'E', 'x', 'p', 'r' };
//     uint8_t product_rev[4] = { '1', '.', '0', '0' };
// //tud_msc_inquiry_cb(1, vendor_id[8], product_id[16], product_rev[4]); 
//   updated = true;
//   FatFSUSB.end();
//   FatFS.end();
//   while (!Serial) {
//     delay(1);
//   }
//   delay(500);

//   if (!FatFS.begin()) {
//     Serial.println("FatFS initialization failed!");
//     while (1) {
//       delay(1);
//     }
//   }
//   Serial.println("FatFS initialization done.");

// //   inPrinting = true;
// //   printDirectory("/", 0);
// //   inPrinting = false;

//   // Set up callbacks
//   FatFSUSB.onUnplug(unplug);
//   FatFSUSB.onPlug(plug);
//   FatFSUSB.driveReady(mountable);
//   // Start FatFS USB drive mode
//   FatFSUSB.begin();
//   Serial.println("FatFSUSB started.");
//  // Serial.println("Connect drive via USB to upload/erase files and re-display");
  
// }

// void USBloop() {
//   if (updated && !driveConnected) {
//     inPrinting = true;
//    // Serial.println("\n\nDisconnected, new file listing:");
//     printDirectory("/", 0);
//     updated = false;
//     inPrinting = false;
//   }
//   //FatFSUSB.end();
// }

// void USBdisconnect() {
//     driveConnected = false;
//   updated = true;
// unplug(0);

//   FatFSUSB.end();
//   delay(10);
//   FatFS.end();
//   FatFS.begin();
// }

// void printDirectory(String dirName, int numTabs) {
//   Dir dir = FatFS.openDir(dirName);

//   while (true) {

//     if (!dir.next()) {
//       // no more files
//       break;
//     }
//     // for (uint8_t i = 0; i < numTabs; i++) {
//     //   Serial.print('\t');
//     // }
//     Serial.println(dir.fileName());
//     if (dir.isDirectory()) {
//      // Serial.println("/");
//      // printDirectory(dirName + "/" + dir.fileName(), numTabs + 1);
//     } else {
//       // files have sizes, directories do not
//       //Serial.print("\t\t");
//       //Serial.print(dir.fileSize(), DEC);
//       time_t cr = dir.fileCreationTime();
//       struct tm* tmstruct = localtime(&cr);
//       //Serial.printf("\t%d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
//     }
//   }
// }


// #endif