# PicoTorrent

[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/picotorrent/picotorrent?svg=true)](https://ci.appveyor.com/project/picotorrent/picotorrent)
[![Join the chat at https://gitter.im/picotorrent/picotorrent](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/picotorrent/picotorrent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A tiny, hackable BitTorrent client written in modern C++. Based on
Rasterbar-libtorrent to provide high performance and low memory usage.

<p align="center">
    <img src="res/screenshot1.png?raw=true" />
</p>


## Quick facts

- (Azureus-style) peer ID: `-PI-`. Example: `-PI0091-` (major: 0, minor: 09, patch: 1).
- User agent: `PicoTorrent/x.y.z`.
- Native look-and-feel across Windows versions.
- Easy to use with high performance.

*The portable version of PicoTorrent requires manual installation of [the Visual C++ 2017 redistributable](https://go.microsoft.com/fwlink/?LinkId=746572)*


## Building PicoTorrent

PicoTorrent depends only on what Rasterbar-libtorrent needs (Boost.System
and OpenSSL) and all dependencies are conveniently pre-packaged in a NuGet
package which will be downloaded as a part of the build process.

To successfully build PicoTorrent, you need the following tools installed,

- CMake (>= v3.8) (installed and added to `PATH`)
- Visual Studio 2017 (w/ C++ toolset)
- Chocolatey (>= v0.10.7)

Build PicoTorrent by running the following in a PowerShell prompt,

```
PS> .\build.ps1
```


## License

Copyright (c) Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see [LICENSE](LICENSE).
