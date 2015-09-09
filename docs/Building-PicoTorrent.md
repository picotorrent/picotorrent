# Build instructions

PicoTorrent is x64-only and everything but the runtime is statically linked.

**Prerequisites**

- Visual Studio 2015 (Community edition is free).
- Perl (the ActiveState version, required to build OpenSSL).
- Git (to keep your libtorrent sources up to date).
- CMake (to generate the Visual Studio project).

**Library dependencies**

PicoTorrent depends on a few libraries, most of which are dependencies of
libtorrent.

- OpenSSL (1.0.2d).
- Boost (1.59).
- Rasterbar-libtorrent (1.1, the master branch).


## Building dependencies


### OpenSSL

1. Grab [the OpenSSL sources](http://openssl.org/source/openssl-1.0.2d.tar.gz).
2. Extract the sources to `C:/Libs/openssl-1.0.2d`.
3. Set the environment variable `OPENSSL_ROOT` to this path.
4. Open the *VS2015 x64 Native Tools Command Prompt* and change directory to
   the OpenSSL directory.
5. Run the following commands in order,
   - `perl Configure VC-WIN64A no-shared no-idea`
   - `ms\do_win64a.bat`
   - `nmake -f ms\nt.mak`


### Boost

1. Grab [the Boost sources](http://sourceforge.net/projects/boost/files/boost/1.59.0/).
2. Extract the sources to `C:/Libs/boost_1_59_0`.
3. Set the environment variable `BOOST_ROOT` to this path.
4. Open a command prompt and change directory to the Boost directory.
5. Run the following commands in order,
   - `bootstrap.bat`
   - `b2.exe toolset=msvc-14.0 address-model=64 link=static --with-date_time --with-filesystem --with-log --with-program_options --with-random --with-regex --with-system --with-thread`


### Rasterbar-libtorrent

1. Open a command prompt where you have Git available.
2. Change directory to `C:/Libs`.
3. Set the environment variable `LIBTORRENT_ROOT` to `C:/Libs/libtorrent`.
4. Run the following commands in order,
   - `git clone https://github.com/arvidn/libtorrent`
   - `cd libtorrent`
   - `..\boost_1_59_0\b2.exe toolset=msvc-14.0 address-model=64 link=static crypto=openssl deprecated-functions=off include="C:/Libs/openssl-1.0.2d" variant=debug,release`


## Building PicoTorrent

1. Open a command prompt where you have Git and CMake available.
2. Change directory to `C:/Code`.
3. Run the following commands in order,
   - `git clone https://github.com/picotorrent/picotorrent`
   - `cd picotorrent`
   - `.\build.ps1`

This will build a *release* version of PicoTorrent. To build the debug version,
run `.\build.ps1 -Configuration Debug`.

The executable (as well as the `.msi` package) is outputted to
`build/[Debug|Release]`.

A solution file is generated at `build/PicoTorrent.sln`. This can be used to
start PicoTorrent and debug the application.
