var fs = require("fs");

fs.readdir("./plugins", function (err, files) {
    if (err) {
        return;
    }

    console.log(files);
});
