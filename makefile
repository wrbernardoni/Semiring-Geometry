CC = g++
CFLAGS  = -o3 -fopenmp --std=c++17

LDFLAGS=-lssl
LDFLAGS+=-lcrypto


ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
    LDFLAGS+=-lws2_32
	LDFLAGS+=-lgdi32
	LDFLAGS+=-ladvapi32
	LDFLAGS+=-lcrypt32
    LDFLAGS+=-luser32
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
    CFLAGS += -pthread
endif

INCLUDES = -I. -ISemiring -IMonoid -IUtilities

all: BuildPath TropicalDStar

BuildPath : 
	mkdir -p build

TropicalDStar : tropicalDStar.cpp
	$(CC) $(CFLAGS) -o build/tropicalDStar tropicalDStar.cpp $(INCLUDES) $(LDFLAGS) 

clean:
	rm -r -f build/*

