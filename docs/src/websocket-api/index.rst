The WebSocket API
=================

PicoTorrent features an embedded WebSocket API which pushes events to connected
clients in real time. This is different from most other BitTorrent client APIs
since you do not need to poll for changes.


.. note:: All examples shown here assumes a default configuration.


Configuring the WebSocket
-------------------------

To enable the WebSocket API, go to
:menuselection:`View --> Preferences --> Remote`. From here you can also
change the listen port as well as view certificate information and
authentication token.


Connecting
----------

The WebSocket is listening for connections on :file:`wss://localhost:7676/`.
This is an *SSL* protected endpoint, and PicoTorrent will generate a
self-signed certificate upon first launch. This certificate is not trusted and
may throw warnings.

To successfully connect, you will need the authentication token which is
randomly generated upon first launch. You can get this from the
:file:`PicoTorrent.json` configuration file, or by navigating to
:menuselection:`View --> Preferences --> Remote` in the application.

The authentication token must be passed in the header `X-PicoTorrent-Token`,
otherwise the connection *will* fail.


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
   torrent_removed <messages/torrent_removed_message>
   torrent_updated <messages/torrent_updated_message>


Objects
-------

Each message may contain one or more well-documented objects.

.. toctree::
   :maxdepth: 1

   torrent <objects/torrent>
