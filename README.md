# PicoTorrent

[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/picotorrent/picotorrent?svg=true)](https://ci.appveyor.com/project/picotorrent/picotorrent)
[![Join the chat at https://gitter.im/picotorrent/picotorrent](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/picotorrent/picotorrent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A tiny, hackable BitTorrent client written in modern C++. Based on
Rasterbar-libtorrent to provide high performance and low memory usage.

<p align="center">
    <img src="res/screenshot1.png?raw=true" width="614" />
</p>


## Quick facts

- Full support for BitTorrent 2.0 ([BEP-52](http://bittorrent.org/beps/bep_0052.html)), v1, v2 and v1+v2 hybrid torrents.
- Supports DHT, PeX, LSD, UPnP.
- (Azureus-style) peer ID: `-PI-`. Example: `-PI0151-` (major: 0, minor: 15, patch: 1).
- User agent: `PicoTorrent/x.y.z`.
- Native look-and-feel across Windows versions.
- Easy to use with high performance.

*The portable version of PicoTorrent requires manual installation of [the Microsoft Visual C++ Redistributable for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/)*


## Building PicoTorrent

To successfully build PicoTorrent, you need the following libraries and
applications installed,

 * [CMake (>= v3.14)](https://cmake.org/download/).
 * [Visual Studio 2019 Build Tools](https://visualstudio.microsoft.com/downloads/) (or regular Visual Studio 2019 with the C++ toolset).
 * [OpenSSL v1.1.1](https://slproweb.com/products/Win32OpenSSL.html) (*not*
   the light version) for the architecture you plan on building. Both Win32
   and Win64 can be installed side by side.
 * [Boost 1.73.0](https://www.boost.org).
 * [Chocolatey](https://chocolatey.org/install) (Optional) if you plan to build
   or test the Chocolatey package.

Make sure all submodules are updated.

```
λ git submodule update --init --recursive
```

Build your desired configuration and architecture.

```
λ .\build.ps1 --platform [x86|x64] --configuration [Debug|Release]
```


## Translations

PicoTorrent uses [Weblate](https://translate.picotorrent.org/) to handle the translation process. If you want to help, feel free to signup and give your contribution.


## License

Copyright (c) Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see [LICENSE](LICENSE).
