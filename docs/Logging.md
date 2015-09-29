# Logging in PicoTorrent

The logging framework in PicoTorrent is simple and effective. By overriding the
stream operators one can write any type of content to the log.

```
#include <picotorrent/logging/log.hpp>

void my_class::some_function(core::torrent &torrent)
{
    LOG(info) << "This is a torrent: " << torrent;
}
```

## Initializing logging

The framework is initialized in `main.cpp` by calling
`picotorrent::logging::init()`. This will set up unhandled exception handlers
as well as opening an `std::ostream` to a configured log file (the default is
`PicoTorrent.log` in the current working directory).
