import glob
import libtorrent as lt
import os, os.path
import picotorrent_api as pico_api
from threading import Thread, Timer

default_flags = (lt.add_torrent_params_flags_t.flag_paused |
                 lt.add_torrent_params_flags_t.flag_auto_managed |
                 lt.add_torrent_params_flags_t.flag_update_subscribe |
                 lt.add_torrent_params_flags_t.flag_apply_ip_filter)

class SessionManager(object):
    _is_running = False
    _session = None
    _thread = None

    def __init__(self):
        self._thread = Thread(target=self._read_alerts)
        self._thread.daemon = True

    def load(self):
        self._session = lt.session()
        self._session.set_alert_mask(lt.alert.category_t.all_categories)

        # Load state and torrents
        self._load_session_state()
        self._load_torrents()

        self._is_running = True
        self._thread.start()

        Timer(1.0, self._post_updates).start()


    def unload():
        self._is_running = False
        self._thread.join()
        
        self._save_session_state()
        self._save_torrents()


    def _load_session_state(self):
        if not os.path.isfile(".session_state"):
            return

        with open(".session_state", "rb") as f:
            state = lt.bdecode(f.read())
            self._session.load_state(state)

        # TODO send this to log instead.
        pico_api.set_application_status("Loaded session state.")


    def _load_torrents(self):
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

            self._session.async_add_torrent(params)


    def _save_session_state(self):
        with open(".session_state", "wb") as f:
            entry = self._session.save_state()
            data = lt.bencode(entry)
            f.write(data)
            f.flush()
            os.fsync(f.fileno())


    def _save_torrents(self):
        self._session.pause()

        num_failed = 0
        num_paused = 0
        num_outstanding_requests = 0

        for torrent in self._session.get_torrents():
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
            if not self._session.wait_for_alert(10*1000):
                continue

            alerts = self._session.pop_alerts()

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


    def _read_alerts(self):
        while self._is_running:
            # Wait for an alert for 250ms, then restart the wait.
            if not self._session.wait_for_alert(250):
                continue

            for alert in self._session.pop_alerts():
                try:
                    self._handle_alert(alert)
                except Exception, e:
                    raise e


    def _handle_alert(self, alert):
        alert_type = type(alert).__name__

        if alert_type == "add_torrent_alert":
            if alert.error.value():
                # TOdo implement logging and error handling
                # ie. notify user.
                return

            status = alert.handle.status()
            name = str(status.info_hash)

            if status.has_metadata:
                torrent_file = alert.handle.torrent_file()
                name = torrent_file.name()
                
                # Save torrent to disk.
                self._save_torrent_file(torrent_file)

            pico_api.add_torrent(status)
            pico_api.set_application_status("%s added" % name)

        elif alert_type == "state_update_alert":
            torrents = { status.info_hash : status for status in alert.status }
            pico_api.update_torrents(torrents)


    def _save_torrent_file(self, torrent_file):
        creator = lt.create_torrent(torrent_file)
        entry = creator.generate()
        data = lt.bencode(entry)
        torrent_hash = str(torrent_file.info_hash())
        torrent_path = os.path.join("torrents", torrent_hash + ".torrent")

        with open(torrent_path, "wb") as f:
            f.write(data)
            f.flush()
            os.fsync(f.fileno())


    def _post_updates(self):
        if not self._is_running: return

        # Restart timer and post our updates
        Timer(1.0, self._post_updates).start()
        self._session.post_torrent_updates()
