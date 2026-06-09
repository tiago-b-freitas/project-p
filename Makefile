RAYLIB_PATH = ./raylib-6.0_linux_amd64

CFLAGS  = -O3 -ggdb -Wall -Wextra -I$(RAYLIB_PATH)/include
LIBS = -L$(RAYLIB_PATH)/lib -l:libraylib.a -lm -lX11
SIM = simulation01

all: build/p

build/p: p.c build/parser.o build/base.o build/vec.o $(SIM).o
	cc $(CFLAGS) -o build/p p.c build/vec.o build/base.o build/parser.o $(SIM).o $(LIBS)

build/$(SIM).o: $(SIM).c simulation.h base.h
	cc $(CFLAGS) -c -o build/$(SIM).o $(SIM).c

build/parser.o: parser.c parser.h base.h
	cc $(CFLAGS) -c -o build/parser.o parser.c

build/vec.o: vec.c vec.h base.h
	cc $(CFLAGS) -c -o build/vec.o vec.c

build/base.o: base.c base.h
	cc $(CFLAGS) -c -o build/base.o base.c
