# push2d2

A Raspberry PI based "push-to" telescope system.

### Mac Setup
There's a saying ... `the best maker setup is the one you actually use`

- [CLion](https://www.jetbrains.com/clion/download/#section=mac) by JetBrains on the Mac. Remote development is great. Remote debugging works, CMake integration is seamless, and remote header includes is pretty amazing.
- git

### Raspberry PI Setup
The project requires a few dependencies to be installed on the Raspberry PI.

- [gcc](https://gcc.gnu.org/) compiler  
- [cmake](https://cmake.org/) a build system for the build system
- [WiringPi](https://github.com/WiringPi/WiringPi) an open source popular GPIO library for the Raspberry PI
- [libconfuse](https://github.com/libconfuse/libconfuse) an open source config file library

These commands are all thats needed to get the PI configured for development.
```console
sudo apt-get install gcc
sudo apt-get install cmake
sudo apt-get install wiringpi
wget https://github.com/libconfuse/libconfuse/releases/download/v3.3/confuse-3.3.tar.gz
tar -xf confuse-3.3.tar.gz
cd confuse-3.3/
./configure --prefix=/usr && make -j9
sudo ldconfig
sudo make install
```
