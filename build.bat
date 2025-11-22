@echo off

setlocal EnableDelayedExpansion

set libs=-luser32 -lopengl32 -lgdi32
set includes=-I..\third_party -I..\third_party\KHR
set warnings=-D_CRT_SECURE_NO_WARNINGS -Wno-writable-strings -Wno-format-security 
set outfile=cloneste.exe

IF NOT EXIST build mkdir build
pushd build
rem del *.pdb > NUL 2> NUL
clang++ %includes% -g ../src/main.cpp -o%outfile% %libs% %warnings% 
rem del *.obj
popd
