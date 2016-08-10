# PicoTorrent

[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/picotorrent/picotorrent?svg=true)](https://ci.appveyor.com/project/picotorrent/picotorrent)
[![Join the chat at https://gitter.im/picotorrent/picotorrent](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/picotorrent/picotorrent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A tiny, hackable BitTorrent client written in modern C++. Based on
Rasterbar-libtorrent to provide high performance and low memory usage.

![](https://cdn.rawgit.com/picotorrent/picotorrent.github.io/master/img/screenshots/picotorrent1.png)


## Quick facts

- (Azureus-style) peer ID: `-PI-`. Example: `-PI0091-` (major: 0, minor: 09, patch: 1).
- User agent: `PicoTorrent/x.y.z`.
- The x86 executable is less than 1.5MB if compressed with UPX.
- Native look-and-feel across Windows versions.
- Support for magnet link pre-loading.
- Extensible with plugins written in C++ or any .NET compliant language.


## Building PicoTorrent

PicoTorrent depends only on what Rasterbar-libtorrent needs (Boost.System,
and Boost.Random) and all dependencies are conveniently pre-packaged in a
NuGet package which will be downloaded as a part of the build process.

To successfully build PicoTorrent, you need the following tools installed,

- CMake (>= v2.8) (installed and added to `PATH`)
- Visual Studio 2015 (Community edition) (with C++ toolset installed)
- Chocolatey (>= v0.9.9.11)

Build PicoTorrent by running the following in a PowerShell prompt,

```
PS> git submodule update --init
PS> .\build.ps1
```


## License

Copyright (c) 2015, Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see [LICENSE](LICENSE).
