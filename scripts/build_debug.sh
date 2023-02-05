echo "Configuring CMake ..."
cmake -B ../build -DCMAKE_BUILD_TYPE=Debug -G Ninja -DCMAKE_C_COMPILER=/usr/local/bin/clang ..
echo "Building CHIP-8 ..."
cmake --build ../build --config Debug --target CHIP-8