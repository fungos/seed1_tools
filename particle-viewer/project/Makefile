VERSION = $$Id: $$
PROJECT=demo

include Objects.in

TARGET = ../bin/$(PROJECT)

# Flags/Compilers
GCC = g++
CFLAGS = -Wall -fPIC -g `sdl-config --cflags` $(CDEFS) -I/home/fungos/dev/SDK/seed/include
CDEFS = -D _SDL_ -D _DEBUG_ -D _PC_ -D _OGL_ -D _OAL_OGG_
LFLAGS = `sdl-config --libs` -L/bin -lSDL_image -lopenal -lGL -lseed -logg -lvorbis -lvorbisfile
CPPFLAGS = -Weffc++ $(CFLAGS)
LIBS = -L/home/fungos/dev/SDK/seed

# targets...

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	$(GCC) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)

run:
	$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) *~ src/*~ core src/inet/*~ src/inet/enet/*~ src/gui/*~ src/sdl/*~
