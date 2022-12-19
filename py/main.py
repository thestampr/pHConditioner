"""App logger"""


from logger import logging

import BlynkLib

from enums import Virtual as V
from secret import * # secret.py

__log__ = logging.getLogger(__name__)
    

blynk = BlynkLib.Blynk(auth=BLYNK_AUTH_TOKEN)

@blynk.on("connected")
def on_connect():
    __log__.info("Connected")

@blynk.on("disconnected")
def on_disconnect():
    __log__.info("Disconnected")

@blynk.on("V*")
def on_pin_change(pin: int, values: list[str]) -> None:
    __log__.info(f"V{pin} value: {values[0]}")

def main() -> None:
    while True:
        blynk.run()

if __name__ == "__main__":
    main()