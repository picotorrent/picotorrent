# PicoTorrent

[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/picotorrent/picotorrent?svg=true)](https://ci.appveyor.com/project/picotorrent/picotorrent)
[![Join the chat at https://gitter.im/picotorrent/picotorrent](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/picotorrent/picotorrent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A tiny, hackable BitTorrent client written in modern C++. Based on
Rasterbar-libtorrent to provide high performance and low memory usage.

<p align="center">
    <img src="res/screenshot1.png?raw=true" />
</p>


## Quick facts

- Supports DHT, PeX, LSD, UPnP.
- Geo IP lookups based on libmaxminddb.
- (Azureus-style) peer ID: `-PI-`. Example: `-PI0151-` (major: 0, minor: 15, patch: 1).
- User agent: `PicoTorrent/x.y.z`.
- Native look-and-feel across Windows versions.
- Easy to use with high performance.

*The portable version of PicoTorrent requires manual installation of [the Visual C++ 2017 redistributable](https://go.microsoft.com/fwlink/?LinkId=746572)*


## Building PicoTorrent

To successfully build PicoTorrent, you need the following libraries and
applications installed,

 * [CMake (>= v3.14)](https://cmake.org/download/).
 * [Visual Studio 2019 Build Tools](https://visualstudio.microsoft.com/downloads/) (or regular Visual Studio 2019 with the C++ toolset).
 * [OpenSSL v1.1.1](https://slproweb.com/products/Win32OpenSSL.html) (*not*
   the light version) for the architecture you plan on building. Both Win32
   and Win64 can be installed side by side.
 * [Qt v5.12.3](https://www.qt.io/download-qt-installer) for the architecture
   you plan on building. Both Win32 and Win64 is recommended, as well as debug
   symbols.
 * [Chocolatey](https://chocolatey.org/install) (Optional) if you plan to build
   or test the Chocolatey package.

Next, set the `CMAKE_PREFIX_PATH` environment variable to your Qt prefix path
(ie. `C:\Qt\5.12.3\msvc2017` or `C:\Qt\5.12.3\msvc2017_64`).

Make sure all Git submodules are updated, then run `.\build.ps1` to start
building and packaging PicoTorrent. If you want to build for x86, you can pass
the target platform to the script.

```
PS> .\build.ps1 --platform [x86|x64] --configuration [Debug|Release]
```


## License

Copyright (c) Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see [LICENSE](LICENSE).
