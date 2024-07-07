@echo off
mkdir ..\build
pushd ..\build
cl /EHsc -FC -Zi -I.\ ..\source\main.cpp ..\source\clSliFile.cpp ..\source\clSliceData.cpp ..\source\clJobSliceFile.cpp ..\source\clJobFile.cpp ..\source\clJobFileInterpreter.cpp ..\source\clError.cpp ..\source\clFile.cpp
popd
