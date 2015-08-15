from threading import Thread
import update_checker

def on_load():
    t = Thread(target=update_checker.check_for_update)
    t.daemon = True
    t.start()
