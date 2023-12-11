@echo off

:: setup build directory
if not exist build mkdir build
pushd build
setlocal

:: common flags
set compiler_flags=/Zi /nologo /std:c17 /utf-8
set linker_flags=/INCREMENTAL:NO

:: compile main
cl %compiler_flags% /Fe:dk.exe ..\main.c  	/link %linker_flags%

endlocal
popd
