The `torrent_updated` message
=============================

The `torrent_updated` message is sent approximately once per second and
contains a list of torrents which have been updated (i.e transfer rates,
progress, etc).


Example
-------

.. code-block:: javascript
   :linenos:

   {
     "type": "torrent_updated",

     // A list of torrents which has been updated.
     "torrents": [ ... ]
   }
