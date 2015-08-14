import picotorrent_api as pico_api
import sys

sys.path.insert(0, "C:/tmp/python27.zip")
sys.path.insert(1, "C:/tmp/python27_all/DLLs")

import json
import urllib2
from threading import Thread

url = "https://api.github.com/repos/hadouken/hadouken/releases/latest"

def on_load():
    t = Thread(target=check_for_update)
    t.daemon = True
    t.start()

def check_for_update():
    pico_api.SetApplicationStatus("Checking for updates...")

    req = urllib2.Request(url)
    opener = urllib2.build_opener()
    f = opener.open(req)
    data = json.loads(f.read())

    pico_api.SetApplicationStatus(str(data["author"]["login"]))
