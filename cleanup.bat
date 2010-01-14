rem Be sure to be in Seed root
cd /d %SEEDSDK%

del *.ncb /s
del *.suo /s
del *.pdb /s
del *.ilk /s
del *.obj /s
del *.o /s
del seed.lib /s
del *.vcproj.*.user /s
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *VTune*') DO RMDIR /S /Q %%G

rem Demos
cd demos
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S bin') DO RMDIR /S /Q %%G
cd ..

cd seed
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S bin') DO RMDIR /S /Q %%G
cd ..























