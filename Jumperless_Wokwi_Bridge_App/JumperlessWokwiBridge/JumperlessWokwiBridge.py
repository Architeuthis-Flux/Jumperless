# SPDX-License-Identifier: MIT

from bs4 import BeautifulSoup

import pathlib

import requests
import json
import serial
import time

import sys
import codecs
import os
import pyduinocli

import psutil
import shutil
from urllib.request import urlretrieve

import ssl
ssl._create_default_https_context = ssl._create_unverified_context

os.system("")
#import platform

#from watchedserial import WatchedReaderThread

import serial.tools.list_ports


debug = False



justreconnected = 0

global serialconnected
serialconnected = 0

portSelected = 0

stringified = ' '
lastDiagram = [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']
diagram = [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']


menuEntered = 0

portName = ' '



arduinoPort = 0

noWokwiStuff = False

disableArduinoFlashing = 1

def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    base_path = getattr(sys, '_MEIPASS', os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base_path, relative_path)
#Path.mkdir(Path.cwd() / "JumperlessFiles/", exist_ok=True)


slotAssignmentsFile = ("JumperlessFiles/slotAssignments.txt")  
savedProjectsFile = ("JumperlessFiles/savedProjects.txt")

pathlib.Path(slotAssignmentsFile).parent.mkdir(parents=True, exist_ok=True)
pathlib.Path(savedProjectsFile).parent.mkdir(parents=True, exist_ok=True)
# Path.home() / "JumperlessFiles" / "savedProjects.txt"

#arduino = pyduinocli.Arduino("arduino-cli")

#### If you're running this in thonny, make sure you download arduino-cli and put it in the same folder as this script
#### then uncomment this below and comment the one above
# arduino = pyduinocli.Arduino("./arduino-cli")
noArduinocli = True
# try:
#     arduino = pyduinocli.Arduino(resource_path("arduino-cli"))
# except:
#     try:
#         arduino = pyduinocli.Arduino("arduino-cli")
#     except:
#         try:
#             arduino = pyduinocli.Arduino("./arduino-cli")
#         except:
#             print ("Couldn't find arduino-cli")
#             noArduinocli = True
#             pass
    

# print ('\n\n\n\rarduino')    
# print(arduino)

serialconnected = 0


if (noArduinocli == True):
    
    disableArduinoFlashing = 1


def openSerial():
    global portName
    global ser
    global serTickle
    global arduinoPort
    global disableArduinoFlashing
    global serialconnected

    portSelected = 0
    foundports = []
    
    print("\n")

    while portSelected == False:
        autodetected = -1
        ports = serial.tools.list_ports.comports()
        
        i = 0
        for port, desc, hwid in ports:
            i = i + 1
            
            hwidString = hwid
            splitAt = "VID:PID="
            splitInd = hwidString.find(splitAt)
            
            hwidString = hwidString[splitInd+8:splitInd+17]
            #print (hwidString)
            
            
            vid = hwidString[0:4]
            pid = hwidString[5:9]
            #print ("vid = " + vid)
            #print ("pid = " + pid)
            print("{}: {} [{}]".format(i, port, desc))
            if desc == "Jumperless" or pid == "ACAB" or pid == "1312":
                autodetected = i
                foundports.append(ports[autodetected-1][0])
                
        selection = -1
        sortedports = sorted(foundports,key = lambda x:x[-1])
        #print (foundports)
        #print(sortedports)
        print ("\n")
        
        jumperlessIndex = chooseJumperlessPort(sortedports)
        arduinoIndex = (jumperlessIndex + 1) % 2
        
        #print (jumperlessIndex)
        #print (arduinoIndex)
        
        if autodetected != -1:
        #if False:    
            try:
                selection = autodetected
                
                #portName = ports[int(selection) - 1].device
                
                portName = sortedports[jumperlessIndex]
                
                arduinoPort = sortedports[arduinoIndex]
                
                portSelected = True
                serialconnected = 1
                
                print("\nAutodetected Jumperless at", end=" ")
                print(portName)
                
                print ("Autodetected USB-Serial at ", end="")
                print (arduinoPort)
            except:
                pass

            

        else:
            selection = input(
                "\n\nSelect the port connected to your Jumperless   ('r' to rescan)\n\n(Choose the lower numbered port, the other is routable USB-Serial)\n\n")
            
            
            if selection.isdigit() == True and int(selection) <= i:
                portName = ports[int(selection) - 1].device
                print("\n\n")
                i = 0
                
                for port, desc, hwid in ports:
                    i = i + 1
                    print("{}: {} [{}]".format(i, port, desc))

                        
                ArduinoSelection = -1
                sortedports = sorted(foundports,key = lambda x:x[-1])
                #print (foundports)
                #print(sortedports)
                print ("\n\n")
                ArduinoSelection = input(
                        "\n\nChoose the Arduino port   ('x' to skip)\n\n(Choose the higher numbered port)\n\n")
                
                if (ArduinoSelection == 'x' or ArduinoSelection == 'X'):
                    disableArduinoFlashing = 1
                    
                if ArduinoSelection.isdigit() == True and int(ArduinoSelection) <= i:
                    
                    arduinoPort = ports[int(ArduinoSelection) - 1].device
                    aPortSelected = True
                    print(ports[int(ArduinoSelection) - 1].device)
                
                
                portSelected = True
                print(ports[int(selection) - 1].device)
                
                
                
                serialconnected = 1
                
                

        


#portName =  '/dev/cu.usbmodem11301'

    ser = serial.Serial(portName, 115200, timeout=None)
    #ser.open()
    

jumperlessFirmwareNumber = [0,0,0,0,0,0]

def chooseJumperlessPort(sortedports):
    global jumperlessFirmwareString
    global jumperlessFirmwareNumber
    
    jumperlessFirmwareString = ' '
    tryPort = 0
    
    while (tryPort < len(sortedports)):
    
        tempSer1 = serial.Serial(sortedports[tryPort], 115200, timeout=None)
        #print (tryPort)
        tempSer1.write(b'?')
        
        time.sleep(0.1)
        inputBuffer2 = b' '
        
        if (tempSer1.in_waiting > 0):
                        #justChecked = 0
                        #reading = 1
                        inputBuffer2 = b' '

                        waiting = tempSer1.in_waiting

                        while (serialconnected >= 0):
                            inByte = tempSer1.read()

                            inputBuffer2 += inByte

                            if (tempSer1.in_waiting == 0):
                                time.sleep(0.05)

                                if (tempSer1.in_waiting == 0):
                                    break
                                else:
                                    continue
                        tempSer1.close()
                        
                        inputBuffer2 = str(inputBuffer2)
                        inputBuffer2 = inputBuffer2.strip('b\'\\n \\r ')
                        
                        jumperlessFirmwareString = inputBuffer2.split('\\r\\n')[0]
                        
                        #print (inputBuffer2)
                        #print (jumperlessFirmwareString)
                        
                        if (jumperlessFirmwareString.startswith("Jumperless firmware version:") == True):
                            
                            #print(jumperlessFirmwareString[29:39])
                            
                            
                            jumperlessFirmwareNumber = jumperlessFirmwareString[29:39].split('.')
                            
                            #print (jumperlessFirmwareNumber)
                            
                            #print ("found a match!")
                            #
                            return tryPort
                            
                        else:
                            
                            tryPort = tryPort+1
        else:
           tryPort = tryPort+1
    #print ("fuck")
    return 0
            
        
    
        


justChecked = 0
reading = 0


latestFirmwareAddress = "https://github.com/Architeuthis-Flux/Jumperless/releases/latest/download/firmware.uf2"

url_link = 0

currentString = 'fuck'

def checkIfFWisOld ():
    global currentString
    global jumperlessFirmwareString
    
    response = requests.get("https://github.com/Architeuthis-Flux/Jumperless/releases/latest")
    version = response.url.split("/").pop()
    #print(version)
    
    latestVersion = version.split('.')
    latestString = latestVersion[0] + '.' + latestVersion[1] + '.' + latestVersion[2]
    
    splitIndex = jumperlessFirmwareString.rfind(':')

    #print(jumperlessFirmwareString)
    currentString = jumperlessFirmwareString[splitIndex+2:]
    


    
    
    latestList = latestString.split('.')
    currentList = currentString.split('.')
    # print(currentList)
    # print(latestList)


    #latestInt = (int(latestList[0])* 100) + (int(latestList[2])* 10) + (int(latestList[2]))
    #currentInt = (int(currentList[0])* 100) + (int(currentList[2])* 10) + (int(currentList[2]))
    # try:
    latestInt = int("".join(latestList))
    try:
        currentInt = int("".join(currentList))
    except:
        currentInt = 0
        #return True
    # print (latestInt)
    # print (currentInt)
    
    
    if (latestInt > currentInt):
        
    
        print("\n\n\rThe latest firmware is: " + latestString)
        print(      "You're running version: " + currentString)
        return True
    else:
        return False

    

def updateJumperlessFirmware(force):
    global ser
    global menuEntered
    global currentString
    global serialconnected
    
    #newFirmware = r
    
    if (force == False):
        if (checkIfFWisOld() == False):
            #print ("\n\n\r'update' to force firmware update - yours is up to date (" + currentString + ")")
            return
    
    print("\n\rWould you like to update your Jumperless with the latest firmware? Y/n\n\r")
    if (force == True or input ("\n\r").lower() == "y"):
        
        print ("\n\rDownloading latest firmware...")
        
        serialconnected = 0
        menuEntered = 1
        
        urlretrieve(latestFirmwareAddress, "firmware.uf2")
        
        ser.close()
        time.sleep(0.50)
        
        print("Putting Jumperless in BOOTSEL...")
        
        serTickle = serial.Serial(portName, 1200, timeout=None)
        
        
        serTickle.close()
        time.sleep(0.55)
        
#         serTickle.open()
#         time.sleep(0.95)
#         serTickle.close()
              
        print ("Waiting for mounted drive...")
        
        foundVolume = "none"
        
        while (foundVolume == "none"):
            time.sleep(0.5)
            partitions = psutil.disk_partitions()
            
            for p in partitions:
                #print(p.mountpoint)
                if (p.mountpoint.endswith("RPI-RP2") == True):
                    foundVolume = p.mountpoint
                    print("Found Jumperless at " + foundVolume + "...")
                    break
                
                
                #if (win32api.GetVolumeInformation(p.mountpoint)[0] == "RPI-RP2"):
                 #   foundVolume = p.mountpoint
                  #  print("Found Jumperless at " + foundVolume + "...")
                   # break
            
        fullPathRP = os.path.join(foundVolume, "firmware.uf2")
        #print(fullPathRP)
        time.sleep(0.2)
        print ("Copying firmware.uf2 to Jumperless...\n\r")
        try:
            shutil.copy("firmware.uf2", fullPathRP)
            
            
        except:
            pass
        
        time.sleep(0.75) 
        print("Jumperless updated to latest firmware!")
        
        
        #ser.open()
        time.sleep(0.75)
        
        #openSerial()
        ser = serial.Serial(portName, 115200, timeout=None)
        ser.flush()
        menuEntered = 0
        serialConnected = 1



defaultWokwiSketchText = 'void setup() {'


# 555 project

# https://wokwi.com/projects/369024970682423297


# the website URL
#url_link = "https://wokwi.com/projects/369024970682423297"
menuEntered = 0


def bridgeMenu():
    global menuEntered
    global ser
    global disableArduinoFlashing
    global noWokwiStuff
    global url_link
    global currentString
    global numAssignedSlots
    #global serTickle


    while(menuEntered == 1):

        print("\t\t\tBridge App Menu\n\n")

        print("\t\ta = Assign Wokwi Links to Slots", end='')
        if (numAssignedSlots > 0):
            print(" - " + str(numAssignedSlots) + " assigned")
        else:
            print()
        print("\t\td = Delete Saved Projects")
        print("\t\tr = Restart Bridge App")
        print("\t\ts = Restart Serial")
        #print("\t\tl = Load Project")
        # print("\t\tf = Disable Auto-flashing Arduino - ", end='')
        # if (disableArduinoFlashing == 0):
        #     print("Enabled")
        # else:
        #     print("Disabled")
        
        print("\t\tu = Update Jumperless Firmware - " + currentString)

        print("\t\tj = Go Back To Jumperless\n")

        menuSelection = input("\n\n")
        
        
        
        # if(menuSelection == 'f'):
        #     if (disableArduinoFlashing == 0):
        #         disableArduinoFlashing = 1
        #     else:
        #         disableArduinoFlashing = 0

        #     #disableArduinoFlashing = 1
        #     break

        if (menuSelection == 's'):
            ser.close()
            
                       
            openSerial()
            #time.sleep(1)
            menuEntered = 0
            time.sleep(.25)
            ser.write(b'm')
            break
        
        if (menuSelection == 'l'):
            openProject()
            #time.sleep(1)
            menuEntered = 0
            time.sleep(.25)
            ser.write(b'm')
            
            break
        
        if (menuSelection == 'r'):
            ser.close()
            openSerial()
            #openProject()
            assignWokwiSlots()
            #time.sleep(1)
            menuEntered = 0
            time.sleep(.5)
            ser.write(b'm')
            
            break
            
        if(menuSelection == 'j'):
            menuEntered = 0
            time.sleep(.25)
            ser.write(b'm')
            break

        if(menuSelection == 'u'):
            updateJumperlessFirmware(True)
            menuEntered = 0
            time.sleep(.25)
            ser.write(b'm')
            break

        if(menuSelection == 'a'):
            assignWokwiSlots()
            menuEntered = 0
            time.sleep(.25)
            ser.write(b'm')
            break
        
        
        while (menuSelection == 'd'):
            
            print('\n\nEnter the index of the project you\'d like to delete:\n\nr = Return To Menu\ta = Delete All\n\n')

            try:
                f = open(savedProjectsFile, "r")
            except:
                f = open(savedProjectsFile, "x")
                f = open(savedProjectsFile, "r")

            index = 0

            lines = f.readlines()

            for line in lines:
                if (line != '\n'):
                    index += 1
                    print(index, end="\t")

                    print(line)

            linkInput = input('\n\n')
            
            if (linkInput == 'a'):
                f.close()
                f = open(savedProjectsFile, "w")
                f.close()
            
            
            
            if (linkInput.isdigit() == True) and (int(linkInput) <= index):
                otherIndex = 0
                realIndex = 0
                for idx in lines:
                    
                    if (idx != '\n'):
                        
                        otherIndex += 1
                        
                        if (otherIndex == int(linkInput)):
                            print(idx)
                            del lines[realIndex]
                            #del lines[idx+1]
                            idx = idx.rsplit('\t\t')
                            idxLink = idx[1].rstrip('\n')
                            print("\n\nDeleting project ", end='')
                            print(idx[0])
                            break
                        
                    realIndex += 1
                        
                f.close()
                f = open(savedProjectsFile, "w")
                
                for line in lines:
                
                    f.write(line)
                f.close()
                f = open(savedProjectsFile, "r")
                print (f.read())
                f.close()
                #menuEntered = 0
            else:
                break


slotLines = [ "slot 0\n", "slot 1\n", "slot 2\n", "slot 3\n", "slot 4\n", "slot 5\n", "slot 6\n", "slot 7\n"]
defaultSlotLines = [ "slot 0\n", "slot 1\n", "slot 2\n", "slot 3\n", "slot 4\n", "slot 5\n", "slot 6\n", "slot 7\n"]
slotURLs = [ '!', '!', '!', '!', '!', '!', '!', '!', '!']

numAssignedSlots = 0

def countAssignedSlots():
    global slotLines
    global slotURLs
    global numAssignedSlots
    numAssignedSlots = 0
    try:
        slot = open(slotAssignmentsFile, "r")
    except:
        return 
    slot.seek(0)
    slotLines = slot.readlines()
    splitLine = ' '

    idx = 0
    for line in slotLines:
        if (line != '\n'):
            splitLine = line.split('\t')
            if (len(splitLine) > 1):
                slotURLs[idx] = splitLine[1]
                numAssignedSlots += 1
            idx += 1

    #print(slotURLs)
    slot.close()
    return numAssignedSlots

def printSavedProjects():
    try:
        f = open(savedProjectsFile, "r")
    except:
        f = open(savedProjectsFile, "x")
        f = open(savedProjectsFile, "r")

    index = 0

    lines = f.readlines()

    for line in lines:
        if (line != '\n'):
            index += 1
            print(index, end="\t")

            print(line, end='')

    f.close()


def printOpenProjectOptions():

    print("\n\n")

    print(" 'menu'   to open the Bridge App menu")  
    print(" 'skip'   to disable Wokwi updates and just use as a terminal")
    print(" 'update' to force firmware update - yours is up to date (" + currentString + ")")
    print(" 'slots'  to assign Wokwi projects to slots (this menu) ", end='')
    
    countAssignedSlots()  
    if (numAssignedSlots > 0):
        print("- " + str(numAssignedSlots) + " assigned")
    # else:
    #     print("\n")

    print("\n    ^--   (you can enter these commands at any time)\n")

    print("  ENTER   to accept and go to Jumperless\n")

def printSlotOptions():
    
    print( "      'x' to clear all slots")
    print( "      'c' to clear a single slot")
    print( "\nEnter the slot number you'd like to assign a project to:\n")
savedProjectIndex = 0

somethingChoosen = False
    

def assignWokwiSlots():
    global url_link
    global slotLines
    global slotURLs
    global numAssignedSlots
    printOpenProjectOptions()
    printSlotOptions()
    #countAssignedSlots()

    try:
        slot = open(slotAssignmentsFile, "r")

    except:
        
        slot = open(slotAssignmentsFile, "x")
        slot.close()
        slot = open(slotAssignmentsFile, "w")
      
        
        slot.writelines(defaultSlotLines)
            
            #print(slot.read())
        slot.close()
        slot = open(slotAssignmentsFile, "r")

    slot.seek(0)
    firstLine = slot.readline()
    
    firstLine = firstLine.strip()
    firstLineIndex = 0

    while ((firstLine.startswith('slot 0') == False or firstLine.startswith('#') == True or firstLine.startswith('//') == True) ):

        # print("firstLine = ", end='')
        # print(firstLine)
        firstLine = slot.readline()
        firstLine = firstLine.strip()
        firstLineIndex = firstLineIndex + 1

        if (firstLineIndex > 20):
            slot = open(slotAssignmentsFile, "w")

            
            slot.writelines(defaultSlotLines)
            firstLineIndex = 0
            #slot.close()
            slot = open(slotAssignmentsFile, "r")
            break

    index = 0
    # print("\n\n")
    # print(firstLineIndex)
    # print("\n\n")
    # slot.seek(firstLineIndex)
    
    # slot = open(slotAssignmentsFile, "r")
    slot.seek(0)
    # while (index < firstLineIndex):
    #     slot.readline()
    #     index += 1

    lines = slot.readlines()
    #0print("\n\n")
    # print(firstLineIndex)
    # print(lines[0])
    index = 0

    numAssignedSlots = countAssignedSlots()
    #print(numAssignedSlots)

    for line in lines:
        
        if (line != '\n'):

            index += 1
            print(line, end='')
    slot.close()

    slotInput = ''
    linkInput = ''
    slotInput = input('\n\n')
    #print(slotInput)


    
    # if (slotInput == 'r'):
    #     openProject()


    if (slotInput == 'x'):
        print("\n\nClearing all slots\n\n")
        slot = open(slotAssignmentsFile, "w")
        index = 0
        slot.writelines(defaultSlotLines)
        slot = open(slotAssignmentsFile, "r")
        #print(slot.readlines())
        numAssignedSlots = 0
        slot.close()
        somethingChoosen = False
        assignWokwiSlots()

    elif (slotInput.startswith('c')):
        #slot = open(slotAssignmentsFile, "w")
        #slotInput = slotInput.lstrip('c')
        slotInput = input("\n\nEnter the slot number you'd like to clear:\n")
        
        if (slotInput.isdigit() == True ):
            if (int(slotInput) < len(lines)):
                slotInputInt = int(slotInput)
                if (slotInputInt < len(lines)):
                    slot = open(slotAssignmentsFile, "w")
                    lines[slotInputInt] = "slot " + str(slotInputInt) + "\n"

                    slot.writelines(lines)
                    numAssignedSlots -= 1
                    slot.close()
                    assignWokwiSlots()
                else:
                    print("\nInvalid slot")
                    
                    assignWokwiSlots()
            else:
                print("\nInvalid slot")
                    
                assignWokwiSlots()
        else:

            print("\nInvalid slot")
            
            assignWokwiSlots()


    elif (slotInput == ''):
        #print("skipping")
        #print(slotInput)
        countAssignedSlots()

        if (numAssignedSlots > 0):
            return
        else:
            #assignWokwiSlots()
            slotInput = '0'
    elif (slotInput.isdigit() == False ):
        slotInput = ''
        print("\nInvalid slot")
        assignWokwiSlots()

    elif (int(slotInput) >= len(lines)):
        slotInput = ''
        print("\nInvalid slot")
        assignWokwiSlots()
    else:
        print("\nChoose from saved or paste a link to a Wokwi project for Slot ", end='')
        print( slotInput )
        print("\n\n")
        printSavedProjects()

        if (slotInput.isdigit() == True):
            if (int(slotInput) <= len(lines)):
                linkInput = searchSavedProjects(input())
                checkurl = ' '
                url_link = linkInput
                try:
                    checkurl = requests.get(url_link)
                    if (checkurl.status_code == requests.codes.ok):

                        
                        notes = searchSavedProjects(linkInput, True)
                    
                        print("\n" + notes + " selected for slot " + slotInput)

                        slot = open(slotAssignmentsFile, "w")
                        lines[int(slotInput)] = "slot " + slotInput + "\t" + url_link + "\t " + notes + "\n"
                        #for line in lines:
                        slot.writelines(lines)
                            #print(line)
                        slot.close()
                        assignWokwiSlots()
                    else:
                        print("\n\nBad Link")
                except Exception as e:
                    print(e)
                    print("\n\nBad Link!")
                    assignWokwiSlots()
                    

    countAssignedSlots()



def searchSavedProjects(inputToSearchFor, returnName = False):
    try:
        f = open(savedProjectsFile, "r")
    except:
        f = open(savedProjectsFile, "x")
        f = open(savedProjectsFile, "r")

    index = 0
    matchFound = 0
    returnLink = ' '

    lines = f.readlines()

    linkInput = inputToSearchFor

    if (linkInput.startswith("http") == True): #pasted a link
        noWokwiStuff = False
        entryType = "link"
        if (returnName == False):
            return linkInput
        #return linkInput

    elif (linkInput.isdigit() == True ): #entered a number
        #print(linkInput)
       # print(int(linkInput))
        linkInt = int(linkInput)
        otherIndex = 0
        entryType = "index"

        for idx in lines:
            if (idx != '\n'):
                otherIndex += 1
                if (otherIndex == linkInt):
                    idx = idx.rsplit('\t\t')
                    idxLink = idx[1].rstrip('\n')
                    linkInput = idxLink.rstrip('\n')
                    if (returnName == False):
                        return idxLink.rstrip('\n')
                    break
    else:   #entered a name
        #print("entered a name")
        for name in lines:
            if name != '\n':
                name = name.rsplit('\t\t')
                nameText = name[0]
                #print (nameText)
                if (nameText == linkInput):
                    entryType = "name"
                    linkInput = name[1].rstrip('\n')
                    if (returnName == False):

                        return linkInput
                    break
                index += 1
    matchFound = 0
    line = 0
    index = 0

    for line in lines:
        if (line != '\n'):
            line = line.rsplit('\t\t')
            name = line[0]
            line = line[1]
            
            line = line.rstrip('\n')
            index += 1

            if line == linkInput:
                #print ( "Match Found at index ", end = '')
                matchFound = index
                if (returnName == False):
                    return line
                else:
                    return name
                #return line
                # break
                # url_selected = 1
                


    if matchFound == 0:
        try:
            checkurl = requests.get(linkInput)
            #print(checkurl.status_code)
            if (checkurl.status_code == requests.codes.ok):
                url_selected = 1
                noWokwiStuff = False

                name = input("\n\nEnter a name for this new project\n\n")
                f.close()
                f = open(savedProjectsFile, "a")
                f.write(name)
                f.write('\t\t')
                f.write(linkInput)
                f.write("\n")
                f.close()
                if (returnName == False):
                    return linkInput
                else:
                    return name
                
            else:
                assignWokwiSlots()
        except:
            assignWokwiSlots()







def openProject():
    global url_link
    global disableArduinoFlashing
    global noWokwiStuff
    global menuEntered
    global currentString
    global numAssignedSlots
    url_entered = 0
    url_selected = 0
    entryType = -1  # 0 for index, 1 for name, 2 for link
    try:
        numAssignedSlots = countAssignedSlots()
    except:
        numAssignedSlots = 0

    disableArduinoFlashing = 1




    linkInput = input('\n\n')

    if (linkInput == 'slots' or linkInput == 'slot' or linkInput == 's'):
        assignWokwiSlots()
        openProject()
    elif (linkInput == 'force' or linkInput == 'update' or linkInput == 'force update'):
        jumperlessFirmwareString = ' '
        updateJumperlessFirmware(True)

    elif(linkInput == 'skip' ):
        noWokwiStuff = True
        url_link = ' '
        print("\nWokwi updates disabled\n\n")
        ser.write('m'.encode())
        url_selected = 1
        #break
        return


    elif(linkInput == 'menu' or linkInput == 'm'):
        menuEntered = 1
        
        #while(menuEntered == 1):
        bridgeMenu()
        
        openProject()

    else:
        linkInput = searchSavedProjects(linkInput)


    checkurl = ' '
    url_link = linkInput

#         print("\n\n linkInput = ", end='')
#         print(linkInput)
#         print("\n\n url_link = ", end='')
#         print(url_link)

    #checkurl = requests.get(url_link)
    #print(checkurl.status_code)
    if (noWokwiStuff == False):
        try:
            checkurl = requests.get(url_link)
            #print(checkurl.status_code)
            if (checkurl.status_code == requests.codes.ok):
                url_selected = 1
                noWokwiStuff = False
                # break
            else:
                print("\n\nBad Link - Status Code: ", end='')  
                print(checkurl.status_code)
                url_link = 0
                linkInput = 0
                openProject()

        except:
            print("\n\nBad Link!!!")
            url_link = 0
            openProject()

    matchFound = 0
    line = 0
    index = 0

    
    #saveConfig()   
        

    
    
    
    
    
    
openSerial()    
updateJumperlessFirmware(False)
assignWokwiSlots()
#openProject()

if (noWokwiStuff == False):
    print("\n\nSave your Wokwi project to update the Jumperless\n\n")
    





                            
portNotFound = 1
                            
def check_presence(correct_port, interval=.15):
    global ser
    global justreconnected
    global serialconnected
    global justChecked
    global reading

    portFound = 0
    while True:
        
       
        if (reading == 0):
            
            portFound = 0

            for port in serial.tools.list_ports.comports():

                if portName in port.device:
                    
                    portFound = 1

            #print (portFound)

            if portFound == 1:
                try:
                    #print (portName)
                    #ser = serial.Serial(portName, 115200)
                    #print (portName)
                    #ser.open(portName)
                    justChecked = 1
                    serialconnected = 1
                    time.sleep(0.1)
                    justChecked = 0
                    
                    
                except:
                    
                    continue

            else:
                justreconnected = 1
                justChecked = 0
                serialconnected = 0

                ser.close()

            time.sleep(interval)


import threading
port_controller = threading.Thread(
    target=check_presence, args=(portName, .15,), daemon=True)
# port_controller.daemon(True)
port_controller.start()

#ser.in_waiting            



def serialTermIn():
    global serialconnected
    global ser
    global justChecked
    global reading
    global menuEntered
    global portNotFound
    
    while True:
        readLength = 0
        
        
        while menuEntered == 0:
            try:
                if (ser.in_waiting > 0):
                    #justChecked = 0
                    #reading = 1
                    inputBuffer = b' '

                    waiting = ser.in_waiting

                    while (serialconnected >= 0):
                        inByte = ser.read()

                        inputBuffer += inByte

                        if (ser.in_waiting == 0):
                            time.sleep(0.05)

                            if (ser.in_waiting == 0):
                                break
                            else:
                                continue

                    inputBuffer = str(inputBuffer)

                    inputBuffer.encode()
                    decoded_string = codecs.escape_decode(
                        bytes(inputBuffer, "utf-8"))[0].decode("utf-8")

                    decoded_string = decoded_string.lstrip("b' ")
                    decoded_string = decoded_string.rstrip("'")
                    decoded_string = decoded_string.rstrip("\"")

                    print(decoded_string, end='')
                    #print ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
                    readlength = 0
                    #justChecked = 0
                    reading = 0
                    portNotFound = 0

            except:
                try:
                    ser.close()
                except:
                    pass
                print("Disconnected")
                portNotFound = 1
                while (portNotFound == 1):
                    portFound = 0
                   
                    time.sleep(0.2)
                    for port in serial.tools.list_ports.comports():

                        if portName in port.device:

                            portFound = 1
                            portNotFound = 0
                            #print ("found ")
                            #print (port.device)

                    if portFound == 1:
                        try:
                            ser = serial.Serial(portName, 115200, timeout=None)
                            justChecked = 1
                            serialconnected = 1
                            time.sleep(0.2)
                            justChecked = 0
                            portNotFound = 0
                            #justreconnected = 1
                        except:
                            portFound = 0
                            portNotFound = 1
                            time.sleep(0.25)

                    else:
                        #justreconnected = 1
                        justChecked = 0
                        serialconnected = 0

                        #ser.close()
                        portNotFound = 1
                        time.sleep(.1)


port_controller = threading.Thread(target=serialTermIn, daemon=True)
# port_controller.daemon(True)
port_controller.start()

forceWokwiUpdate = 0

def serialTermOut():
    global serialconnected
    global ser
    global justChecked
    global justreconnected
    global menuEntered
    global forceWokwiUpdate
    global noWokwiStuff
    
    

    while True:
        justreconnected = 0
        resetEntered = 0

        while (menuEntered == 0):

            outputBuffer = input()

            if (outputBuffer == 'menu') or (outputBuffer == 'Menu'):
                print("Menu Entered")
                menuEntered = 1
                continue
            if (outputBuffer == 'slots') or (outputBuffer == 'Slots'):
                assignWokwiSlots()
                #outputBuffer = ' '
                continue
            if (outputBuffer == 'wokwi'):
                forceWokwiUpdate = 1
                print("Wokwi update forced")
                continue

            if(outputBuffer == 'skip'):
                if (noWokwiStuff == False):
                    noWokwiStuff = True
                    print("\nWokwi updates disabled\n\n")
                else:
                    print("\nWokwi updates enabled\n\n")
                    noWokwiStuff = True
                outputBuffer = ' '
                continue
    

            #if outputBuffer == b'r':
                #resetEntered = 1

            if (serialconnected == 1):
                #justChecked = 0
                while (justChecked == 0):
                    time.sleep(0.01)
                else:

                    #print (outputBuffer)
                    if (outputBuffer != ' '):
                        try:
                            #print (outputBuffer.encode('ascii'))
                            ser.write(outputBuffer.encode('ascii'))
                        except:
                            portNotFound = 1

                            while (portNotFound == 1):
                                portFound = 0

                                for port in serial.tools.list_ports.comports():

                                    if portName in port.device:

                                        portFound = 1
                                        print (port.device)

                                    if portFound >= 1:
                                        #
                                        justChecked = 1
                                        serialconnected = 1
                                        time.sleep(0.05)
                                        justChecked = 0
                                        portNotFound = 0

                                    else:
                                        justreconnected = 0
                                        justChecked = 0
                                        serialconnected = 0

                                        ser.close()
                                        portNotFound = 1
                                        time.sleep(.1)
                            try:
                                print(outputBuffer.encode('ascii'))
                                ser.write(outputBuffer.encode('ascii'))
                            except:
                                serialconnected=0

                        if (resetEntered == 1):
                            time.sleep(.5)
                            print("reset")
                            justreconnected = 1

    # time.sleep(.5)
    
def removeLibraryLines(line):
    
    if "#" in line:
        return False
    if (len(line) == 0):
        return False
    else:
        return True
        


def findsketchindex(decoded):
    
    doneSearching = 0
    index = 0
    
    while (doneSearching == 0):
        if (decoded['props']['pageProps']['p']['files'][index]['name'] == "sketch.ino"):
            doneSearching = 1
            #print ("sketch found at index " , end='')
            #print(index)
            return index
        else:
            if (index > 20):
                doneSearching = 1
                return 0
            
            else:
                index = index + 1
        
    
    
    
def finddiagramindex(decoded):
    
    doneSearching = 0
    index = 0
    
    while (doneSearching == 0):
        if (decoded['props']['pageProps']['p']['files'][index]['name'] == "diagram.json"):
            doneSearching = 1
            #print ("diagram found at index " , end='')
            #print(index)
            return index
        else:
            if (index > 20):
                doneSearching = 1
                return 2
            
            else:
                index = index + 1
                
                
                
def findlibrariesindex(decoded):
    doneSearching = 0
    index = 0
    
    while (doneSearching == 0):
        if (decoded['props']['pageProps']['p']['files'][index]['name'] == "libraries.txt"):
            doneSearching = 1
            #print ("libraries found at index " , end='')
            #print(index)
            return index
        else:
            if (index > 20):
                doneSearching = 1
                return 3
            
            else:
                index = index + 1
    #print (decoded)
    
    
    

port_controller = threading.Thread(target=serialTermOut, daemon=True)

port_controller.start()

time.sleep(.75)


sketch = [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']
lastsketch = ['  ', '  ', '  ', '  ', '  ', '  ', '  ', '  ']
lastlibraries = '  '
blankDiagrams = [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

#print (arduino.board.attach(arduinoPort,None,"WokwiSketch"))
#print (arduinoPort)

try:        
    ser.write('m'.encode())
except:
    pass

currentSlotUpdate = 1
cycled = 0
while True:
   
    if (menuEntered == 1):
        bridgeMenu()

    # while portIsUsable(portName) == True:
     #   print('fuck')
      #  ser.close()
       # time.sleep(.5)
        #ser = serial.Serial(portName, 460800, timeout=0.050)

    while (justreconnected == 1):

        #print("just reconnected")
        lastDiagram = blankDiagrams
        
        forceWokwiUpdate = 1
        time.sleep(.1)
        ser.close()
        time.sleep(.1)#######
        #if (portNotFound != 1):
            #ser = serial.Serial(portName, 115200, timeout=None)
        if (serialconnected == 1):
            print('Reconnected')
            portNotFound = 0
            portFound = 1
            justreconnected = 0
            forceWokwiUpdate = 1
            break
    else:
        justreconnected = 0
        
    #print("noWokwiStuff")
    #print(noWokwiStuff)

    if (serialconnected == 1 and noWokwiStuff == False):
        #print ("connected!!!")

        # if (forceWokwiUpdate == 1 and cycled == 0):
        #     #currentSlotUpdate = 0
        #     cycled = 0

        if (numAssignedSlots > 0):
            # print(currentSlotUpdate)
            # print(slotURLs)
            currentSlotUpdate += 1


            if (currentSlotUpdate > 7):

                currentSlotUpdate = 0
                #cycled += 1


            # if (forceWokwiUpdate == 1 and cycled == 2):
                
            #     forceWokwiUpdate = 0
            #     cycled = 0

            while(slotURLs[currentSlotUpdate] == '!'):
                
                #print(currentSlotUpdate)
                currentSlotUpdate += 1
                if (currentSlotUpdate > 7):
                    currentSlotUpdate = 0
                    forceWokwiUpdate = 0

            try:

                result = requests.get(slotURLs[currentSlotUpdate]).text
            except Exception as e:
                #print(e)
                continue


        else:
            currentSlotUpdate = 0
            assignWokwiSlots()
            #result = requests.get(url_link).text
            
        #print(currentSlotUpdate)


        
        doc = BeautifulSoup(result, "html.parser")
     

        s = doc.find('script', type='application/json').get_text()

        stringex = str(s)
        
        #print (stringex)

        d = json.loads(stringex)
        
        decoded = json.loads(stringex)
        
        #print (decoded['props']['pageProps']['p']['files'][0]['name'])


        librariesExist = 0
        
        

        # c = decoded['props']['pageProps']['p']['files'][findsketchindex(decoded)]['content']

        try:
            l = d['props']['pageProps']['p']['files'][findlibrariesindex(decoded)]['content']
            libraries = str(l)
            librariesExist = 1
        except:
            pass

        d = decoded['props']['pageProps']['p']['files'][finddiagramindex(decoded)]['content']
        #print (d)

        
        f = json.loads(d)

        #cf = json.loads(c)

        diagram[currentSlotUpdate] = str(d)
        #sketch[currentSlotUpdate] = str(c)
        #print(c)

        if debug == True:
            print("\n\ndiagram.json\n")
            print(diagram[currentSlotUpdate])

            # print("\n\nsketch.ino\n")
            # print(sketch[currentSlotUpdate])

            print("\n\nlibraries.txt\n")
            print(libraries)

        # print("\n\ndiagram.json\n")
        # print(diagram[currentSlotUpdate])
        justFlashed = 0
            
        # if (sketch[0] != lastsketch[0] and disableArduinoFlashing == 0 and noWokwiStuff == False):
            
        #     #all of this sucks and should probably never be used (on Rev 3 at least)
        #     lastsketch = sketch
        #     justFlashed = 1
            
            
        #     try:
        #         newpath = './WokwiSketch'
        #         compilePath = './WokwiSketch/compile'
                
        #         if not os.path.exists(newpath):
        #             os.makedirs(newpath)
        #             os.makedirs(compilePath)
                

        #         #print("\n\rFlashing Arduino")
        #         sk = open("./WokwiSketch/WokwiSketch.ino", "w")
        #         sk.write(sketch[0])
        #         sk.close()
        #         time.sleep(0.1)
                
        #         ser.write("f 116-70,117-71,".encode())
        #         time.sleep(0.3)
                
        #         try:
        #             menuEntered = 1
        #             cor = ['arduino:avr']
        #             #arduino.core.download(cor)
        #             #arduino.commands.core.install.(cor)
        #             #arduino.core.update_index()
        #             arduino.core.install(cor, no_overwrite= None)
        #             time.sleep(0.5)
                    
        #             menuEntered = 0
                

        #             #installedCores = arduino.core.list()
        #             #print(installedCores)
        #         except Exception as c:
        #             print(c)

        #         if librariesExist == 1 and lastlibraries != libraries:
        #             lastlibraries = libraries
        #             libList = list(libraries.split("\n"))
        #             filteredLibs = list(filter(lambda x: removeLibraryLines(x), libList))
        #             if len(filteredLibs) > 0:
        #                 print("Installing Arduino Libraries ", end="")
        #                 liberror = arduino.lib.install(filteredLibs)
        #                 print(filteredLibs)

                
                
        #         #ser.write('r\n'.encode())
        #         time.sleep(0.1)
                
        #         #arduino.compile( "./WokwiSketch" ,port=arduinoPort,fqbn="arduino:avr:nano", upload=True)
        #        # try:
        #             #arduino.config("-v")
        #         ser.write("_".encode())
        #         print ("Compiling...")
        #         compiledCode = arduino.compile( "./WokwiSketch" ,port=arduinoPort,fqbn="arduino:avr:nano", build_path="./WokwiSketch/compile" )
        #         ser.write("-".encode())
        #         time.sleep(0.1)
        #         print ("Flashing Arduino...")
        #         ser.write("f {116-70,117-71, }".encode())
        #         time.sleep(0.1)
                
        #         arduino.upload( "./WokwiSketch" ,port=arduinoPort,fqbn="arduino:avr:nano", input_dir="./WokwiSketch/compile", discovery_timeout="3s", verify=False  )
        #         print ("Arduino flashed successfully!")
        #         time.sleep(0.1)
        #        # except:# Exception as ardEx:
        #             #print (arduino.errors)
        #           #  print (ardEx)
                   
                    
                
        #     except Exception as e:
               
        #         #errors = ({'__stdout': '{\n  "compiler_out": "",\n  "compiler_err": "",\n  "builder_result": null,\n  "upload_result": null,\n  "success": false,\n  "error": "Error during build: Platform \'arduino:avr\' not found: platform not installed\\nTry running `arduino-cli core install arduino:avr`"\n}', '__stderr': '', 'result': {'compiler_out': '', 'compiler_err': '', 'builder_result': None, 'upload_result': None, 'success': False, 'error': "Error during build: Platform 'arduino:avr' not found: platform not installed\nTry running `arduino-cli core install arduino:avr`"}},)
        #         try:
        #             jd = json.loads(e.args[0]['__stdout'] )
        #             print(jd['error'])
        #         except:
        #             print(e)
        #         #errorMessages = e.args[0]['__stdout'] 
        #         #print (errorMessages)
        #         #print ("Couldn't Flash Arduino")

        #         #continue
        #     try:        
        #         ser.write('m'.encode())
        #     except:
        #         pass
        # #if(noWokwiStuff == True and justreconnected == 1):
        #     #time.sleep(0.1)
        #     #print('Just Reconnected\n\n')
        #     #justreconnected = 0
            


        if ((lastDiagram != diagram or justFlashed == 1 and noWokwiStuff == False) or (forceWokwiUpdate == 1)):
            justFlashed = 0
            #print(forceWokwiUpdate)
            forceWokwiUpdate = 0
            justreconnected = 0
            length = len(f["connections"])

            p = "{ "

            for i in range(length):

                conn1 = str(f["connections"][i][0])

                if conn1.startswith('pot1:SIG'):
                    conn1 = "106"
                elif conn1.startswith('pot2:SIG'):
                    conn1 = "107"
                    
                if conn1.startswith('logic1:'):
                    if conn1.endswith('0'):
                        conn1 = "110"
                    elif conn1.endswith('1'):
                        conn1 = "111"
                    elif conn1.endswith('2'):
                        conn1 = "112"
                    elif conn1.endswith('3'):
                        conn1 = "113"
                    elif conn1.endswith('4'):
                        conn1 = "108"
                    elif conn1.endswith('5'):
                        conn1 = "109"
                    elif conn1.endswith('6'):
                        conn1 = "116"                        
                    elif conn1.endswith('7'):
                        conn1 = "117"
                    elif conn1.endswith('D'):
                        conn1 = "114"                        
                        
                if conn1.startswith("bb1:") == True:
                    periodIndex = conn1.find('.')
                    conn1 = conn1[4:periodIndex]

                    if conn1.endswith('t') == True:
                        conn1 = conn1[0:(len(conn1) - 1)]
                    elif conn1.endswith('b') == True:
                        conn1 = conn1[0:(len(conn1) - 1)]
                        conn1 = int(conn1)
                        conn1 = conn1 + 30
                        conn1 = str(conn1)
                    elif conn1.endswith('n') == True:
                        conn1 = "100"
                    elif conn1.startswith("GND") == True:
                        conn1 = "100"
                    elif conn1.endswith('p') == True:
                        if conn1.startswith('t') == True:
                            conn1 = "105"
                        elif conn1.startswith('b') == True:
                            conn1 = "103"

                if conn1.startswith("nano:") == True:
                    periodIndex = conn1.find('.')
                    conn1 = conn1[5:len(conn1)]

                    if conn1.startswith("GND") == True:
                        conn1 = "100"
                    elif conn1 == "AREF":
                        conn1 = "85"
                    elif conn1 == "RESET":
                        conn1 = "84"
                    elif conn1 == "5V":
                        conn1 = "105"
                    elif conn1 == "3.3V":
                        conn1 = "103"
                    elif conn1 == "5V":
                        conn1 = "105"

                    elif conn1.startswith("A") == True:
                        conn1 = conn1[1:(len(conn1))]
                        conn1 = int(conn1)
                        conn1 = conn1 + 86
                        conn1 = str(conn1)
                    elif conn1.isdigit() == True:
                        conn1 = int(conn1)
                        conn1 = conn1 + 70
                        conn1 = str(conn1)

                conn2 = str(f["connections"][i][1])

                if conn2.startswith('pot1:SIG'):
                    conn2 = "106"
                elif conn2.startswith('pot2:SIG'):
                    conn2 = "107"
                    
                if conn2.startswith('logic1:'):
                    if conn2.endswith('0'):
                        conn2 = "110"
                    elif conn2.endswith('1'):
                        conn2 = "111"
                    elif conn2.endswith('2'):
                        conn2 = "112"
                    elif conn2.endswith('3'):
                        conn2 = "113"
                    elif conn2.endswith('4'):
                        conn2 = "108"
                    elif conn2.endswith('5'):
                        conn2 = "109"
                    elif conn2.endswith('6'):
                        conn2 = "116"                        
                    elif conn2.endswith('7'):
                        conn2 = "117"
                    elif conn2.endswith('D'):
                        conn2 = "114"                             

                if conn2.startswith("bb1:") == True:
                    periodIndex = conn2.find('.')
                    conn2 = conn2[4:periodIndex]

                    if conn2.endswith('t') == True:
                        conn2 = conn2[0:(len(conn2) - 1)]
                    elif conn2.endswith('b') == True:
                        conn2 = conn2[0:(len(conn2) - 1)]
                        conn2 = int(conn2)
                        conn2 = conn2 + 30
                        conn2 = str(conn2)
                    elif conn2.endswith('n') == True:
                        conn2 = "100"
                    elif conn2.startswith("GND") == True:
                        conn2 = "100"
                    elif conn2.endswith('p') == True:
                        if conn2.startswith('t') == True:
                            conn2 = "105"
                        elif conn2.startswith('b') == True:
                            conn2 = "103"

                if conn2.startswith("nano:") == True:
                    periodIndex = conn2.find('.')
                    conn2 = conn2[5:len(conn2)]

                    if conn2.startswith("GND") == True:
                        conn2 = "100"
                    elif conn2 == "AREF":
                        conn2 = "85"
                    elif conn2 == "RESET":
                        conn2 = "84"
                    elif conn2 == "5V":
                        conn2 = "105"
                    elif conn2 == "3.3V":
                        conn2 = "103"
                    elif conn2 == "5V":
                        conn2 = "105"

                    elif conn2.startswith("A") == True and conn2 != "AREF":

                        conn2 = conn2[1:(len(conn2))]
                        conn2 = int(conn2)
                        conn2 = conn2 + 86
                        conn2 = str(conn2)
                    elif conn2.isdigit() == True:
                        conn2 = int(conn2)
                        conn2 = conn2 + 70
                        conn2 = str(conn2)

                if conn1.isdigit() == True and conn2.isdigit() == True:

                    p = (p + conn1 + '-')
                    p = (p + conn2 + ',')

            p = ( p + "} ")

            lastDiagram[currentSlotUpdate] = diagram[currentSlotUpdate]
            #lastDiagram = diagram

            try:
                #print(f)
                #print(p)
                time.sleep(0.1)
                if (numAssignedSlots > 0 and slotURLs[currentSlotUpdate] != '!'):
                    ser.write('o'.encode())
                    time.sleep(0.010)
                    ser.write("Slot ".encode())
                    #print("Slot " + str(currentSlot) + " " + )
                    ser.write(str(currentSlotUpdate).encode())
                    ser.write("\n\n\rf ".encode())
                    ser.write(p.encode())
                    time.sleep(0.4)
                # else:
                #     ser.write('f'.encode())
                #     time.sleep(0.001)
                #     ser.write(p.encode())
                #time.sleep(0.001)
                #print(p)
                

            except Exception as e:
                #print('try again')
                forceWokwiUpdate = 1
                # if (forceWokwiUpdate >= 2):
                #     print(e)
                #     forceWokwiUpdate = 0
                #     #print('try again')
                #     continue
                time.sleep(.5)
                continue
                # waitForReconnect()

                # ser.write('f'.encode())

                # time.sleep(0.05)

                # ser.write(p.encode())

            #print (p)

        
        
        
        
        
        else:
            if (noWokwiStuff == False):
                time.sleep(.75)

