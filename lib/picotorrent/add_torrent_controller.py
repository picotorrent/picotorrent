"""
Controls the "Add torrents" dialog.
"""

import picotorrent_api as pico_api

class AddTorrentController(pico_api.AddTorrentController):
    def __init__(self, torrents):
        super().__init__()
        self._params = [ self._create_params(t) for t in torrents ]


    def _create_params(self, torrent):
        params = {}
        params["file_priorities"] = [ 1 for i in range(torrent.num_files()) ]
        params["save_path"] = "C:/Downloads"
        params["ti"] = torrent
        return params


    def _get_torrent(self, index):
        return self._params[index]["ti"]

    def perform_add(self):
        pass


    def get_name(self, torrent_index):
        return self._get_torrent(torrent_index).name()


    def get_comment(self, torrent_index):
        return self._get_torrent(torrent_index).comment()


    def get_creation_date(self, torrent_index):
        return "" # str(self._get_torrent(torrent_index).creation_date())


    def get_creator(self, torrent_index):
        return self._get_torrent(torrent_index).creator()


    def get_save_path(self, torrent_index):
        return self._params[torrent_index]["save_path"]


    def get_size(self, torrent_index):
        return str(self._get_torrent(torrent_index).total_size())


    def get_count(self):
        return len(self._params)


    def get_file_count(self, torrent_index):
        return self._get_torrent(torrent_index).num_files()


    def get_file_name(self, torrent_index, file_index):
        return self._get_torrent(torrent_index).file_at(file_index).path


    def get_file_size(self, torrent_index, file_index):
        return str(self._get_torrent(torrent_index).file_at(file_index).size)


    def get_file_priority(self, torrent_index, file_index):
        return self._params[torrent_index]["file_priorities"][file_index]


    def set_save_path(self, torrent_index, save_path):
        self._params[torrent_index]["save_path"] = save_path


    def set_file_name(self, torrent_index, file_index, file_name):
        self._get_torrent(torrent_index).rename_file(file_index, file_name)


    def set_file_priority(self, torrent_index, file_index, file_priority):
        self._params[torrent_index]["file_priorities"][file_index] = file_priority


    def to_friendly_priority(self, file_priority):
        return str(file_priority)


