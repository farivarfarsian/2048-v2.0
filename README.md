# 2048 v2.0

2048 v2.0 is a C/C++ implementation of the classic puzzle game 2048, where the goal is to combine numbered tiles to reach the tile with the value 2048.
This version is a standalone app built with SDL3 for graphics and input handling.

## Requirements
* CMake (used for build files)
* A C/C++ compiler (e.g., GCC, Clang, MSVC)
* SDL3 Library (Included)

## Build & Run
Before you build you must set the enviroment variable labeled __CMAKE_PREFIX_PATH__ to the root directory of the repo

After that for building the project in Windows with GCC compiler you can just run the **build.bat** file in the root folder and the executable file will be at _out/2048-v2.0.exe_

For other compilers like MSVC or Visual Studio Solutions you can use cmake to generate what you need

__Other platforms aren't supported yet !__

## How to play
Use the keyboard arrow keys:
| Key | Action               |
| --- | -------------------- |
| ↑   | Move all tiles up    |
| ↓   | Move all tiles down  |
| ←   | Move all tiles left  |
| →   | Move all tiles right |

## Screenshots


### License
This project is licensed under the MIT License — see LICENSE.txt for details.