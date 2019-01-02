CREATE TABLE session_state (
    id         INTEGER PRIMARY KEY,
    state_data BLOB    NOT NULL,
    timestamp  INTEGER NOT NULL
);
