VERSION = $$Id: $$
PROJECT=maskgen

include Objects.in

TARGET = bin/$(PROJECT)

# Flags/Compilers
GCC = g++
CFLAGS = -Wall -fPIC -g `sdl-config --cflags` $(CDEFS)
CDEFS = -D LINUX -D SDL -D _DEBUG -D _SECURITY -D POSIX
LFLAGS = `sdl-config --libs` -L/bin -lSDL_image
CPPFLAGS = -Weffc++ $(CFLAGS)
LIBS =

# targets...

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	$(GCC) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)


valgrind:
	valgrind --leak-check=full $(TARGET)

run:
	$(TARGET)

test:
	cd tests ; ./run.sh ; cd ..
#
# Cleanup
#
clean:
	rm -f $(OBJECTS) $(TARGET) *~ src/*~ core src/inet/*~ src/inet/enet/*~ src/gui/*~ src/sdl/*~
