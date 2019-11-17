#include <ESP8266WiFi.h>
#include "errors.h"
//#include "nonvol.h"
#include "NTP.h"
#include "ConfigMode.h"

#include "secrets.h"
const char * ssid = STASSID; // your network SSID (name)
const char * pass = STAPSK;  // your network password

static const char* ntpServerName = "time.nist.gov";
IPAddress nist;

void setup()
{
Serial.begin(115200);
delay(100);
Serial.println();

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, pass);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}

WiFi.hostByName(ntpServerName, nist);

while( NTP.network_update(nist) != 0 );

ConfigMode();
}

void loop()
{
}
