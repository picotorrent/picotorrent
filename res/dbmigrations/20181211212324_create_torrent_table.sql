CREATE TABLE torrent (
    info_hash      TEXT    PRIMARY KEY,
    queue_position INTEGER NOT NULL
);
