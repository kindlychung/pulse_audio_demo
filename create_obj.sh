#!/usr/bin/env bash
objcopy --input-target binary --output-target elf64-x86-64 --binary-architecture i386:x86-64  bell.wav bell.o
