#include <ESP8266WiFi.h>
#include "errors.h"
//#include "nonvol.h"
#include "NTP.h"

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

// Serial.println(Nonvol.data.ssid);
// Serial.println(Nonvol.data.apsk);
// Serial.println(Nonvol.data.port);
// Serial.println(Nonvol.data.host);
}

void loop()
{
    error_state s = NTP.network_update(nist);
    Serial.print("Network NTP status: "); Serial.println(s);
    for(uint i = 0; i < 300; i++)
    {
        Serial.println(NTP.now());
        delay(750);
    }
}
