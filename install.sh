#!/bin/bash
gcc $(pkg-config --cflags gtk+-3.0) ./src/deskimg.c -o ~/.local/bin/deskimg $(pkg-config --libs gtk+-3.0)
