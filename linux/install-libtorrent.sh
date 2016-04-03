#!/bin/sh
set -e

# check if we have already installed libtorrent
if [ ! -d "$HOME/libtorrent/lib" ]; then
  wget -O libtorrent.tar.gz -P ~/ https://github.com/arvidn/libtorrent/archive/f662d86458abc0aae6920202ab17674c3d59b9bd.tar.gz
  tar -xzvf libtorrent.tar.gz -C ~/ > /dev/null
  cd ~/libtorrent-f662d86458abc0aae6920202ab17674c3d59b9bd
  ./autotool.sh
  ./configure --enable-debug=no --enable-deprecated-functions=no --prefix=$HOME/libtorrent
  make
  make install
else
  echo "Using cache directory"
fi
