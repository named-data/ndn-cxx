#!/usr/bin/env bash
set -exo pipefail

PROJ=ndn-cxx

sudo rm -f /usr/local/bin/ndnsec*
sudo rm -fr /usr/local/include/"$PROJ"
sudo rm -f /usr/local/lib{,64}/lib"$PROJ"*
sudo rm -f /usr/local/lib{,64}/pkgconfig/{,lib}"$PROJ".pc
