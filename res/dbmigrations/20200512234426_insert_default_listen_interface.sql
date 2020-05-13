/*
Inserts the default listen interface. This is taken from the setting table.
*/

INSERT INTO listen_interface (address, port)

/* Select a single address (or 0.0.0.0 if {any}). */
          SELECT
              (SELECT CASE WHEN string_value = '{any}' THEN '0.0.0.0' ELSE string_value END FROM setting WHERE key = 'listen_interface') AS address,
              (SELECT int_value FROM setting WHERE key = 'listen_port') AS port
          WHERE INSTR(address, '|') = 0

/* Select IPv4 address from '|' concatenated string. */
UNION ALL SELECT
              (SELECT SUBSTR(string_value, 0, INSTR(string_value, '|')) FROM setting WHERE key = 'listen_interface') AS address,
              (SELECT int_value FROM setting WHERE key = 'listen_port') AS port
          WHERE (SELECT INSTR(string_value, '|') FROM setting WHERE key = 'listen_interface') > 0

/* Select IPv6 address from '|' concatenated string. */
UNION ALL SELECT
              (SELECT '[' || TRIM(SUBSTR(string_value, INSTR(string_value, '|')), '|') || ']' FROM setting WHERE key = 'listen_interface') AS address,
              (SELECT int_value FROM setting WHERE key = 'listen_port') AS port
          WHERE (SELECT INSTR(string_value, '|') FROM setting WHERE key = 'listen_interface') > 0

;
