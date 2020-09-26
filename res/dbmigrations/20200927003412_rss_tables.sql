CREATE TABLE rss_feed (
    id INTEGER PRIMARY KEY,
    url TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    updated_at INTEGER
);

CREATE TABLE rss_feed_filter (
    id INTEGER PRIMARY KEY,
    feed_id INTEGER NOT NULL,
    is_regex INTEGER NOT NULL,
    include_filter TEXT,
    exclude_filter TEXT,

    FOREIGN KEY(feed_id) REFERENCES rss_feed(id)
);
