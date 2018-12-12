CREATE TABLE column_state (
    id         INTEGER PRIMARY KEY,
    list_id    TEXT    NOT NULL,
    column_id  INTEGER NOT NULL,
    width      INTEGER NOT NULL,
    is_visible INTEGER NOT NULL DEFAULT 1,
    position   INTEGER NOT NULL,

    UNIQUE (list_id, column_id) ON CONFLICT REPLACE
);
