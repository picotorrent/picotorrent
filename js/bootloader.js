(function () {
    Duktape.modSearch = function (id, require, exports, module) {
        var found = false;
        var src;

        if (requireNative(id, require, exports, module)) {
            found = true;
        }

        if (!found) {
            throw new Error("Could not find module '" + id + "'.");
        }
    };
})();

var fs = require("fs");
fs.readdir("./plugins", function (err, files) {
    if (err) {
        return;
    }

    console.log(files);
});
