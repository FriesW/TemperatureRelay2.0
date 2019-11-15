#ifndef _NTP_H
#define _NTP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "timeout.h"
#include "errors.h"

#define NTP_PACKET_SIZE 48

class _NTP {

public:

    error_state network_update(IPAddress& server_ip)
    {
        Timeout timeout;
        uint rb;
        unsigned long candidate_update;

        if( WiFi.status() != WL_CONNECTED )
            return NO_NET;

        udp.begin(localPort);

        rb = 0;
        for(uint i = 0; rb == 0 && i < 5; i++)
        {

            send_packet(server_ip);
            timeout.init(1500);
            while( rb == 0 && timeout.valid() )
            {
                candidate_update = millis();
                rb = udp.parsePacket();
            }
        }
        if( rb == 0 )
        {
          return TIMEOUT;
        }

        udp.read(packetBuffer, NTP_PACKET_SIZE);

        //the timestamp starts at byte 40 and is four bytes long
        //form ulong and convert NTP time (Jan 1 1900) to unix time
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        epoch = (highWord << 16 | lowWord) - 2208988800UL;
        last_update = candidate_update;

        return NO_ERROR;
    }

    void soft_update()
    {
        unsigned long now = millis();
        unsigned long delta = now - last_update;
        epoch += delta / 1000;
        last_update = now - (delta % 1000);
    }

    unsigned long now()
    {
        soft_update();
        return epoch;
    }


private:

    unsigned long last_update;
    unsigned long epoch;
    byte packetBuffer[NTP_PACKET_SIZE];
    WiFiUDP udp;

    unsigned int localPort = 2390;

    void send_packet(IPAddress& address) {
      Serial.println("sending NTP packet...");
      // set all bytes in the buffer to 0
      memset(packetBuffer, 0, NTP_PACKET_SIZE);
      // Initialize values needed to form NTP request
      // (see URL above for details on the packets)
      packetBuffer[0] = 0b11100011;   // LI, Version, Mode
      packetBuffer[1] = 0;     // Stratum, or type of clock
      packetBuffer[2] = 6;     // Polling Interval
      packetBuffer[3] = 0xEC;  // Peer Clock Precision
      // 8 bytes of zero for Root Delay & Root Dispersion
      packetBuffer[12]  = 49;
      packetBuffer[13]  = 0x4E;
      packetBuffer[14]  = 49;
      packetBuffer[15]  = 52;

      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      udp.beginPacket(address, 123); //NTP requests are to port 123
      udp.write(packetBuffer, NTP_PACKET_SIZE);
      udp.endPacket();
    }

};

#undef NTP_PACKET_SIZE

_NTP NTP;

#endif
