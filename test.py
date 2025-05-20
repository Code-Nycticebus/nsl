#!/bin/python3
import subprocess


if __name__ == "__main__":
    subprocess.run(
        ["gcc", "-Werror", "-Wall", "-Wextra", "-Iinclude", "tests/main.c", "-o", "build/test"]
    )
    subprocess.run(
        ["./build/test"]
    )
