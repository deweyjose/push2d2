# push2d2

A Raspberry PI based "push-to" telescope system. 

Why... there's a saying... `the best telescope is the one you use`. I'm currently under Bortle class 5 night skies. This makes star hopping, while possible, difficult. Integrating my 12" Dobsonian with planetarium software like [Planetarium](http://stellarium.org/) seemed like a fun and challenging way to navigate Bortle 5 easier. If I'm honest though... I really like to tinker on the Raspberry Pi.

### Mac Setup
My local setup is relatively straightforward.

- [CLion](https://www.jetbrains.com/clion/download/#section=mac) by JetBrains on the Mac. Remote development is great. Remote debugging works, CMake integration is seamless, and remote header includes is pretty amazing. It is not free, not after 30 days anyway, but it does make remote development on the Raspberry Pi turn-key. There are plenty of things to be frustrated about during development, removing complexities around remote developement and debugging was worth the fee IMO.  
- [git](https://github.com/git-guides/install-git#install-git-on-mac)

### Raspberry PI Setup
The project requires a few dependencies to be installed on the Raspberry PI.

- [gcc](https://gcc.gnu.org/) compiler  
- [cmake](https://cmake.org/) a build system for the build system
- [WiringPi](https://github.com/WiringPi/WiringPi) a popular open source GPIO library for the Raspberry PI
- [libconfuse](https://github.com/libconfuse/libconfuse) an open source config file library
- [p2d2.service](p2d2.service) a systemd service for running P2D2 as a service.

PI Setup commands:
```console
sudo apt-get install gcc
sudo apt-get install cmake
sudo apt-get install wiringpi
wget https://github.com/libconfuse/libconfuse/releases/download/v3.3/confuse-3.3.tar.gz
tar -xf confuse-3.3.tar.gz
cd confuse-3.3/
./configure --prefix=/usr && make -j9
sudo ldconfig
cp p2d2.service /etc/systemd/system
sudo systemctl daemon-reload
sudo systemctl enable p2d2.service
git clone https://github.com/deweyjose/push2d2.git
cd push2d2
cmake .
make
cp src/p2d2 .
```
