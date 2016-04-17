Documentation
=============

Overview
--------

PicoTorrent is a tiny BitTorrent client for Windows. It is written in C++ using
Rasterbar-libtorrent and the raw Win32 API.

.. note:: In case you find errors in this documentation you can help by sending
          `pull requests <https://github.com/picotorrent/picotorrent>`_!


For developers
--------------


The WebSocket API
~~~~~~~~~~~~~~~~~

PicoTorrent has an embedded WebSocket API to allow remote clients to connect
and consume a live stream of updates. The WebSocket endpoint is (by default)
available at :file:`http://localhost:7676/`.

.. toctree::
   :maxdepth: 1

   Authentication <websocket-api/authentication>
   Messages <websocket-api/messages>
