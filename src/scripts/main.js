const i18n = require('i18n');
const on = require('on');

function downloadingActive(torrent) {
    return torrent.downloadPayloadRate > 0;
}

function uploadingActive(torrent) {
    return torrent.uploadPayloadRate > 0;
}

on('load', function load(app) {
    app.addFilter(downloadingActive, i18n('filter_downloading_active'));
    app.addFilter(uploadingActive,   i18n('filter_uploading_active'));
});
