#!/bin/bash

echo Building asset files
rm cache.sdl.txt 2> /dev/null
rm stringcache.txt 2> /dev/null
./sexporter -i assets.xml -p sdl -v
mv assets.h ../source/assets.h
rm cache.sdl.txt 2> /dev/null
rm stringcache.txt 2> /dev/null