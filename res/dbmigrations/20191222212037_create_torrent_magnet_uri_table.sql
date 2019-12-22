CREATE TABLE torrent_magnet_uri (
    info_hash   TEXT PRIMARY KEY,
    magnet_uri  TEXT NOT NULL,
    save_path   TEXT NOT NULL,

    FOREIGN KEY(info_hash) REFERENCES torrent(info_hash)
);
