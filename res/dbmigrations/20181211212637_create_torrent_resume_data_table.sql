CREATE TABLE torrent_resume_data (
    info_hash   TEXT PRIMARY KEY,
    resume_data BLOB NOT NULL,

    FOREIGN KEY(info_hash) REFERENCES torrent(info_hash)
);
