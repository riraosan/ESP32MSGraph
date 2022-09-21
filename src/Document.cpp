/*
Document Library

Original Source:
https://github.com/toblum/ESPTeamsPresence

Licence:
[MIT](https://github.com/riraosan/Document/blob/master/LICENSE)

Author:
refactored by
[riraosan_0901](https://twitter.com/riraosan_0901)
[riraosan](https://github.com/riraosan)

Contributors:
[toblum](https://github.com/toblum)
*/

#include <memory>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <FS.h>
#include <SPIFFS.h>

#include <ArduinoJson.h>
#include <esp32-hal-log.h>

#include <config.h>
#include <filter.h>
#include <Document.h>

#ifndef DISABLECERTCHECK

// Tool to get certs: https://projects.petrucci.ch/esp32/

// certificate for https://login.microsoftonline.com
// DigiCert Global Root CA, Valid until: 10/Nov/2031
// From: https://www.digicert.com/kb/digicert-root-certificates.htm
constexpr char rootCACertificateLogin[] = R"(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)";

// certificate for https://graph.microsoft.com
// DigiCert Assured ID Root G2, Valid until: 15/Jan/2038
// From: https://www.digicert.com/kb/digicert-root-certificates.htm
constexpr char rootCACertificateGraph[] = R"(
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl
MrY=
-----END CERTIFICATE-----
)";
#endif

int Document::getTokenLifetime() {
  return (_expires - millis()) / 1000;
}

void Document::removeContext() {
  SPIFFS.remove(CONTEXT_FILE);
  log_d("removeContext() - Success");
}

/**
 * API request handler
 */
bool Document::requestJsonApi(JsonDocument& doc, ARDUINOJSON_NAMESPACE::Filter filter, String url, String payload, String type, bool sendAuth) {
  std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);

#ifndef DISABLECERTCHECK
  if (url.indexOf("graph.microsoft.com") > -1) {
    client->setCACert(rootCACertificateGraph);
  } else {
    client->setCACert(rootCACertificateLogin);
  }
#endif

  HTTPClient https;

  if (https.begin(*client, url)) {  // HTTPS
    https.setConnectTimeout(10000);
    https.setTimeout(10000);
    https.useHTTP10(true);

    // Send auth header?
    if (sendAuth) {
      String header = "Bearer " + _access_token;
      https.addHeader("Authorization", header);
      log_i("[HTTPS] Auth token valid for %d s.", getTokenLifetime());
    }

    // Start connection and send HTTP header
    int httpCode = 0;
    if (type == "POST") {
      httpCode = https.POST(payload);
    } else {
      httpCode = https.GET();
    }

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      // log_i("[HTTPS] Method: %s, Response code: %d", type.c_str(), httpCode);

      // File found at server (HTTP 200, 301), or HTTP 400 with response payload
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_BAD_REQUEST) {
        // Parse JSON data
        DeserializationError error = deserializeJson(doc, https.getStream(), filter);

        // serializeJsonPretty(doc, Serial);
        // Serial.println();

        if (error) {
          log_e("deserializeJson() failed: %s", error.c_str());
          https.end();
          return false;
        } else {
          log_i("deserializeJson() Success: %s", error.c_str());
          https.end();
          return true;
        }
      } else {
        log_e("[HTTPS] Other HTTP code: %d", httpCode);
        https.end();
        return false;
      }
    } else {
      log_e("[HTTPS] Request failed: %s", https.errorToString(httpCode).c_str());
      https.end();
      return false;
    }
  } else {
    log_e("[HTTPS] can't begin().");
    return false;
  }

  return false;
}

void Document::handleGetSettings() {
  // log_d("handleGetSettings()");

  // const int capacity = JSON_OBJECT_SIZE(13);

  // StaticJsonDocument<capacity> responseDoc;
  // responseDoc["client_id"].set(_paramClientIdValue);
  // responseDoc["tenant"].set(_paramTenantValue);
  // responseDoc["poll_interval"].set(_paramPollIntervalValue);
  // responseDoc["num_leds"].set(_paramNumLedsValue);

  // responseDoc["heap"].set(ESP.getFreeHeap());
  // responseDoc["min_heap"].set(ESP.getMinFreeHeap());
  // responseDoc["sketch_size"].set(ESP.getSketchSize());
  // responseDoc["free_sketch_space"].set(ESP.getFreeSketchSpace());
  // responseDoc["flash_chip_size"].set(ESP.getFlashChipSize());
  // responseDoc["flash_chip_speed"].set(ESP.getFlashChipSpeed());
  // responseDoc["sdk_version"].set(ESP.getSdkVersion());
  // responseDoc["cpu_freq"].set(ESP.getCpuFreqMHz());

  // responseDoc["sketch_version"].set(VERSION);

  // serializeJsonPretty(responseDoc, Serial);

  // _server->send(200, "application/json", responseDoc.as<String>());
}

// Delete EEPROM by removing the trailing sequence, remove context file
void Document::handleClearSettings() {
  log_d("handleClearSettings()");

  for (int t = 0; t < 4; t++) {
    EEPROM.write(t, 0);
  }
  EEPROM.commit();
  removeContext();

  //_server->send(200, "application/json", "{\"action\": \"clear_settings\", \"error\": false}");
  ESP.restart();
}

bool Document::startDevicelogin() {
  // Request device login context
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(6) + 540);

  bool res = requestJsonApi(doc,
                            DeserializationOption::Filter(_loginFilter),
                            "https://login.microsoftonline.com/" + _paramTenantValue + "/oauth2/v2.0/devicecode",
                            "client_id=" + _paramClientIdValue + "&scope=offline_access%20openid%20Presence.Read");

  if (res && doc.containsKey("device_code") && doc.containsKey("user_code") && doc.containsKey("interval") && doc.containsKey("verification_uri") && doc.containsKey("message")) {
    // Save _device_code, _user_code and _interval
    _device_code = doc["device_code"].as<String>();
    _user_code   = doc["user_code"].as<String>();
    _interval    = doc["interval"].as<unsigned int>();

    // Prepare response JSON
    //ここで、ドキュメントクラスにメンバー変数にユーザーコードを保存できること
    // このデータを使って、AutoConnectの画面にユーザーコードを表示できること
    DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(3));
    responseDoc["user_code"]        = doc["user_code"].as<const char*>();
    responseDoc["verification_uri"] = doc["verification_uri"].as<const char*>();
    responseDoc["message"]          = doc["message"].as<const char*>();

    // Send JSON response
    _server->send(200, "application/json", responseDoc.as<String>());
    return true;
  } else {
    _server->send(500, "application/json", "{\"error\": \"devicelogin_unknown_response\"}");
    return false;
  }
  return false;
}

/**
 * SPIFFS webserver
 */
bool Document::exists(String path) {
  bool yes  = false;
  File file = SPIFFS.open(path, "r");
  if (!file.isDirectory()) {
    yes = true;
  }
  file.close();
  return yes;
}

String Document::getContentType(String filename) {
  if (_server->hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool Document::handleFileRead(String path) {
  log_d("handleFileRead: %s", path);
  if (path.endsWith("/")) {
    path += "index.htm";
  }
  String contentType = getContentType(path);
  String pathWithGz  = path + ".gz";
  if (exists(pathWithGz) || exists(path)) {
    if (exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    _server->streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

// Poll for access token
bool Document::pollForToken(void) {
  bool   success = false;
  String payload = "client_id=" + String(_paramClientIdValue) + "&grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code=" + _device_code;

  DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(7) + 5000);

  bool res = requestJsonApi(responseDoc,
                            DeserializationOption::Filter(_refleshtokenFilter),
                            "https://login.microsoftonline.com/" + String(_paramTenantValue) + "/oauth2/v2.0/token", payload);

  if (!res) {
    success = false;
  } else if (responseDoc.containsKey("error")) {
    const char* _error             = responseDoc["error"];
    const char* _error_description = responseDoc["error_description"];

    if (strcmp(_error, "authorization_pending") == 0) {
      log_i("pollForToken() - Wating for authorization by user: %s", _error_description);
    } else {
      log_e("pollForToken() - Unexpected error: %s, %s", _error, _error_description);
    }

    success = false;
  } else {
    if (responseDoc.containsKey("access_token") && responseDoc.containsKey("refresh_token") && responseDoc.containsKey("id_token")) {
      // Save tokens and expiration
      unsigned int _expires_in = responseDoc["expires_in"].as<unsigned int>();
      _access_token            = responseDoc["access_token"].as<String>();
      _refresh_token           = responseDoc["refresh_token"].as<String>();
      _id_token                = responseDoc["id_token"].as<String>();
      _expires                 = _expires_in * 1000;  // ms

      log_i("Set : SMODEAUTHREADY");
      success = true;
    } else {
      log_e("pollForToken() - Unknown response: ");
      success = false;
    }
  }
  return false;
}

// Refresh the access token
bool Document::refreshToken(void) {
  log_d("refreshToken()");

  bool success = false;

  // See: https://docs.microsoft.com/de-de/azure/active-directory/develop/v1-protocols-oauth-code#refreshing-the-access-tokens
  String payload = "client_id=" + _paramClientIdValue + "&grant_type=refresh_token&refresh_token=" + _refresh_token;

  DynamicJsonDocument responseDoc(6144);  // from ArduinoJson Assistant

  bool res = requestJsonApi(responseDoc,
                            DeserializationOption::Filter(_refleshtokenFilter),
                            "https://login.microsoftonline.com/" + _paramTenantValue + "/oauth2/v2.0/token",
                            payload);

  // Replace tokens and expiration
  if (res && responseDoc.containsKey("access_token") && responseDoc.containsKey("refresh_token")) {
    if (!responseDoc["access_token"].isNull()) {
      _access_token = responseDoc["access_token"].as<String>();
    }

    if (!responseDoc["refresh_token"].isNull()) {
      _refresh_token = responseDoc["refresh_token"].as<String>();
    }

    if (!responseDoc["id_token"].isNull()) {
      _id_token = responseDoc["id_token"].as<String>();
    }

    if (!responseDoc["expires_in"].isNull()) {
      int _expires_in = responseDoc["expires_in"].as<unsigned int>();
      _expires        = millis() + (_expires_in * 1000);  // Calculate timestamp when token _expires
    }

    //_state = SMODEPOLLPRESENCE;
    log_d("refreshToken() - Success");
    success = true;
  } else {
    log_d("refreshToken() - Error:");
    // Set retry after timeout
    //_tsPolling = millis() + (DEFAULT_ERROR_RETRY_INTERVAL * 1000);
    success = false;
  }
  return success;
}

// Implementation of a statemachine to handle the different application states
// void Document::statemachine(void) {

//   // Statemachine: Devicelogin started
//   if (_state == SMODEDEVICELOGINSTARTED) {
//     // log_d("SMODEDEVICELOGINSTARTED");
//     if (_laststate != SMODEDEVICELOGINSTARTED) {
//       setAnimation(0, FX_MODE_THEATER_CHASE, PURPLE);
//       log_d("Device login failed");
//     }
//     if (millis() >= _tsPolling) {
//       pollForToken();
//       _tsPolling = millis() + (_interval * 1000);
//       log_d("pollForToken");
//     }
//   }

//   // Statemachine: Devicelogin failed
//   if (_state == SMODEDEVICELOGINFAILED) {
//     log_d("Device login failed");
//     _state = SMODEWIFICONNECTED;  // Return back to initial mode
//   }

//   // Statemachine: Auth is ready, start polling for presence immediately
//   if (_state == SMODEAUTHREADY) {
//     saveContext();
//     _state     = SMODEPOLLPRESENCE;
//     _tsPolling = millis();
//   }

//   // Statemachine: Poll for presence information, even if there was a error before (handled below)
//   if (_state == SMODEPOLLPRESENCE) {
//     if (millis() >= _tsPolling) {
//       log_i("%s", "Polling presence info ...");
//       pollPresence();
//       _tsPolling = millis() + (atoi(_paramPollIntervalValue.c_str()) * 1000);
//       log_i("--> Availability: %s, Activity: %s", availability.c_str(), activity.c_str());
//     }

//     if (getTokenLifetime() < TOKEN_REFRESH_TIMEOUT) {
//       log_w("Token needs refresh, valid for %d s.", getTokenLifetime());
//       _state = SMODEREFRESHTOKEN;
//     }
//   }

//   // Statemachine: Refresh token
//   if (_state == SMODEREFRESHTOKEN) {
//     if (_laststate != SMODEREFRESHTOKEN) {
//       setAnimation(0, FX_MODE_THEATER_CHASE, RED);
//     }
//     if (millis() >= _tsPolling) {
//       boolean success = refreshToken();
//       if (success) {
//         saveContext();
//       }
//     }
//   }

//   // Statemachine: Polling presence failed
//   if (_state == SMODEPRESENCEREQUESTERROR) {
//     if (_laststate != SMODEPRESENCEREQUESTERROR) {
//       _retries = 0;
//     }

//     log_e("Polling presence failed, retry #%d.", _retries);
//     if (_retries >= 5) {
//       // Try token refresh
//       _state = SMODEREFRESHTOKEN;
//     } else {
//       _state = SMODEPOLLPRESENCE;
//     }
//   }

//   // Update laststate
//   if (_laststate != _state) {
//     _laststate = _state;
//     log_d("======================================================================");
//   }
// }

bool Document::loadContext(void) {
  File    file    = SPIFFS.open(CONTEXT_FILE);
  boolean success = false;

  if (!file) {
    log_d("loadContext() - No file found");
  } else {
    size_t size = file.size();
    if (size == 0) {
      log_d("loadContext() - File empty");
    } else {
      const int            capacity = JSON_OBJECT_SIZE(3) + 10000;
      DynamicJsonDocument  contextDoc(capacity);
      DeserializationError err = deserializeJson(contextDoc, file);

      if (err) {
        log_d("loadContext() - deserializeJson() failed with code: %s", err.c_str());
      } else {
        int numSettings = 0;
        if (!contextDoc["access_token"].isNull()) {
          _access_token = contextDoc["access_token"].as<String>();
          numSettings++;
        }
        if (!contextDoc["refresh_token"].isNull()) {
          _refresh_token = contextDoc["refresh_token"].as<String>();
          numSettings++;
        }
        if (!contextDoc["id_token"].isNull()) {
          _id_token = contextDoc["id_token"].as<String>();
          numSettings++;
        }
        if (numSettings == 3) {
          success = true;
          log_d("loadContext() - Success");
          if (_paramClientIdValue.length() > 0 && _paramTenantValue.length() > 0) {
            log_d("loadContext() - Next: Refresh token.");
            //_state = SMODEREFRESHTOKEN;
          } else {
            log_d("loadContext() - No client id or tenant setting found.");
          }
        } else {
          log_e("loadContext() - ERROR Number of valid settings in file: %d, should be 3.", numSettings);
        }
        // log_d(contextDoc.as<String>());
      }
    }
    file.close();
  }

  return success;
}

// Save context information to file in SPIFFS
void Document::saveContext(void) {
  const size_t        capacity = JSON_OBJECT_SIZE(3) + 5000;
  DynamicJsonDocument contextDoc(capacity);
  contextDoc["access_token"]  = _access_token.c_str();
  contextDoc["refresh_token"] = _refresh_token.c_str();
  contextDoc["id_token"]      = _id_token.c_str();

  File   contextFile  = SPIFFS.open(CONTEXT_FILE, FILE_WRITE);
  size_t bytesWritten = serializeJsonPretty(contextDoc, contextFile);
  contextFile.close();
  log_d("saveContext() - Success: %d", bytesWritten);
  // log_d("%s", contextDoc.as<String>().c_str());
}

// TODO
// Get presence information
// user method
void Document::pollPresence(void) {
  log_d("pollPresence()");
  // See: https://github.com/microsoftgraph/microsoft-graph-docs/blob/ananya/api-reference/beta/resources/presence.md
  const size_t        capacity = JSON_OBJECT_SIZE(4) + 500;
  DynamicJsonDocument responseDoc(capacity);

  bool res = requestJsonApi(responseDoc,
                            DeserializationOption::Filter(_presenceFilter),
                            "https://graph.microsoft.com/v1.0/me/presence",
                            "",
                            "GET",
                            true);

  if (!res) {
    //_state = SMODEPRESENCEREQUESTERROR;
    _retries++;
    log_e("Presence request error. retry:#%d", _retries);
  } else if (responseDoc.containsKey("error")) {
    const char* _error_code = responseDoc["error"]["code"];
    if (strcmp(_error_code, "InvalidAuthenticationToken")) {
      log_e("pollPresence() - Refresh needed");
      _tsPolling = millis();
      //_state     = SMODEREFRESHTOKEN;
    } else {
      log_e("pollPresence() - Error: %s\n", _error_code);
      //_state = SMODEPRESENCEREQUESTERROR;
      _retries++;
    }
  } else {
    log_i("success to get Presence");

    // Store presence info
    // availability = responseDoc["availability"].as<String>();
    // activity     = responseDoc["activity"].as<String>();
    // _retries     = 0;

    // // TODO
    // setPresenceAnimation();
  }
}
