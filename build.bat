@echo off
pushd .
setlocal

:: setup build directory
if not exist build mkdir build
cd build

:: common flags
set compiler_flags=/Zi /nologo /std:c17 /utf-8
set linker_flags=/INCREMENTAL:NO

:: compile main
cl %compiler_flags% ..\main.c /Fedansk.exe /link %linker_flags%

endlocal
popd
