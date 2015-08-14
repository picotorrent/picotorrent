# Configuration overview

PicoTorrent reads its configuration from the `PicoTorrent.json` file which it
expects to find in the *current working directory*. When running PicoTorrent
from a default installation (on Windows), the file will be located at
`%LOCALAPPDATA%/PicoTorrent/PicoTorrent.json`.

If this file is not found, PicoTorrent has a default set of configuration
options which it reads instead.


## Default configuration

This is the default configuration which is hard-coded into PicoTorrent. You can
edit the configuration from within PicoTorrent or by editing the
`PicoTorrent.json` with a text editor.

```javascript
{
  // The interface and port on which PicoTorrent will listen.
  "bt.net.interface": [ "0.0.0.0", 6881 ],

  // The default save path for new torrents.
  "savePath": "%USERPROFILE%\Downloads"
}
```
