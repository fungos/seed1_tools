VERSION = $$Id: $$
PROJECT=sexporter

include Objects.in

TARGET = bin/$(PROJECT)

# Flags/Compilers
GCC = g++
CDEFS = -D LINUX -D SDL -D _DEBUG -D _SECURITY -D POSIX
CFLAGS = -Wall -fPIC -g `sdl-config --cflags` $(CDEFS)
LFLAGS = `sdl-config --libs` -L/bin -lSDL_image -lboost_filesystem-mt -lboost_system-mt
CPPFLAGS = -Weffc++ $(CFLAGS) -I/opt/local/include
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
