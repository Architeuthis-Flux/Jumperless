# Jumperless - Wokwi Bridge


This is a MacOS app that continuously pulls the diagram.json from the specified Wokwi project link and auto updates the connections on your Jumperless when you save the project.

![JumperlessWokwiBridgeIcon](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/afbe928c-4dce-448f-88d7-f77aaf4f56e4)

Running this on MacOS should be as simple as clicking the app (it should be in you Applications folder, but it might not matter.) It should open a Terminal and ask you to select the port and paste a link to the Wokwi project. (I need to figure out a way to package a terminal with the app so I and have the app icon I made show up, if you have any hot tips on doing this, let me know)

If you'd like to use my 555 circuit that I've been using for testing, here's the link:

[https://wokwi.com/projects/367384677537829889](https://wokwi.com/projects/367384677537829889)

It also does the translation from the diagram.json format to the super-simplistic nodeFile format in the script to make it a little bit snappier on the updates. Doing all the parsing on the chip (mainly from LittleFS operations) was taking like a quarter of a second and it was bugging me.

If someone would like to build this for Windows with PyInstaller (you can only package apps for the platform it's running on,) I've included a text file with the settings I used. (share it with me and I'll add it here)

It's really just a Python script so if you'd rather run it in Thonny or something, I've added the .py file and that should work too.


This is literally the first thing I've ever written in Python (or packaged as an app with PyInstaller) so if you have any improvements for this, I'd be happy to hear them.
