A generalized workflow for embedded systems development using Python and PlatformIO Core to provide an extensible and unified approach to toolchain automation.

requires:
+ direnv (optional) &ndash; [Docs] https://direnv.net/ &ndash; [Src] https://github.com/direnv/direnv
+ uv (optional) &ndash; [Docs] https://docs.astral.sh/uv/ &ndash; [Src] https://github.com/astral-sh/uv
+ Python3.x pinned (strict)
+ requirements.txt (strict)

direnv and uv are (optional) convenience tools for Python package and project management. Alternatively, manage the environment and dependencies using the standard Python, venv and pip workflow for (strict) requirements.  

entrypoints:
+ startup.bat &ndash; CMD  
+ . ./startup.sh &ndash; Bash 