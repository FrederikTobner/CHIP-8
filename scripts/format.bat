:: Formats specific parts of the library.

:: .clang-format is automatically picked up by the npm package. 
:: We should maybe check first if the clang-format npm package is installed at all.

@ECHO OFF
IF NOT EXIST ..\chip8 (
    ECHO Can not find source directory
    EXIT
)

cd ..\chip8

ECHO Formatting all the source file's in the chip8 directory

for /r %%t in (*.cpp *.cc *.c *.hpp *.hh *.h) do clang-format -i --style=file "%%t"

cd ..\scripts