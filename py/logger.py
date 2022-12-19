import logging
import os

LOG_FILE = "log.log"

if os.path.exists(LOG_FILE):
    os.remove(LOG_FILE)

__mute__: list[str] = []

__show__: list[str] = []

for name in __mute__:
    if name.startswith(tuple(__show__)):
        continue
    else:
        logging.getLogger(name).setLevel(logging.CRITICAL)


logFormatter = logging.Formatter("%(asctime)s %(levelname)s: %(message)s", datefmt="%H:%M:%S")
rootLogger = logging.getLogger()

fileHandler = logging.FileHandler(LOG_FILE)
fileHandler.setFormatter(logFormatter)
rootLogger.addHandler(fileHandler)

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(logFormatter)
rootLogger.addHandler(consoleHandler)

rootLogger.setLevel(logging.INFO)