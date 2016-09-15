Configuration
=============

PicoTorrent is stores its configuration in a JSON file and depending on if
PicoTorrent was installed or not the file will reside at different locations.

* If installed, the file can be found at
  :file:`%APPDATA%/PicoTorrent/PicoTorrent.json`.
* If not installed, the file will be placed next to :file:`PicoTorrent.exe`.


Advanced settings
-----------------

The following is an annotated example file with all settings that are not
available for configuration from the GUI (ie. you need to edit the JSON file
directly).

.. code-block:: javascript
   :linenos:

   {
     // If a user chooses to ignore an update, that version number is stored
     // in this field.
     "ignored_update": "",

     // The API URL where PicoTorrent will check for the latest release.
     "update_url": "https://api.github.com/repos/picotorrent/picotorrent/releases/latest",

     "session": {
        // The max number of simultaneous torrents to allow in 'checking'.
        "active_checking": 1,

        // The max number of torrents to announce to the DHT.
        "active_dht_limit": 88,

        // The max number of active downloads.
        "active_downloads": 3,

        // The max number of active torrents across states.
        "active_limit": 15,

        // The max number of torrents that are allowed to be *loaded* at
        // any given time. PicoTorrent will at times load and unload inactive
        // torrents from memory to conserve memory usage.
        "active_loaded_limit": 100,

        // The max number of torrents to announce to the local network over the
        // local service discovery protocol.
        "active_lsd_limit": 60,

        // The max number of active seeds.
        "active_seeds": 5,

        // The max number of torrents to announce to their trackers.
        "active_tracker_limit": 1600
     }
   }

