CREATE TABLE label (
    id                   INTEGER PRIMARY KEY,
    name                 TEXT    NOT NULL,
    color                TEXT,
    color_enabled        INTEGER NOT NULL,
    save_path            TEXT,
    save_path_enabled    INTEGER NOT NULL,
    apply_filter         TEXT,
    apply_filter_enabled INTEGER NOT NULL
);

/* Add label_id column to torrent table */
ALTER TABLE torrent ADD COLUMN label_id INTEGER REFERENCES label(id);
