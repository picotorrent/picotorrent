"""
Sets up logging for PicoTorrent.
"""

import logging
import sys

logger = logging.getLogger(__name__)

def log_exception(type, value, traceback):
    logger.critical("Unhandled Python exception. PicoTorrent is unable to continue. Details: %s, %s, %s", type, value, traceback)
    sys.__excepthook__(type, value, traceback)


def log_asyncio_exception(loop, context):
    logger.critical("Error in asyncio loop: %s.", context["message"])

    if "exception" in context:
        logger.critical("%s", context["exception"])


def setup(loop):
    sys.excepthook = log_exception
    loop.set_exception_handler(log_asyncio_exception)

    logging.basicConfig(filename="PicoTorrent.log", level=logging.DEBUG)
    logging.getLogger(__name__).info("Initialized logging.")
