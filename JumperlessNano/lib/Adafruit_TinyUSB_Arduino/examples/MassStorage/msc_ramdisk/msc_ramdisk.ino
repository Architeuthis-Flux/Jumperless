/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include "Adafruit_TinyUSB.h"

// 8KB is the smallest size that windows allow to mount
#define DISK_BLOCK_NUM  16
#define DISK_BLOCK_SIZE 512
#include "ramdisk.h"

Adafruit_USBD_MSC usb_msc;

// Eject button to demonstrate medium is not ready e.g SDCard is not present
// whenever this button is pressed and hold, it will report to host as not ready
#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(ARDUINO_NRF52840_CIRCUITPLAY)
  #define BTN_EJECT   4   // Left Button
  bool activeState = true;

#elif defined(ARDUINO_FUNHOUSE_ESP32S2)
  #define BTN_EJECT   BUTTON_DOWN
  bool activeState = true;

#elif defined PIN_BUTTON1
  #define BTN_EJECT   PIN_BUTTON1
  bool activeState = false;
#endif


// the setup function runs once when you press reset or power the board
void setup()
{ 
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as
  // - mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("Adafruit", "Mass Storage", "1.0");

  // Set disk size
  usb_msc.setCapacity(DISK_BLOCK_NUM, DISK_BLOCK_SIZE);

  // Set callback
  usb_msc.setReadWriteCallback(msc_read_callback, msc_write_callback, msc_flush_callback);

  // Set Lun ready (RAM disk is always ready)
  usb_msc.setUnitReady(true);

#ifdef BTN_EJECT
  pinMode(BTN_EJECT, activeState ? INPUT_PULLDOWN : INPUT_PULLUP);
  usb_msc.setReadyCallback(msc_ready_callback);
#endif

  usb_msc.begin();

  Serial.begin(115200);
  //while ( !Serial ) delay(10);   // wait for native usb

  Serial.println("Adafruit TinyUSB Mass Storage RAM Disk example");
}

void loop()
{
  // nothing to do
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_callback (uint32_t lba, void* buffer, uint32_t bufsize)
{
  uint8_t const* addr = msc_disk[lba];
  memcpy(buffer, addr, bufsize);

  return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_callback (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  uint8_t* addr = msc_disk[lba];
  memcpy(addr, buffer, bufsize);

  return bufsize;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_callback (void)
{
  // nothing to do
}


#ifdef BTN_EJECT
// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool msc_ready_callback(void)
{
  // button not active --> medium ready
  return digitalRead(BTN_EJECT) != activeState;
}
#endif
