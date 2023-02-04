:: Creates installer and zipped executable of the Interpreter for x86 and x64
@ECHO OFF
:: Configuring CMake and building for x86 and x64 using gcc
ECHO Configuring CMake for x64 ...
cmake -B ../build/gcc_x64 -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_C_COMPILER=C:/MinGW64/bin/gcc.exe ..
ECHO Building Emulator for x64 ...
cmake --build ./build/gcc_x64 --config Release --target CHIP-8
ECHO Configuring CMake for x86 ...
cmake -B ../build/gcc_x86 -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_C_COMPILER=C:/MinGW32/bin/gcc.exe ..
ECHO Building Emulator for x86 ...
cmake --build ../build/gcc_x86 --config Release --target CHIP-8
:: Zipping binaries and creating installer using NSIS (nullsoft scriptable install system) 
:: (https://sourceforge.net/projects/nsis/) for Windows for x86 and x64
ECHO Zipping cellox x64 ...
IF NOT EXIST ..\build\gcc_x64\src (
    ECHO Can not find x64 Build folder ...
    EXIT
)
cd ..\build\gcc_x64\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x64 using NSIS ...
cpack -G NSIS64 --config ../CPackConfig.cmake
ECHO Zipping cellox x86 ...
IF NOT EXIST ..\..\gcc_x86\src (
    ECHO Can not find x86 Build folder ...
    EXIT
)
cd ..\..\gcc_x86\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x86 using NSIS ...
cpack -G NSIS --config ../CPackConfig.cmake
cd ..\..\..\scripts