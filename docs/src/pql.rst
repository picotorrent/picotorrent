PQL - The PicoTorrent Query Language
====================================

PicoTorrent ships with an embedded query language (called PQL) which makes it
possible to filter the torrent list view in order to quickly show relevant
information.

The query language is somewhat based on SQL but is designed to make querying
torrents easy. The building blocks of the language are the `Types`_, `Fields`_,
`Units`_ and `Operators`_.

Types
-----
PQL supports two primitive and two compound types:

Primitive types
^^^^^^^^^^^^^^^
- :code:`number` is a positive whole or float number;
- :code:`string` is a doublequoted series of Unicode characters.

Compound types
^^^^^^^^^^^^^^
- :code:`size` - consists of a :code:`number` followed by zero or one of the `Units of size`_. See `Examples`_ section.
- :code:`speed` - consists of a :code:`number` and one of the `Units of speed`_. See `Examples`_ section.


Fields
------
These are the fields available to query:

- :code:`name` (`string` type) - the name of the torrent as seen in the UI;
- :code:`progress` (`number` type) - the current progress in percents;
- :code:`size` (`size` type) - the *total wanted* size - e.g. total size excluding skipped files;
- :code:`dl` (`speed` type) - the current download speed;
- :code:`ul` (`speed` type) - the current upload speed;
- :code:`status` (string) - the torrent current status. The possible values are:

  - :code:`downloading`;
  - :code:`error`;
  - :code:`paused`;
  - :code:`queued` - either for downloading or uploading;
  - :code:`seeding`;
  - :code:`uploading`.


Units
-----
PQL use the JEDEC Standard 100B.01 prefixes for its units of size and speed.
The units are used unquoted and are case insensitive.

Units of Size
^^^^^^^^^^^^^^
Used in conjunction with the `size` field.

- :code:`byte` - the default unit of size if not other unit provided;
- :code:`kb` - Kilobytes;
- :code:`mb` - Megabytes;
- :code:`gb` - Gigabytes.

Units of Speed
^^^^^^^^^^^^^^
Used in conjunction with `dl` and `ul` fields.

- :code:`bps` - the default unit of speed if not other unit provided;
- :code:`kbps` - Kilobyte per second;
- :code:`mbps` - megabyte per second;
- :code:`gbps` - Gigabyte per second.


Operators
---------
PQL supports a handful of operators to make filtering flexible.

Logical Operators
^^^^^^^^^^^^^^^^^
There are two logical operators:

- :code:`and` - Logical And;
- :code:`or` - Logical Or.

Comparison Operators
^^^^^^^^^^^^^^^^^^^^
In PQL you can use 6 comparison operators:

- :code:`<` - less than.
- :code:`<=` - less than or equal to.
- :code:`>` - greater than.
- :code:`>=` - greater than or equal to.
- :code:`=` - equal.
- :code:`~` - like. Case insensitive string matching.


Examples
--------

- Example of :code:`size` type with *number* equal to *5* and *size unit* equal to *kb*.
  ::

    5kb

- Example of :code:`speed` type with *number* equal to *10* and *speed unit* equal to *kbps*.
  ::

    10kpbs

- Torrents *larger than 1GB*.
  ::

    size > 1gb

- Torrents where the name contains *ubuntu*.
  ::

    name ~ "ubuntu"

- Torrents with either *1080p* or *720p* in the name.
  ::

    name ~ "1080p" or name ~ "720p"

- Torrents *downloaded at least to 90%*.
  ::

    progress >= 90

- Torrents *larger than 1GB* that are *currently downloading*.
  ::

    size > 1gb and status = "downloading"

- Torrents that are currently *queued*.
  ::

    status = "queued"

- Torrents that are *downloading* with *more than 10 mbps*.
  ::

    dl > 10mpbs

- Torrents that are *uploading* with *more than 5 mbps*.
  ::

    ul > 5mpbs
