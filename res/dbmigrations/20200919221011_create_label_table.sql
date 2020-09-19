CREATE TABLE label (
    id        INTEGER PRIMARY KEY,
    name      TEXT NOT NULL,
    color     TEXT,
    save_path TEXT
);

/*
The torrent_label table maps a torrent to a label.
*/

CREATE TABLE torrent_label (
    label_id  INTEGER NOT NULL,
    info_hash TEXT    NOT NULL,

    FOREIGN KEY(label_id)  REFERENCES label(id),
    FOREIGN KEY(info_hash) REFERENCES torrent(info_hash),

    UNIQUE (label_id, info_hash)
);
