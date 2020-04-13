#ifndef _WirelessFramework_H_Included_
#define _WirelessFramework_H_Included_

#include <ESP8266WebServer.h>


void setupWifiFramework();

struct WFAPModeConfig 
{
  ///time for auto-shutdown of AP mode. 0 - no shutdown
  int AutoShutdownTimeSec; 
  ///automatic AP shutdown after client disconnects
  uint8_t AutoShutdownAfterDisconnect;
};

struct WFConfig {
  uint8_t DefaultWifiMode; //WIFI_AP, WIFI_OFF, WIFI_STA
};
 
void enableWifiAP(WFAPModeConfig cfg);

///shuts down Wifi AP
void stopWifiAP();

void handleClientConnections();

///returns 1 if connected, 0 else.
uint8_t enableWifiStation(const char* ssid, const char* pwd);

extern ESP8266WebServer server;

#endif
