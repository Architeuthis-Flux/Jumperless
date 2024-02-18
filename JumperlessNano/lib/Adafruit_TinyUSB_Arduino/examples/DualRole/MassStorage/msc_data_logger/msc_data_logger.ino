/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/


/* This example demonstrates use of both device and host, where
 * - Device run on native usb controller (controller0)
 * - Host run on bit-banging 2 GPIOs with the help of Pico-PIO-USB library (controller1)
 *
 * Example will log CPU temperature periodically (ms,value) to USB thumbdrive
 *
 * Requirements:
 * - [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) library
 * - 2 consecutive GPIOs: D+ is defined by PIN_USB_HOST_DP, D- = D+ +1
 * - Provide VBus (5v) and GND for peripheral
 * - CPU Speed must be either 120 or 240 Mhz. Selected via "Menu -> CPU Speed"
 */

// pio-usb is required for rp2040 host
#include "pio_usb.h"

// SdFat is required for using Adafruit_USBH_MSC_SdFatDevice
#include "SdFat.h"

// TinyUSB lib
#include "Adafruit_TinyUSB.h"

// Pin D+ for host, D- = D+ + 1
#ifndef PIN_USB_HOST_DP
#define PIN_USB_HOST_DP       16
#endif

// Pin for enabling Host VBUS. comment out if not used
#ifndef PIN_5V_EN
#define PIN_5V_EN        18
#endif

#ifndef PIN_5V_EN_STATE
#define PIN_5V_EN_STATE  1
#endif


#define LOG_FILE        "cpu_temp.csv"
#define LOG_INTERVAL    5000

// USB Host object
Adafruit_USBH_Host USBHost;

// USB Host MSC Block Device object which implemented API for use with SdFat
Adafruit_USBH_MSC_BlockDevice msc_block_dev;

// file system object from SdFat
FatVolume fatfs;
File32 f_log;

// if file system is successfully mounted on usb block device
volatile bool is_mounted = false;

//--------------------------------------------------------------------+
// Setup and Loop on Core0
//--------------------------------------------------------------------+

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  //while ( !Serial ) delay(10);   // wait for native usb

  Serial.println("TinyUSB Host MassStorage Data Logger Example");
}

void loop()
{
  if (!is_mounted) {
    // nothing to do
    delay(1000);
    return;
  }

  // Turn on LED when start writing
  digitalWrite(LED_BUILTIN, HIGH);

  f_log = fatfs.open(LOG_FILE, O_WRITE | O_APPEND | O_CREAT);

  if (!f_log) {
    Serial.println("Cannot create file: " LOG_FILE);
  }else {
    float cpu_temp = analogReadTemp();
    uint32_t ms = millis();

    Serial.printf("%u,%.02f\r\n", millis(), cpu_temp);
    f_log.printf("%u,%.02f\r\n", millis(), cpu_temp);

    f_log.close();
  }

  delay(LOG_INTERVAL);
}

//--------------------------------------------------------------------+
// Setup and Loop on Core1
//--------------------------------------------------------------------+

void setup1() {
  //while ( !Serial ) delay(10);   // wait for native usb
  Serial.println("Core1 setup to run TinyUSB host with pio-usb");

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
    while ( !Serial ) {
      delay(10);   // wait for native usb
    }
    Serial.printf("Error: CPU Clock = %lu, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n");
    while(1) {
      delay(1);
    }
  }

#ifdef PIN_5V_EN
  pinMode(PIN_5V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, PIN_5V_EN_STATE);
#endif

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = PIN_USB_HOST_DP;
  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}

void loop1()
{
  USBHost.task();
  Serial.flush();
}

//--------------------------------------------------------------------+
// TinyUSB Host callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted (configured)
void tuh_mount_cb (uint8_t daddr)
{
  (void) daddr;
}

/// Invoked when device is unmounted (bus reset/unplugged)
void tuh_umount_cb(uint8_t daddr)
{
  (void) daddr;
}

// Invoked when a device with MassStorage interface is mounted
void tuh_msc_mount_cb(uint8_t dev_addr)
{
  // Initialize block device with MSC device address
  msc_block_dev.begin(dev_addr);

  // For simplicity this example only support LUN 0
  msc_block_dev.setActiveLUN(0);

  msc_block_dev.setWriteCompleteCallback(write_complete_callback);

  is_mounted = fatfs.begin(&msc_block_dev);

  if (is_mounted) {
    fatfs.ls(&Serial, LS_SIZE);
  }else {
    Serial.println("Failed to mount mass storage device. Make sure it is formatted as FAT");
  }
}

// Invoked when a device with MassStorage interface is unmounted
void tuh_msc_umount_cb(uint8_t dev_addr)
{
  (void) dev_addr;

  // unmount file system
  is_mounted = false;
  fatfs.end();

  // end block device
  msc_block_dev.end();
}


bool write_complete_callback(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data)
{
  (void) dev_addr;
  (void) cb_data;

  // turn off LED after write is complete
  // Note this only marks the usb transfer is complete, device can take longer to actual
  // write data to physical flash
  digitalWrite(LED_BUILTIN, LOW);

  return true;
}

