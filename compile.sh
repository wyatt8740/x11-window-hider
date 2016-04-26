#! /bin/bash
# compiling script. Does a program this small really need autoconf/automake?

# Your C compiler
CC=gcc

"$CC" -Wall hide.c -o hide -lxcb
"$CC" -Wall unhide.c -o unhide -lxcb
