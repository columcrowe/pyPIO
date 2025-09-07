Import("env")

print("\n=== Pycom Lopy Build Environment ===")
#import pprint
#pprint.pprint(dict(env))
#print("CC :", env.get("CC"))
#print("CXX:", env.get("CXX"))

import subprocess
import os
import sys

#TODO: fix - hardcoded for now
PY_FILE = r"src\pycom_lopy\main.py"

def run_command(command):
    try:
        result = subprocess.run(command, check=True, text=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print("Error:", e)
        return False
    return True

def mpremote_copy(env_name, com_port):
    print(f"Building and uploading firmware to {env_name}...")
    command = ["mpremote", "connect", com_port, "fs", "cp", PY_FILE, ":main.py"]
    return run_command(command) 

def mpremote_exit():
    print(f"Disconnecting mpremote...")
    command = ["mpremote", "disconnect"]
    print(f"Press Ctrl-X to exit mpremote...")
    subprocess.run(command, check=True)       

if not mpremote_copy(env.get("PIOENV"), env.get("UPLOAD_PORT")):
    print("Failed")
    sys.exit(1)
else:
    print("Success")
    mpremote_exit()
    sys.exit(0)

