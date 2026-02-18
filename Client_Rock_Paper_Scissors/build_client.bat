@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64
if %errorlevel% neq 0 exit /b %errorlevel%

set "QT_DIR=C:\Qt\6.10.2\msvc2022_64"
set "CMAKE_PREFIX_PATH=%QT_DIR%"
set "Path=%QT_DIR%\bin;%Path%"

"C:\Qt\Tools\CMake_64\bin\cmake.exe" -S . -B build_msvc -G "NMake Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%

"C:\Qt\Tools\CMake_64\bin\cmake.exe" --build build_msvc --config Release
if %errorlevel% neq 0 exit /b %errorlevel%

echo BUILD SUCCESSFUL
