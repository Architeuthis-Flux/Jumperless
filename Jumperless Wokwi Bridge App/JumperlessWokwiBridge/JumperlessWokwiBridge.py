from bs4 import BeautifulSoup


import requests
import json
import serial
import time

import serial.tools.list_ports



portSelected = 0

print("\n\r")

while portSelected == False:
    
    ports = serial.tools.list_ports.comports()
    i = 0
    for port, desc, hwid in sorted(ports):
            i = i + 1
            print("{}: {} [{}]".format(i, port, desc))
            
    selection = input ("\n\n\rSelect the port connected to your Jumperless   ('r' to rescan)\n\n\r")
    if selection.isdigit() == True and int(selection) <= i:
        portName = ports[int(selection) - 1].device
        portSelected = True
        print(ports[int(selection) - 1].device)
    #print(0 in ports)
    


#portName =  '/dev/cu.usbmodem11301'

def portIsUsable(portName):
    try:
       ser = serial.Serial(port=portName)
       return False
    except:
       return True
    
    

    
ser = serial.Serial(portName, 460800, timeout=0.050)

#the website URL
#url_link = "https://wokwi.com/projects/367384677537829889"


url_link = input('\n\n\rPaste the link to you Wokwi project here:\n\n\r')

print("\n\n\rSave your Wokwi project to update the Jumperless\n\n\r")


stringified = 0
lastDiagram = 1

while True:
    result = requests.get(url_link).text
    doc = BeautifulSoup(result, "html.parser")

    


    s = doc.find('script', type='application/json').get_text()

    stringex = str(s)

    d = json.loads(stringex)

    d = d['props']['pageProps']['p']['files'][1]['content']

    f = json.loads(d)

    
    

    stringified = str(f)
    
    

    if lastDiagram != stringified:
        

        
        
        length = len(f["connections"])
        
        
        p = "{\n"
        
        
        for i in range(length):
            
            conn1 = str(f["connections"][i][0])
            
            
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
        
        lastDiagram = stringified


        ser.write('f'.encode())
        
        time.sleep(0.1)

        ser.write(p.encode())
        
        #print (p)
        
    else:
        time.sleep(0.5)


