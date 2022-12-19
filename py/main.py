"""
App logger

This script sets up a connection to the Blynk IoT platform using the BlynkLib library. 
It sets up event handlers for various events, such as when the connection is established or lost, and when the value of a virtual pin changes. 
It also includes a main function that runs an infinite loop to handle communication with the Blynk server.
"""


from logger import logging

import BlynkLib

from enums import Virtual as V
from secret import * # secret.py

__log__ = logging.getLogger(__name__)
    

blynk = BlynkLib.Blynk(auth=BLYNK_AUTH_TOKEN)


@blynk.on("connected")
def on_connect() -> None:
    """Event handler for when the connection to the Blynk server is established"""

    __log__.info("Connected")

@blynk.on("disconnected")
def on_disconnect() -> None:
    """Event handler for when the connection to the Blynk server is lost"""

    __log__.info("Disconnected")

@blynk.on("V*")
def on_pin_change(pin: int, values: list[str]) -> None:
    """Event handler for when the value of a virtual pin changes"""

    __log__.info(f"V{pin} value: {values[0]}")

def main() -> None:
    """Infinite loop to handle communication with the Blynk server"""

    while True:
        blynk.run()


if __name__ == "__main__":
    main()