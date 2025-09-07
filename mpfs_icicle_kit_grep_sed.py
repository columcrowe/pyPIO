with open(r"./makefile", "r") as f:
    lines = f.readlines()

with open(r"./makefile", "w") as f:
    for line in lines:
        if ".py" in line:
            line = line.replace("-", "-python ", 1)  # only replace the first dash
        f.write(line)
