echo "Configuring CMake ..."
cmake -B ../build -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_C_COMPILER=/usr/local/bin/clang ..
echo "Building CHIP-8 ..."
cmake --build ../build --config Release --target CHIP-8
echo "Installing CHIP-8 ..."
sudo cmake --install ../build/src --config Release