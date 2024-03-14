// SPDX-License-Identifier: MIT

#ifdef USE_TINYUSB
#include <Adafruit_TinyUSB.h>
#endif

#ifdef CFG_TUSB_CONFIG_FILE
#include CFG_TUSB_CONFIG_FILE
#else
#include "tusb_config.h"
#endif

#include <ADCInput.h>

// Other parts of the code can just read from here instead of using `analogRead`
volatile uint16_t adcReadings[4] = { 0, 0, 0, 0 };

// samples all four pins, round-robin
ADCInput adc(A0, A1, A2, A3);

// called when the ADC buffer has data
static void adcCallback() {
  if (adc.available() < 4) {
    return;
  }
  // need a separate buffer here, since `sendReport` can't accept a volatile buffer.
  uint16_t buf[4];
  buf[0] = adcReadings[0] = adc.read();
  buf[1] = adcReadings[1] = adc.read();
  buf[2] = adcReadings[2] = adc.read();
  buf[3] = adcReadings[3] = adc.read();

  // if (tud_jumperless_ready()) {
  //   tud_jumperless_send_measurements(buf, sizeof(buf));
  // }
}


// This class is needed to feed the custom interface descriptor to the Adafruit USB library.
// The actual functionality for allocating endpoints, handling transfers etc. is implemented
// within `class/jumperless/jumperless_device.c` in the tinyusb tree.
class JumperlessUsbInterface : public Adafruit_USBD_Interface {
public:
  JumperlessUsbInterface();

  virtual uint16_t getInterfaceDescriptor(uint8_t itfnum, uint8_t *buf,
                                          uint16_t bufsize);
};

JumperlessUsbInterface::JumperlessUsbInterface() {
  // this->setStringDescriptor("Jumperless Analog");
}

uint16_t JumperlessUsbInterface::getInterfaceDescriptor(uint8_t itfnum,
                                                        uint8_t *buf,
                                                        uint16_t bufsize) {
    uint16_t epsize = CFG_TUD_JUMPERLESS_EP_BUFSIZE;
    uint8_t ep_interval = 1;
    uint8_t desc[] = {
        9, TUSB_DESC_INTERFACE, itfnum, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0x00, 0x00, 0,
        7, TUSB_DESC_ENDPOINT, 0x80, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(epsize), ep_interval,
        7, TUSB_DESC_ENDPOINT, 0x00, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(epsize), ep_interval
    };
    uint16_t const len = sizeof(desc);
    if (bufsize < len) {
        return 0;
    }

    if (buf != NULL) {
        memcpy(buf, desc, len);
    }
    return len;
}

JumperlessUsbInterface jlUsbInterface;

void setupAdcUsbStuff() {
  TinyUSBDevice.addInterface(jlUsbInterface);

  analogReadResolution(12);

  adc.setBuffers(4, 32);
  adc.onReceive(adcCallback);
  // the sample rate passed here is already adjusted for the number of pins.
  // in other words: passing 1000 actually samples at a rate of 4000, such that
  // values for all 4 pins are available every millisecond.
  adc.begin(1000);
}
