:: Creates installer and zipped executable of the Emulator for x86 and x64
@ECHO OFF

:: x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
ECHO Configuring CMake for x64 ...
cmake -B ../build/x64 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x64 ...
cmake --build ../build/x64 --config Release --target CHIP-8

:: x86
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
ECHO Configuring CMake for x86 ...
cmake -B ../build/x86 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x86 ...
cmake --build ../build/x86 --config Release --target CHIP-8

:: x64_x86
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"
ECHO Configuring CMake for x64_x86 ...
cmake -B ../build/x64_x86 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x64_x86 ...
cmake --build ../build/x64_x86 --config Release --target CHIP-8

:: x86_x64
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsx86_amd64.bat"
ECHO Configuring CMake for x86_x64 ...
cmake -B ../build/x86_x64 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x86_x64 ...
cmake --build ../build/x86_x64 --config Release --target CHIP-8

:: Zipping binaries and creating installer using NSIS (nullsoft scriptable install system) 
:: (https://sourceforge.net/projects/nsis/) for Windows for x86 and x64

:: x64
ECHO Zipping CHIP-8 x64 ...
IF NOT EXIST ..\build\x64\src (
    ECHO Can not find x64 Build folder ...
    EXIT
)
cd ..\build\x64\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x64 using NSIS ...
cpack -G NSIS64 --config ../CPackConfig.cmake

:: x86
ECHO Zipping CHIP-8 x86 ...
IF NOT EXIST ..\..\x86\src (
    ECHO Can not find x86 Build folder ...
    EXIT
)
cd ..\..\x86\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x86 using NSIS ...
cpack -G NSIS --config ../CPackConfig.cmake

:: x64_x86
ECHO Zipping CHIP-8 x64_x86 ...
IF NOT EXIST ..\..\x64_x86\src (
    ECHO Can not find x64_x86 Build folder ...
    EXIT
)
cd ..\..\x64_x86 \src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x64_x86  using NSIS ...
cpack -G NSIS --config ../CPackConfig.cmake

:: x86_x64
ECHO Zipping CHIP-8 x86_x64 ...
IF NOT EXIST ..\..\x86_x64\src (
    ECHO Can not find x86_x64 Build folder ...
    EXIT
)
cd ..\..\x86_x64\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x86_x64 using NSIS ...
cpack -G NSIS --config ../CPackConfig.cmake

cd ..\..\..\scripts