# PicoTorrent

![CI](https://github.com/picotorrent/picotorrent/workflows/CI/badge.svg)
[![Discord](https://img.shields.io/discord/759537913205227580)](https://discord.gg/tV3dFrv)

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

*The portable version of PicoTorrent requires manual installation of [the Microsoft Visual C++ Redistributable for Visual Studio 2026](https://visualstudio.microsoft.com/downloads/).*


## Download for Windows

[![Download zip](https://custom-icon-badges.demolab.com/badge/-Download-blue?style=for-the-badge&logo=download&logoColor=white "Download zip")](https://github.com/picotorrent/picotorrent/releases)


## Building PicoTorrent

To successfully build PicoTorrent, you need the following installed,

 * [CMake (>= v3.20)](https://cmake.org/download/).
 * [Visual Studio 2026 Build Tools](https://visualstudio.microsoft.com/downloads/) (or regular Visual Studio 2026) with the C++ toolset (v145).
 * [vcpkg](https://github.com/microsoft/vcpkg) — the Visual Studio integration works, or set `VCPKG_ROOT` to a local clone. Dependencies are restored from `vcpkg.json` in manifest mode (no system OpenSSL/Boost needed).
 * [.NET SDK 10](https://dotnet.microsoft.com/download). Used for the build scripts.

```
λ git submodule update --init --recursive
λ dotnet tool restore
λ dotnet cake --platform=x64 --configuration=Release
```


## Translations

PicoTorrent uses [Weblate](https://translate.picotorrent.org/) to handle the translation process. If you want to help, feel free to signup and give your contribution.


## License

Copyright (c) 2015-2026 Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see [LICENSE](LICENSE).
