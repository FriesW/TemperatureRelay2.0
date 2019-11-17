#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ConfigMode.h"
#include "nonvol.h"

static ESP8266WebServer server(80);

static const char * ssid = "Temp Mon Config";
static const char * pass = "temppass"; //TODO

static const char * webpage =
"<html><body>"
"<h1>Temperature Monitor Settings</h1>"
"<form action='/' method='post'>"
"Internet AP SSID: <input type='text' name='ssid' maxlength='32' value='%s'><br>"
"Internet AP password: <input type='text' name='apsk' maxlength='32' value='%s'><br>"
"Report to domain/host/ip: <input type='text' name='host' maxlength='32' value='%s'><br>"
"Report to port: <input type='number' name='port' min='0' max='65535' value='%d'><br>"
"<input type='submit' value='Apply'>"
"</form>"
"</body></html>";

static char temp_buf[750];

static void handleRoot();

void ConfigMode()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);
    server.on("/", handleRoot);
    server.begin();
    while(true)
    {
        server.handleClient();
    }

}

static void handleRoot()
{
    boolean modified = false;
    if( server.hasArg("ssid") ) { modified = true;
        server.arg("ssid").toCharArray(Nonvol.data.ssid, SSID_LEN); }
    if( server.hasArg("apsk") ) { modified = true;
        server.arg("apsk").toCharArray(Nonvol.data.apsk, APSK_LEN); }
    if( server.hasArg("host") ) { modified = true;
        server.arg("host").toCharArray(Nonvol.data.host, HOST_LEN); }
    if( server.hasArg("port") ) { modified = true;
        Nonvol.data.port = server.arg("port").toInt(); }

    if( modified ) Nonvol.save();

    if( Nonvol.is_valid() )
        snprintf(
            temp_buf, sizeof(temp_buf), webpage,
            Nonvol.data.ssid, Nonvol.data.apsk, Nonvol.data.host, Nonvol.data.port
        );
    else
    snprintf(
        temp_buf, sizeof(temp_buf), webpage,
        "", "", "", 0
    );
    server.send(200, "text/html", temp_buf);
}
