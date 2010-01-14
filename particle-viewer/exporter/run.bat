@echo Building assets
rem del cache.sdl.txt
del StringCache.txt
sexport -i sdl.xml -p sdl -v
copy sdl.h ..\assets.h
@echo Done!
pause