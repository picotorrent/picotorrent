import sys

sys.path.insert(0, "C:/tmp/python27.zip")
sys.path.insert(1, "C:/tmp/python27_all/DLLs")

from threading import Thread
import update_checker

def on_load():
    t = Thread(target=update_checker.check_for_update)
    t.daemon = True
    t.start()
