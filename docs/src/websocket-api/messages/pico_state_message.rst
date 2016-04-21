The `pico_state` message
========================

The `pico_state` message is sent when a client connection is opened and
contains the full PicoTorrent state (all managed torrents) as well as some
version information.


Example
-------

.. code-block:: javascript
   :linenos:

   {
     "type": "pico_state",

     "version_info": {
       // The WebSocket API version.
       "api_version": 1,

       // The name of the Git branch PicoTorrent was built from.
       "git_branch": "master",

       // The Git commit hash (short form) of the build.
       "git_commit_hash": "<sha1 hash of commit>",

       // The PicoTorrent version.
       "picotorrent_version": "x.y.z"
     },

     // An array of torrent objects managed by PicoTorrent.
     "torrents": [ ... ]
   }
