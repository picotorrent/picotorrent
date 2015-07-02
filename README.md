# PicoTorrent

PicoTorrent is a BitTorrent client for Windows. *It is currently in its early
stages of becoming something usable, but feel free to follow the development.*

## Goals

* Fast and easy to use. PicoTorrent should be fast and responsive even with a
  lot of torrents. Controlling torrents should be easy and intuitive.
* Support *at least* Windows XP SP1, Windows 2000 Pro SP3, and up.
* No ads, no cruft, no nonsense. Other tiny BitTorrent clients for Windows
  has a lot of unnecessary ads and bundles. Each feature in PicoTorrent is
  carefully planned and implemented and users can voice opinions in each pull
  request.

## Development guidelines

* Keep dependencies to a minimum. We use `libtorrent` which in turn depends on
  a few `boost` components. Other than that we only use what is in the Win32
  API.
* Clean code. Code should be easy to read and understand. When it is not, its
  purpose should be commented.
* Keep builds as fast as possible. It is boring to wait for the build to finish.
* The `master` branch is pristine and always has releasable code.
* The `develop` branch is a moving target, but committing directly to it is a
  sin and should be avoided. To get code into the `develop` branch, fork this
  project, create a feature branch, then send a pull request. This lets anyone
  comment and review the code.
