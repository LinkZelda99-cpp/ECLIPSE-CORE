#include "CoreLink.h"
#include "Config.h"
#include "CoreWebPage.h"
#include <WiFiS3.h>

#if __has_include("arduino_secrets.h")
  #include "arduino_secrets.h"
  #define CORE_LINK_HAS_CREDENTIALS 1
#else
  #define CORE_LINK_HAS_CREDENTIALS 0
#endif

namespace {
WiFiServer coreWebServer(80);
bool serverStarted = false;
bool wifiAttemptStarted = false;
unsigned long bootMillis = 0;

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

void sendResponse(WiFiClient& client, const char* type, const String& body, int code = 200, const char* label = "OK") {
  client.print("HTTP/1.1 "); client.print(code); client.print(" "); client.println(label);
  client.print("Content-Type: "); client.println(type);
  client.println("Cache-Control: no-store");
  client.println("Connection: close");
  client.print("Content-Length: "); client.println(body.length());
  client.println();
  client.print(body);
}

String statusJson() {
  String json = "{\"wifiConnected\":true,\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",\"app\":\""; json += appName();
  json += "\",\"uptimeSeconds\":"; json += (unsigned long)((millis()-bootMillis)/1000);
  json += ",\"light\":"; json += lightReading;
  json += ",\"distanceCm\":";
  if (distanceReading < 0) json += "null"; else json += distanceReading;
  json += ",\"temperatureF\":";
  if (!dhtValid) json += "null"; else json += dhtTempF;
  json += ",\"humidity\":";
  if (!dhtValid) json += "null"; else json += dhtHumidity;
  json += ",\"sensorsEnabled\":"; json += sensorsEnabled() ? "true" : "false";
  json += ",\"soundEnabled\":"; json += soundEnabled() ? "true" : "false";
  json += "}";
  return json;
}

void handleClient(WiFiClient& client) {
  client.setTimeout(8);
  String requestLine = client.readStringUntil('\\n');
  requestLine.trim();
  if (requestLine.length() == 0) { client.stop(); return; }
  int firstSpace = requestLine.indexOf(' ');
  int secondSpace = requestLine.indexOf(' ', firstSpace + 1);
  if (firstSpace < 0 || secondSpace < 0) { client.stop(); return; }
  String method = requestLine.substring(0, firstSpace);
  String path = requestLine.substring(firstSpace + 1, secondSpace);

  // Drain currently buffered headers; requests are intentionally kept body-free.
  while (client.available()) client.read();

  if (method == "GET" && path == "/") {
    sendResponse(client, "text/html; charset=utf-8", String(CORE_WEB_HTML));
  } else if (method == "GET" && path == "/api/v1/status") {
    sendResponse(client, "application/json; charset=utf-8", statusJson());
  } else if (method == "POST" && path == "/api/v1/input/up") {
    encoderDelta--;
    sendResponse(client, "application/json", "{\"ok\":true,\"action\":\"up\"}");
  } else if (method == "POST" && path == "/api/v1/input/down") {
    encoderDelta++;
    sendResponse(client, "application/json", "{\"ok\":true,\"action\":\"down\"}");
  } else if (method == "POST" && path == "/api/v1/input/select") {
    encoderPressEvent = true;
    sendResponse(client, "application/json", "{\"ok\":true,\"action\":\"select\"}");
  } else if (method == "POST" && path == "/api/v1/input/back") {
    backPressEvent = true;
    sendResponse(client, "application/json", "{\"ok\":true,\"action\":\"back\"}");
  } else {
    sendResponse(client, "application/json", "{\"ok\":false,\"error\":\"not_found\"}", 404, "Not Found");
  }
  client.stop();
}
}

void startCoreLink() {
  bootMillis = millis();
#if CORE_LINK_HAS_CREDENTIALS
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  wifiAttemptStarted = true;
#else
  // No credentials file: CORE runs normally with networking disabled.
  wifiAttemptStarted = false;
#endif
}

void serviceCoreLink() {
#if CORE_LINK_HAS_CREDENTIALS
  if (!wifiAttemptStarted) return;
  if (!serverStarted && WiFi.status() == WL_CONNECTED) {
    coreWebServer.begin();
    serverStarted = true;
  }
  if (serverStarted) {
    WiFiClient client = coreWebServer.available();
    if (client) handleClient(client);
  }
#endif
}
