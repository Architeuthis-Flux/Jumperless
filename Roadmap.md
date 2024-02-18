# Jumperlab/Jumperless/jlctl Roadmap
###### this is just random stuff I thought of right now, there will be more


## Jumperlab/jlctl

- Packaged app
- Support for special functions
- Integrated terminal to see serial comms
- ADC and I_Sense readouts/plots
- Function generator/DAC interface
- Get rails/special functions/nano header to work in camera mode
- (maybe) use some object detection to automatically align board in camera mode
- Port auto firmware update and new port detection from JumperlessWokwiBridge
- Have jlctl auto disconnect when something else connects to the port and then reopen (like the bridge app) so you can update firmware without killing jlctl first.
- Integrate arduino-cli and some way to flash an Arduino

#### minor fixes
- Changing rail supply switch to anything but 5V sends ::setsupplyswitch on every netlist update



## Jumperless

- open an HID port so ADC readings/DAC outputs can be sent with minimal overhead
- Scan all rows with ADCs
- Support connecting a probe on GPIO 18 and 19 to poke out connections without a computer (and have it default to this with no host)
- Support changing config on arduino-cli to support non-standard Arduinos (anything but a regular Nano)
- Make some board adapters for nano header
- Independently test connections after they're all made and report issues (so we're not trusting the same buggy code to report it's own issues
- Support onboard (micropython of circuitpython?) scripting to make sequences of connections
- Make a separate class of nets for special functions/power. (I_Sense+- gets added twice when connected to a special function, it should be treated like the ADCs)


#### minor fixes
- Save machine mode netlist on filesystem
