
@echo off

rem IF NOT EXIST ..\build mkdir ..\build
rem pushd ..\build
rem 
rem set CompilerFlags=-nologo -Od -MTd -WX -W4 -wd4505 -wd4267 -wd4456 -wd4201 -wd4100 -wd4189 -FAsc -Zi -Oi -GR- -EHa
rem 
rem rem -std:c++17
rem 
rem 
rem cl %CompilerFlags% -FmMain.map %Macros% ..\code\Main.cpp /link -INCREMENTAL:no %libs%
rem 
rem rem cl %CompilerFlags% -FmMain.map %Macros% ..\code\Test.cpp /link -INCREMENTAL:no %libs%
rem 
rem popd
	

gcc -o Test Test.c
