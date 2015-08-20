"""
This is the main PicoTorrent script and it is responsible for loading and
unloading the BitTorrent session, among other things. 
"""
import os
import sys
sys.path.append(os.path.dirname(sys.argv[0]))

import asyncio
loop = asyncio.get_event_loop()

# Import picotorrent_api as soon as possible so we can use all the sweetness
# it provides. The very first thing we do is to set up logging (both to file
# and to the GUI).
import picotorrent.logger
picotorrent.logger.setup(loop)

import logging
logger = logging.getLogger(__name__)

from picotorrent.session_manager import SessionManager
import picotorrent_api as pico_api
from threading import Thread

session = SessionManager(loop)


def parse_command_line(arguments):
    result = {}

    for arg in arguments:
        pass

    return result


def on_instance_already_running():
    """Called when an instance of PicoTorrent is already running.

    From here, we will try to connect to the existing PicoTorrent instance and
    send our command line arguments to it via a TCP socket which PicoTorrent
    binds to when on_load is called.

    """
    pass


def on_load():
    logger.debug("on_load()")

    session.load()
    pico_api.set_application_status("PicoTorrent loaded.")

    # After loading everything, parse the command line and add any torrents
    # passed.
    cmd_line = parse_command_line(sys.argv);
    if "files" in cmd_line: pico_api.show_add_torrents(cmd_line["files"])

    t = Thread(target=loop.run_forever)
    t.daemon = True
    t.start()


def on_unload():
    loop.stop()

    logger.debug("on_unload()")
    session.unload()


def on_menu_item_clicked(item_id):
    logger.debug("on_menu_item_clicked(%d)", item_id)

    if item_id == pico_api.menu_item_t.FILE_EXIT:
        pico_api.exit()


def on_torrent_item_activated(info_hash):
    logger.debug("on_torrent_item_activated(%s)", info_hash)
    torrent = session.find_torrent(info_hash)

    if not torrent.is_valid():
        return

    torrent_file = torrent.torrent_file()
    pico_api.set_application_status("Torrent activated: %s" % torrent_file.name())


def on_torrent_item_selected(info_hash):
    logger.debug("on_torrent_item_selected(%s)", info_hash)
    torrent = session.find_torrent(info_hash)

    if not torrent.is_valid():
        return

    torrent_file = torrent.torrent_file()
    pico_api.set_application_status("Torrent selected: %s" % torrent_file.name())
