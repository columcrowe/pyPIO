Import("env")

print("\n=== MPFS Icicle Kit Build Environment ===")
#import pprint
#pprint.pprint(dict(env))
#print("CC :", env.get("CC"))
#print("CXX:", env.get("CXX"))

import os
import sys
import shutil
import subprocess

#TODO: fix - hardcoded for now
build_config = "DDR-Release-IcicleKitEs"
### For eclipse ###
os.environ["PATH"] = r"C:\Microchip\SoftConsole-v2022.2-RISC-V-747\eclipse" + os.pathsep + os.environ.get("PATH", "")    
### For toolchain ###
os.environ["PATH"] = r"C:\Microchip\SoftConsole-v2022.2-RISC-V-747\riscv-unknown-elf-gcc\bin" + os.pathsep + os.environ.get("PATH", "")   
### For Make ###
os.environ["PATH"] = r"C:\Microchip\SoftConsole-v2022.2-RISC-V-747\build_tools\bin" + os.pathsep + os.environ.get("PATH", "")    
### For Python ###
os.environ["PATH"] = r"C:\Microchip\SoftConsole-v2022.2-RISC-V-747\python3" + os.pathsep + os.environ.get("PATH", "")
### For Git ###
os.environ["PATH"] = r"E:\windows\PortableGit\bin" + os.pathsep + os.environ.get("PATH", "")

#cd C:\tmp\My_workspace
### For eclipse ###
#set PATH=C:\Microchip\SoftConsole-v2022.2-RISC-V-747\eclipse;%PATH%
### For toolchain ###
#set PATH=C:\Microchip\SoftConsole-v2022.2-RISC-V-747\riscv-unknown-elf-gcc\bin;%PATH%
### For Make ###
#set PATH=C:\Microchip\SoftConsole-v2022.2-RISC-V-747\build_tools\bin;%PATH%
### For Python ###
#set PATH=C:\Microchip\SoftConsole-v2022.2-RISC-V-747\python3;%PATH%
#rm -rf C:\tmp\My_workspace\.metadata
#eclipse.exe -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data C:\tmp\My_workspace -import C:\Microchip\SoftConsole-v2022.2-RISC-V-747\extras\workspace.examples\mpfs-blank-baremetal -cleanBuild mpfs-blank-baremetal/DDR-Release-IcicleKitEs
#cd C:\Microchip\SoftConsole-v2022.2-RISC-V-747\extras\workspace.examples\mpfs-blank-baremetal\DDR-Release-IcicleKitEs
#wsl grep -q .py makefile && sed -i.bak 's|^\(\s*-\)\(.*.py.*\)|\1python \2|' makefile
#or
#uv run mpfs_icicle_kit_grep_sed.py
#make pre-build
#make all
#cd E:\windows\pyPIO\src\mpfs_icicle_kit\mpfs-blank-baremetal\resources
#wsl ./hss-payload-generator -c hss-bm-payload-ddr.yaml output.bin
#wsl mv -f ./output.bin ./../DDR-Release-IcicleKitEs/mpfs-blank-baremetal.bin
#or
#copy /Y output.bin ..\DDR-Release-IcicleKitEs\mpfs-blank-baremetal.bin
#cd E:\windows\pyPIO\
### For Flashing eNVM ###
#SET FPGENPROG=C:\Microchip\Program_Debug_v2023.2\Program_Debug_Tool\bin64\fpgenprog.exe
#cd C:\Microchip\SoftConsole-v2022.2-RISC-V-747\extras\workspace.examples\mpfs-hal-ddr-demo\Icicle-Kit-eNVM-Scratchpad-Release
#C:\Microchip\SoftConsole-v2022.2-RISC-V-747\eclipse\jre\bin\java.exe -jar C:\Microchip\SoftConsole-v2022.2-RISC-V-747\eclipse\/../extras/mpfs/mpfsBootmodeProgrammer.jar --bootmode 1 --die MPFS250T_ES --package FCVG484


def main():

    current_dir = os.getcwd()
    tmp_workspace = os.path.join(current_dir, ".pio", "build")
    current_dir = os.path.join(env.get("PROJECT_SRC_DIR"), env.get("PIOENV"))
    os.chdir(current_dir)  
    if os.path.exists(tmp_workspace):
        shutil.rmtree(tmp_workspace)
    folders = [f for f in os.listdir(current_dir) if os.path.isdir(os.path.join(current_dir, f))]
    if len(folders) != 1:
        raise RuntimeError(f"Expected exactly 1 folder in {current_dir}, found {len(folders)}: {folders}")
    project_folder = folders[0]
    print("Using Project:", project_folder)

    os.makedirs(tmp_workspace, exist_ok=True)
    if os.path.exists(os.path.join(tmp_workspace, ".metadata")):
        shutil.rmtree(os.path.join(tmp_workspace, ".metadata"))        
    command = "eclipse.exe -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data " + tmp_workspace + " -import " + os.path.join(current_dir, project_folder) + " -cleanBuild " + project_folder + "/" + build_config
    result = subprocess.run(command, shell=True, capture_output=True, text=True)

    os.chdir(os.path.join(current_dir, project_folder, build_config))
    with open(r"./makefile", "r") as f:
        lines = f.readlines()

    with open(r"./makefile", "w") as f:
        for line in lines:
            if ".py" in line:
                line = line.replace("-", "-python ", 1)  # only replace the first dash
            f.write(line)
            
    result = subprocess.run(["make", "pre-build"], capture_output=True, text=True)
    result = subprocess.run(["make", "all"], capture_output=True, text=True)
    
    os.chdir(os.path.join(current_dir, project_folder, "resources"))

    command = "hss-payload-generator.exe -c hss-bm-payload-ddr.yaml output.bin"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    print(result)
    print(result.stdout)

    shutil.move(r"output.bin", os.path.join(current_dir, project_folder, build_config, "output.bin"))

    return True


if not main():
    print("Failed")
    sys.exit(1)
else:
    print("Success")
    sys.exit(0)
