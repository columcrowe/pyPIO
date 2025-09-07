Import("env")

print("\n=== MPFS Icicle Kit Build Environment ===")
#import pprint
#pprint.pprint(dict(env))
#print("CC :", env.get("CC"))
#print("CXX:", env.get("CXX"))

import serial
import logging
import math
import os
import sys
import time
from typing import Optional, Any, Union

from ymodem.Protocol import ProtocolType
from ymodem.Socket import ModemSocket

#TODO: fix - hardcoded for now
BIN_FILE = r"src\mpfs_icicle_kit\mpfs-blank-baremetal\DDR-Release-IcicleKitEs\mpfs-blank-baremetal.bin"

class TaskProgressBar:
    def __init__(self):
        self.bar_width = 50
        self.last_task_name = ""
        self.current_task_start_time = -1

    def show(self, task_index, task_name, total, success):
        if task_name != self.last_task_name:
            self.current_task_start_time = time.perf_counter()
            if self.last_task_name != "":
                print('\n', end="")
            self.last_task_name = task_name

        success_width = math.ceil(success * self.bar_width / total)

        a = "#" * success_width
        b = "." * (self.bar_width - success_width)
        progress = (success_width / self.bar_width) * 100
        cost = time.perf_counter() - self.current_task_start_time

        print(f"\r{task_index} - {task_name} {progress:.2f}% [{a}->{b}]{cost:.2f}s", end="")

def ymodem_transfer(ser, filename):
    print(f"Starting YMODEM transfer")
    def read(size: int, timeout: Optional[float] = 3) -> Any:
        ser.timeout = timeout
        return ser.read(size)

    def write(data: Union[bytes, bytearray], timeout: Optional[float] = 3) -> Any:
        ser.write_timeout = timeout
        ser.write(data)
        ser.flush()
        return

    try:
        progress_bar = TaskProgressBar()
        socket = ModemSocket(
            read, 
            write,
            packet_size=1024,
            protocol_type=ProtocolType.YMODEM,
            protocol_type_options=[]
        )     
        # Send
        file_path = os.path.abspath(filename)
        print("Waiting for YMODEM transfer...")
        socket.send([file_path], progress_bar.show)      
        print(f"YMODEM transfer success")
        return True
        
    except Exception as e:
        print(f"YMODEM transfer failed: {e}")
        return False

def main():
    print("Reboot...")
    time.sleep(5)

    print("Opening serial port")
    ser = serial.Serial(env.get("UPLOAD_PORT"), env.get("UPLOAD_SPEED"), timeout=5)
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    # Wait for HSS prompt 
    print("Waiting for HSS boot...")
    while True: 
        line = ser.readline().decode(errors='ignore').strip() 
        if line: 
            print("Board:", line) 
            if "Press a key to enter CLI" in line: 
                #print("Enter\r\n")
                ser.write(b"\r\n")
                ser.flush()
                break

    # Wait for CLI prompt
    print("Waiting for CLI prompt...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if ">>" in line:
                #print("YMODEM\r\n")
                ser.write(b"YMODEM\r")
                ser.flush()
                break

    # Wait for MMC Utility menu and start MMC init
    print("Waiting for MMC Utility menu...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if "Select a number:" in line:
                #print("#2 MMC Init\r\n")
                ser.write(b"2") #doesn't need carriage return
                time.sleep(1) #ser.flush()
                break

    # Wait for MMC Utility menu (MMC init success) and start YMODEM Receive
    print("Waiting for MMC Utility menu...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if "Select a number:" in line:
                #print("#3 MMC Receive\r\n")
                ser.write(b"3")
                time.sleep(1) #ser.flush()
                break

    # Wait for YMODEM Receive
    print("Waiting for YMODEM Receive...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if "Attempting to receive .bin file" in line:
                print(f"Starting YMODEM Receive")
                break

    success = ymodem_transfer(ser, BIN_FILE)
    
    if not success:
        print("YMODEM Receive failed")
        ser.close()
        return False
    # Wait for YMODEM Receive success
    print("Waiting for YMODEM Receive success...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if "Received" in line and "bytes" in line:
                print("YMODEM Receive success")
                break

    # Wait for MMC Utility menu and start MMC Write
    print("Waiting for MMC Utility menu...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            print("Board:", line) 
            if "Select a number:" in line:
                #print("#5 MMC Write\r\n")
                ser.write(b"5")
                time.sleep(1) #ser.flush()
                break

    # Wait for MMC Utility menu (MMC write success) and quit
    print("Waiting for MMC Utility menu...")
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line: 
            #print("Board:", line) # causes UART error during MMC write
            if "Select a number:" in line:
                print("MMC write success")
                ser.write(b"6") # Quit
                time.sleep(1) #ser.flush()
                break
                
    ser.close()
    return True


if not main():
    print("Failed")
    sys.exit(1)
else:
    print("Success")
    sys.exit(0)
