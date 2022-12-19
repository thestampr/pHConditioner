from enum import Enum
    
class Virtual(str, Enum):
    WORKER = "V0"
    TEMPERATURE = "V1"
    PH = "V2"
    PH_TARGET = "V3"
    PROCESS_PERCENT = "V4"

    def __str__(self) -> str:
        return str.__str__(self)