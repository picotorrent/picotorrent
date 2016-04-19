The `torrent_added` message
===========================

The `torrent_added` message is sent when a torrent is successfully added to the
session and contains a `torrent` object representing the newly added torrent.


Example
-------

.. code-block:: javascript
   :linenos:

   {
     "type": "torrent_added",

     // A torrent object representing the added torrent.
     "torrent": { ... }
   }
