@ECHO OFF
set buildType=%1
IF "%2"=="" (
set buildTarget=all
goto :build
)
set buildTarget=%2
:build
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
ECHO Configuring CMake ...
cmake -B ../build -DCMAKE_BUILD_TYPE=%buildType% -G "Ninja" ..
ECHO Building ...
cmake --build ../build --config %buildType% --target %buildTarget%