function downloadingActive(torrent) {
    return torrent.downloadPayloadRate > 0;
}

function uploadingActive(torrent) {
    return torrent.uploadPayloadRate > 0;
}

addFilter(downloadingActive, 'Downloading (active)');
addFilter(uploadingActive,   'Uploading (active)');
