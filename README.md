# push2d2

A Raspberry PI based "push-to" telescope system. 

Why... there's a saying... `the best telescope is the one you use`. My night sky is Bortle class 5. This makes star hopping, while possible, difficult. Integrating my 12" Dobsonian with planetarium software like [Planetarium](http://stellarium.org/) seemed like a fun and challenging way to navigate Bortle 5 easier. If I'm honest though... I really like to tinker on the Raspberry Pi.

### TODO
Update this REAME with Diagrams, pictures, credits to all the previous art I've read through while building this. 

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
