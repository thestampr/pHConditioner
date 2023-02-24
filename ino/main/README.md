# Main source code

Board : `Nodemcu v3`

# 

### create `secret.h` in `utils` and should contain:

```ino
// Secret **don't share

#pragma once

// default wifi ssid
#define SSID "your ssid"
#define PASS "your password"

// Blynk
#define BLYNK_TEMPLATE_ID "your_BLYNK_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "your_BLYNK_DEVICE_NAME"
#define BLYNK_AUTH_TOKEN "your_BLYNK_AUTH_TOKEN"
```

Be sure to keep the `secret.h` file in a secure location and do not share it with others.