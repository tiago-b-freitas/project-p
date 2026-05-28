RAYLIB_PATH = $(HOME)/.local/lib/raylib
# RAYLIB_PATH = ./raylib-6.0_linux_amd64

CFLAGS  = -ggdb -Wall -Wextra -I$(RAYLIB_PATH)/include
LIBS = -L$(RAYLIB_PATH)/lib -l:libraylib.a -lm
build/p: p.c
	cc -O3 $(CFLAGS) -o build/p p.c $(LIBS)

