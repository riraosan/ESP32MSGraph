# ESP32_8BIT_CVBS AnimatedGIF Sample

## Summary

This program is a sample of AnimatedGIF Library.
You can play "[Mobile Suit NON-chan](https://nosferatunon.wixsite.com/nonchan/kn-non)" Animated GIF and BGM on your Digital TV (composite).

## Devices

I have tested this sample program with the following devices.

- [ATOM Lite ESP32 IoT Development Kit](https://www.switch-science.com/catalog/6262/)
- [ATOM TF-Card Reader Development Kit up to 16GB](https://www.switch-science.com/catalog/6475/)
- [ATOM Speaker Kit (NS4168)](https://www.switch-science.com/catalog/7092/)
- microSD 16M(TF-CARD)
- External Digital Analog Converter(DAC Bit per sample is 32bit or 16bit)
  - NS4168(Bit per sample 16bit)
  - ES9038MQ2M(Bit per sample 32bi)

## How to build

This sample has been tested to build only in the PlatformIO IDE environment.
We have not checked if it can be built in the Arduino IDE environment. If you know how to build it, we would appreciate it if you commit it to this repository.

If you want to build in Arduino IDE environment, please change the setting of platformio.ini for Arduino IDE environment. We believe that the settings will probably be almost the same.

### Library's

I checked with arduino-esp32 library version. If you use arduino-esp32 library of series 2, you may step on compile error or operation bug. Please be aware of this.
> For example, the arduino-esp32's I2S library has different names for the constant definitions to be set in the driver between Series 1 and Series 2. I didn't notice the difference and got stuck for a while.

```yaml
[arduino-esp32]
platform          = platformio/espressif32@^3.5.0
```

I have put the GitHub link to the library under the `lib_deps =` directive. You can download the library from GitHub yourself and register it with the Arduino IDE.

```yaml
lib_deps =
        https://github.com/m5stack/M5Atom.git
        https://github.com/FastLED/FastLED.git
        https://github.com/bitbank2/AnimatedGIF.git#1.4.7
        https://github.com/m5stack/M5Unified.git#0.0.7
        https://github.com/riraosan/ESP_8_BIT_composite.git ;for ESP32_8BIT_CVBS
        https://github.com/earlephilhower/ESP8266Audio.git#1.9.5
        https://github.com/LennartHennigs/Button2.git
```

> **Note**
> 
> You must use the ESP_8_BIT_composite library modified by @riraosan.
> - Modified to switch between G25 and G26 outputs
> - Modified to switch between double buffer and single buffer.

### Composite(CVBS) Signal Output

- [x] If you want the composite video output port to be G26, please include `ENABLE_GPIO26` in the build flags. The default output port is G25.

```yaml:platformio.ini
build_flags =
        -D ENABLE_GPIO26
````
## How to use

- You must somehow connect the G25 (or G26) port to the composite input of your digital TV. (Instructions are omitted.)
- Turn on the ATOM Lite. The digital TV screen will display `Long Click: episode 5` on a black background.
- If you click the G39 button, ATOM Lite will play Mobile Suit NON-CHAN episode 5 "OSクラッシャー".

> **Note**
>
> I do not guarantee that it will work if you single press the G39 button (I will address this soon.)

## License

MIT License

Feel free to modify or reprint. We would appreciate it if you could reprint the URL of this repository.

## Acknowledgements

If I could look out over the distance, it was on the shoulders of giants.
We would like to thank the authors of each library. Thank you very much.

https://github.com/toblum/ESPTeamsPresence

## Conclusion

It would be a great pleasure and a blessing if I could contribute in some way to someone somewhere.

Enjoy!👍
