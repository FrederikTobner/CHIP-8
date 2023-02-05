:: Creates installer and zipped executable of the Emulator for x86 and x64
@ECHO OFF
:: Use 64-bit msvc
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
ECHO Configuring CMake for x64 ...
cmake -B ../build/msvc_x64 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x64 ...
cmake --build ../build/msvc_x64 --config Release --target CHIP-8
:: Use 32-bit msvc
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
ECHO Configuring CMake for x86 ...
cmake -B ../build/msvc_x86 -DCMAKE_BUILD_TYPE=Release -G "Ninja" ..
ECHO Building Emulator for x86 ...
cmake --build ../build/msvc_x86 --config Release --target CHIP-8
:: Zipping binaries and creating installer using NSIS (nullsoft scriptable install system) 
:: (https://sourceforge.net/projects/nsis/) for Windows for x86 and x64
ECHO Zipping CHIP-8 x64 ...
IF NOT EXIST ..\build\msvc_x64\src (
    ECHO Can not find x64 Build folder ...
    EXIT
)
cd ..\build\msvc_x64\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x64 using NSIS ...
cpack -G NSIS64 --config ../CPackConfig.cmake
:: x86
ECHO Zipping CHIP-8 x86 ...
IF NOT EXIST ..\..\msvc_x86\src (
    ECHO Can not find x86 Build folder ...
    EXIT
)
cd ..\..\msvc_x86\src
cpack -G ZIP --config ../CPackConfig.cmake
ECHO Creating installer - x86 using NSIS ...
cpack -G NSIS --config ../CPackConfig.cmake
cd ..\..\..\scripts