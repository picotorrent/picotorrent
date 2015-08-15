import picotorrent_api as pico_api
from threading import Thread

import update_checker

def on_load():
    session = pico_api.get_session()
    pico_api.set_application_status("Listen port: " + str(session.listen_port()))

    t = Thread(target=update_checker.check_for_update)
    t.daemon = True
    #t.start()
