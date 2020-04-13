
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <ESP8266WebServer.h>
#include "WirelessFramework.h"
#include <DNSServer.h>
/*
WiFiEventHandler onStationModeConnected(std::function<void(const WiFiEventStationModeConnected&)>);
        WiFiEventHandler onStationModeDisconnected(std::function<void(const WiFiEventStationModeDisconnected&)>);
        WiFiEventHandler onStationModeAuthModeChanged(std::function<void(const WiFiEventStationModeAuthModeChanged&)>);
        WiFiEventHandler onStationModeGotIP(std::function<void(const WiFiEventStationModeGotIP&)>);
        WiFiEventHandler onStationModeDHCPTimeout(std::function<void(void)>);
        WiFiEventHandler onSoftAPModeStationConnected(std::function<void(const WiFiEventSoftAPModeStationConnected&)>);
        WiFiEventHandler onSoftAPModeStationDisconnected(std::function<void(const WiFiEventSoftAPModeStationDisconnected&)>);
        WiFiEventHandler onSoftAPModeProbeRequestReceived(std::function<void(const WiFiEventSoftAPModeProbeRequestReceived&)>);
        WiFiEventHandler onWiFiModeChange(std::function<void(const WiFiEventModeChange&)>);
*/

ESP8266WebServer server(80);
DNSServer dnsServer;


void setupHttpHandlers();


void wifiAPStationConnected(const WiFiEventSoftAPModeStationConnected& ev) {
  Serial.println("Someone has connected ");
}

void wifiAPStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& ev) {
  Serial.println("Someone has disconnected");  
  if (WiFi.softAPgetStationNum() == 0) {
    Serial.println("Shutdown AP");
    stopWifiAP();
  }
}

void setupWifiFramework() {

  Serial.print("Current wifi mode:");
  WiFi.onSoftAPModeStationConnected(wifiAPStationConnected);
  WiFi.onSoftAPModeStationConnected(wifiAPStationConnected);
  WiFi.onSoftAPModeStationDisconnected(wifiAPStationDisconnected);
  
  
}


void enableWifiAP(WFAPModeConfig cfg) 
{
  char buf[20];
  uint32_t id = ESP.getChipId();
  sprintf(buf, "XSP_%X", id);
  Serial.print("Setting AP:");
  Serial.println(buf);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(buf, NULL);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  dnsServer.start(53, "*", myIP);
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void handleClientConnections() {
  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.processNextRequest();
  }
  server.handleClient();
  
}

void stopWifiAP() {
  if (WiFi.getMode() != WIFI_AP) {
    Serial.println("not in ap mode");
    return;
  }
  server.stop();
  WiFi.softAPdisconnect(true);
};

uint8_t enableWifiStation(const char* ssid, const char* pwd)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pwd);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 15) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_OFF);
    return 0;
  }
  server.begin();
  return 1;
};
