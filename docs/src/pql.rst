PQL - The PicoTorrent Query Language
====================================

PicoTorrent ships with an embedded query language (called PQL) which makes it
possible to filter the torrent list view in order to quickly show relevant
information.

The query language is somewhat based on SQL but is designed to make querying
torrents easy.

For example, to filter all torrents larger than 1GB, the following query can
be used:

.. code-block:: sql

   size > 1gb


Types
-----

PQL supports two scalar primitive types:

- :code:`number` is a positive whole or float number;
- :code:`string` is a doublequoted series of Unicode characters.


Fields
------

These are the fields available to query:

- :code:`name` (string) - the name of the torrent as seen in the UI;
- :code:`progress` (number) - the current progress in percents;
- :code:`size` (number) - the *total wanted* size - e.g. total size excluding skipped files. Folowed by one of the Units_;
- :code:`status` (string) - the torrent current status. The possible values are:

  - :code:`downloading`;
  - :code:`error`;
  - :code:`paused`;
  - :code:`queued` - either for downloading or uploading;
  - :code:`seeding`;
  - :code:`uploading`.


Operators
---------

PQL supports a handfull of operators to make filtering flexible.

Comparison Operators
^^^^^^^^^^^^^^^^^^^^

In PQL you can use 6 comparison operators:

- :code:`<` - less than.
- :code:`<=` - less than or equal to.
- :code:`>` - greater than.
- :code:`>=` - greater than or equal to.
- :code:`=` - equal.
- :code:`~` - like. Case insensitive string matching.

Logical Operators
^^^^^^^^^^^^^^^^^

There are two logical operators:

- :code:`and` - Logical And;
- :code:`or` - Logical Or.


Units
-----

PQL uses the JEDEC Standard 100B.01 prefixes for its units. The units are used
unquoted and are case insensitive.

- :code:`byte` - the default unit if not other unit provided;
- :code:`KB` - kilobyte also used as `kb`;
- :code:`MB` - megabyte also used as `mb`;
- :code:`GB` - gigabyte also used as `gb`.


Examples
--------

- Torrents larger than 1GB.
  ::

    size > 1gb

- Torrents where the name contains *ubuntu*.
  ::

    name ~ "ubuntu"

- Torrents with either *1080p* or *720p* in the name.
  ::

    name ~ "1080p" or name ~ "720p"

- Torrents downloaded at 90% or more.
  ::

    progress >= 90

- Torrents larger than 1GB that are currently downloading.
  ::

    size > 1gb and status = "downloading"

- Torrents that are currently queued.
  ::

    status = "queued"
