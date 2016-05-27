Windows Service support
=======================

PicoTorrent has the ability to run as a native Windows Service. This gives
users the option to have a tiny BitTorrent server running without worrying if a
desktop application is started. A Windows Service starts with Windows and does
not require an active user session.

.. note:: To install PicoTorrent as a Windows Service you need administrative
          privileges on your machine.


Installing
----------

Open up an *elevated* command prompt.

.. code-block:: bat

   sc create PicoTorrent --binPath= "C:\Path\To\PicoTorrent.exe --daemon"


Uninstalling
------------

Open up an *elevated* command prompt.

.. code-block:: bat

   sc delete PicoTorrent
