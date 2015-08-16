import json
import picotorrent_api as pico_api
import urllib2
from threading import Thread
import webbrowser

releases_url = "https://api.github.com/repos/hadouken/hadouken/releases/latest"

def get_json(url):
    req = urllib2.Request(url)
    opener = urllib2.build_opener()
    f = opener.open(req)
    return json.loads(f.read())

def check_for_update():
    pico_api.set_application_status("Checking for updates...")

    release = get_json(releases_url)
    title = "PicoTorrent " + str(release["tag_name"]) + " available"
    message = "A new version of PicoTorrent is available."

    if pico_api.prompt(title + "\n\n" + message):
        webbrowser.open(release["html_url"], new=2)
