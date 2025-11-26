# ESP8266 WiFi Configuration

## Setup Instructions

Before uploading to ESP8266, update the following in `esp_wifi.ino`:

```cpp
#define WLAN_SSID "Your_WiFi_Name"
#define WLAN_PASS "Your_WiFi_Password"
#define AIO_USERNAME "Your_Adafruit_Username"
#define AIO_KEY "Your_Adafruit_Key"
```

**DO NOT commit your actual credentials to GitHub!**

## How to Get Adafruit Credentials

1. Go to https://io.adafruit.com/
2. Click on "My Key" (yellow key icon)
3. Copy your Username and Active Key
4. Replace the placeholders in the code

## MQTT Feed

Temperature feed: `<your_username>/feeds/lab8-temp`
