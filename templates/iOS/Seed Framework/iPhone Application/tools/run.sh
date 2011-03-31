#!/bin/bash

echo Building iPhone assets
rm cache.sdl.txt 2> /dev/null
rm stringcache.txt 2> /dev/null
./sexporter -i assets.xml -p iph -v
cp assets.h ../files/assets.h
