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

// certificate for https://login.microsoftonline.com
// DigiCert Global Root CA, Valid until: 10/Nov/2031
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
  return _expires - (millis() / 1000);
}

String Document::getDeviceCode(void) {
  return _device_code;
}

String Document::getUserCode(void) {
  return _user_code;
}

/**
 * API request handler
 */
bool Document::requestGraphAPI(JsonDocument& doc, ARDUINOJSON_NAMESPACE::Filter filter, String url, String payload, String type, bool sendAuth) {
  std::unique_ptr<WiFiClientSecure> client(new WiFiClientSecure);

  bool success = false;

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
      // File found at server (HTTP 200, 301), or HTTP 400 with response payload
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_BAD_REQUEST) {
        // Parse JSON data
        DeserializationError error = deserializeJson(doc, https.getStream(), filter);

        serializeJsonPretty(doc, Serial);
        Serial.println();

        if (error) {
          log_e("deserializeJson() failed: %s", error.c_str());
          https.end();
          success = false;
        } else {
          log_i("deserializeJson() Success!");
          https.end();
          success = true;
        }
      } else {
        log_e("[HTTPS] Other HTTP code: %d", httpCode);
        https.end();
        success = false;
      }
    } else {
      log_e("[HTTPS] Request failed: %s", https.errorToString(httpCode).c_str());
      https.end();
      success = false;
    }
  } else {
    log_e("[HTTPS] can't begin().");
    success = false;
  }

  return success;
}

void Document::handleGetSettings() {
  // log_d("handleGetSettings()");

  // const int capacity = JSON_OBJECT_SIZE(13);

  // StaticJsonDocument<capacity> responseDoc;
  // responseDoc["client_id"].set(_paramClientIdValue);
  // responseDoc["tenant"].set(_paramTenantValue);
  // responseDoc["poll_interval"].set(_paramPollIntervalValue);

  // serializeJsonPretty(responseDoc, Serial);

  // _server->send(200, "application/json", responseDoc.as<String>());
}

/*
{
  "user_code": "SZVKQXTYC",
  "device_code": "xxx",
  "verification_uri": "https://microsoft.com/devicelogin",
  "expires_in": 900,
  "interval": 5,
  "message": "To sign in, use a web browser to open the page https://microsoft.com/devicelogin and enter the code SZVKQXTYC to authenticate."
}
*/
bool Document::startDevicelogin() {
  bool success = false;
  // Request device login context
  DynamicJsonDocument doc(JSON_OBJECT_SIZE(6) + 540);

  String URI     = "https://login.microsoftonline.com/{tenand_id}/oauth2/v2.0/devicecode";
  String payload = "client_id={client_id}&scope=offline_access%20openid%20Presence.Read";

  URI.replace("{tenand_id}", _paramTenantValue);
  payload.replace("{client_id}", _paramClientIdValue);

  bool res = requestGraphAPI(doc,
                             DeserializationOption::Filter(_loginFilter),
                             URI,
                             payload);

  if (res) {
    if (doc.containsKey("device_code") && doc.containsKey("user_code") && doc.containsKey("interval") && doc.containsKey("verification_uri") && doc.containsKey("message")) {
      // Save _device_code, _user_code and _interval
      _user_code        = doc["user_code"].as<String>();
      _device_code      = doc["device_code"].as<String>();
      _verification_uri = doc["verification_uri"].as<String>();
      _expires          = doc["expires_in"].as<unsigned int>() + (millis() / 1000);  // [seconds]
      _interval         = doc["interval"].as<unsigned int>();
      _message          = doc["message"].as<String>();

      log_i("expires = %d[s]", _expires);
      // Send JSON response
      _server->send(200, "application/json", doc.as<String>());
      success = true;
    } else {
      _server->send(500, "application/json", "{\"error\": \"devicelogin_unknown_response\"}");
      success = false;
    }
  } else {
    log_e("response fail.");
  }

  return success;
}

/*
{
  "token_type": "Bearer",
  "scope": "openid Presence.Read profile email",
  "expires_in": 4870,
  "ext_expires_in": 4870,
  "access_token": "",
  "refresh_token": "",
  "id_token": ""
}
*/
// Poll for access token
bool Document::pollForToken(void) {
  bool   success = false;
  String URI("https://login.microsoftonline.com/{tenant_id}/oauth2/v2.0/token");
  String payload("client_id={client_id}&grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code={device_code}");

  DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(7) + 5000);

  URI.replace("{tenant_id}", _paramTenantValue);
  payload.replace("{client_id}", _paramClientIdValue);
  payload.replace("{device_code}", _device_code);

  log_i("payload = %s", payload.c_str());
  log_i("url     = %s", URI.c_str());

  bool res = requestGraphAPI(responseDoc,
                             DeserializationOption::Filter(_refleshtokenFilter),
                             URI,
                             payload);

  if (!res) {
    log_e("response failer");
    success = false;
  } else if (responseDoc.containsKey("error")) {
    const char* _error             = responseDoc["error"];
    const char* _error_description = responseDoc["error_description"];

    if (strcmp(_error, "authorization_pending") == 0) {
      log_i("Wating for authorization by user: %s", _error_description);
    } else {
      log_e("Unexpected error: %s, %s", _error, _error_description);
    }

    success = false;
  } else {
    if (responseDoc.containsKey("access_token") && responseDoc.containsKey("refresh_token") && responseDoc.containsKey("id_token")) {
      // Save tokens and expiration
      unsigned int _expires_in = responseDoc["expires_in"].as<unsigned int>();
      _access_token            = responseDoc["access_token"].as<String>();
      _refresh_token           = responseDoc["refresh_token"].as<String>();
      _id_token                = responseDoc["id_token"].as<String>();
      _expires                 = _expires_in + (millis() / 1000);  //[seconds]

      log_i("pollForToken expires = %d[s]", _expires);

      log_d("pollForToken() - Success");
      success = true;
    } else {
      log_e("No response: ");
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
  String payload("client_id={client_id}&grant_type=refresh_token&refresh_token={refresh_token}");
  String URI("https://login.microsoftonline.com/{tenant_id}/oauth2/v2.0/token");

  URI.replace("{tenant_id}", _paramTenantValue);
  payload.replace("{client_id}", _paramClientIdValue);
  payload.replace("{refresh_token}", _refresh_token);

  DynamicJsonDocument responseDoc(6144);  // from ArduinoJson Assistant

  bool res = requestGraphAPI(responseDoc,
                             DeserializationOption::Filter(_refleshtokenFilter),
                             URI,
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
      _expires        = _expires_in + (millis() / 1000);  // Calculate timestamp when token _expires
      log_i("refreshToken expires = %d[s]", _expires);
    }

    log_d("refreshToken() - Success");
    success = true;
  } else {
    log_d("refreshToken() - Error:");
    success = false;
  }

  return success;
}

// Save context information to EEPROM
void Document::saveContext(void) {
  // TODO
}

bool Document::loadContext(void) {
  // TODO
}
