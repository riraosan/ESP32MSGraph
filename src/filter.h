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

#include <Arduino.h>

constexpr char loginFilter[] = R"(
{
  "user_code"        : true,
  "device_code"      : true,
  "verification_uri" : true,
  "expires_in"       : true,
  "interval"         : true,
  "message"          : true
}
)";

constexpr char refleshtokenFilter[] = R"(
{
  "token_type"     : true,
  "scope"          : true,
  "expires_in"     : true,
  "ext_expires_in" : true,
  "access_token"   : true,
  "refresh_token"  : true,
  "id_token"       : true
}
)";

constexpr char presenceFilter[] = R"(
{
  "id"           : true,
  "availability" : true,
  "activity"     : true"
}
)";

// TODO Please add User filter below.
