var PicoTorrent = require("picotorrent");

PicoTorrent.on("load", function (app, session) {
    app.alert("Hello");
});
