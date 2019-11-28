#ifndef _ERRORS_H
#define _ERRORS_H

typedef enum
{
    NO_ERROR = 0,
    E_NO_NET,
    E_TIMEOUT,
    E_PKT_SIZE,
    E_PKT_VERSION,
    E_PKT_CONST_ID,
    E_PKT_HMAC,

} error_state;

#endif
