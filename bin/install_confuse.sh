#!/bin/bash
wget https://github.com/libconfuse/libconfuse/releases/download/v3.3/confuse-3.3.tar.gz
tar -xf confuse-3.3.tar.gz
cd confuse-3.3/
./configure --prefix=/usr && make -j9
sudo ldconfig
sudo make install
