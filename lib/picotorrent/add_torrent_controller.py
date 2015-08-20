"""
Controls the "Add torrents" dialog.
"""

import picotorrent_api as pico_api

class AddTorrentControllerFoo(pico_api.AddTorrentController):
    def perform_add(self):
        pass


    def get_name(self, torrent_index):
        return "torrent name"


    def get_comment(self, torrent_index):
        return "comment"


    def get_creation_date(self, torrent_index):
        return "some date"


    def get_creator(self, torrent_index):
        return "libtorrent"


    def get_save_path(self, torrent_index):
        return "C:/DOwnloads"


    def get_size(self, torrent_index):
        return "12324GB"


    def get_count(self):
        return 12


    def get_file_count(self, torrent_index):
        return 8


    def get_file_name(self, torrent_index, file_index):
        return "file name"


    def get_file_size(self, torrent_index, file_index):
        return "123 MB"


    def get_file_priority(self, torrent_index, file_index):
        return 1


    def set_save_path(self, torrent_index, save_path):
        pass


    def set_file_name(self, torrent_index, file_index, file_name):
        pass


    def set_file_priority(self, torrent_index, file_index, file_priority):
        pass


    def to_friendly_priority(self, file_priority):
        return "Friendly priority"


