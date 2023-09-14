# Jumperless
### a jumperless breadboard

##### If you want one of these, they're available in [my Tindie store](https://www.tindie.com/products/architeuthisflux/jumperless/)


![JumperlessCropLights](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/690bea80-02cc-4e61-a65d-97b9b1a597ab)




Using a bunch of analog crosspoint switches wired together into one bigger switch, this breadboard makes real, hardware connections between any points on the board or the Arduino Nano header at the top via software command, instead of needing to use jumper wires.



![DSC00734](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/70a6e627-1bbc-4ad0-a715-84616e95f7e9)


Here's an example of me using this thing to connect some I2C pins from an Arduino to an OLED

https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/0da0fdcb-c629-4f16-8638-6fab5f5c69fc



Jumperless also has a ton of voltage/current sensing so the RGB LEDs underneath each row can show a ton of information about what's going on with your circuit. 2 buffered high-current DACs (one 0-5V and one ±8V), 4 buffered and level-shifted 12-bit ADCs (3 are 0-5V and 1 for ±8V), 2 INA219 current and voltage measurement ICs, and 4 GPIO to simulate digital or analog signals that can be routed to anywhere on the breadboard or the Arduino Nano header.



https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/e701d5c8-2349-49a1-8d57-8574962e7c29




Use it to probe the pins on an unknown IC (which is what I'm doing in the video above), or for some automated fuzzing, or to help guide you through converting a schematic to a real circuit, or do whatever the hell you want with it. 


![IMG_4228](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/4a243d6c-3955-4523-b7f4-9b39831ca136)



The connections are real and fully analog (-8V to +8V, up to around 1 MHz before signal quality starts to degrade) instead of cheating with some measure-then-simulate setup, although you can do that too if you want to simulate a [memristor](https://en.wikipedia.org/wiki/Memristor) or to send jumpers over the internet.

https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/2c203686-e02e-4a0c-b5eb-0cab925386fb


### If you want to read about how the code and stuff works, [the README in JumperlessNano](https://github.com/Architeuthis-Flux/Jumperless/tree/main/JumperlessNano) goes through how each part works in more detail.

### Or check out the [Hackaday.io page](https://hackaday.io/project/191238-jumperless) and the [Hackaday Blog Writeup](https://hackaday.com/2023/08/25/hackaday-prize-2023-jumperless-the-jumperless-jumperboard/)

And no, it's not an FPGA.

![IMG_3827](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/963cac46-b46d-4c64-a201-00305d2d0bbc)

![Schematic](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/202a61f6-0eb1-44bd-9d80-3b208e9c4be2)

Here it is running a 555 circuit (note that this is a Rev 1 without the through-PCB LEDs)

https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/9c5d2b5a-3727-4197-b1d0-bd45b5f95559


<a href="https://www.tindie.com/stores/architeuthisflux/?ref=offsite_badges&utm_source=sellers_ArchiteuthisFux&utm_medium=badges&utm_campaign=badge_large"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104"></a>


Here are some fun bonus shots
![RainBounceLarge](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/76cc09b5-138b-4f5a-8b3c-4cb5f0c9df18)

![RandomLarge](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/4674d12f-ab89-47a5-aa29-7bf0252024dd)


https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/b933e817-c6e4-4d97-9fc0-7b8d9e875ebb
[SchematicRevision3.pdf](https://github.com/Architeuthis-Flux/Jumperless/files/12603152/SchematicRevision3.pdf)


![DSC01523](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/c61467fa-b122-4526-9a08-8fbbb16326da)
![usage](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/68da817f-0bb4-4501-9174-7fe092e9d210)


https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/d070d99d-b6b3-43e3-adee-4ca2d7358a9a
![DSC01237-2](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/f757717a-d5b4-462b-af42-624a8e96e469)

![DSC01154](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/375efc39-2015-4e42-9d00-62606cfff685)

![DSC00683](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/b3e3ac9d-74ca-4bfe-ad72-39505801d832)

![IMG_3589](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/c21f4942-95c3-4b88-a182-8505ca510e19)
![IMG_3886](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/25ff1f6f-f4e1-422c-a191-0f43e870d189)

