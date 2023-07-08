from bs4 import BeautifulSoup


import requests
import json
import serial
import time
import runpy
import subprocess
import sys
import codecs
#import pyduinocli

#from watchedserial import WatchedReaderThread

import serial.tools.list_ports



debug = False


justreconnected = 0

global serialconnected
serialconnected = 0

portSelected = 0

stringified = 0
lastDiagram = 1


print("\n\r")

while portSelected == False:
    autodetected = -1
    ports = serial.tools.list_ports.comports()
    i = 0
    for port, desc, hwid in ports:
        i = i + 1
        print("{}: {} [{}]".format(i, port, desc))
        if desc == "Jumperless":
            autodetected = i
    selection = -1
    
    if autodetected != -1:
        selection = autodetected
        print ("\n\n\rAutodetected Jumperless at",end=" ")
        print(ports[int(selection) - 1].device)
        
        
        portName = ports[int(selection) - 1].device
        
        portSelected = True
        serialconnected = 1
        
    else:
        selection = input ("\n\n\rSelect the port connected to your Jumperless   ('r' to rescan)\n\n\r")
    #selection = "3"
        if selection.isdigit() == True and int(selection) <= i:
            portName = ports[int(selection) - 1].device
            portSelected = True
            print(ports[int(selection) - 1].device)
            serialconnected = 1
    #print(0 in ports)
    


#portName =  '/dev/cu.usbmodem11301'
        
ser = serial.Serial(portName, 115200, timeout = None )

justChecked = 0
reading = 0


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
                

                
            if portFound >= 1:
                try:
                    ser = serial.Serial(portName, 115200, timeout= None)
                    justChecked = 1
                    serialconnected = 1
                    time.sleep(0.05)
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
port_controller = threading.Thread(target=check_presence, args=(portName, .15,), daemon=True)
#port_controller.daemon(True)
port_controller.start()



    
#555 project
    
#https://wokwi.com/projects/369024970682423297
    


#the website URL
#url_link = "https://wokwi.com/projects/369024970682423297"
url_link = input('\n\n\rPaste the link to you Wokwi project here:\n\n\r')


while True:
    
    checkurl = ' '

    try:
        checkurl = requests.get(url_link)
        if (checkurl.status_code == requests.codes.ok):
            break
        else:
            url_link = input('\n\n\rBad link\n\n\rPaste the link to you Wokwi project here:\n\n\r')
            continue
    except:
        url_link = input('\n\n\rBad link\n\n\rPaste the link to you Wokwi project here:\n\n\r')



print("\n\n\rSave your Wokwi project to update the Jumperless\n\n\r")



    
def serialTermIn():
    global serialconnected
    global ser
    global justChecked
    global reading
    readLength = 0
    
    while True:
        try:
            if (ser.in_waiting > 0):
                #justChecked = 0
                reading = 1
                inputBuffer = b' '
                

                waiting = ser.in_waiting
                
                while True:
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
                decoded_string = codecs.escape_decode(bytes(inputBuffer, "utf-8"))[0].decode("utf-8")
                
                decoded_string = decoded_string.lstrip("b' ")
                decoded_string = decoded_string.rstrip("'")
                
                print (decoded_string, end='')
                #print ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
                readlength = 0
                #justChecked = 0
                reading = 0
                

        except:
            portNotFound = 1
            print("Disconnected")
            while (portNotFound == 1):
                portFound = 0        
                       
                for port in serial.tools.list_ports.comports():
                    
                    if portName in port.device:
                        
                        portFound = 1
                        #print (port.device)
                    

                    
                if portFound >= 1:
                    ser = serial.Serial(portName, 115200, timeout= None)
                    justChecked = 1
                    serialconnected = 1
                    time.sleep(0.05)
                    justChecked = 0
                    portNotFound = 0

                    
                    
                else:
                    justreconnected = 1
                    justChecked = 0
                    serialconnected = 0

                    ser.close()
                    portNotFound = 1
                    time.sleep(.1)
                    
                
    
port_controller = threading.Thread(target=serialTermIn, daemon=True)
#port_controller.daemon(True)
port_controller.start()


def serialTermOut():
    global serialconnected
    global ser
    global justChecked
    global justreconnected
    
    resetEntered = 0
    while True:
        
        outputBuffer = input()
        
        if outputBuffer == b'r':
            resetEntered = 1
            
        if (serialconnected == 1):
            #justChecked = 0
            while (justChecked == 0):
                time.sleep(0.0001)
            else:

                #print (outputBuffer)
                if (outputBuffer != " "):
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
                                    #print (port.device)
                                

                                
                                if portFound >= 1:
                                    ser = serial.Serial(portName, 115200, timeout= None)
                                    justChecked = 1
                                    serialconnected = 1
                                    time.sleep(0.05)
                                    justChecked = 0
                                    portNotFound = 0

                                    
                                    
                                else:
                                    justreconnected = 1
                                    justChecked = 0
                                    serialconnected = 0

                                    ser.close()
                                    portNotFound = 1
                                    time.sleep(.1)
                        print (outputBuffer.encode('ascii'))
                        ser.write(outputBuffer.encode('ascii'))
                    
                    
                    if (resetEntered == 1):
                        time.sleep(.5)
                        print ("reset")
                        justreconnected = 1
                        
                
    #time.sleep(.5)
    
port_controller = threading.Thread(target=serialTermOut, daemon=True)

port_controller.start()
    
time.sleep(.75)

while True:
    
    
    #while portIsUsable(portName) == True:
     #   print('fuck')
      #  ser.close()
       # time.sleep(.5)
        #ser = serial.Serial(portName, 460800, timeout=0.050)
        
    while (justreconnected == 1):
        time.sleep(.01)
        lastDiagram = '-1'
        if (serialconnected == 1):
            print ('Reconnected')
            break
    else:
        justreconnected = 0
        
            
    if (serialconnected == 1):      
            
        result = requests.get(url_link).text
        doc = BeautifulSoup(result, "html.parser")

        


        s = doc.find('script', type='application/json').get_text()

        stringex = str(s)

        d = json.loads(stringex)

        
        
        c = d['props']['pageProps']['p']['files'][0]['content']
        
        l = d['props']['pageProps']['p']['files'][2]['content']

        d = d['props']['pageProps']['p']['files'][1]['content']
        
        

        f = json.loads(d)

    #    cf = json.loads(c)
        

        diagram = str(d)
        sketch = str(c)
        libraries = str(l)
        
        if debug == True:
            print("\n\n\rdiagram.json\n\r")
            print(diagram)
            
            print("\n\n\rsketch.ino\n\r")
            print(sketch)
            
            print("\n\n\rlibraries.txt\n\r")
            print(libraries)
         
         
                     
        #if (justreconnected == 1):
            
        
            
            #print (lastDiagram)
            #time.sleep(1.8)
        
            
            


        if (lastDiagram != diagram):
            
            justreconnected = 0
            length = len(f["connections"])
            
            
            p = "{\n"
            
            
            for i in range(length):
                
                conn1 = str(f["connections"][i][0])
                
                if conn1.startswith('pot1:SIG'):
                    conn1 = "106"
                elif conn1.startswith('pot2:SIG'):
                    conn1 = "107"
                               
                
                if conn1.startswith("bb1:") == True:
                    periodIndex = conn1.find('.')
                    conn1 = conn1[4:periodIndex]
                    
                    if conn1.endswith('t') == True:
                        conn1 = conn1[0:(len(conn1)-1)]
                    elif conn1.endswith('b') == True:
                        conn1 = conn1[0:(len(conn1)-1)]
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
                   
          
                if conn2.startswith("bb1:") == True:
                    periodIndex = conn2.find('.')
                    conn2 = conn2[4:periodIndex]
                    
                    if conn2.endswith('t') == True:
                        conn2 = conn2[0:(len(conn2)-1)]
                    elif conn2.endswith('b') == True:
                        conn2 = conn2[0:(len(conn2)-1)]
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
                        
                    
                if conn1.isdigit()== True and conn2.isdigit() == True:

                    p = (p + conn1 + '-')
                    p = (p + conn2 + ',\n')
                

                    
            p = (p + "}\n{\n}")
            
            lastDiagram = diagram

            try:
                ser.write('f'.encode())
                
                time.sleep(0.05)

                ser.write(p.encode())
                
            except:
                continue
                #waitForReconnect()
                
                #ser.write('f'.encode())
                
                #time.sleep(0.05)

                #ser.write(p.encode())
                
            #print (p)
            
        else:
            time.sleep(.5)


