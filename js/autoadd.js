var fs          = require("fs");
var PicoTorrent = require("picotorrent");

fs.watch("C:/Torrents", function (event, filename) {
    console.log(event);
});
