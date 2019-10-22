CREATE TABLE log (
    id        INTEGER PRIMARY KEY,
    category  TEXT    NOT NULL,
    file      TEXT    NOT NULL,
    function  TEXT    NOT NULL,
    line      TEXT    NOT NULL,
    version   TEXT    NOT NULL,
    message   TEXT    NOT NULL,
    timestamp INTEGER NOT NULL
);
