


char menuTree[] = {"\n\
$Rails$\n\
\n\
-*Both* *Top* *Bottom*\n\
-->v1\n\
\n\
Apps\n\
\n\
-Oscill oscope\n\
-MIDI   Synth\n\
-I2C    Scanner\n\
-Self   Dstruct\n\
-EEPROM Dumper\n\
-7 Seg  Mapper\n\
-Rick   Roll\n\
-$Circuts>$\n\
--555\n\
--Op Amp\n\
--$7400$\n\
---*74x109**74x161**74x42**74x595*\n\
-$Games  >$\n\
--*DOOM**Pong**Tetris**Snake*\n\
-$Manage >$\n\
--Delete\n\
--->a3\n\
--Upload\n\
--->a4\n\
-Logic  Analyzr\n\
-Scan\n\
-Calib  DACs\n\
Slots\n\
\n\
-$Load$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
-$Clear$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
-$Save to$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
Show\n\
-$Digital$\n\
\n\
--$GPIO$\n\
---*0**1**2**3**4**5**6**7*\n\
---->n4\n\
\n\
--$UART$\n\
---*Tx* *Rx*\n\
---->n2\n\
\n\
--$I2C$\n\
---*SDA* *SCL*\n\
---->n2\n\
\n\
-$Current$\n\
--*Pos* *Neg*\n\
--->n2\n\
\n\
-Options\n\
\n\
--Analog Display\n\
---$Type$\n\
----*Mid Out**Bot Up**Bright**Color* \n\
-----$Range$\n\
------>r\n\
---$Range$\n\
---->r\n\
\n\
--DigitalOptions\n\
---Output\n\
----*USB 2*  *Print*\n\
---$UART$\n\
----$Baud$\n\
-----*9600**19200**57600**115200*\n\
---$I2C$\n\
----$Speed$\n\
-----*100 K**400 K** 1  M**3.4 M*\n\
\n\
-$Voltage$\n\
--*0* *1* *2*\n\
--->n3\n\
\n\
Output\n\
\n\
-$GPIO$\n\
--*0**1**2**3**4**5**6**7*\n\
--->n4\n\
-$UART$\n\
--*Tx* *Rx*\n\
---Nodes>n2\n\
----*USB 2*  *Print*\n\
-----*9600* *115200*\n\
\n\
-$Buffer$\n\
--*In* *Out*\n\
--->n2\n\
\n\
--DigitalOptions\n\
---Output\n\
----*USB 2*  *Print*\n\
---$UART$\n\
----$Baud$\n\
-----*9600**19200**57600**115200*\n\
---$I2C$\n\
----$Speed$\n\
-----*100 K**400 K**1   M**3.4 M*\n\
\n\
-$Voltage$\n\
--$DAC$\n\
--*0* *1*\n\
--->v2\n\
---->n1\n\
\n\
DisplayOptions\n\
-$DEFCON$\n\
--*On**Off**Fuck*\n\
-$Colors$\n\
--*Rainbow**Shuffle*\n\
-$Jumpers$\n\
--*Wires* *Lines*\n\
-$Bright$\n\
--*1**2**3**4**5**6**7**8*\n\
\n\
RoutingOptions\n\
-Stack\n\
--$Rails$\n\
---*0**1**2**3**4**Max *\n\
--$DACs$\n\
---*0**1**2**3**4**Max *\n\
--$Paths$\n\
---*0**1**2**3**4**Max *\n\0"};



/*



char menuTree[] = {"\n\
$Rails$\n\
\n\
-*Both* *Top* *Bottom*\n\
-->v1\n\
\n\
Apps\n\
\n\
-Oscill oscope\n\
-MIDI   Synth\n\
-I2C    Scanner\n\
-Self   Dstruct\n\
-EEPROM Dumper\n\
-7 Seg  Mapper\n\
-Rick   Roll\n\
-$Circuts>$\n\
--555\n\
--Op Amp\n\
--$7400$\n\
---*74x109**74x161**74x42**74x595*\n\
-$Games  >$\n\
--*DOOM**Pong**Tetris**Snake*\n\
-$Manage >$\n\
--Delete\n\
--->a3\n\
--Upload\n\
--->a4\n\
-Logic  Analyzr\n\
Slots\n\
\n\
-$Load$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
-$Clear$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
-$Save to$\n\
--*0**1**2**3**4**5**6**7*>s\n\
\n\
Show\n\
-$Digital$\n\
\n\
--$GPIO$\n\
---*5V* *3.3V*\n\
----*0* *1* *2* *3*\n\
----->n4\n\
\n\
--$UART$\n\
---*Tx* *Rx*\n\
---->n2\n\
\n\
--$I2C$\n\
---*SDA* *SCL*\n\
---->n2\n\
\n\
-$Current$\n\
--*Pos* *Neg*\n\
--->n2\n\
\n\
-Options\n\
\n\
--Analog Display\n\
---$Type$\n\
----*Mid Out**Bot Up**Bright**Color* \n\
-----$Range$\n\
------>r\n\
---$Range$\n\
---->r\n\
\n\
--DigitalOptions\n\
---Output\n\
----*USB 2*  *Print*\n\
---$UART$\n\
----$Baud$\n\
-----*9600**19200**57600**115200*\n\
---$I2C$\n\
----$Speed$\n\
-----*100 K**400 K** 1  M**3.4 M*\n\
\n\
-$Voltage$\n\
--*0* *1* *2*\n\
--->n3\n\
\n\
Output\n\
\n\
-$GPIO$\n\
--*5V* *3.3V*\n\
---*0* *1* *2* *3*\n\
---->n4\n\
-$UART$\n\
--*Tx* *Rx*\n\
---Nodes>n2\n\
----*USB 2*  *Print*\n\
-----*9600* *115200*\n\
\n\
-$Buffer$\n\
--*In* *Out*\n\
--->n2\n\
\n\
--DigitalOptions\n\
---Output\n\
----*USB 2*  *Print*\n\
---$UART$\n\
----$Baud$\n\
-----*9600**19200**57600**115200*\n\
---$I2C$\n\
----$Speed$\n\
-----*100 K**400 K**1   M**3.4 M*\n\
\n\
-$Voltage$\n\
--$Range$\n\
--*5V* *~8V*\n\
--->v2\n\
---->n1\n\
\n\
DisplayOptions\n\
-$DEFCON$\n\
--*On**Off**Fuck*\n\
-$Colors$\n\
--*Rainbow**Shuffle*\n\
-$Jumpers$\n\
--*Wires* *Lines*\n\
-$Bright$\n\
--*1**2**3**4**5**6**7**8*\n\0"};
*/