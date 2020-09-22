CREATE TABLE label (
    id           INTEGER PRIMARY KEY,
    name         TEXT    NOT NULL,
    color        TEXT,
    save_path    TEXT,
    apply_filter TEXT
);

/* Add label_id column to torrent table */
ALTER TABLE torrent ADD COLUMN label_id INTEGER REFERENCES label(id);
