PQL - The PicoTorrent Query Language
====================================

PicoTorrent ships with an embedded query language which makes it possible to
filter the torrent list view in order to quickly show relevant information.

The query language (called PQL) is somewhat based on SQL but is designed to
make querying torrents easy.

For example, to query all torrents larger than 1GB, the following query can
be used;

.. code-block:: sql

   size > 1gb

Examples
--------

- Torrents larger than 1gb that is currently downloading.
  ::

     size > 1gb and status = "downloading"

- Torrents that are currently queued (either for downloading or uploading).
  ::

     status = "queued"

- Torrents with either *1080p* or *720p* in the name.
  ::

     name ~ "1080p" or name ~ "720p"


Fields
------

These are the fields available to query.

- :code:`name` (*string*) - the name of the torrent.
- :code:`progress` (*number*) - the current progress.
- :code:`size` (*number*) - the *total wanted* size - eg. total size excluding skipped files.
- :code:`status` (*string*) - the current status. Possible values are :code:`error`,
  :code:`downloading`, :code:`paused`, :code:`queued`, :code:`seeding`, :code:`uploading`


Comparison operators
--------------------

- :code:`<` - less than.
- :code:`<=` - less than or equal.
- :code:`>` - greater than.
- :code:`>=` - greater than or equal.
- :code:`=` - equal.
- :code:`~` - like. Can be used to match part of torrent name to a string, For
  example :code:`name ~ "ubuntu"` will match all torrents where the name contains
  *ubuntu*.
