# ESP32MSGraph Library

in progress.

## Summary

This library provides an easy login process to AzureAD. I had also included a sample that displays Teams presence.

## Devices

M5Stack ATOM Lite / MATRIX and M5Stack series

## How to build

//TBD 

## How to use

0. Make Client ID with Permission in Azure AD.
1. Turn on the ESP32 power supply.
2. Connect to ATOM-G-AP with PC.
3. Connect ESP32 to any WiFi access point (AutoConnect).
4. Display the AutoConnect Home page of ESP32 from PC.
5. Select "Device Login Settings" from the top menu.
6. Enter Client ID and Tenant ID, then click the button.
7. Copy the user code on the next screen and Input the user code on the AzuruAD login Page.

## Library's

I have confirmed this library works with arduino-esp32 ver 2.0.3.

```yaml
[arduino-esp32]
platform          = platformio/espressif32@^4.4.0
```

I have put the GitHub link to the library under the `lib_deps =` directive. You can download the library from GitHub yourself and register it with the Arduino IDE.

```yaml
lib_deps =
        ;basic
        SPI
        SPIFFS
        SD
        FS
        Wire
        WiFi
        EEPROM
        ESPmDNS
        WebServer
        Update
        DNSServer
        HTTPClient
        WiFiClientSecure
        Ticker
        HTTPUpdate
        ;from GitHub
        https://github.com/bblanchon/ArduinoStreamUtils.git
        https://github.com/bblanchon/ArduinoJson.git#v6.19.4
        https://github.com/Hieromon/AutoConnect.git#v1.3.7
```

## License

MIT License

Feel free to modify or reprint. We would appreciate it if you could reprint the URL of this repository.

## Acknowledgements

If I could look out over the distance, it was on the shoulders of giants.
We would like to thank the authors of each library. Thank you very much.

<https://github.com/toblum/ESPTeamsPresence>

## Conclusion

It would be a great pleasure and a blessing if I could contribute in some way to someone somewhere.

Enjoy!👍
