# Building PicoTorrent

PicoTorrent is built with minimal dependencies. Most of the requirements
come from `libtorrent` which needs Boost and OpenSSL. Apart from that,
PicoTorrent needs just WTL.

## Obtaining dependencies

* Boost 1.58.0 (http://boost.org)
* libtorrent 1.0.5 (http://libtorrent.org)
* WTL 9.0 (http://sourceforge.net/projects/wtl/)

Extract the dependencies to `C:/Libs`.

### Preparing Boost

```
CMD> cd /Libs/boost_1_58_0
CMD> bootstrap
CMD> b2 toolset=msvc-12.0 --with-system --with-date_time --with-thread
```

Set the environment variable `BOOST_ROOT` to `C:/Libs/boost_1_58_0`.

### Preparing libtorrent

```
CMD> cd /Libs/libtorrent-rasterbar-1.0.5
CMD> ../boost_1_58_0/b2 toolset=msvc-12.0 boost=source link=static variant=debug
CMD> ../boost_1_58_0/b2 toolset=msvc-12.0 boost=source link=static variant=release
```

Set the environment variable `LIBTORRENT_ROOT` to
`C:/Libs/libtorrent-rasterbar-1.0.5`.

### Preparing WTL

Extract the header files from the WTL zip file to `C:/Libs/wtl-9.0` and set the
environment variable `WTL_ROOT` to this path as well.


## Compiling PicoTorrent

From a PowerShell session, run

```
PS> cd /Code/picotorrent
PS> .\build.ps1
```

You should now have the PicoTorrent binary at `./build/Release`.
