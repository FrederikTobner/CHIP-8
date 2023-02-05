@ECHO OFF
set buildType=%1
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
ECHO Configuring CMake ...
cmake -B ../build -DCMAKE_BUILD_TYPE=Debug -DCP8_DEBUG_PRINT_BYTECODE=ON -DCP8_DEBUG_TRACE_EXECUTION=ON -G="Ninja" ..
ECHO Building CHIP-8 ...
cmake --build ../build --config Debug