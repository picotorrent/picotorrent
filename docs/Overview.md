# Overview

The PicoTorrent source code is separated into a few different libraries which
enables code re-use and faster compile times.

## PicoApp

The app library binds `PicoCore` with `PicoUI` and also runs the message loop
required by Windows applications.

## PicoCore

The core library contains the Rasterbar-libtorrent abstractions and logic for
handling state and torrents. The main interaction point is the `session` class
which also invokes a few callbacks based on different alerts in libtorrent.

## PicoFileSystem

The file system library contains abstractions for reading/writing files as well
as traversing directories and other file system related operations. Internally
it uses the Windows API.

## PicoLogging

The logging library is a tiny stream-oriented framework for writing log messages
to any type of `std::stream`.

## PicoUI

The UI library contains the user interface for PicoTorrent. It uses the Windows
API to create a native user experience.

## PicoTorrent

The `PicoTorrent` executable calls the `application` class in `PicoApp`. It is
a simple `main.cpp` file which just provides the executable. No logic exists
at this layer.
