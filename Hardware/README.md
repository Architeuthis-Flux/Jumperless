# Jumperless Kit Assembly Instructions

If you get a Jumperless Kit (or order your own boards with SMD parts assembled)
The assembly is pretty easy, it's all through hole stuff.

Here's a montage of all the steps to give an overview of what you'll be doing
https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/2050284a-3e96-4c9a-879f-039811284f40

If you've gone through the trouble of ordering your own boards and everything, I'd be happy to send you a free set of spring clips, which is the only part with a huge minimum order quantity and having them made was a whole thing.


Your kit will come in ~7 parts:

- Main board (all the SMD stuff assembled)
- LED “wishbone” board (the LEDs around the header that shine through the board)
- 2 x 15 pin female headers
- Plastic breadboard shell (I’ll include white and black so you can choose)
- 80 x breadboard spring clips
- 1 x LT1054 in 8 pin DIP
- 4 x Sticky-backed rubber feet

- Carefully snap of the edge rails from the LED wishbone board. Put the V-cut on the edge of a table and slowly bend it until it breaks off. 

![20230730-67](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/345eb4d2-4319-4c58-9033-0ebe074853a4)
![20230730-74](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/a2360566-692c-4ec7-98b6-d72ae2032000)




- Use the 300 grit sandpaper lightly on the tops of the LEDs on the main board to diffuse the light a bit, you don't need to do this on the reverse-mounted "wishbone" board. You can use masking tape so you don't accidentally sand the board for aesthetic reasons (but there are very few traces on the top layer and you shouldn't be sanding hard enough to break a trace.)

![20230730-90](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/9d64dc30-808f-4c2e-a8e5-5ac36befbe38)

![20230730-94](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/ca246279-93da-463c-9ba2-2fb5084ebd9b)

![20230730-97](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/68c4dd48-8bef-47e6-be64-890f7a312406)
![20230730-104](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/3088492a-6ec5-4f76-be9e-261807ed4367)


When you're finished the LEDs should look like this (before is on the left)

![20230730-109](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/3d167920-6062-46bc-9008-64a56d999b3d)

- Put the spring clips into the breadboard shell first and make sure they're all pushed all the way in. Then gently put that onto the main PCB, it may take some careful wiggling to coax all the tabs on the spring clips to go into their holes. If you can't get it seated, take it off and check that they're all straight and in a nice row. If some are giving you trouble, bend it back or swap it out (I have included extras.)

![20230730-110](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/6fb74862-6197-4040-9dd2-449b6a27bb44)

![20230730-114](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/ed032ba7-8415-492c-b36d-f397ea4ca65d)

![20230730-116](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/2c901060-2cea-4e55-b4fc-0484ad73a27d)

![20230731-12](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/12808c27-7cf5-4c61-a588-dd5c220a61aa)

![20230731-14](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/25cd7c54-636c-4709-9dec-e230f3b26fef)


- Stick pin headers into every row of the breadboard and leave them in for soldering. Put them in one hole from the center hole. This makes sure that all the clips are perfectly aligned.

![20230731-17](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/eb0b1548-91ca-4f38-ae0d-8619c2000889)


- Solder all the clips, it works best to use a large soldering iron tip and start from the middle on each row. The breadboard will expand slightly as it gets warm so I try to account for that by making sure it expands evenly from the center.

![20230731-25](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/e6b208d8-4afb-45bb-934b-e7a2c87787f6)

- Place the LT1054 into the footprint between the headers, on the front side of the board. Pin 1 is the bottom left (the square pad) so the notch on the chip should be towards the big yellow capacitors.

- Stick an Arduino Nano in the top headers while you solder them so they stay straight, before doing anything with the wishbone LED board. And wiggle it around to find the nicest looking position (the holes are slightly bigger than the pins so you can wiggle it ~0.5mm) if you're a total perfectionist like me. Then solder it in.

![20230731-29](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/e442a6c0-0eaf-4a98-8b1c-7b0b9171b6ad)

![20230731-30](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/f1fe95bb-5fdb-4bec-af80-1025f10ace9c)


- You'll want to clean off the flux before you put on the wishbone board so there isn't any flux between the LEDs and the PCB. I like to shatter it all by dragging pokey tweezers over it, brush it off, and then use alcohol swabs or MG Chemicals Super Wash spray (this stuff is an absolute game changer, I recommend getting a can)

![20230731-34](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/90454c9f-80be-4056-acc6-a7d0bf772964)

![20230731-36](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/5151081a-4f6e-42d1-92f9-95d3a840dcee)

![20230731-38](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/17b08387-9302-48e2-bb07-be64c7cb89e8)


- For the wishbone board, solder the 2 inner ground pins directly to the backside of the female header pins that are sticking through. Make sure the LEDs lay flat to the main board.

![20230731-43](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/94c8e532-dbc3-40c1-a575-e7234eab0b86)

![20230731-44](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/958f2db1-2822-4871-ae98-aa99a7b5bbe4)



- Stick the 3 other header pins through the wishbone board just far enough that they're flush on the top of the main board. If you stick the short side of the header pin up to the plastic, it turns out that is the perfect length. Tape the headers down so they don't fall out when you flip the board. Solder from the front of the main board first so it's just a flat spot of solder. Now cut the header pins flush before soldering. None of this is strictly necessary, but it leaves the board looking really nice instead of having soldered-then-cut poky headers.

![20230731-47](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/42717f04-87d3-4339-be71-fe93ac5e6b48)

![20230731-49](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/367b45e0-cb5b-4385-a687-0ef4e8dbe1d3)

![20230731-51](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/3bd9fef5-ec29-45fe-a7c4-edd4bb20a4bd)

![20230731-55](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/3b8531a4-f07b-4704-a50c-0142b82f8a2c)

![20230731-60](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/4f702b3e-f0b7-4270-b94b-6926259c03be)


- Clean off all the flux like you did above.

![20230731-41](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/0d9e54d4-38cf-4344-8b75-439951113a24)


- Peel and stick the rubber feet, 2 on the bottom, 1 by the GPIO headers, and 1 just under the logo on the back. If it's a bit wobbly, a small amount of heat will soften the one giving you issues, then press it onto a flat surface until the wobbliness is gone.

![20230802-117](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/81a201bc-360e-43fa-80aa-eadc16112fb0)

![20230802-121](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/05b56278-547c-41e5-8db6-e2362148da29)

- You can load the firmware by plugging it in with the USB Boot button held and dragging the .uf2 file from here into the drive that pops up as RPI-RP2
  
<img width="1181" alt="Screenshot 2023-08-02 at 3 26 36 PM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/a9955178-9c56-457b-9372-f503c9b3211f">

<img width="1138" alt="Screenshot 2023-08-02 at 3 26 11 PM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/234362ab-5560-4711-8b7d-1c17d74adeb1">

<img width="1089" alt="Screenshot 2023-08-02 at 3 26 52 PM" src="https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/a07ebcec-ea63-401c-a600-3eedc2a61228">


And your Jumperless should be good to go!

![Jumperlesscrop](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/90ca8114-9a23-4255-b3a7-bd1df0fb15a8)
