# Jumperless
### a jumperless breadboard



https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/400c624a-a441-4f0a-9a22-d50209759525

Using a bunch of analog crosspoint switches wired together into one bigger switch, this breadboard makes real, hardware connections between any points on the board or the Arduino Nano header at the top via software command, instead of needing to use jumper wires.
![IMG_3580](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/3bd5be34-cf55-41ca-ad52-e8c9f27a0faf)
##### Note: all the images here are Rev 1 without the LEDs around the Nano Header
Jumperless also has a ton of voltage/current sensing so the RGB LEDs underneath each row can show a ton of information about what's going on with your circuit. 2 buffered high-current DACs (one 0-5V and one ±8V), 4 buffered and level-shifted 12-bit ADCs (3 are 0-5V and 1 for ±8V), 2 INA219 current and voltage measurement ICs, and 4 GPIO to simulate digital or analog signals that can be routed to anywhere on the breadboard or the Arduino Nano header.


https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/fdf6164d-cbd7-4331-8ade-f9c2311627e0


Use it to probe the pins on an unknown IC, or for some automated fuzzing, or to help guide you through converting a schematic to a real circuit, or do whatever the hell you want with it. 



![IMG_3903](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/575d3f79-6e66-4ce1-8229-4effe3e1fae3)

The connections are real and fully analog (-8V to +8V, up to around 1 MHz before signal quality starts to degrade) instead of cheating with some measure-then-simulate setup, although you can do that too if you want to simulate a [memristor](https://en.wikipedia.org/wiki/Memristor) or to send jumpers over the internet.

https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/2c203686-e02e-4a0c-b5eb-0cab925386fb

And no, it's not an FPGA.

![IMG_3827](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/963cac46-b46d-4c64-a201-00305d2d0bbc)

![Schematic](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/202a61f6-0eb1-44bd-9d80-3b208e9c4be2)

Here it is running a 555 circuit
https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/9c5d2b5a-3727-4197-b1d0-bd45b5f95559



https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/7cadd02e-8091-4cdd-96e2-ec0ec40cd379

![IMG_3589](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/c21f4942-95c3-4b88-a182-8505ca510e19)
![IMG_3886](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/25ff1f6f-f4e1-422c-a191-0f43e870d189)

