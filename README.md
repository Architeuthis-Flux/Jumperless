# Jumperless

https://user-images.githubusercontent.com/20519442/227739104-85ad8e68-dfe9-4510-ad2b-4cc7110a7baf.mov

This is a new revision of [breadWare](https://github.com/Architeuthis-Flux/breadWare) with a new name. 

That repo has a lot more info that generally applies to this one too, so go read that if you'd like to know more about this thing. 

I'd like to basically rewrite that whole codebase from scratch because it's kind of a mess. So the new stuff will go here. 

## Here's the general vague roadmap (for anyone who wants to contribute):

Matrix interface code (Dumb Mode) - done


Storage of the current state - started but needs a lot more work


Pathfinding - started minutes ago
 
   --Hardware safety checks (throw an error if you try to connect power directly to ground)- not started
 
   --Path Redundancy/priority - not started
  
  
Coming up with a format to send commands - mused upon but not decided
 
   --Command input parsing (UART) - not started
  
   --Command input parsing (I2C) - not started


Power Supply control (needs HW revision or just remove the DAC/Opamp entirely)- not started



   *Everything above will be running on the AVR32DD32 controller chip, below is external interface stuff*



Making an Arduino library for the Master - not started


(optional) Writing a python library to form and send UART commands


(optional) GUI for making connections

(optional) Component placement sensing or just a simple way to define part locations


![IMG_2958](https://user-images.githubusercontent.com/20519442/227739152-74e49d6c-2520-48f8-9951-990b9a40b898.jpeg)
![IMG_2964 2](https://user-images.githubusercontent.com/20519442/227739160-9e5ababd-3c25-4dfd-a0da-be7b85f3c44e.jpeg)
![IMG_2961 2](https://user-images.githubusercontent.com/20519442/227739166-e92fc534-9e3a-48f5-8ef9-c4df4e2a7a27.jpeg)
