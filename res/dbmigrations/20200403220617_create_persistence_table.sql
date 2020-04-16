CREATE TABLE persistent_object (
    key       TEXT PRIMARY KEY,
    value     TEXT    NOT NULL,

    UNIQUE (key) ON CONFLICT REPLACE
);
