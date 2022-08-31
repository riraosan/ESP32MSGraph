/*
ESP32MSGraph Library

Original Source:
https://github.com/toblum/ESPTeamsPresence

Licence:
[MIT](https://github.com/riraosan/ESP32MSGraph/blob/master/LICENSE)

Author:
refactored by
[riraosan_0901](https://twitter.com/riraosan_0901)
[riraosan](https://github.com/riraosan)

Contributors:
[toblum](https://github.com/toblum)
*/

#pragma once

// Global settings
//#define DISABLECERTCHECK                  1                // Uncomment to disable https certificate checks (if not set via build flags)
//#define STATUS_PIN                        LED_BUILTIN      // User builtin LED for status (if not set via build flags)
#define DATAPIN                           27               // GPIO pin used to drive the LED strip (20 == GPIO/D13) (if not set via build flags)
#define NUMLEDS                           25               // Number of LEDs on the strip (if not set via build flags)
#define DEFAULT_POLLING_PRESENCE_INTERVAL "30"             // Default interval to poll for presence info (seconds)
#define DEFAULT_ERROR_RETRY_INTERVAL      30               // Default interval to try again after errors
#define TOKEN_REFRESH_TIMEOUT             60               // Number of seconds until expiration before token gets refreshed
#define CONTEXT_FILE                      "/context.json"  // Filename of the context file
#define VERSION                           "0.0.1"          // Version of the software
