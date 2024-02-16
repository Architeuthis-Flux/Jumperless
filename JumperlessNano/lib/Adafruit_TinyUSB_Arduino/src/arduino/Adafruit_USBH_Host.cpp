/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022, Ha Thach (tinyusb.org) for Adafruit
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "tusb_option.h"

#if CFG_TUH_ENABLED

#include "Adafruit_TinyUSB_API.h"
#include "Adafruit_USBH_Host.h"

Adafruit_USBH_Host::Adafruit_USBH_Host(void) {}

bool Adafruit_USBH_Host::configure(uint8_t rhport, uint32_t cfg_id,
                                   const void *cfg_param) {
  return tuh_configure(rhport, cfg_id, cfg_param);
}

#ifdef ARDUINO_ARCH_RP2040
bool Adafruit_USBH_Host::configure_pio_usb(uint8_t rhport,
                                           const void *cfg_param) {
  return configure(rhport, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, cfg_param);
}
#endif

bool Adafruit_USBH_Host::begin(uint8_t rhport) { return tuh_init(rhport); }

void Adafruit_USBH_Host::task(void) { tuh_task(); }

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
TU_ATTR_WEAK void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                                   uint8_t const *desc_report,
                                   uint16_t desc_len) {
  (void)dev_addr;
  (void)instance;
  (void)desc_report;
  (void)desc_len;
}

// Invoked when device with hid interface is un-mounted
TU_ATTR_WEAK void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  (void)dev_addr;
  (void)instance;
}

// Invoked when received report from device via interrupt endpoint
TU_ATTR_WEAK void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                             uint8_t const *report,
                                             uint16_t len) {
  (void)dev_addr;
  (void)instance;
  (void)report;
  (void)len;
}
#endif
