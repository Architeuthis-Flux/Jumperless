from bs4 import BeautifulSoup

import asyncio
import websocket

import requests
import json
import serial
import time

import serial.tools.list_ports

ws = websocket.WebSocket()
ws.connect("wss://wokwi.com/api/ws/beta")



portSelected = 0

print("\n\r")


    


#the website URL
#url_link = "https://hexi.wokwi.com/build"
wss_link = "wss://wokwi.com/api/ws/beta"

ws.send("Authorization: Bearer wok_f2apXMezNPdtahxhoPFrpLLvqUIt6FmA54113212")

r = ws.recv()

#requests.put(url_link, "Authorization: Bearer wok_f2apXMezNPdtahxhoPFrpLLvqUIt6FmA54113212")

#r = requests.get(url_link)

print(r)

