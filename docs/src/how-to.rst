How-to
======


Use regular expressions to filter files
---------------------------------------

The *Add torrent dialog* has the option to include or exclude files within a
torrent based on a regular expression.

The regular expression should be ECMAScript compatible, and will match against
the whole torrent file path.


Match against a file extension
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: javascript

   .*\.iso$


File name should contain X
~~~~~~~~~~~~~~~~~~~~~~~~~~

This will match against any file that has *md5sum* in its file name.

.. code-block:: javascript

   .*md5sum.*
