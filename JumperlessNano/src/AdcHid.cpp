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

Adafruit_USBD_HID USBHID;

uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(sizeof(adcReadings))
};

// samples all four pins, round-robin
ADCInput adc(A0, A1, A2, A3);

static uint16_t getReportCallback(uint8_t report_id, hid_report_type_t report_type,
                             uint8_t *buffer, uint16_t reqlen) {
  // ignore this.
  return 0;
}

static void setReportCallback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // ignore this as well.
}

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

  // only send readings if HID is "ready". Otherwise `sendReport` just hangs.
  if (tud_hid_n_ready(0)) {
    USBHID.sendReport(0, buf, sizeof(buf));
  }
}

void setupAdcHidStuff() {
  USBHID.setPollInterval(1);
  USBHID.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  USBHID.setStringDescriptor("Jumperless USB Analog");
  USBHID.setReportCallback(getReportCallback, setReportCallback);
  USBHID.begin();

  adc.setBuffers(4, 32);
  adc.onReceive(adcCallback);
  // the sample rate passed here is already adjusted for the number of pins.
  // in other words: passing 1000 actually samples at a rate of 4000, such that
  // values for all 4 pins are available every millisecond.
  adc.begin(1000);
}
