
#include <ESP8266WiFi.h>
#include "WirelessFramework.h"

#define LED1_PIN LED_BUILTIN
#define LED2_PIN 16
#define KRAN1_PIN 4 //gpio 4 = D2
#define KRAN2_PIN 5 //gpio 5 = D1

typedef struct TOutPin {
  const char* Name;
  uint8_t Gpio;
  uint8_t InactiveLevel;
} TOutPin;

TOutPin Pinz[] = {
  {
    "LED 1",
    LED1_PIN,
    1
  },
  {
    "Kran 1",
    KRAN1_PIN,
    1
  },
  {
    "Kran 2",
    KRAN2_PIN,
    1
  }
};

#define MAXPIN sizeof(Pinz) / sizeof(TOutPin)


void handleLed() {
  int idx = atoi(server.arg("led").c_str());
  uint8_t pin = Pinz[idx].Gpio;
  uint8_t l = server.arg("mode") == "off" || server.arg("mode") == "0" ? 0 : 1;
  if (Pinz[idx].InactiveLevel != 0) l = l ? 0 : 1;
  digitalWrite(pin, l);
  Serial.print("Set pin ");
  Serial.print(pin);
  Serial.print(" to ");
  Serial.println(l);
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
}

void handleRoot() {
  String message = "<html>";
  char buf[100];
  message += "<head><style>body {font-size:140%;} a.led {margin:10px;padding:8px;border: solid 1px black; display:block;} a.off {background-color: #303030;color:white} a.on {background-color:#FFC0C0;color:black}</style></head>";
  message += "<body>";
  message += "<h1>Podlewacz 1</h1>";
  for (int i=0; i<MAXPIN; i++) 
  {
    int v = digitalRead(Pinz[i].Gpio);
    if (Pinz[i].InactiveLevel != 0) v = v ? 0 : 1;
    const char* ss = (v == 0 ? "off" : "on");
    sprintf(buf, "<div><a href=\"led?led=%d&mode=%s\" class=\"led %s\">%s %s</a></div>", i, (v == 0 ? "on" : "off"), ss, Pinz[i].Name, ss);
    message += buf;
  }
  
  message += "<div><a href=\"wifi\">Wifi setup</a></div>";

  message += "<br/>";
  message += "</body></html>";
  server.send(200,  "text/html", message);
}

void handleWifiSetup() 
{
  String err = "";
  if (server.method() == HTTP_POST) {
    if (enableWifiStation(server.arg("ssid").c_str(), server.arg("pass").c_str())) 
    {
      server.sendHeader("Location", String("/"), true);
      server.send ( 302, "text/plain", "");
      return;
    }
    else {
      err = "Failed to connect to WIFI ";
      err += server.arg("ssid");
    }
  }
  String m = "<html><head><style></style></head><body>";
  m += "<form method=\"post\">";
  m += "<table cellpadding=\"8\" border=\"1\"><tr><td>SSID:</td>";
  m += "<td><input type=\"text\" name=\"ssid\" value=\"\" /></td></tr>";
  m += "<tr><td>Pass:</td>";
  m += "<td><input type=\"password\" name=\"pass\" value=\"\" /></td>";
  m += "</tr></table>";
  m += "<input type=\"submit\" value=\"OK\" />";
  m += "</form></body>";
  //server.sendHeader("Location", String("/"), true);
  server.send ( 200, "text/html", m);  
}



/*
 * 
 */
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("led built in:");
  Serial.println(LED_BUILTIN);
  for (int i=0; i<MAXPIN; i++) 
  {
    pinMode(Pinz[i].Gpio, OUTPUT);
    digitalWrite(Pinz[i].Gpio, Pinz[i].InactiveLevel);
  }
  Serial.println("Gpio initd");
  
  server.on("/led", handleLed);
  server.on("/", handleRoot);
  server.on("/wifi", handleWifiSetup);

  // put your setup code here, to run once:
  Serial.println("Start");
  setupWifiFramework();
  Serial.println("Wifi ap configured");
  if (!enableWifiStation(NULL, NULL)) {
    Serial.println("Switching to AP");
    WFAPModeConfig cfg = {
      .AutoShutdownTimeSec = 120,
      .AutoShutdownAfterDisconnect = 1
    };
    enableWifiAP(cfg);
    Serial.println("AP enabled");  
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
  handleClientConnections();
  //Serial.println("...im ok");
}
