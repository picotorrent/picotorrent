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

print("Loadin")

import glob
import libtorrent as lt
import os, os.path
from threading import Thread, Timer
import update_checker

session = None
is_running = False

default_flags = (lt.add_torrent_params_flags_t.flag_paused |
                 lt.add_torrent_params_flags_t.flag_auto_managed |
                 lt.add_torrent_params_flags_t.flag_update_subscribe |
                 lt.add_torrent_params_flags_t.flag_apply_ip_filter)

def read_alerts():
    global is_running
    is_running = True

    while is_running:
        if not session.wait_for_alert(250):
            continue

        for alert in session.pop_alerts():
            try:
                handle_alert(alert)
            except Exception, e:
                pico_api.set_application_status("Error when handling alert: %s" % e)


def handle_alert(alert):
    alert_type = type(alert).__name__

    if alert_type == "add_torrent_alert":
        if alert.error.value():
            pico_api.set_application_status("error: %s" % alert.error.message())
            return

        status = alert.handle.status()
        name = str(status.info_hash)

        if status.has_metadata:
            torrent_file = alert.handle.torrent_file()
            name = torrent_file.name()

            save_torrent_file(torrent_file)

        pico_api.add_torrent(status)
        pico_api.set_application_status("%s added." % name)

    elif alert_type == "state_update_alert":
        torrents = {status.info_hash:status for status in alert.status}
        pico_api.update_torrents(torrents)


def post_updates():
    global is_running
    if not is_running: return

    # Restart timer and post our updates
    Timer(1.0, post_updates).start()
    session.post_torrent_updates()

def save_torrent_file(torrent_file):
    creator = lt.create_torrent(torrent_file)
    entry = creator.generate()
    data = lt.bencode(entry)
    torrent_hash = str(torrent_file.info_hash())
    torrent_path = os.path.join("torrents", torrent_hash + ".torrent")

    with open(torrent_path, "wb") as f:
        f.write(data)
        f.flush()
        os.fsync(f.fileno())


def load_session_state():
    if not os.path.isfile(".session_state"):
        return

    with open(".session_state", "rb") as f:
        state = lt.bdecode(f.read())
        session.load_state(state)

    pico_api.set_application_status("Loaded session state.")


def load_torrents():
    if not os.path.isdir("torrents"):
        return

    for torrent_file in glob.glob("torrents/*.torrent"):
        params = {}
        params["flags"] = default_flags
        params["save_path"] = "C:/Downloads"

        resume_file = os.path.splitext(torrent_file)[0] + ".resume"

        if os.path.isfile(resume_file):
            with open(resume_file, "rb") as f:
                params["resume_data"] = f.read()

        with open(torrent_file, "rb") as f:
            entry = lt.bdecode(f.read())
            params["ti"] = lt.torrent_info(entry)

        session.async_add_torrent(params)


def save_session_state():
    with open(".session_state", "wb") as f:
        entry = session.save_state()
        data = lt.bencode(entry)
        f.write(data)
        f.flush()
        os.fsync(f.fileno())


def save_torrents():
    session.pause()

    num_failed = 0
    num_paused = 0
    num_outstanding_requests = 0

    for torrent in session.get_torrents():
        status = torrent.status()

        if not torrent.is_valid():
            continue

        if not status.has_metadata:
            continue

        if not status.need_save_resume:
            continue

        torrent.save_resume_data()
        num_outstanding_requests += 1

    while num_outstanding_requests:
        if not session.wait_for_alert(10*1000):
            continue

        alerts = session.pop_alerts()

        for alert in alerts:
            alert_type = type(alert).__name__

            if alert_type == "torrent_paused_alert":
                num_paused += 1
                continue

            if alert_type == "save_resume_data_failed_alert":
                num_failed += 1
                num_outstanding_requests -= 1
                continue

            if not alert_type == "save_resume_data_alert":
                continue

            num_outstanding_requests -= 1

            if not alert.resume_data:
                continue

            torrent_hash = str(alert.handle.info_hash())
            resume_path = os.path.join("torrents", torrent_hash + ".resume")
            data = lt.bencode(alert.resume_data)

            with open(resume_path, "wb") as f:
                f.write(data)
                f.flush()
                os.fsync(f.fileno())

t = Thread(target=read_alerts)
t.daemon = True


def on_instance_already_running():
    """Called when an instance of PicoTorrent is already running.

    From here, we will try to connect to the existing PicoTorrent instance and
    send our command line arguments to it via a TCP socket which PicoTorrent
    binds to when on_load is called.

    """
    pass


def on_load():
    global session
    session = lt.session()
    session.set_alert_mask(lt.alert.category_t.all_categories)

    load_session_state()
    load_torrents()

    t.start()
    Timer(1.0, post_updates).start()

    pico_api.set_application_status("PicoTorrent loaded.")


def on_unload():
    global is_running
    is_running = False

    t.join()

    save_session_state()
    save_torrents()


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
