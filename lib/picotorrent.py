import glob
import os, os.path
import picotorrent_api as pico_api
from threading import Thread
import update_checker

session = pico_api.get_session()
default_flags = (pico_api.add_torrent_params_flags_t.flag_paused |
                 pico_api.add_torrent_params_flags_t.flag_auto_managed |
                 pico_api.add_torrent_params_flags_t.flag_update_subscribe |
                 pico_api.add_torrent_params_flags_t.flag_apply_ip_filter)

def load_session_state():
    if not os.path.isfile(".session_state"):
        return

    with open(".session_state", "rb") as f:
        state = pico_api.bdecode(f.read())
        session.load_state(state)

    pico_api.set_application_status("Loaded session state.")

def load_torrents():
    if not os.path.isdir("torrents"):
        return

    for torrent_file in glob.glob("torrents/*.torrent"):
        pico_api.set_application_status("adding " + str(torrent_file))

        params = {}
        params["flags"] = default_flags
        params["save_path"] = "C:/Downloads"

        with open(torrent_file, "rb") as f:
            entry = pico_api.bdecode(f.read())
            params["ti"] = pico_api.torrent_info(entry)

        session.async_add_torrent(params)


def on_load():
    load_session_state()
    load_torrents()

    t = Thread(target=update_checker.check_for_update)
    t.daemon = True
    #t.start()
