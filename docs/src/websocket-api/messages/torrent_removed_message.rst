The `torrent_removed` message
=============================

The `torrent_removed` message is sent when a torrent is removed from the
session. It contains an info hash identifying the removed torrent.


Example
-------

.. code-block:: javascript
   :linenos:

   {
     "type": "torrent_removed",

     // An info hash identifying the the removed torrent.
     "info_hash": "..."
   }
