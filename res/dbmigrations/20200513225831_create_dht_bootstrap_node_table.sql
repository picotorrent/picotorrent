CREATE TABLE dht_bootstrap_node (
    id       INTEGER PRIMARY KEY,
    hostname TEXT    NOT NULL    UNIQUE,
    port     INTEGER NOT NULL
);

INSERT INTO dht_bootstrap_node (hostname, port)
    VALUES
        ('router.bittorrent.com',  6881),
        ('router.utorrent.com',    6881),
        ('dht.transmissionbt.com', 6881),
        ('dht.aelitis.com',        6881);
