
@echo off

if not exist obj md obj
if not exist output md output
cd obj
if not exist image md image
cd ..

set PATH=D:\Environment\make381;D:\Environment\RISCVGCC810\bin
set TOOLS_DIR=D:\Environment\RISCVGCC810

set NAME=mbedtls
set VER=1.0

make -s %1 all

pause

