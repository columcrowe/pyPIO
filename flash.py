import subprocess
import argparse
import os
import sys
import configparser

def setup_environment():
    """Set up environment variables for PlatformIO""" 
    current_dir = os.path.dirname(os.path.abspath(__file__)) #where script resides

    # Requires Git
    git_path = r"E:\windows\PortableGit\bin"
    os.environ["PATH"] = git_path + os.pathsep + os.environ.get("PATH", "")
    # Set PlatformIO environment variables here as direnv fail safe to fallback on
    os.environ["PLATFORMIO_HOME_DIR"] = os.path.join(current_dir, "./.platformio")
    if sys.platform.startswith("win"):
        os.environ["PLATFORMIO_PYTHON_EXE"] = os.path.join(current_dir, "./.venv", "Scripts", "python.exe") # Windows
    elif sys.platform.startswith("linux"):
        os.environ["PLATFORMIO_PYTHON_EXE"] = os.path.join(current_dir, "./.venv", "bin", "python.exe") # Linux
    os.environ["UV_PYTHON_INSTALL_DIR"] = os.path.join(current_dir, "./uv-python")
    os.environ["PYTHONIOENCODING"] = "utf-8"

def run_command(command):
    try:
        result = subprocess.run(command, check=True, text=True)
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print("Error:", e)
        return False
    return True

def flash_firmware(env_name, com_port):
    print(f"Building and uploading firmware to {env_name}...")
    command = ["pio", "run", "-t", "upload", "-e", env_name, "--upload-port", com_port, "-v"]
    return run_command(command) 

def main():
    parser = argparse.ArgumentParser(description="PlatformIO Build and Flash Script")
    config = configparser.ConfigParser()
    config.read("platformio.ini")
    envs = [s.split("env:")[1] for s in config.sections() if s.startswith("env:")]
    parser.add_argument("--env", "-e", 
                        choices=envs,
                        help="Environment name to build")
    parser.add_argument("--port", "-p", 
                        help="COM port for upload")   
    args = parser.parse_args()
    
    # Set up environment variables
    setup_environment()
    
    # Build firmware
    if not flash_firmware(args.env, args.port):
        print("Failed, aborting...")
        sys.exit(1)
    else:
        print("Done.")

if __name__ == "__main__":
    main()