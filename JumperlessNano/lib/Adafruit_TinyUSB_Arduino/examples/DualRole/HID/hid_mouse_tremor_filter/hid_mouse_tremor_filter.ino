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
 * Example sketch receive mouse report from host interface (from e.g consumer mouse)
 * and apply a butterworth low pass filter with a specific CUTOFF_FREQUENCY on hid mouse movement report.
 * Filtered report are send via device interface (to PC) acting as a "Mouse Tremor Filter".
 *
 * Requirements:
 * - [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) library
 * - 2 consecutive GPIOs: D+ is defined by PIN_PIO_USB_HOST_DP, D- = D+ +1
 * - Provide VBus (5v) and GND for peripheral
 * - CPU Speed must be either 120 or 240 Mhz. Selected via "Menu -> CPU Speed"
 */

// pio-usb is required for rp2040 host
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"

// Pin D+ for host, D- = D+ + 1
#ifndef PIN_USB_HOST_DP
#define PIN_USB_HOST_DP  16
#endif

// Pin for enabling Host VBUS. comment out if not used
#ifndef PIN_5V_EN
#define PIN_5V_EN        18
#endif

#ifndef PIN_5V_EN_STATE
#define PIN_5V_EN_STATE  1
#endif

// Language ID: English
#define LANGUAGE_ID 0x0409

// USB Host object
Adafruit_USBH_Host USBHost;

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_MOUSE()
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);

//------------- Low pass filter with Butterworth -------------//
// Butterworth low-pass filter coefficients
typedef struct {
  float b0, b1, b2, a1, a2;
} butterworth_coeffs_t;

#define SAMPLING_FREQUENCY 100.0  // Hz
#define CUTOFF_FREQUENCY    10.0  // Hz

// x, y
butterworth_coeffs_t coeffs[2];

butterworth_coeffs_t butterworth_lowpass(float cutoff_frequency, float sampling_frequency);
void filter_report(hid_mouse_report_t const* report);

//--------------------------------------------------------------------+
// Setup and Loop on Core0
//--------------------------------------------------------------------+

void setup()
{
  Serial.begin(115200);
  usb_hid.begin();

  coeffs[0] = butterworth_lowpass(CUTOFF_FREQUENCY, SAMPLING_FREQUENCY);
  coeffs[1] = butterworth_lowpass(CUTOFF_FREQUENCY, SAMPLING_FREQUENCY);

  //while ( !Serial ) delay(10);   // wait for native usb
  Serial.println("TinyUSB Mouse Tremor Filter Example");
}

void loop()
{
  Serial.flush();
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
    while ( !Serial ) delay(10);   // wait for native usb
    Serial.printf("Error: CPU Clock = %lu, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n");
    while(1) delay(1);
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
}


extern "C"
{

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  (void) desc_report;
  (void) desc_len;
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  Serial.printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  Serial.printf("VID = %04x, PID = %04x\r\n", vid, pid);

  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);
  if (itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
    Serial.printf("HID Mouse\r\n");
    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.printf("Error: cannot request to receive report\r\n");
    }
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  Serial.printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
}


// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  filter_report((hid_mouse_report_t const *) report);

  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Serial.printf("Error: cannot request to receive report\r\n");
  }
}

} // extern C

//--------------------------------------------------------------------+
// Low pass filter Functions
//--------------------------------------------------------------------+

butterworth_coeffs_t butterworth_lowpass(float cutoff_frequency, float sampling_frequency) {
  butterworth_coeffs_t coe;

  float omega = 2.0 * PI * cutoff_frequency / sampling_frequency;
  float c = cos(omega);
  float s = sin(omega);
  float t = tan(omega / 2.0);
  float alpha = s / (2.0 * t);

  coe.b0 = 1.0 / (1.0 + 2.0 * alpha + 2.0 * alpha * alpha);
  coe.b1 = 2.0 * coe.b0;
  coe.b2 = coe.b0;
  coe.a1 = 2.0 * (alpha * alpha - 1.0) * coe.b0;
  coe.a2 = (1.0 - 2.0 * alpha + 2.0 * alpha * alpha) * coe.b0;

  return coe;
}

float butterworth_filter(float data, butterworth_coeffs_t *coeffs, float *filtered, float *prev1, float *prev2) {
  float output = coeffs->b0 * data + coeffs->b1 * (*prev1) + coeffs->b2 * (*prev2) - coeffs->a1 * (*filtered) - coeffs->a2 * (*prev1);
  *prev2 = *prev1;
  *prev1 = data;
  *filtered = output;
  return output;
}

void filter_report(hid_mouse_report_t const* report) {
  static float filtered[2] = { 0.0, 0.0 };
  static float prev1[2] = { 0.0, 0.0 };
  static float prev2[2] = { 0.0, 0.0 };

  butterworth_filter(report->x, &coeffs[0], &filtered[0], &prev1[0], &prev2[0]);
  butterworth_filter(report->y, &coeffs[1], &filtered[1], &prev1[1], &prev2[1]);

  hid_mouse_report_t filtered_report = *report;
  filtered_report.x = (int8_t) filtered[0];
  filtered_report.y = (int8_t) filtered[1];

  usb_hid.sendReport(0, &filtered_report, sizeof(filtered_report));
}
