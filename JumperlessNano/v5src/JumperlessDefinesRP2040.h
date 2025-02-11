// SPDX-License-Identifier: MIT

#ifndef JUMPERLESSDEFINESRP2040_H
#define JUMPERLESSDEFINESRP2040_H

extern volatile int sendAllPathsCore2;
#define PICO_RP2350B 1
#define INPUTBUFFERLENGTH 8000

#define PROTOTYPE_VERSION 3 //1 is the hw without the rearranged probe connections

#define REV 4
#define PROBE_REV 4


#define PIOSTUFF 1 // comment these out to remove them for debugging
#define EEPROMSTUFF 1
#define FSSTUFF 1


#define QUADRATURE_A_PIN 12
#define QUADRATURE_B_PIN 13
#define BUTTON_ENC 11

#define PROBE_PIN 10
#define BUTTON_PIN 9

#define LDAC 8



#define REVISIONADDRESS 25
#define SHOW_PROBE_CURRENT_ADDRESS 26
#define MENUBRIGHTNESS_ADDRESS 27
#define NETCOLORMODE_ADDRESS 29
#define DISPLAYMODE_ADDRESS 30
#define ROTARYENCODER_MODE_ADDRESS 31
#define DEBUG_FILEPARSINGADDRESS 32
#define TIME_FILEPARSINGADDRESS 33
#define DEBUG_NETMANAGERADDRESS 34
#define TIME_NETMANAGERADDRESS 35
#define DEBUG_LEDSADDRESS 36
#define DEBUG_NETTOCHIPCONNECTIONSADDRESS 37
#define DEBUG_NETTOCHIPCONNECTIONSALTADDRESS 38
#define LEDBRIGHTNESSADDRESS 39
#define RAILBRIGHTNESSADDRESS 40
#define SPECIALBRIGHTNESSADDRESS 41
#define PROBESWAPADDRESS 42

#define FIRSTSTARTUPADDRESS 43

#define TOP_RAIL_ADDRESS0 44
#define BOTTOM_RAIL_ADDRESS0 52

#define LOGO_TOP_ADDRESS0 60
#define LOGO_TOP_ADDRESS1 64

#define LOGO_BOTTOM_ADDRESS0 68
#define LOGO_BOTTOM_ADDRESS1 72

#define BUILDING_TOP_ADDRESS0 76
#define BUILDING_TOP_ADDRESS1 80

#define BUILDING_BOTTOM_ADDRESS0 84
#define BUILDING_BOTTOM_ADDRESS1 88

#define DAC0_ADDRESS0 94
#define DAC1_ADDRESS0 102



#define DAC0_SPREAD_ADDRESS 110
#define DAC1_SPREAD_ADDRESS 118
#define TOP_RAIL_SPREAD_ADDRESS 126
#define BOTTOM_RAIL_SPREAD_ADDRESS 134

#define DAC0_ZERO_ADDRESS 142
#define DAC1_ZERO_ADDRESS 146
#define TOP_RAIL_ZERO_ADDRESS 150
#define BOTTOM_RAIL_ZERO_ADDRESS 154

#define CALIBRATED_ADDRESS 158

#define PATH_DUPLICATE_ADDRESS 160
#define POWER_DUPLICATE_ADDRESS 161
#define DAC_DUPLICATE_ADDRESS 162
#define POWER_PRIORITY_ADDRESS 163
#define DAC_PRIORITY_ADDRESS 164


#define MAX_NETS 60
#define MAX_BRIDGES 255
#define MAX_NODES 64
#define MAX_DNI 8 // max number of doNotIntersect rules
#define MAX_DUPLICATE 25 // max number of duplicates

#define MAX_NETS_FOR_WIRES 14
#define MAX_PATHS_FOR_WIRES 38
#define LASTCOMMANDADDRESS 1
#define CLEARBEFORECOMMANDADDRESS 4

#define AIRCR_Register (*((volatile uint32_t *)(PPB_BASE + 0x0ED0C)))

#define ENC_A 12
#define ENC_B 13
#define QUADRATURE_A_PIN 12
#define QUADRATURE_B_PIN 13
#define BUTTON_ENC 11

#define BOUNCE_NODE 99
#define CHIP_A 0
#define CHIP_B 1
#define CHIP_C 2
#define CHIP_D 3
#define CHIP_E 4
#define CHIP_F 5
#define CHIP_G 6
#define CHIP_H 7
#define CHIP_I 8
#define CHIP_J 9
#define CHIP_K 10
#define CHIP_L 11

// #define CS_A 6
// #define CS_B 7
// #define CS_C 8
// #define CS_D 9
// #define CS_E 10
// #define CS_F 11
// #define CS_G 12
// #define CS_H 13

// #define CS_I 20
// #define CS_J 21
// #define CS_K 22
// #define CS_L 23

// #define CS_A_EX 0b0000000010000000
// #define CS_B_EX 0b0000000001000000
// #define CS_C_EX 0b0000000000100000
// #define CS_D_EX 0b0000000000010000
// #define CS_E_EX 0b0000000000001000
// #define CS_F_EX 0b0000000000000100
// #define CS_G_EX 0b0000000000000010
// #define CS_H_EX 0b0000000000000001
// #define CS_I_EX 0b1000000000000000
// #define CS_J_EX 0b0100000000000000
// #define CS_K_EX 0b0010000000000000
// #define CS_L_EX 0b0001000000000000

// #define DATAPIN 14
#define RESETPIN 16
// #define CLKPIN 15

#define UART0_TX 0
#define UART0_RX 1

#define NANO_I2C1_SCL 2 // These might be flipped on the RP2040, fuck
#define NANO_I2C1_SDA 3

#define I2C0_SDA 4
#define I2C0_SCL 5

#define LED_DATA_OUT 17

#define ADC0_PIN 40
#define ADC1_PIN 41
#define ADC2_PIN 42
#define ADC3_PIN 43
#define ADC4_PIN 44
#define ADC5_PIN 45
#define ADC6_PIN 46
#define ADC7_PIN 47

#define T_RAIL_POS 31
#define T_RAIL_NEG 0
#define B_RAIL_POS 63
#define B_RAIL_NEG 32

#define TOP_1 1
#define TOP_2 2
#define TOP_3 3
#define TOP_4 4
#define TOP_5 5
#define TOP_6 6
#define TOP_7 7
#define TOP_8 8
#define TOP_9 9
#define TOP_10 10
#define TOP_11 11
#define TOP_12 12
#define TOP_13 13
#define TOP_14 14
#define TOP_15 15
#define TOP_16 16
#define TOP_17 17
#define TOP_18 18
#define TOP_19 19
#define TOP_20 20
#define TOP_21 21
#define TOP_22 22
#define TOP_23 23
#define TOP_24 24
#define TOP_25 25
#define TOP_26 26
#define TOP_27 27
#define TOP_28 28
#define TOP_29 29
#define TOP_30 30

#define BOTTOM_1 31
#define BOTTOM_2 32
#define BOTTOM_3 33
#define BOTTOM_4 34
#define BOTTOM_5 35
#define BOTTOM_6 36
#define BOTTOM_7 37
#define BOTTOM_8 38
#define BOTTOM_9 39
#define BOTTOM_10 40
#define BOTTOM_11 41
#define BOTTOM_12 42
#define BOTTOM_13 43
#define BOTTOM_14 44
#define BOTTOM_15 45
#define BOTTOM_16 46
#define BOTTOM_17 47
#define BOTTOM_18 48
#define BOTTOM_19 49
#define BOTTOM_20 50
#define BOTTOM_21 51
#define BOTTOM_22 52
#define BOTTOM_23 53
#define BOTTOM_24 54
#define BOTTOM_25 55
#define BOTTOM_26 56
#define BOTTOM_27 57
#define BOTTOM_28 58
#define BOTTOM_29 59
#define BOTTOM_30 60

#define NANO_D0 70 // these are completely arbitrary but they should come in handy
#define NANO_D1 71
#define NANO_D2 72
#define NANO_D3 73
#define NANO_D4 74
#define NANO_D5 75
#define NANO_D6 76
#define NANO_D7 77
#define NANO_D8 78
#define NANO_D9 79
#define NANO_D10 80
#define NANO_D11 81
#define NANO_D12 82
#define NANO_D13 83
#define NANO_RESET 84
#define NANO_AREF 85
#define NANO_A0 86
#define NANO_A1 87
#define NANO_A2 88
#define NANO_A3 89
#define NANO_A4 90
#define NANO_A5 91
#define NANO_A6 92
#define NANO_A7 93

#define NANO_RESET_0  94
#define NANO_RESET_1  95

#define NANO_GND_1 96
#define NANO_GND_0 97
#define NANO_3V3 98
#define NANO_5V 99

#define NANO_VIN 69

#define LOGO_PAD_TOP 128
#define LOGO_PAD_BOTTOM 129
#define GPIO_PAD 130
#define DAC_PAD 131
#define ADC_PAD 132
#define BUILDING_PAD_TOP 133
#define BUILDING_PAD_BOTTOM 134

#define GND 100
#define TOP_RAIL 101
#define BOTTOM_RAIL 102
#define SUPPLY_3V3 103
#define SUPPLY_5V 105

#define TOP_RAIL_GND 104
#define BOTTOM_RAIL_GND 126

#define DAC0 106
#define DAC1 107

#define ISENSE_PLUS 108
#define ISENSE_MINUS 109

#define ADC0 110
#define ADC1 111
#define ADC2 112
#define ADC3 113
#define ADC4 114
#define ADC4_5V 114
// #define ADC5 115
// #define ADC5_PROBE_SENSE 115
// #define ADC6 115
// #define ADC6_POWER_MONITOR 115
#define ADC7 115
#define ADC7_PROBE 115

#define RP_GPIO_0 114



#define RP_GPIO_18 118 // these aren't actually connected to anything
#define RP_GPIO_19 119 // but we might as well define names for them

#define SUPPLY_8V_P 120 // not actually connected to anything
#define SUPPLY_8V_N 121 // not actually connected to anything


#define RP_UART_TX 116 // also GPIO_16
#define RP_GPIO_16 116 // these are the same as the UART pins

#define RP_UART_RX 117 // also GPIO_17
#define RP_GPIO_17 117 // but if we want to use them as GPIO we should use these names

#define GPIO_TX_PIN 0
#define GPIO_RX_PIN 1

#define RP_GPIO_20 135
#define RP_GPIO_21 136
#define RP_GPIO_22 137
#define RP_GPIO_23 138

#define RP_GPIO_24 122
#define RP_GPIO_25 123
#define RP_GPIO_26 124
#define RP_GPIO_27 125


#define RP_GPIO_1 135
#define RP_GPIO_2 136
#define RP_GPIO_3 137
#define RP_GPIO_4 138

#define RP_GPIO_5 122
#define RP_GPIO_6 123
#define RP_GPIO_7 124
#define RP_GPIO_8 125

#define GPIO_1_PIN 20
#define GPIO_2_PIN 21
#define GPIO_3_PIN 22
#define GPIO_4_PIN 23
#define GPIO_5_PIN 24
#define GPIO_6_PIN 25
#define GPIO_7_PIN 26
#define GPIO_8_PIN 27



#define MCP_GPIO_0 122
#define MCP_GPIO_1 123
#define MCP_GPIO_2 124
#define MCP_GPIO_3 125



#define ROUTABLE_BUFFER_IN 139
#define ROUTABLE_BUFFER_OUT 140
/*

#define GND  100
#define 3V3  103
#define 5V  105

#define 8V  120

#define DAC0 106
#define DAC1  107

#define I_P  108
#define I_N  109

#define ADC0 110
#define ADC1 111
#define ADC2 112
#define ADC3 113

#define GPIO_0 114

#define UART_TX 116
#define GPIO_16 116  //these are the same as the UART pins

#define UART_RX 117
#define GPIO_17 117  //but if we want to use them as GPIO we should use these names


#define GPIO_18 118  //these aren't actually connected to anything
#define GPIO_19 119  //but we might as well define names for them





*/

#define EMPTY_NET 127

#endif
