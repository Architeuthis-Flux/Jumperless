#define PIOSTUFF 1 //comment these out to remove them
#define EEPROMSTUFF 1
#define FSSTUFF 1



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





#define MAX_NETS 64
#define MAX_BRIDGES 255
#define MAX_NODES 64
#define MAX_DNI 8 //max number of doNotIntersect rules

#define LASTCOMMANDADDRESS 1
#define CLEARBEFORECOMMANDADDRESS 4

#define AIRCR_Register (*((volatile uint32_t *)(PPB_BASE + 0x0ED0C)))


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


#define CS_A 6
#define CS_B 7
#define CS_C 8
#define CS_D 9
#define CS_E 10
#define CS_F 11
#define CS_G 12
#define CS_H 13

#define CS_I 20
#define CS_J 21
#define CS_K 22
#define CS_L 23

//#define DATAPIN 14
#define RESETPIN 24
//#define CLKPIN 15

#define UART0_TX 0
#define UART0_RX 1

#define NANO_I2C1_SCL 2 //These might be flipped on the RP2040, fuck
#define NANO_I2C1_SDA 3

#define I2C0_SDA 4
#define I2C0_SCL 5 

#define LED_DATA_OUT 6

#define ADC0_PIN 26
#define ADC1_PIN 27
#define ADC2_PIN 28
#define ADC3_PIN 29

#define T_RAIL_POS 31
#define T_RAIL_NEG 0
#define B_RAIL_POS 63
#define B_RAIL_NEG 32


#define t1  1
#define t2  2
#define t3  3
#define t4  4
#define t5  5
#define t6  6
#define t7  7
#define t8  8
#define t9  9
#define t10 10
#define t11 11
#define t12 12
#define t13 13
#define t14 14
#define t15 15
#define t16 16
#define t17 17
#define t18 18
#define t19 19
#define t20 20
#define t21 21
#define t22 22
#define t23 23
#define t24 24
#define t25 25
#define t26 26
#define t27 27
#define t28 28
#define t29 29
#define t30 30


#define b1  31
#define b2  32
#define b3  33
#define b4  34
#define b5  35
#define b6  36
#define b7  37
#define b8  38
#define b9  39
#define b10 40
#define b11 41
#define b12 42
#define b13 43
#define b14 44
#define b15 45
#define b16 46
#define b17 47
#define b18 48
#define b19 49
#define b20 50
#define b21 51
#define b22 52
#define b23 53
#define b24 54
#define b25 55
#define b26 56
#define b27 57
#define b28 58
#define b29 59
#define b30 60






#define NANO_D0  70 //these are completely arbitrary but they should come in handy
#define NANO_D1  71 
#define NANO_D2  72 
#define NANO_D3  73 
#define NANO_D4  74 
#define NANO_D5  75 
#define NANO_D6  76 
#define NANO_D7  77 
#define NANO_D8  78 
#define NANO_D9  79 
#define NANO_D10  80 
#define NANO_D11  81 
#define NANO_D12  82 
#define NANO_D13  83 
#define NANO_RESET  84 
#define NANO_AREF  85 
#define NANO_A0  86 
#define NANO_A1  87 
#define NANO_A2  88 
#define NANO_A3  89 
#define NANO_A4  90 
#define NANO_A5  91 
#define NANO_A6  92 
#define NANO_A7  93


#define GND  100 
#define SUPPLY_3V3  103
#define SUPPLY_5V  105

#define DAC0_5V  106 
#define DAC1_8V  107

#define CURRENT_SENSE_PLUS  108
#define CURRENT_SENSE_MINUS  109

#define ADC0_5V 110
#define ADC1_5V 111
#define ADC2_5V 112
#define ADC3_8V 113



#define EMPTY_NET 127