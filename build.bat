@echo off
SET "LOG_FILE=SDL3\i686-w64-mingw32\bin\SDL3.dll"

mkdir out

IF EXIST "%LOG_FILE%" (
    cd out
    cmake .. -G "MinGW Makefiles"
    cmake --build .
    cls
    color 0a
    echo The game has been built in out/2048-v2.0.exe
) ELSE (
    color 0c 
    echo The SDL DLL file is missing! Build Failed!
)

pause
