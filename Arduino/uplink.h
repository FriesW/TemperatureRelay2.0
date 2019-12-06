#ifndef _UPLINK_H
#define _UPLINK_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "packet_type.h"
#include "sha256.h"
#include "nonvol.h"
#include "timer.h"
#include "errors.h"

class _Uplink {

public:

    void init(unsigned int port)
    {
        if(init_state) return;
        init_state = true;
        local_port = port;
        udp.begin(local_port);
    }

    error_state send_data(int16_t data[], uint count, uint32_t time, IPAddress dest_addr)
    {
        if(!init_state) return E_INIT;

        if( count > PKT_MAX_SAMPLE_COUNT )
            return E_PKT_SIZE;

        count = min(count, (uint)PKT_MAX_SAMPLE_COUNT);
        uint count_b = count * sizeof(uint16_t);
        uint size_b = PKT_HEADER_SIZE + count_b;

        memcpy( pkt.samples, data, count_b );

        pkt.version = 1;
        pkt.const_id = PKT_CONST_ID;
        pkt.nonce = 0xBEEF;//TODO get rand
        pkt.epoch_time = time;//NTP.now();

        pkt_hmac(size_b);
        memcpy(pkt.hmac, sha_hash.output, 8);

        udp.beginPacket(dest_addr, local_port);
        udp.write((byte *)&pkt, size_b);
        udp.endPacket();

        return NO_ERROR;
    }

    error_state wait_for_ack(uint wait_time)
    {
        if(!init_state) return E_INIT;

        Timer timeout;
        uint rb;

        rb = 0;
        timeout.set(wait_time);
        do
        {
            rb = udp.parsePacket();
        }
        while( rb == 0 && timeout.running() );

        if( rb == 0 )
            return E_TIMEOUT;

        // if( rb < PKT_HEADER_SIZE || rb > MAX_PKT_SIZE ||
        //     (rb - PKt_HEADER_SIZE) % 2 != 0 )
        if( rb != PKT_HEADER_SIZE )
            return E_PKT_SIZE;

        udp.read((byte *)&pkt, rb);

        if( pkt.version != 1)
            return E_PKT_VERSION;
        if( pkt.const_id != PKT_CONST_ID )
            return E_PKT_CONST_ID;

        pkt_hmac(rb);
        for(uint i = 0; i < 8; i++)
            if( pkt.hmac[i] != sha_hash.output[i] )
                return E_PKT_HMAC;

        return NO_ERROR;

    }

private:

    WiFiUDP udp;

    boolean init_state = false;
    unsigned int local_port;

    Sha256 sha_hash;
    packet_type pkt;

    void pkt_hmac(uint size)
    {
        sha_hash.reset();
        if( is_str(Nonvol.data.hmac_key, HKEY_LEN) )
            sha_hash.update((BYTE *)Nonvol.data.hmac_key, strlen(Nonvol.data.hmac_key));
        sha_hash.update(((BYTE *)(&pkt)) + 8, size - 8);
        sha_hash.digest();
    }

};

extern _Uplink Uplink;

#endif
