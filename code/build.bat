
@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set CompilerFlags=-nologo -Od -MTd -WX -W4 -wd4505 -wd4267 -wd4456 -wd4201 -wd4100 -wd4189 -std:c++17 -FAsc -Zi -Oi -GR- -EHa


cl %CompilerFlags% -FmMain.map %Macros% ..\code\Main.cpp /link -INCREMENTAL:no %libs%

rem cl %CompilerFlags% -FmMain.map %Macros% ..\code\Test.cpp /link -INCREMENTAL:no %libs%

popd
	
