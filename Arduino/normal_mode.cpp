#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "normal_mode.h"

#include "scheduler.h"
#include "sensor.h"
#include "packet_type.h"
#include "uplink.h"
#include "timer.h"
#include "errors.h"
#include "status.h"

#define S_SENSE (0)
#define S_WIFI (1)
#define S_IP (2)
#define S_TIME (3)
#define S_SEND (4)
#define S_ACK (5)

static Task status;
static Task temp_read;
static Task net;

static void status_fn();
static void temp_read_fn();
static void net_fn();


void normal_mode()
{
    wifi_set_sleep_type(LIGHT_SLEEP_T);

    status.init(status_fn, 5, 20, "Status");
    temp_read.init(temp_read_fn, 0, 10*60, "Sensor reading");
    net.init(net_fn, 10, 5*60, "Net");

    // Sensor.init();
    Uplink.init(Nonvol.data.port);

    Scheduler.run();
}


static void status_fn()
{
Status.flash();
}


static uint16_t reading_count = 0;
static int16_t readings[PKT_MAX_SAMPLE_COUNT] = {0};


static void temp_read_fn()
{
    if(reading_count >= PKT_MAX_SAMPLE_COUNT)
    {
        Serial.println("Sample queue is full");
        memmove(&readings[0], &readings[1], (PKT_MAX_SAMPLE_COUNT-1) * sizeof(int16_t));
        reading_count--;
    }

    int16_t res = Sensor.read();
    if(res == SENSOR_ERROR)
    {
        Status.off(S_SENSE);
        return;
    }

    Status.on(S_SENSE);
    readings[reading_count] = res;
    reading_count++;
}


static error_state wifi_on();
static void wifi_off();

static void net_fn()
{
    static Timer dns_ttl;
    static IPAddress ip;

    if( reading_count == 0 )
    {
        Serial.println("Empty queue, nothing to send");
        return;
    }

    Status.off(S_WIFI);
    if( wifi_on() != NO_ERROR )
    {
        Serial.println("Error connecting to wifi");
        wifi_off();
        return;
    }
    Status.on(S_WIFI);
    Serial.println("Connected to wifi");

    Status.off(S_IP);
    if( dns_ttl.expired() )
    {
        Serial.println("Refreshing dest IP");
        dns_ttl.set(24 * 60 * 60 * 1000);
        if( !ip.fromString(Nonvol.data.host) )
        {
            if( !WiFi.hostByName(Nonvol.data.host, ip) )
            {
                Serial.println("Error getting dest IP addr");
                wifi_off();
                return;
            }
            else Serial.println("Success loading from DNS");
        }
        else Serial.println("Success loading immediate IP addr");
    }
    Status.on(S_IP);

    error_state res;

    Status.off(S_SEND);
    res = Uplink.send_data(
        readings, reading_count, 0, ip ); //TODO the time!
    if( res != NO_ERROR )
    {
        Serial.print("Error sending pkt: ");
        Serial.println(res);
        wifi_off();
        return;
    }
    Status.on(S_SEND);

    res = Uplink.wait_for_ack(5000);
    if( res != NO_ERROR )
    {
        Status.off(S_ACK);
        Serial.print("Error waiting for ack: ");
        Serial.println(res);
    }
    else
    {
        Status.on(S_ACK);
        reading_count = 0;
    }

    wifi_off();
}

static error_state wifi_on()
{
    Timer to;
    WiFi.mode(WIFI_STA);
    WiFi.begin(Nonvol.data.ssid, Nonvol.data.apsk);
    to.set(30*1000);

    while( to.running() && WiFi.status() != WL_CONNECTED )
        delay(100);
    if( WiFi.status() == WL_CONNECTED )
        return NO_ERROR;

    wifi_off();
    return E_NO_NET;
}

static void wifi_off()
{
WiFi.disconnect();
WiFi.mode(WIFI_OFF);
}
