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

/* Setting to toggle background color in list view */
INSERT INTO setting (key, value, default_value)
VALUES ('use_label_as_list_bgcolor', NULL, 'false');
