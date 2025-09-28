@echo off

setlocal EnableDelayedExpansion

SET cppFiles=
FOR /R %%f in (src\*.cpp) do (
    SET cppFiles=!cppFiles! %%f
)

set libs=-luser32
set warnings=-Wno-writable-strings
set outfile=cloneste.exe

IF NOT EXIST build mkdir build
pushd build
rem del *.pdb > NUL 2> NUL
clang %cppFiles% -o%outfile% %libs% %warnings% 
rem del *.obj
popd