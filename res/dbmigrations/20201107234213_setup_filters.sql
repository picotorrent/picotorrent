INSERT INTO setting (key, value, default_value)
VALUES ('current_filter', NULL, NULL);

CREATE TABLE filter (
    id     INTEGER PRIMARY KEY,
    name   TEXT    NOT NULL,
    filter TEXT    NOT NULL
);

INSERT INTO filter (name, filter) VALUES
('Downloading (active)', 'status = "downloading" and dl > 1kbps'),
('Uploading (active)',   'status = "uploading" and ul > 1kbps');
