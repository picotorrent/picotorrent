CREATE TABLE path_history (
    id        INTEGER PRIMARY KEY,
    path      TEXT    NOT NULL,
    type      TEXT    NOT NULL,
    timestamp INTEGER NOT NULL,

    UNIQUE (path, type) ON CONFLICT REPLACE
);
