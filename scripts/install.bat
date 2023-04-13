:: Installs CHIP-8 on your system but does not add it to the path
:: Administrator permission is needed to install CHIP-8
@ECHO OFF
CALL build.bat
ECHO Installing CHIP-8 ...
cmake --install ../build/chip8/main/src/ --config Release