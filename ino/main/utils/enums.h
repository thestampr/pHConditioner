// enums.h

#pragma once


struct BlynkState {
    const String connected = "connected";
    const String disconnected = "offline";
    const String restarting = "restarting...";
    const String resetting = "resetting...";
    const String working = "working";
    const String idle = "idle";
    const String done = "done";
};


BlynkState State;