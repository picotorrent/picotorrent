WebSocket
=========

The WebSocket plugin adds a WebSocket server to PicoTorrent where torrent
updates are broadcasted to connected clients. It is currently unprotected
and therefore not suitable for internet traffic.


Configuring the WebSocket
-------------------------

Configuration is stored in the :file:`PicoTorrent.json` file.

.. code-block:: javascript
   :linenos:

   {
     "websocket": {
       // Set to `true` to enable the WebSocket plugin.
       "enabled": false,

       // The port which the WebSocket server will listen on.
       "listen_port": 7676
     }
   }


Messages
--------

The message API is JSON based and each message contains the `type` hash which
tells the consumer what type of message it is.

The first message sent is the `pico_state` message, which contains the full
state for PicoTorrent.

.. toctree::
   :maxdepth: 1

   pico_state <messages/pico_state_message>
   torrent_added <messages/torrent_added_message>
   torrent_finished <messages/torrent_finished_message>
   torrent_removed <messages/torrent_removed_message>
   torrent_updated <messages/torrent_updated_message>


Objects
-------

Each message may contain one or more well-defined objects.

.. toctree::
   :maxdepth: 1

   torrent <objects/torrent>
