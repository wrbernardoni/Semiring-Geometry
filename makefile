CC = g++
CFLAGS  = -o3 --std=c++20

# LDFLAGS=-lssl
# LDFLAGS+=-lcrypto


ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
#     LDFLAGS+=-lws2_32
# 	LDFLAGS+=-lgdi32
# 	LDFLAGS+=-ladvapi32
# 	LDFLAGS+=-lcrypt32
#     LDFLAGS+=-luser32
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
    CFLAGS += -pthread
endif

INCLUDES = -I. -ISemiring -IMonoid -IUtilities

UTILITYSOURCES=$(wildcard Utilities/*.cpp)
UTILITYOBJECTS=$(patsubst Utilities/%.cpp, %.o, $(UTILITYSOURCES))
UTILITYCOMPILED=$(patsubst %.o, build/%.o, $(UTILITYOBJECTS))

all: BuildPath TropicalDStar StorageDemo PolygonTest CGRTest

BuildPath : 
	mkdir -p build

TropicalDStar : Utilities tropicalDStar.cpp
	$(CC) $(CFLAGS) -o build/tropicalDStar tropicalDStar.cpp $(UTILITYCOMPILED) $(INCLUDES) $(LDFLAGS) 

PolygonTest : Utilities PolygonTests.cpp
	$(CC) $(CFLAGS) -o build/PolygonTests PolygonTests.cpp $(UTILITYCOMPILED) $(INCLUDES) $(LDFLAGS) 

CGRTest : Utilities CGRTest.cpp
	$(CC) $(CFLAGS) -o build/CGRTest CGRTest.cpp $(UTILITYCOMPILED) $(INCLUDES) $(LDFLAGS) 

StorageDemo : Utilities StorageDemo.cpp
	$(CC) $(CFLAGS) -o build/StorageDemo StorageDemo.cpp $(UTILITYCOMPILED) $(INCLUDES) $(LDFLAGS) 

Utilities : $(UTILITYOBJECTS)

$(UTILITYOBJECTS) : %.o : Utilities/%.cpp
	$(CC) $(CFLAGS) -o build/$@ -c $< $(INCLUDES) $(LDFLAGS)

clean:
	rm -r -f build/*

