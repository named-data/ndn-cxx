#!/usr/bin/env bash
set -ex

sudo rm -f /usr/local/bin/ndnsec*
sudo rm -fr /usr/local/include/ndn-cxx
sudo rm -f /usr/local/lib{,64}/libndn-cxx*
sudo rm -f /usr/local/lib{,64}/pkgconfig/libndn-cxx.pc
