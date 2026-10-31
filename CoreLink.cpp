#include "CoreLink.h"
#include "Config.h"
#include "CoreWebPage.h"
#include <WiFiS3.h>
#include <stdio.h>
#include <string.h>

#if __has_include("arduino_secrets.h")
  #include "arduino_secrets.h"
  #define CORE_LINK_HAS_CREDENTIALS 1
#else
  #define CORE_LINK_HAS_CREDENTIALS 0
#endif

namespace {
WiFiServer coreWebServer(80);
WiFiClient activeClient;
bool serverStarted = false;
bool wifiAttemptStarted = false;
unsigned long bootMillis = 0;
unsigned long lastWiFiAttempt = 0;

char requestLineBuffer[160];
size_t requestLineLength = 0;

char responseHeader[192];
size_t responseHeaderLength = 0;
size_t responseHeaderOffset = 0;
const char* responseBody = nullptr;
size_t responseBodyLength = 0;
size_t responseBodyOffset = 0;
String responseStorage;
bool responseActive = false;

const char* appName() {
  switch (state) {
    case STATE_MENU: return "MENU";
    case STATE_GAMES_MENU: return "GAMES";
    case STATE_SENSORS_MENU: return "SENSORS";
    case STATE_CODE: return "ECLIPSE CODE";
    case STATE_REACT_WAIT: case STATE_REACT_READY: case STATE_REACT_RESULT: return "ECLIPSE REACT";
    case STATE_MEMORY_SHOW: case STATE_MEMORY_INPUT: case STATE_MEMORY_RESULT: return "ECLIPSE MEMORY";
    case STATE_SNAKE_READY: case STATE_SNAKE: case STATE_SNAKE_GAME_OVER: return "SNAKE";
    case STATE_LIGHT: return "LIGHT";
    case STATE_DISTANCE: return "DISTANCE";
    case STATE_TEMPERATURE: return "TEMP/HUMIDITY";
    case STATE_SCORES_MENU: case STATE_SCORE_DETAIL: return "SCORES";
    case STATE_SOLUNE: return "SOLUNE";
    case STATE_SONGS: return "SONGS";
    case STATE_SETTINGS: return "SETTINGS";
    case STATE_CORE_INFO: return "CORE INFO";
    default: return "UNKNOWN";
  }
}

String statusJson() {
  String json = "{\\"wifiConnected\\":true,\\"ip\\":\\"";
  json += WiFi.localIP().toString();
  json += "\\",\\"app\\":\\""; json += appName();
  json += "\\",\\"uptimeSeconds\\":"; json += (unsigned long)((millis()-bootMillis)/1000);
  json += ",\\"light\\":"; json += lightReading;
  json += ",\\"distanceCm\\":";
  if (distanceReading < 0) json += "null"; else json += distanceReading;
  json += ",\\"temperatureF\\":";
  if (!dhtValid) json += "null"; else json += dhtTempF;
  json += ",\\"humidity\\":";
  if (!dhtValid) json += "null"; else json += dhtHumidity;
  json += ",\\"sensorsEnabled\\":"; json += sensorsEnabled() ? "true" : "false";
  json += ",\\"soundEnabled\\":"; json += soundEnabled() ? "true" : "false";
  json += "}";
  return json;
}

void beginResponse(const char* type, const char* body, size_t bodyLength,
                   int code = 200, const char* label = "OK") {
  responseHeaderLength = (size_t)snprintf(
    responseHeader, sizeof(responseHeader),
    "HTTP/1.1 %d %s\\r\\nContent-Type: %s\\r\\nCache-Control: no-store\\r\\nConnection: close\\r\\nContent-Length: %u\\r\\n\\r\\n",
    code, label, type, (unsigned int)bodyLength
  );
  responseHeaderOffset = 0;
  responseBody = body;
  responseBodyLength = bodyLength;
  responseBodyOffset = 0;
  responseActive = true;
}

void routeRequest() {
  requestLineBuffer[requestLineLength] = '\\0';
  String requestLine(requestLineBuffer);
  int firstSpace = requestLine.indexOf(' ');
  int secondSpace = requestLine.indexOf(' ', firstSpace + 1);
  if (firstSpace < 0 || secondSpace < 0) {
    responseStorage = "{\\"ok\\":false,\\"error\\":\\"bad_request\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length(), 400, "Bad Request");
    return;
  }

  String method = requestLine.substring(0, firstSpace);
  String path = requestLine.substring(firstSpace + 1, secondSpace);

  if (method == "GET" && path == "/") {
    beginResponse("text/html; charset=utf-8", CORE_WEB_HTML, strlen(CORE_WEB_HTML));
  } else if (method == "GET" && path == "/api/v1/status") {
    responseStorage = statusJson();
    beginResponse("application/json; charset=utf-8", responseStorage.c_str(), responseStorage.length());
  } else if (method == "POST" && path == "/api/v1/input/up") {
    encoderDelta--;
    responseStorage = "{\\"ok\\":true,\\"action\\":\\"up\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length());
  } else if (method == "POST" && path == "/api/v1/input/down") {
    encoderDelta++;
    responseStorage = "{\\"ok\\":true,\\"action\\":\\"down\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length());
  } else if (method == "POST" && path == "/api/v1/input/select") {
    encoderPressEvent = true;
    responseStorage = "{\\"ok\\":true,\\"action\\":\\"select\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length());
  } else if (method == "POST" && path == "/api/v1/input/back") {
    backPressEvent = true;
    responseStorage = "{\\"ok\\":true,\\"action\\":\\"back\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length());
  } else {
    responseStorage = "{\\"ok\\":false,\\"error\\":\\"not_found\\"}";
    beginResponse("application/json", responseStorage.c_str(), responseStorage.length(), 404, "Not Found");
  }
}

void serviceRequestInput() {
  // Read only a small amount per loop so the matrix scanner and local UI keep running.
  uint8_t budget = 64;
  while (activeClient && activeClient.connected() && activeClient.available() && budget--) {
    char ch = (char)activeClient.read();
    if (ch == '\\n') {
      routeRequest();
      return;
    }
    if (ch != '\\r' && requestLineLength < sizeof(requestLineBuffer) - 1) {
      requestLineBuffer[requestLineLength++] = ch;
    } else if (requestLineLength >= sizeof(requestLineBuffer) - 1) {
      activeClient.stop();
      requestLineLength = 0;
      return;
    }
  }
}

void serviceResponse() {
  if (!activeClient || !activeClient.connected()) {
    activeClient.stop();
    responseActive = false;
    responseHeaderOffset = responseHeaderLength = 0;
    responseBodyOffset = responseBodyLength = 0;
    return;
  }

  // Send bounded chunks across loop iterations rather than blocking the display scanner.
  if (responseHeaderOffset < responseHeaderLength) {
    size_t remaining = responseHeaderLength - responseHeaderOffset;
    size_t amount = remaining > 48 ? 48 : remaining;
    size_t sent = activeClient.write((const uint8_t*)responseHeader + responseHeaderOffset, amount);
    responseHeaderOffset += sent;
    return;
  }

  if (responseBodyOffset < responseBodyLength) {
    size_t remaining = responseBodyLength - responseBodyOffset;
    size_t amount = remaining > 48 ? 48 : remaining;
    size_t sent = activeClient.write((const uint8_t*)responseBody + responseBodyOffset, amount);
    responseBodyOffset += sent;
    return;
  }

  activeClient.stop();
  responseActive = false;
  responseStorage = "";
  requestLineLength = 0;
}
}

void startCoreLink() {
  bootMillis = millis();
#if CORE_LINK_HAS_CREDENTIALS
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  lastWiFiAttempt = millis();
  wifiAttemptStarted = true;
#else
  // No credentials file: CORE runs normally with networking disabled.
  wifiAttemptStarted = false;
#endif
}

bool coreLinkConnected() {
#if CORE_LINK_HAS_CREDENTIALS
  return serverStarted && WiFi.status() == WL_CONNECTED;
#else
  return false;
#endif
}

String coreLinkAddress() {
#if CORE_LINK_HAS_CREDENTIALS
  if (coreLinkConnected()) return WiFi.localIP().toString();
#endif
  return "WEB OFFLINE";
}

void serviceCoreLink() {
#if CORE_LINK_HAS_CREDENTIALS
  if (!wifiAttemptStarted) return;

  if (WiFi.status() == WL_CONNECTED) {
    if (!serverStarted) {
      coreWebServer.begin();
      serverStarted = true;
    }
  } else {
    if (serverStarted) {
      coreWebServer.end();
      serverStarted = false;
      activeClient.stop();
      responseActive = false;
      requestLineLength = 0;
    }
    if ((unsigned long)(millis() - lastWiFiAttempt) >= 10000UL) {
      WiFi.begin(SECRET_SSID, SECRET_PASS);
      lastWiFiAttempt = millis();
    }
    return;
  }

  if (responseActive) {
    serviceResponse();
    return;
  }

  if (!activeClient || !activeClient.connected()) {
    activeClient.stop();
    activeClient = coreWebServer.available();
    requestLineLength = 0;
  }

  if (activeClient) serviceRequestInput();
#endif
}
