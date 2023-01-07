# Main source code

Board : `Nodemcu v3`

# 

### create `secret.h` and should contain:

```ino
// Secret **don't share

#ifndef SECRET_H
#define SECRET_H

// Wifi
#define SSID "your_wifi_ssid"
#define PASS "your_wifi_password"

// Blynk
#define BLYNK_TEMPLATE_ID "your_BLYNK_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "your_BLYNK_DEVICE_NAME"
#define BLYNK_AUTH_TOKEN "your_BLYNK_AUTH_TOKEN"

#endif // SECRET_H
```

Be sure to keep the `secret.h` file in a secure location and do not share it with others.