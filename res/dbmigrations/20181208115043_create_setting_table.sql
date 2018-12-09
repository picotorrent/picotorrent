create table setting (
    id           INTEGER PRIMARY KEY       ,
    key          TEXT    NOT NULL    UNIQUE,
    int_value    INTEGER,
    string_value TEXT
);
