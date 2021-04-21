Debian buster сильно отстает по скорости обновления пакетов, поэтому собираем qt5 статически.
./configure -developer-build -opensource -no-widgets -no-gui -nomake examples -nomake tests -confirm-license -static -prefix /8/static -release -device linux-imx6-g++ -device-option   CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf- -sysroot /usr/lib/gcc-cross/arm-linux-gnueabihf/
