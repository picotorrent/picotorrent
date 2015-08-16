"""
This is the main PicoTorrent script and it is responsible for loading and
unloading the BitTorrent session, among other things. 
"""

# Import picotorrent_api as soon as possible so we can use all the sweetness
# it provides.
import picotorrent_api as pico_api
import sys

# Redirect stdout and stderr to the API with these small classes.
class LogStdOut(object):
    def write(self, text):
        pico_api.log(text)

class LogStdErr(object):
    def write(self, text):
        pico_api.log("ERROR: %s" % text)

sys.stdout = LogStdOut()
sys.stderr = LogStdErr()

from session_manager import SessionManager
import update_checker

session = SessionManager()

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
    session.load()
    pico_api.set_application_status("PicoTorrent loaded.")

    # After loading everything, parse the command line and add any torrents
    # passed.
    cmd_line = parse_command_line(sys.argv);
    if "files" in cmd_line: pico_api.show_add_torrents(cmd_line["files"])


def on_unload():
    session.unload()


def on_menu_item_clicked(item_id):
    if item_id == pico_api.menu_item_t.FILE_EXIT:
        pico_api.exit()


def on_torrent_item_activated(info_hash):
    torrent = session.find_torrent(info_hash)

    if not torrent.is_valid():
        return

    torrent_file = torrent.torrent_file()
    pico_api.set_application_status("Torrent activated: %s" % torrent_file.name())


def on_torrent_item_selected(info_hash):
    torrent = session.find_torrent(info_hash)

    if not torrent.is_valid():
        return

    torrent_file = torrent.torrent_file()
    pico_api.set_application_status("Torrent selected: %s" % torrent_file.name())
