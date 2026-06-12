RAYLIB_FOLDER = ./third-party/raylib-6.0_linux_amd64
ENGINE_FOLDER = ./src/engine

CFLAGS  = -O3 -ggdb -Wall -Wextra -I$(RAYLIB_FOLDER)/include
LIBS = -L$(RAYLIB_FOLDER)/lib -l:libraylib.a -lm -lX11

all: build/simulation01 lib/engine.so

build/simulation01: src/simulation01.c lib/engine.so
	cc $(CFLAGS) -I./src/engine -o build/simulation01 src/simulation01.c  -L./lib -l:engine.so -Wl,-rpath=/home/tiago/Documents/programacao/project-p/lib/

lib/engine.so: src/engine/*.c src/engine/*.h
	cc $(CFLAGS) -shared -o lib/engine.so $(ENGINE_FOLDER)/engine.c $(ENGINE_FOLDER)/base.c $(ENGINE_FOLDER)/vec.c $(ENGINE_FOLDER)/parser.c $(LIBS)
