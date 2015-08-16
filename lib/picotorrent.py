import glob
import libtorrent as lt
import os, os.path
import picotorrent_api as pico_api
from threading import Thread
import update_checker

session = lt.session()
session.set_alert_mask(lt.alert.category_t.all_categories)

is_running = False

default_flags = (lt.add_torrent_params_flags_t.flag_paused |
                 lt.add_torrent_params_flags_t.flag_auto_managed |
                 lt.add_torrent_params_flags_t.flag_update_subscribe |
                 lt.add_torrent_params_flags_t.flag_apply_ip_filter)

def read_alerts():
    global is_running
    is_running = True

    while is_running:
        if not session.wait_for_alert(1000):
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

def on_load():
    load_session_state()
    load_torrents()

    t.start()

    pico_api.set_application_status("PicoTorrent loaded.")


def on_unload():
    global is_running
    is_running = False

    t.join()

    save_session_state()
    save_torrents()
