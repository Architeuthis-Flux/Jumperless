So, when I was designing the Jumperless, I put in these protection resistors so you wouldn't blow up the RP2040 with the +-9V that could be floating around on the board.
<img width="808" alt="Screenshot 2023-12-21 at 9 56 31 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/defc368c-b060-4667-9ef2-326ce38906c4">
You still could do that, or we just can trust the built-in protection diodes and make it so the Jumperless is able to flash a 5V Arduino Nano with just one USB cable.

![TxRx00083](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/680c9692-f577-4b67-9476-460c68855bae)

An Arduino will program with 3.3V logic signals, but the 1K resistor is dropping the voltage below the threshold where it registers as a logic high (if the Arduino's inputs were ideal, infinite input impedance it wouldn't matter, but they're not.)

Also, just in case you're worrying about this, the RP2040's inputs are 5V tolerant even though they're not realy advertised as such.

So we're going to short these 0402 resistors, here's how:
![TxRx00000](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/270b69e0-fcac-4f0f-b571-9bc29e7465ca)

First, peel off the rubber foot right there
![TxRx00002](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/fa029239-779d-490b-ae64-2213bb464392)

0402 resistors are super easy to remove, you just melt a glob of solder onto them and they'll just float in the solder and come off stuck to your soldering iron
![3131g](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/f94a87e7-d0d7-4b51-abaf-bc3966fef71a)

Clean off the blob of solder (with the resistor probably inside) and do the same thing to the other two
![TxRx00010](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7e8ba0d3-7983-4efb-86a9-5c3ddf24f59a)

![TxRx00016](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/5b58bf2b-b381-4b47-93c0-afebe1eed2bd)

Now get a random resistor, preferably the cheap ones with the skinny legs. Or you can use some thin stripped wire.
![TxRx00022](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7de387e1-e9c6-4159-989e-faf6f31c298c)

Now tin the whole leg with solder. Hot tip: stick it into the GPIO hole to hold it.
![TxRx00023](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/358e2190-5da0-4adf-ba26-1972e37689b4)
![TxRx00024](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/272915d3-065c-4e90-b937-c8bda4989a7b)

There should be enough solder left on the pads and the leg, so you should need to add any more. Hold the leg across the 2 pads and melt it all down.
![TxRx00027](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/da736645-63cd-42d4-ad67-e9e521b6601e)
![TxRx00028](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/d0c2d391-f756-4400-b92c-b1c5c2ac2c24)
![TxRx00030](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7061a5ee-ffc9-4e3a-b582-7a3ec9290186)

Now cut off the leg right up near the pads and do the same thing with the rest of them.
![TxRx00031](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/62f7ff62-d8b0-4689-a293-ade88a11e257)
![TxRx00041](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/c8a80ddd-3a22-4040-a418-2db6eb873199)
![TxRx00044](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/480ebac4-b486-4676-a909-6934bf6702d5)

Now clean off the flux with rubbing alcohol or MG Chemicals Heavy Duty Flux Remover (this stuff is amazing, it'll mess up certain plastics, but the PA-12 Nylon the breadboard is made of isn't one of them)
![TxRx00047](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/d657b325-a206-4f82-a3af-94ca7286364c)

Replace the rubber foot
![TxRx00081](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/731f5673-9c88-4fbf-8968-3164270cadeb)


Now your hardware should be good to go. Let's flash the Arduino from Arduino IDE.

For this to work, you need to connect your UART lines between the Jumperless and the Arduino in Wokwi first. Save that so it updates on the Jumperless. 
<img width="991" alt="Screenshot 2023-12-21 at 10 56 45 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/6b3e37a8-dd39-413a-a0ee-fafafec9dd2e">

Here's what those pins on the Logic Analyzer connect to:
![LogicAnalyzerGuide](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/d75a7ff6-8560-448d-a8e9-bd6698d43e32)

Hot tip: those don't necessarily need to go to the Arduino header, you can also just have your Arduino somewhere on the breadboard, in that case you'll also need to connect the RESET lines to the appropriate place.

Now in Arduino IDE, select the higher numbered port (So on my Mac it's /dev/cu.usbmodem03, on a PC it will be COM\[higher number]
<img width="486" alt="Screenshot 2023-12-21 at 11 02 37 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/c2309117-1e52-465a-a909-d86d57b7146d">

Now hit Upload and it should work. 
<img width="1727" alt="Screenshot 2023-12-21 at 11 06 42 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/b5d0bc7e-15d9-4462-bf01-b3f4f55899b5">


If you're getting something like this

<img width="866" alt="Screenshot 2023-12-21 at 11 08 03 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/2e4f5910-30d0-42df-b459-d6588c174e23">

There are a few possible things going on:

- The Tx and Rx lines aren't connected or swapped, either Wokwi didn't update them or you messed up with the soldering and didn't short the resistors

- Something is holding the RESET line high, if it's connected somewhere in Wokwi

- The Arduino is running a sketch where it's outputting Serial data regularly, this happens a lot to me so here's what to do about it.

First, it's a good idea to put some fairly long delay(2500); in your setup() before writing anything to Serial. When flashing over direct USB it's less of an issue, but when you're doing in via the Jumperless, it seems to get in the way. 

Then hold down the Reset button on the Arduino, unplug the Jumperless from USB, plug it back in while still holding the Reset button (and continue holding it), make the connections in Wokwi to connect the UART lines and save, then press Upload in Arduino IDE and watch the output. 
When it gets to here
<img width="1720" alt="Screenshot 2023-12-21 at 9 40 03 AM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7f2c9acf-65d2-43fd-8ede-2c0836badb71">
let go of the Reset button.

That process along with the delays in setup() make sure the Serial lines are clear when it tries to flash the sketch to the Arduino.

And you should be good! Let me know if this works for you.



### Also, once all this works, the Arduino can reprogram the Jumperless by sending commands back over UART

For example, this sketch

 ```
 
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  delay(3500);
  Serial.begin(115200);
  delay(500);
}

int count = 1;

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(160);
  digitalWrite(LED_BUILTIN, LOW);
  delay(10);

  if (digitalRead(10) == 0) {
    delay(3000);
  }

  Serial.write("f 80-"); //f is to recieve connections, 80 is D10
  Serial.print(count);
  Serial.write(",116-70,117-71,");  //these 2 connections reconnect the UART lines so it's ready to send the next connection


  count++;
  if (count > 60) {
    count = 1;
  }
}

```

Will do this:

https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/362c90fa-3068-4378-ab45-0c57de7f4d49

The Arduino is telling the Jumperless to scan through each row, then pause for 3 seconds when it reads GND somewhere. This is about as fast as it can go right now (~170mS) but I'm working on making it faster.



