#!/bin/python3
import subprocess
from pathlib import Path


if __name__ == "__main__":
    build = Path("build")
    build.mkdir(exist_ok=True)
    
    Path(build, ".gitignore").write_text("*")

    subprocess.run(
        ["gcc", "-Werror", "-Wall", "-Wextra", "-Iinclude", "tests/main.c", "-o", "build/test"]
    )
    subprocess.run(
        ["./build/test"]
    )
