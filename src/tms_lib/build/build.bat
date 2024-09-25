
@echo off

if not exist obj md obj
if not exist output md output
cd obj
if not exist image md image
cd ..

set PATH=D:\Environment\make381;D:\Environment\RISCVGCC810\bin
set TOOLS_DIR=D:\Environment\RISCVGCC810

set NAME=dsp_tms
set VER=1.0
set output_dir=.\output

make -s %1 all

REM copy /Y %cd%\lib%NAME%%VER%.a D:\DspRead_Work\A50\A50-trio\trio\MainApp\libs\
pause

