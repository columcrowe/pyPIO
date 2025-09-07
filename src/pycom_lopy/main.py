import pycom
import time
import machine
from network import WLAN
import socket
import gc
gc.collect()
print("Hard Reset", machine.reset_cause())

# Disable default blue heartbeat
pycom.heartbeat(False)
# Blink red instead
def blinky():
    pycom.rgbled(0xFF0000)   # Red on
    time.sleep(1)
    pycom.rgbled(0x000000)   # Off

wlan = WLAN()
wlan.antenna(WLAN.EXT_ANT)

# Setup AP
wlan.init(mode=WLAN.AP, ssid='Galaxy A01 Core8885', auth=(WLAN.WPA2, 'randompass123'))
wlan.ifconfig(config=('192.168.43.1', '255.255.255.0', '192.168.43.56', '8.8.8.8'))
time.sleep(5)
print("IP Config:", wlan.ifconfig())

while True:
    blinky()
    time.sleep(5)