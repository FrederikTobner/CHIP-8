# Installs CHIP-8 on your system
/bin/bash build_release.sh
echo "Installing CHIP-8 ..."
sudo cmake --install ../build/chip8/main/src/  --config Release