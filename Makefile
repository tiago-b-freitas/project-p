RAYLIB_PATH = $(HOME)/.local/lib/raylib
# RAYLIB_PATH = ./raylib-6.0_linux_amd64

CFLAGS  = -ggdb -Wall -Wextra -I$(RAYLIB_PATH)/include
LIBS = -L$(RAYLIB_PATH)/lib -l:libraylib.a -lm

all: build/p

build/p: p.c parser.c base.c
	cc $(CFLAGS) -o build/p p.c base.c $(LIBS)

build/base.o: base.h base.c
	cc $(CFLAGS) -c -o build/base.o base.c $(LIBS)

# build/parser: parser.c
# 	cc $(CFLAGS) -o build/parser parser.c $(LIBS)
