// SPDX-License-Identifier: MIT

#ifndef JUMPERLESS_ADC_USB_H
#define JUMPERLESS_ADC_USB_H

// if `setupAdcHidStuff` was called, this always contains somewhat up-to-date ADC readings
extern uint16_t adcReadings[4];

void setupAdcUsbStuff();

#endif
