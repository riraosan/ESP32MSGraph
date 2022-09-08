/*
Documents Library

Original Source:
https://github.com/toblum/ESPTeamsPresence

Licence:
[MIT](https://github.com/riraosan/Documents/blob/master/LICENSE)

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

#include <IotWebConf.h>
#include <ArduinoJson.h>
#include <WS2812FX.h>
#include <esp32-hal-log.h>

#include <config.h>
#include <filter.h>
#include <Documents.h>

extern IotWebConfParameter _paramClientId;
extern IotWebConfParameter _paramTenant;
extern IotWebConfParameter _paramPollInterval;
extern IotWebConfParameter _paramNumLeds;

extern WS2812FX ws2812fx;
// extern int      numberLeds;

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

int Documents::getTokenLifetime() {
  return (_expires - millis()) / 1000;
}

void Documents::removeContext() {
  SPIFFS.remove(CONTEXT_FILE);
  log_d("removeContext() - Success");
}

/**
 * API request handler
 */
bool Documents::requestJsonApi(JsonDocument& doc, ARDUINOJSON_NAMESPACE::Filter filter, String url, String payload, String type, bool sendAuth) {
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
      String header = "Bearer " + access_token;
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

void Documents::handleRoot() {
  log_d("handleRoot()");
  // -- Let IotWebConf test and handle captive portal requests.
  if (_iotWebConf->handleCaptivePortal()) {
    return;
  }

  String response = R"(
<!DOCTYPE html>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
<link href="https://fonts.googleapis.com/css?family=Press+Start+2P" rel="stylesheet">
<link href="https://unpkg.com/nes.css@2.3.0/css/nes.min.css" rel="stylesheet" />
<style type="text/css">
  body {padding:3.5rem}
  .ml-s {margin-left:1.0rem}
  .mt-s {margin-top:1.0rem}
  .mt {margin-top:3.5rem}
  #dialog-devicelogin{max-width : 800px }
</style>
<script>

function closeDeviceLoginModal() {
  document.getElementById('dialog-devicelogin').close();
}

function performClearSettings() {
  fetch('/api/clearSettings').then(r => r.json()).then(data => {
    console.log('clearSettings', data);
    document.getElementById('dialog-clearsettings').close();
    document.getElementById('dialog-clearsettings-result').showModal();
  });
}

function openDeviceLoginModal() {
  fetch('/api/startDevicelogin').then(r => r.json()).then(data => {
    console.log('startDevicelogin', data);
    if (data && data.user_code) {
      document.getElementById('btn_open').href         = data.verification_uri;
      document.getElementById('lbl_message').innerText = data.message;
      document.getElementById('code_field').value      = data.user_code;
    }
    document.getElementById('dialog-devicelogin').showModal();
  });
}

</script>
<title>ESP32 teams presence</title>
</head>
<body>
<h2>ESP32 Teams Presence - Ver.__VERSION__ </h2>

<section class="mt"><div class="nes-balloon from-left">
__MESSAGE__
__BUTTON__

<dialog class="nes-dialog is-rounded" id="dialog-devicelogin">
<p class="title">Start device login</p>
<p id="lbl_message"></p>
<input type="text" id="code_field"class="nes-input" disabled >
<menu class="dialog-menu">
<button id="btn_close" class="nes-btn" onclick="closeDeviceLoginModal();">Close</button>
<a class="nes-btn is-primary ml-s" id="btn_open" href="https://microsoft.com/devicelogin" target="_blank">Open device login</a>
</menu>
</dialog>

</section>
<div class="nes-balloon from-left mt">
Go to <a href="config">configuration page</a> to change settings.
</div>
<section class="nes-container with-title"><h3 class="title">Current settings</h3>
<div class=" nes-field mt-s "><label for=" name_field ">Client-ID</label><input type=" text " id=" name_field " class=" nes-input " disabled value=__CLIENTID__ ></div>
<div class=" nes-field mt-s "><label for=" name_field ">Tenant hostname / ID</label><input type=" text " id=" name_field " class=" nes-input " disabled value=__TENANTVALUE__ ></div>
<div class=" nes-field mt-s "><label for=" name_field ">Polling interval (sec)</label><input type=" text " id=" name_field " class=" nes-input " disabled value=__POLLVALUE__ ></div>
<div class=" nes-field mt-s "><label for=" name_field ">Number of LEDs</label><input type=" text " id=" name_field " class=" nes-input " disabled value=__LEDSVALUE__ ></div>
</section>

<section class="nes-container with-title mt"><h3 class="title">Memory usage</h3>
<div>Sketch: __SKETCHSIZE__ of __FREESKETCHSPACE__ bytes free</div>
<progress class="nes-progress" value="__SKETCHSIZE__" max="__FREESKETCHSPACE__"></progress>
<div class="mt-s">RAM: __FREEHEAP__ of 327680 bytes free</div>
<progress class="nes-progress" value="__USEDHEAP__" max="327680"></progress>
</section>

<section class="nes-container with-title mt"><h3 class="title">Danger area</h3>
<dialog class="nes-dialog is-rounded" id="dialog-clearsettings">
<p class="title">Really clear all settings?</p>
<button class="nes-btn" onclick="document.getElementById('dialog-clearsettings').close();">Close</button>
<button class="nes-btn is-error" onclick="performClearSettings();">Clear all settings</button>
</dialog>
<dialog class="nes-dialog is-rounded" id="dialog-clearsettings-result">
<p class="title">All settings were cleared.</p>
</dialog>
<div><button type="button" class="nes-btn is-error" onclick="document.getElementById('dialog-clearsettings').showModal();">Clear all settings</button></div>
</section>

<div class="mt">
<i class=" nes-icon github "></i> Find the <a href="https://github.com/toblum/ESPTeamsPresence" target="_blank">ESPTeamsPresence</a> project on GitHub.</i>
</div>
</body>
</html>
)";

  if (_paramTenantValue.isEmpty() || _paramClientIdValue.isEmpty()) {
    response.replace("__MESSAGE__", R"(<p class="note nes-text is-error">Some settings are missing. Go to <a href="config">configuration page</a> to complete setup.</p></div>)");
    response.replace("__BUTTON__", "");
  } else {
    if (access_token == "") {
      response.replace("__MESSAGE__", R"(<p class="note nes-text is-error">No authentication info's found, start device login flow to complete widget setup!</p></div>)");
    } else {
      response.replace("__MESSAGE__", R"(<p class="note nes-text">Device setup complete, but you can start the device login flow if you need to re-authenticate.</p></div>)");
    }

    response.replace("__BUTTON__", R"(<div><button type="button" class="nes-btn" onclick="openDeviceLoginModal();" >Start device login</button></div>)");
  }

  response.replace("__CLIENTID__", _paramClientIdValue);
  response.replace("__TENANTVALUE__", _paramTenantValue);
  response.replace("__POLLVALUE__", _paramPollIntervalValue);
  response.replace("__LEDSVALUE__", _paramNumLedsValue);

  response.replace("__VERSION__", VERSION);
  response.replace("__SKETCHSIZE__", String(ESP.getSketchSize()));
  response.replace("__FREESKETCHSPACE__", String(ESP.getFreeSketchSpace()));
  response.replace("__FREEHEAP__", String(ESP.getFreeHeap()));
  response.replace("__USEDHEAP__", String(327680 - ESP.getFreeHeap()));

  _server->send(200, "text/html", response);
}

void Documents::handleGetSettings() {
  log_d("handleGetSettings()");

  const int capacity = JSON_OBJECT_SIZE(13);

  StaticJsonDocument<capacity> responseDoc;
  responseDoc["client_id"].set(_paramClientIdValue);
  responseDoc["tenant"].set(_paramTenantValue);
  responseDoc["poll_interval"].set(_paramPollIntervalValue);
  responseDoc["num_leds"].set(_paramNumLedsValue);

  responseDoc["heap"].set(ESP.getFreeHeap());
  responseDoc["min_heap"].set(ESP.getMinFreeHeap());
  responseDoc["sketch_size"].set(ESP.getSketchSize());
  responseDoc["free_sketch_space"].set(ESP.getFreeSketchSpace());
  responseDoc["flash_chip_size"].set(ESP.getFlashChipSize());
  responseDoc["flash_chip_speed"].set(ESP.getFlashChipSpeed());
  responseDoc["sdk_version"].set(ESP.getSdkVersion());
  responseDoc["cpu_freq"].set(ESP.getCpuFreqMHz());

  responseDoc["sketch_version"].set(VERSION);

  serializeJsonPretty(responseDoc, Serial);

  _server->send(200, "application/json", responseDoc.as<String>());
}

// Delete EEPROM by removing the trailing sequence, remove context file
void Documents::handleClearSettings() {
  log_d("handleClearSettings()");

  for (int t = 0; t < 4; t++) {
    EEPROM.write(t, 0);
  }
  EEPROM.commit();
  removeContext();

  _server->send(200, "application/json", "{\"action\": \"clear_settings\", \"error\": false}");
  ESP.restart();
}

void Documents::startDevicelogin() {
  // Only if not already started
  if (_state != SMODEDEVICELOGINSTARTED) {
    log_d("handleStartDevicelogin()");

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
      DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(3));
      responseDoc["user_code"]        = doc["user_code"].as<const char*>();
      responseDoc["verification_uri"] = doc["verification_uri"].as<const char*>();
      responseDoc["message"]          = doc["message"].as<const char*>();

      // Set state, update polling timestamp
      _state     = SMODEDEVICELOGINSTARTED;
      _tsPolling = millis() + (_interval * 1000);

      // Send JSON response
      _server->send(200, "application/json", responseDoc.as<String>());
    } else {
      _server->send(500, "application/json", "{\"error\": \"devicelogin_unknown_response\"}");
    }
  } else {
    _server->send(409, "application/json", "{\"error\": \"devicelogin_already_running\"}");
  }
}

/**
 * SPIFFS webserver
 */
bool Documents::exists(String path) {
  bool yes  = false;
  File file = SPIFFS.open(path, "r");
  if (!file.isDirectory()) {
    yes = true;
  }
  file.close();
  return yes;
}

void Documents::handleMinimalUpload() {
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send(200, "text/html", "<!DOCTYPE html>\
			<html>\
			<head>\
				<title>ESP8266 Upload</title>\
				<meta charset=\"utf-8\">\
				<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\
				<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
			</head>\
			<body>\
				<form action=\"/fs/upload\" method=\"post\" enctype=\"multipart/form-data\">\
				<input type=\"file\" name=\"data\">\
				<input type=\"text\" name=\"path\" value=\"/\">\
				<button>Upload</button>\
				</form>\
			</body>\
			</html>");
}

void Documents::handleFileUpload() {
  File        fsUploadFile;
  HTTPUpload& upload = _server->upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    log_d("handleFileUpload Name: %s", filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename     = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    // DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
    log_d("handleFileUpload Size: %d", upload.totalSize);
  }
}

void Documents::handleFileDelete() {
  if (_server->args() == 0) {
    return _server->send(500, "text/plain", "BAD ARGS");
  }
  String path = _server->arg(0);
  log_d("handleFileDelete: %s", path);
  if (path == "/") {
    return _server->send(500, "text/plain", "BAD PATH");
  }
  if (!exists(path)) {
    return _server->send(404, "text/plain", "FileNotFound");
  }
  SPIFFS.remove(path);
  _server->send(200, "text/plain", "");
  path = String();
}

void Documents::handleFileList() {
  if (!_server->hasArg("dir")) {
    _server->send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = _server->arg("dir");
  log_d("handleFileList: %s", path);

  File root     = SPIFFS.open(path);
  path          = String();
  String output = "[";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (output != "[") {
        output += ',';
      }
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.name()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
  output += "]";
  _server->send(200, "text/json", output);
}

String Documents::getContentType(String filename) {
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

bool Documents::handleFileRead(String path) {
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
void Documents::pollForToken(void) {
  String payload = "client_id=" + String(_paramClientIdValue) + "&grant_type=urn:ietf:params:oauth:grant-type:device_code&device_code=" + _device_code;

  DynamicJsonDocument responseDoc(JSON_OBJECT_SIZE(7) + 5000);

  bool res = requestJsonApi(responseDoc,
                            DeserializationOption::Filter(_refleshtokenFilter),
                            "https://login.microsoftonline.com/" + String(_paramTenantValue) + "/oauth2/v2.0/token",
                            payload);

  if (!res) {
    _state = SMODEDEVICELOGINFAILED;
  } else if (responseDoc.containsKey("error")) {
    const char* _error             = responseDoc["error"];
    const char* _error_description = responseDoc["error_description"];

    if (strcmp(_error, "authorization_pending") == 0) {
      log_i("pollForToken() - Wating for authorization by user: %s", _error_description);
    } else {
      log_e("pollForToken() - Unexpected error: %s, %s", _error, _error_description);
      _state = SMODEDEVICELOGINFAILED;
    }
  } else {
    if (responseDoc.containsKey("access_token") && responseDoc.containsKey("refresh_token") && responseDoc.containsKey("id_token")) {
      // Save tokens and expiration
      unsigned int _expires_in = responseDoc["expires_in"].as<unsigned int>();
      access_token             = responseDoc["access_token"].as<String>();
      _refresh_token           = responseDoc["refresh_token"].as<String>();
      id_token                 = responseDoc["id_token"].as<String>();
      _expires                 = millis() + (_expires_in * 1000);  // Calculate timestamp when token _expires

      // Set state
      _state = SMODEAUTHREADY;

      log_i("Set : SMODEAUTHREADY");
    } else {
      log_e("pollForToken() - Unknown response: ");
    }
  }
}

// Refresh the access token
bool Documents::refreshToken(void) {
  bool success = false;
  // See: https://docs.microsoft.com/de-de/azure/active-directory/develop/v1-protocols-oauth-code#refreshing-the-access-tokens
  String payload = "client_id=" + _paramClientIdValue + "&grant_type=refresh_token&refresh_token=" + refresh_token;
  log_d("refreshToken()");

  DynamicJsonDocument responseDoc(6144);  // from ArduinoJson Assistant

  bool res = requestJsonApi(responseDoc,
                            DeserializationOption::Filter(_refleshtokenFilter),
                            "https://login.microsoftonline.com/" + _paramTenantValue + "/oauth2/v2.0/token",
                            payload);

  // Replace tokens and expiration
  if (res && responseDoc.containsKey("access_token") && responseDoc.containsKey("refresh_token")) {
    if (!responseDoc["access_token"].isNull()) {
      access_token = responseDoc["access_token"].as<String>();
      success      = true;
    }
    if (!responseDoc["refresh_token"].isNull()) {
      _refresh_token = responseDoc["refresh_token"].as<String>();
      success        = true;
    }
    if (!responseDoc["id_token"].isNull()) {
      id_token = responseDoc["id_token"].as<String>();
    }
    if (!responseDoc["expires_in"].isNull()) {
      int _expires_in = responseDoc["expires_in"].as<unsigned int>();
      _expires        = millis() + (_expires_in * 1000);  // Calculate timestamp when token _expires
    }

    log_d("refreshToken() - Success");
    _state = SMODEPOLLPRESENCE;
  } else {
    log_d("refreshToken() - Error:");
    // Set retry after timeout
    _tsPolling = millis() + (DEFAULT_ERROR_RETRY_INTERVAL * 1000);
  }
  return success;
}

// Implementation of a statemachine to handle the different application states
void Documents::statemachine(void) {
  // Statemachine: Check states of iotWebConf to detect AP mode and WiFi Connection attempt
  byte iotWebConfState = _iotWebConf->getState();
  if (iotWebConfState != _lastIotWebConfState) {
    if (iotWebConfState == IOTWEBCONF_STATE_NOT_CONFIGURED || iotWebConfState == IOTWEBCONF_STATE_AP_MODE) {
      log_d("Detected AP mode");
      setAnimation(0, FX_MODE_THEATER_CHASE, WHITE);
    }
    if (iotWebConfState == IOTWEBCONF_STATE_CONNECTING) {
      log_d("WiFi connecting");
      _state = SMODEWIFICONNECTING;
    }
  }
  _lastIotWebConfState = iotWebConfState;

  // Statemachine: Wifi connection start
  if (_state == SMODEWIFICONNECTING && _laststate != SMODEWIFICONNECTING) {
    setAnimation(0, FX_MODE_THEATER_CHASE, BLUE);
  }

  // Statemachine: After wifi is connected
  if (_state == SMODEWIFICONNECTED && _laststate != SMODEWIFICONNECTED) {
    setAnimation(0, FX_MODE_THEATER_CHASE, GREEN);
    // startMDNS();
    loadContext();
    // WiFi client
    log_d("Wifi connected, waiting for requests ...");
  }

  // Statemachine: Devicelogin started
  if (_state == SMODEDEVICELOGINSTARTED) {
    // log_d("SMODEDEVICELOGINSTARTED");
    if (_laststate != SMODEDEVICELOGINSTARTED) {
      setAnimation(0, FX_MODE_THEATER_CHASE, PURPLE);
      log_d("Device login failed");
    }
    if (millis() >= _tsPolling) {
      pollForToken();
      _tsPolling = millis() + (_interval * 1000);
      log_d("pollForToken");
    }
  }

  // Statemachine: Devicelogin failed
  if (_state == SMODEDEVICELOGINFAILED) {
    log_d("Device login failed");
    _state = SMODEWIFICONNECTED;  // Return back to initial mode
  }

  // Statemachine: Auth is ready, start polling for presence immediately
  if (_state == SMODEAUTHREADY) {
    saveContext();
    _state     = SMODEPOLLPRESENCE;
    _tsPolling = millis();
  }

  // Statemachine: Poll for presence information, even if there was a error before (handled below)
  if (_state == SMODEPOLLPRESENCE) {
    if (millis() >= _tsPolling) {
      log_i("%s", "Polling presence info ...");
      pollPresence();
      _tsPolling = millis() + (atoi(_paramPollIntervalValue.c_str()) * 1000);
      log_i("--> Availability: %s, Activity: %s", availability.c_str(), activity.c_str());
    }

    if (getTokenLifetime() < TOKEN_REFRESH_TIMEOUT) {
      log_w("Token needs refresh, valid for %d s.", getTokenLifetime());
      _state = SMODEREFRESHTOKEN;
    }
  }

  // Statemachine: Refresh token
  if (_state == SMODEREFRESHTOKEN) {
    if (_laststate != SMODEREFRESHTOKEN) {
      setAnimation(0, FX_MODE_THEATER_CHASE, RED);
    }
    if (millis() >= _tsPolling) {
      boolean success = refreshToken();
      if (success) {
        saveContext();
      }
    }
  }

  // Statemachine: Polling presence failed
  if (_state == SMODEPRESENCEREQUESTERROR) {
    if (_laststate != SMODEPRESENCEREQUESTERROR) {
      _retries = 0;
    }

    log_e("Polling presence failed, retry #%d.", _retries);
    if (_retries >= 5) {
      // Try token refresh
      _state = SMODEREFRESHTOKEN;
    } else {
      _state = SMODEPOLLPRESENCE;
    }
  }

  // Update laststate
  if (_laststate != _state) {
    _laststate = _state;
    log_d("======================================================================");
  }
}

bool Documents::loadContext(void) {
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
          access_token = contextDoc["access_token"].as<String>();
          numSettings++;
        }
        if (!contextDoc["refresh_token"].isNull()) {
          _refresh_token = contextDoc["refresh_token"].as<String>();
          numSettings++;
        }
        if (!contextDoc["id_token"].isNull()) {
          id_token = contextDoc["id_token"].as<String>();
          numSettings++;
        }
        if (numSettings == 3) {
          success = true;
          log_d("loadContext() - Success");
          if (_paramClientIdValue.length() > 0 && _paramTenantValue.length() > 0) {
            log_d("loadContext() - Next: Refresh token.");
            _state = SMODEREFRESHTOKEN;
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
void Documents::saveContext(void) {
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
void Documents::pollPresence(void) {
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
    _state = SMODEPRESENCEREQUESTERROR;
    _retries++;
    log_e("Presence request error. retry:#%d", _retries);
  } else if (responseDoc.containsKey("error")) {
    const char* _error_code = responseDoc["error"]["code"];
    if (strcmp(_error_code, "InvalidAuthenticationToken")) {
      log_e("pollPresence() - Refresh needed");
      _tsPolling = millis();
      _state     = SMODEREFRESHTOKEN;
    } else {
      log_e("pollPresence() - Error: %s\n", _error_code);
      _state = SMODEPRESENCEREQUESTERROR;
      _retries++;
    }
  } else {
    log_i("success to get Presence");

    // Store presence info
    availability = responseDoc["availability"].as<String>();
    activity     = responseDoc["activity"].as<String>();
    _retries     = 0;

    // TODO
    setPresenceAnimation();
  }
}

// TODO
// Neopixel control
// user method
void Documents::setAnimation(uint8_t segment, uint8_t mode, uint32_t color, uint16_t speed, bool reverse) {
  uint16_t startLed, endLed = 0;

  // Support only one segment for the moment
  if (segment == 0) {
    startLed = 0;
    endLed   = ws2812fx.getLength();
  }

  log_i("setAnimation: %d, %d-%d, Mode: %d, Color: %d, Speed: %d", segment, startLed, endLed, mode, color, speed);

  ws2812fx.setSegment(segment, startLed, endLed, mode, color, speed, reverse);
}

// TODO
//  user method...
//   Activity
//   Available,
//   Away,
//   BeRightBack,
//   Busy,
//   DoNotDisturb,
//   InACall,
//   InAConferenceCall,
//   Inactive,
//   InAMeeting,
//   Offline,
//   OffWork,
//   OutOfOffice,
//   PresenceUnknown,
//   Presenting,
//   UrgentInterruptionsOnly
void Documents::setPresenceAnimation() {
  if (activity.equals("Available")) {
    setAnimation(0, FX_MODE_STATIC, GREEN);
  }
  if (activity.equals("Away")) {
    setAnimation(0, FX_MODE_STATIC, YELLOW);
  }
  if (activity.equals("BeRightBack")) {
    setAnimation(0, FX_MODE_STATIC, ORANGE);
  }
  if (activity.equals("Busy")) {
    setAnimation(0, FX_MODE_STATIC, PURPLE);
  }
  if (activity.equals("DoNotDisturb") || activity.equals("UrgentInterruptionsOnly")) {
    setAnimation(0, FX_MODE_STATIC, PINK);
  }
  if (activity.equals("InACall")) {
    setAnimation(0, FX_MODE_BREATH, RED);
  }
  if (activity.equals("InAConferenceCall")) {
    setAnimation(0, FX_MODE_BREATH, RED, 9000);
  }
  if (activity.equals("Inactive")) {
    setAnimation(0, FX_MODE_BREATH, WHITE);
  }
  if (activity.equals("InAMeeting")) {
    setAnimation(0, FX_MODE_SCAN, RED);
  }
  if (activity.equals("Offline") || activity.equals("OffWork") || activity.equals("OutOfOffice") || activity.equals("PresenceUnknown")) {
    setAnimation(0, FX_MODE_STATIC, BLACK);
  }
  if (activity.equals("Presenting")) {
    setAnimation(0, FX_MODE_COLOR_WIPE, RED);
  }
}
