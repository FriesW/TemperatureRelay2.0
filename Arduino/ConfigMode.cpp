#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "ConfigMode.h"
#include "nonvol.h"

static IPAddress ap_ip(192, 168, 4, 1);
static ESP8266WebServer server(80);
static DNSServer dns_server;

static const char * webpage =
"<html><body>"
"<h1>Temperature Monitor Settings</h1>"
"<form action='/' method='post'>"
"Internet AP SSID: <input type='text' name='ssid' maxlength='31' value='%s'><br>"
"Internet AP password: <input type='text' name='apsk' maxlength='31' value='%s'><br>"
"Report to domain/host/ip: <input type='text' name='host' maxlength='31' value='%s'><br>"
"Report to port: <input type='number' name='port' min='0' max='65535' value='%d'><br>"
"Service key: <input type='text' name='key' maxlength='31' value='%s'><br>"
"<input type='submit' value='Apply'>"
"</form>"
"</body></html>";

static char temp_buf[850];

static void handle_redirect();
static void handle_root();

_ConfigMode::_ConfigMode()
{
    running = false;
}

void _ConfigMode::start()
{
    running = true;

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
    WiFi.softAP("Temp Sensor Config");

    server.on("/", handle_root);
    server.onNotFound(handle_redirect);
    server.begin();
    dns_server.start(53, "*", ap_ip);
}

void _ConfigMode::stop()
{
    if(!running)
        return;

    running = false;
    dns_server.stop();
    server.stop();
}

void _ConfigMode::periodic()
{
    if(!running)
        return;

    dns_server.processNextRequest();
    server.handleClient();
}

static void handle_redirect()
{
    server.sendHeader("Location", "/",true);
    server.send(307, "text/plain","redirecting...");
}

static void handle_root()
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
    if( server.hasArg("key") ) { modified = true;
        server.arg("key").toCharArray(Nonvol.data.hmac_key, HKEY_LEN); }

    if( modified ) Nonvol.save();

    if( Nonvol.is_valid() )
        snprintf(
            temp_buf, sizeof(temp_buf), webpage,
            Nonvol.data.ssid, Nonvol.data.apsk, Nonvol.data.host, Nonvol.data.port, Nonvol.data.hmac_key
        );
    else
    snprintf(
        temp_buf, sizeof(temp_buf), webpage,
        "", "", "", 0, ""
    );
    server.send(200, "text/html", temp_buf);
}

_ConfigMode ConfigMode;
