import logging

import blynklib

from enums import Virtual as V
from secret import * # secret.py

import time

SERVER = "blynk.cloud"
__log__ = logging.getLogger(__name__)


def read(virtual: V) -> str:
    return f"read {virtual}"

def write(virtual: V) -> str:
    return f"write {virtual}"
    


blynk = blynklib.Blynk(BLYNK_AUTH_TOKEN, server=SERVER)

@blynk.handle_event("connect")
def connect_handler():
    __log__.info("Connected")

@blynk.handle_event("disconnect")
def disconnect_handler():
    __log__.info("Disconnected")

@blynk.handle_event(write(V.PROCESS_PERCENT))
def on_start_switch(pin, value) -> None:
    print(pin, value)


print(write(V.PROCESS_PERCENT))

def main() -> None:
    blynk.run()
    blynk.virtual_write(V.WORKER, 1)
    while True:
        blynk.run()

if __name__ == "__main__":
    main()