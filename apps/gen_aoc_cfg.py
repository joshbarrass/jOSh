#!/usr/bin/env python3

import sys
import os
import re

def parse_day(arg):
    if os.path.splitext(arg)[-1].lower() != ".elf":
        return None
    m = re.search("aoc(\\d\\d).elf", arg)
    if not m:
        print("Bad match")
        return None
    return int(m.groups()[0])

def main():
    cfg_file = sys.argv[1]
    assert os.path.splitext(cfg_file)[-1].lower() not in [".py", ".c", ".h", ".elf", ".ld"]
    if os.path.exists(cfg_file):
        with open(cfg_file, "rb") as f:
            assert len(f.read()) == 0

    args = sorted(sys.argv[2:])
    with open(cfg_file, "w") as f:
        # create the example input submenu
        f.write("submenu \"Test inputs\" {\n")
        for arg in args:
            day = parse_day(arg)
            if day is None:
                continue
            f.write(
f"""menuentry "Test - Day {day:02}" {{
	multiboot /boot/jOShload.elf
        module /boot/jOSh.elf "KERNEL.ELF"
        module /aoc{day:02}.elf "init.elf"
        module /inputs/day{day}_example "input"
}}

"""
                )
        f.write("}\n\n")

        # Create the regular entries
        for arg in args:
            day = parse_day(arg)
            if day is None:
                continue
            f.write(
f"""menuentry "Day {day:02}" {{
	multiboot /boot/jOShload.elf
        module /boot/jOSh.elf "KERNEL.ELF"
        module /aoc{day:02}.elf "init.elf"
        module /inputs/day{day} "input"
}}

"""
                )

if __name__ == "__main__":
    main()
