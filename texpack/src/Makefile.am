bin_PROGRAMS = texpack
texpack_SOURCES = texpack.cpp iimage.cpp ierror.cpp rectpack.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
texpack_LDFLAGS = $(all_libraries) 
texpack_LDADD = -lILU -lIL


noinst_HEADERS = iimage.hpp ierror.hpp texpack.h rectpack.h
