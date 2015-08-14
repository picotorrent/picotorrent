# PicoTorrent

PicoTorrent is a cross-platform BitTorrent client written in C++. As the name
implies, PicoTorrent is a small and efficient alternative to other BitTorrent
clients.

*It is currently in its early stages of becoming something usable, but feel
free to follow the development.*


## Goals

* PicoTorrent should be able to handle at least 100k torrents at the same time,
  and still have a responsive user interface.
* Easy to use. Finding torrents should be easy, controlling them equally so.
* No ads, no nonsense. Other tiny BitTorrent clients for Windows has a lot of
  unnecessary ads and bundles. Each feature in PicoTorrent is carefully planned
  and implemented and users can voice opinions in each pull request.


## Development guidelines

* Clean code. Code should be easy to read and understand. When it is not, its
  purpose should be commented.
* Keep builds as fast as possible. It is boring to wait for the build to finish.
* The `master` branch is pristine and always has releasable code.
* The `develop` branch is a moving target, but committing directly to it is a
  sin and should be avoided. To get code into the `develop` branch, fork this
  project, create a feature branch, then send a pull request. This lets anyone
  comment and review the code.


## License

Copyright (c) 2015, Viktor Elofsson and contributors. PicoTorrent is provided
as-is under the MIT license. For more information see
[LICENSE](https://github.com/picotorrent/picotorrent/blob/develop/LICENSE).

* For Boost, see http://www.boost.org/LICENSE_1_0.txt
* For Rasterbar-libtorrent, see https://github.com/arvidn/libtorrent/blob/master/LICENSE
* For RapidJSON, see https://github.com/miloyip/rapidjson/blob/master/license.txt
* For wxWidgets, see https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt
