# Creates zipped executable of the Emulator for x64
# Configuring CMake
/bin/bash build_release.sh
# Zipping binaries
echo "Zipping cellox x64 ..."
if [ -d "../build/linux_x64/src" ] 
then
    cd ../build/linux_x64/src
    cpack -G ZIP --config ../CPackConfig.cmake.    
else
    echo "Can not find x64 linux build folder ..."
    exit 70
fi