#ifndef _PACKET_TYPE_H
#define _PACKET_TYPE_H

#include <Arduino.h>

#define PKT_MAX_SAMPLE_COUNT (720)  //5 days worth of samples
#define PKT_CONST_ID (0x37AD)


typedef struct __attribute__((__packed__))
{
    char hmac[8];
    uint8_t version;
    uint32_t const_id;
    uint32_t nonce;
    uint32_t epoch_time;
    int16_t samples[PKT_MAX_SAMPLE_COUNT];
} packet_type;

#define PKT_MAX_SIZE (sizeof(packet_type))
#define PKT_HEADER_SIZE (PKT_MAX_SIZE - PKT_MAX_SAMPLE_COUNT * sizeof(uint16_t))

#endif
