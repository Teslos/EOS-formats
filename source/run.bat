@echo off
copy test.inp ..\build
copy test.sli ..\build
pushd ..\build
main.exe test.inp
popd
