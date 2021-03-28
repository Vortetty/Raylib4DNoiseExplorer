@echo off
setlocal EnableExtensions DisableDelayedExpansion

:: Clean up previous
set "FILEMASK=%NAMEPART%.x86.*"
for /F "eol=| delims=" %%F in ('
    dir /B /A:-D "%FILEMASK%" ^| findstr /R "\.[0-9][0-9]*$"
') do (
    del /F "%%~F"
)
del %NAMEPART%.x86.upx

:: .
:: Compile your examples using:  raylib_compile_execute.bat core/core_basic_window.c
:: .
:: > Setup required Environment
:: -------------------------------------
set RAYLIB_SRC="raylib\src"
set RAYLIB_RES_FILE="raylib\src\raylib.rc.data"
set COMPILER="g++.exe"
set MSVC="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.27.29110\bin\Hostx86\x86\cl.exe"
set EXTRAS=-Iraygui -Inuklear_raylib -Inuklear_raylib/nuklear -fpermissive
::set ORIG_PATH=%PATH%
::set TDM_LIBS="TDM-GCC\x86_64-w64-mingw32\lib"

where /q %COMPILER%
if errorlevel 1 (
echo "TDM-GCC, MINGW, or some other g++ distribution for windows, must be installed and on path"
exit /B
)


:: Get full filename path for input file %1
set FILENAME=%~f1
set NAMEPART=%FILENAME:~0,-4%
cd %~dp0

:: .
:: > Cleaning latest build
:: ---------------------------
cmd /c if exist %NAMEPART%.exe del /F %NAMEPART%.exe

:: .
:: > Compiling program
:: --------------------------
:: -s: Remove all symbol table and relocation information from the executable
:: -O2   : Optimization Level 2, this option increases both compilation time and the performance of the generated code
:: -std=c99  : Use C99 language standard
:: -Wall : Enable all compilation Warnings
:: -mwindows : Compile a Windows executable, no cmd window
%COMPILER% %FILENAME% %RAYLIB_RES_FILE% -o %NAMEPART%.x86.exe -s -Ofast %extras% -I%RAYLIB_SRC% -L%RAYLIB_SRC% -Lextra-dlls -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17 -Wall -m32 -mwindows -mthreads -static -fdata-sections -ffunction-sections -Wl,--gc-sections

:: Max optimize with upx
upx\upx.exe --ultra-brute -9 --best -v -f --compress-icons=1 --compress-resources=1 --strip-relocs=1 --compress-exports=1 %NAMEPART%.x86.exe


:: Ensure dirs for built exes
mkdir bin\x86

:: Move builds to respective binary directories
move %NAMEPART%.x86.exe bin\x86

:: Clean up
set "FILEMASK=%NAMEPART%.x86.*"
for /F "eol=| delims=" %%F in ('
    dir /B /A:-D "%FILEMASK%" ^| findstr /R "\.[0-9][0-9]*$"
') do (
    del /F "%%~F"
)
del %NAMEPART%.x86.upx

endlocal