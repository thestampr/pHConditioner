from enum import Enum
    
class Virtual(str, Enum):
    WORKER = "V0"
    TEMPERATURE = "V4"
    PH = "V5"
    PH_TARGET = "V6"
    PROCESS_PERCENY = "V7"

    def __str__(self) -> str:
        return str.__str__(self)