The `torrent_finished` message
==============================

The `torrent_finished` message is sent when all non-skipped files of a torrent
has finished downloading.


Example
-------

.. code-block:: javascript
   :linenos:

   {
     "type": "torrent_finished",

     // A torrent object representing the finished torrent.
     "torrent": { ... }
   }
