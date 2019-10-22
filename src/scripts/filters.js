function downloadingActive(torrent) {
    return torrent.downloadPayloadRate > 0;
}

function uploadingActive(torrent) {
    return torrent.uploadPayloadRate > 0;
}

addFilter(downloadingActive, i18n('filter_downloading_active'));
addFilter(uploadingActive,   i18n('filter_uploading_active'));
