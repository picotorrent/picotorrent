CREATE TABLE listen_interface (
    id      INTEGER PRIMARY KEY,
    address TEXT    NOT NULL    UNIQUE,
    port    TEXT
);
