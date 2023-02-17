echo "Configuring CMake ..."
cmake -B ../build -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_C_COMPILER=clang ..
echo "Building CHIP-8 ..."
cmake --build ../build --config Release --target CHIP-8