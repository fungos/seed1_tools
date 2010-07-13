@echo Building assets
rem del cache.sdl.txt
del StringCache.txt
sexport -i project.xml -p sdl -v
copy project.h ..\assets.h
@echo Done!
pause