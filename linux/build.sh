#!/bin/bash

cd "$(dirname "$0")"

export CPATH=$HOME/libtorrent/include

( mkdir -p build ; cd build ; cmake ../../ ; make )
