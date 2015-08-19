import asyncio
import glob
import functools
import libtorrent as lt
import logging
import os, os.path
import picotorrent_api as pico_api

# Out logger.
logger = logging.getLogger(__name__)

default_flags = (lt.add_torrent_params_flags_t.flag_paused |
                 lt.add_torrent_params_flags_t.flag_auto_managed |
                 lt.add_torrent_params_flags_t.flag_update_subscribe |
                 lt.add_torrent_params_flags_t.flag_apply_ip_filter)


class SessionManager(object):
    _is_running = False
    _loop = None
    _session = None

    def __init__(self, loop):
        self._loop = loop

    def load(self):
        logger.debug("load")

        self._session = lt.session()
        self._session.set_alert_mask(lt.alert.category_t.all_categories)

        # Load state and torrents
        self._load_session_state()
        self._load_torrents()

        # Listen
        self._session.listen_on(6881, 6889)

        # Start our scheduled event calls
        logger.debug("Scheduling calls.")

        self._is_running = True
        self._loop.call_soon(self._read_alerts)
        self._loop.call_later(1.0, self._post_updates)


    def unload(self):
        logger.debug("unload")

        self._is_running = False

        self._save_session_state()
        self._save_torrents()


    def _load_session_state(self):
        if not os.path.isfile(".session_state"):
            logger.debug("File '.session_state' does not exist.")
            return

        with open(".session_state", "rb") as f:
            state = lt.bdecode(f.read())
            self._session.load_state(state)

        logger.info("Session state loaded.")


    def _load_torrents(self):
        if not os.path.isdir("torrents"):
            logger.debug("Directory 'torrents' does not exist.")
            return

        for torrent_file in glob.glob("torrents/*.torrent"):
            params = {}
            params["flags"] = default_flags
            params["save_path"] = "C:/Downloads"

            resume_file = os.path.splitext(torrent_file)[0] + ".resume"

            if os.path.isfile(resume_file):
                logger.debug("Reading resume file '%s'.", resume_file)

                with open(resume_file, "rb") as f:
                    params["resume_data"] = f.read()

            with open(torrent_file, "rb") as f:
                entry = lt.bdecode(f.read())
                params["ti"] = lt.torrent_info(entry)

            logger.debug("Adding '%s' 'torrents' directory.", torrent_file)
            self._session.async_add_torrent(params)


    def _save_session_state(self):
        logger.debug("Saving session state to file '.session_state'.")

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

        logger.debug("Saving state for %d torrents.", num_outstanding_requests)

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

                logger.debug("Writing resume data for torrent '%s' to '%s'.", torrent_hash, resume_path)

                with open(resume_path, "wb") as f:
                    f.write(data)
                    f.flush()
                    os.fsync(f.fileno())


    def _read_alerts(self):
        logger.debug("_read_alerts (is_running: %r)", self._is_running)

        if not self._is_running:
            return

        alert = yield from self._loop.run_in_executor(None, self._wait_for_alert)

        if alert:
            logger.debug("alert!!")
            alerts = self._session.pop_alerts()
            self._handle_alerts(alerts)

        self._loop.call_soon(self._read_alerts)


    def _wait_for_alert(self):
        return self._session.wait_for_alert(250)


    def _handle_alerts(self, alerts):
        for alert in alerts:
            try:
                self._handle_alert(alert)
            except Exception as e:
                logger.error("Could not handle alert '%s': %s.", type(alert).__name__, e)


    def _handle_alert(self, alert):
        alert_type = type(alert).__name__
        logger.debug("alert: %s: %s", alert_type, alert.message())

        if alert_type == "add_torrent_alert":
            if alert.error.value():
                logger.error("Could not add torrent, error %d: %s.", alert.error.value(), alert.error.message())
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
            if not alert.status:
                return

            torrents = { status.info_hash : status for status in alert.status }
            pico_api.update_torrents(torrents)


    def _save_torrent_file(self, torrent_file):
        creator = lt.create_torrent(torrent_file)
        entry = creator.generate()
        data = lt.bencode(entry)
        torrent_hash = str(torrent_file.info_hash())
        torrent_path = os.path.join("torrents", torrent_hash + ".torrent")

        if os.path.isfile(torrent_path):
            return

        logger.debug("Saving torrent file to '%s'.", torrent_path)

        with open(torrent_path, "wb") as f:
            f.write(data)
            f.flush()
            os.fsync(f.fileno())


    def _post_updates(self):
        if not self._is_running: return
        
        # Re-schedule the call, then post the updates.
        self._loop.call_later(1.0, self._post_updates)
        self._session.post_torrent_updates()
