/* Insert our new columns for the enhanced table */
ALTER TABLE setting ADD COLUMN value         TEXT;
ALTER TABLE setting ADD COLUMN default_value TEXT;

/* Move all settings to the new columns. Also JSON stringify them */
UPDATE setting SET value = '"' || (SELECT REPLACE(string_value, '\', '\\') FROM setting WHERE key = 'default_save_path') || '"', default_value = '"' || REPLACE(get_known_folder_path('FOLDERID_Downloads'), '\', '\\') || '"' WHERE key = 'default_save_path';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'language_id'), default_value = get_user_default_ui_language() WHERE key = 'language_id';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'move_completed_downloads'), default_value = 'false' WHERE key = 'move_completed_downloads';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'move_completed_downloads_path') || '"' WHERE key = 'move_completed_downloads_path';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'move_completed_downloads_from_default_only'), default_value = 'false' WHERE key = 'move_completed_downloads_from_default_only';

UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'proxy_type'), default_value = '0' WHERE key = 'proxy_type';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'proxy_host') || '"' WHERE key = 'proxy_host';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'proxy_port') WHERE key = 'proxy_port';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'proxy_username') || '"' WHERE key = 'proxy_username';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'proxy_password') || '"' WHERE key = 'proxy_password';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'proxy_hostnames'), default_value = 'false' WHERE key = 'proxy_hostnames';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'proxy_peers'), default_value = 'false' WHERE key = 'proxy_peers';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'proxy_trackers'), default_value = 'false' WHERE key = 'proxy_trackers';

UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'start_position'), default_value = '0' WHERE key = 'start_position';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'skip_add_torrent_dialog'), default_value = 'false' WHERE key = 'skip_add_torrent_dialog';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'show_in_notification_area'), default_value = 'true' WHERE key = 'show_in_notification_area';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'close_to_notification_area'), default_value = 'false' WHERE key = 'close_to_notification_area';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'minimize_to_notification_area'), default_value = 'false' WHERE key = 'minimize_to_notification_area';

UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_checking'), default_value = '1' WHERE key = 'active_checking';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_dht_limit'), default_value = '80' WHERE key = 'active_dht_limit';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_downloads'), default_value = '3' WHERE key = 'active_downloads';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_limit'), default_value = '15' WHERE key = 'active_limit';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_lsd_limit'), default_value = '60' WHERE key = 'active_lsd_limit';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_seeds'), default_value = '5' WHERE key = 'active_seeds';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'active_tracker_limit'), default_value = '1600' WHERE key = 'active_tracker_limit';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'anonymous_mode'), default_value = 'false' WHERE key = 'anonymous_mode';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'enable_dht'), default_value = 'true' WHERE key = 'enable_dht';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'enable_lsd'), default_value = 'true' WHERE key = 'enable_lsd';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'enable_pex'), default_value = 'true' WHERE key = 'enable_pex';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'enable_download_rate_limit'), default_value = 'false' WHERE key = 'enable_download_rate_limit';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'enable_upload_rate_limit'), default_value = 'false' WHERE key = 'enable_upload_rate_limit';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'download_rate_limit'), default_value = '1024' WHERE key = 'download_rate_limit';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'upload_rate_limit'), default_value = '1024' WHERE key = 'upload_rate_limit';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'pause_on_low_disk_space'), default_value = 'false' WHERE key = 'pause_on_low_disk_space';
UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'pause_on_low_disk_space_limit'), default_value = '5' WHERE key = 'pause_on_low_disk_space_limit';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'require_incoming_encryption'), default_value = 'false' WHERE key = 'require_incoming_encryption';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'require_outgoing_encryption'), default_value = 'false' WHERE key = 'require_outgoing_encryption';

UPDATE setting SET value = (SELECT int_value FROM setting WHERE key = 'stop_tracker_timeout'), default_value = '5' WHERE key = 'stop_tracker_timeout';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'update_checks.enabled'), default_value = 'true' WHERE key = 'update_checks.enabled';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'update_checks.ignored_version') || '"' WHERE key = 'update_checks.ignored_version';
UPDATE setting SET value = '"' || (SELECT string_value FROM setting WHERE key = 'update_checks.url') || '"', default_value = '"https://api.picotorrent.org/releases/latest"' WHERE key = 'update_checks.url';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'ui.show_details_panel'), default_value = 'true' WHERE key = 'ui.show_details_panel';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'ui.show_status_bar'), default_value = 'true' WHERE key = 'ui.show_status_bar';

UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'announce_to_all_tiers'), default_value = 'true' WHERE key = 'announce_to_all_tiers';
UPDATE setting SET value = (SELECT CASE WHEN int_value = 1 THEN 'true' ELSE 'false' END FROM setting WHERE key = 'announce_to_all_trackers'), default_value = 'false' WHERE key = 'announce_to_all_trackers';

/* Remove unused settings */
DELETE FROM setting WHERE key IN(
    'encrypt_config_file',
    'geoip.database_url',
    'geoip.enabled',
    'proxy_force',
    'ui.widgets.main_window.height',
    'ui.widgets.main_window.width',
    'ui.widgets.splitter.sizes');

/* Rename old setting table */
ALTER TABLE setting RENAME TO setting_old;

/* Create new setting table since SQLite does not support removing cols */
CREATE TABLE setting (
    id            INTEGER PRIMARY KEY,
    key           TEXT    NOT NULL    UNIQUE,
    value         TEXT,
    default_value TEXT
);

INSERT INTO setting (key,value,default_value) SELECT key,value,default_value FROM setting_old;

DROP TABLE setting_old;

/* Rename settings - add libtorrent prefix to all settings which is destined for libtorrent */
UPDATE setting SET key = 'libtorrent.enable_dht' WHERE key = 'enable_dht';
UPDATE setting SET key = 'libtorrent.enable_lsd' WHERE key = 'enable_lsd';
UPDATE setting SET key = 'libtorrent.enable_pex' WHERE key = 'enable_pex';

UPDATE setting SET key = 'libtorrent.active_checking' WHERE key = 'active_checking';
UPDATE setting SET key = 'libtorrent.active_dht_limit' WHERE key = 'active_dht_limit';
UPDATE setting SET key = 'libtorrent.active_downloads' WHERE key = 'active_downloads';
UPDATE setting SET key = 'libtorrent.active_limit' WHERE key = 'active_limit';
UPDATE setting SET key = 'libtorrent.active_lsd_limit' WHERE key = 'active_lsd_limit';
UPDATE setting SET key = 'libtorrent.active_seeds' WHERE key = 'active_seeds';
UPDATE setting SET key = 'libtorrent.active_tracker_limit' WHERE key = 'active_tracker_limit';

UPDATE setting SET key = 'libtorrent.announce_to_all_tiers' WHERE key = 'announce_to_all_tiers';
UPDATE setting SET key = 'libtorrent.announce_to_all_trackers' WHERE key = 'announce_to_all_trackers';

UPDATE setting SET key = 'libtorrent.require_incoming_encryption' WHERE key = 'require_incoming_encryption';
UPDATE setting SET key = 'libtorrent.require_outgoing_encryption' WHERE key = 'require_outgoing_encryption';

UPDATE setting SET key = 'libtorrent.anonymous_mode' WHERE key = 'anonymous_mode';
UPDATE setting SET key = 'libtorrent.stop_tracker_timeout' WHERE key = 'stop_tracker_timeout';
UPDATE setting SET key = 'libtorrent.download_rate_limit' WHERE key = 'download_rate_limit';
UPDATE setting SET key = 'libtorrent.enable_download_rate_limit' WHERE key = 'enable_download_rate_limit';
UPDATE setting SET key = 'libtorrent.upload_rate_limit' WHERE key = 'upload_rate_limit';
UPDATE setting SET key = 'libtorrent.enable_upload_rate_limit' WHERE key = 'enable_upload_rate_limit';

UPDATE setting SET key = 'libtorrent.proxy_type' WHERE key = 'proxy_type';
UPDATE setting SET key = 'libtorrent.proxy_host' WHERE key = 'proxy_host';
UPDATE setting SET key = 'libtorrent.proxy_port' WHERE key = 'proxy_port';
UPDATE setting SET key = 'libtorrent.proxy_username' WHERE key = 'proxy_username';
UPDATE setting SET key = 'libtorrent.proxy_password' WHERE key = 'proxy_password';
UPDATE setting SET key = 'libtorrent.proxy_hostnames' WHERE key = 'proxy_hostnames';
UPDATE setting SET key = 'libtorrent.proxy_peers' WHERE key = 'proxy_peers';
UPDATE setting SET key = 'libtorrent.proxy_trackers' WHERE key = 'proxy_trackers';
