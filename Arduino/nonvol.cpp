#include "nonvol.h"

_Nonvol Nonvol;

boolean is_str(char * str_ptr, unsigned int str_len)
{
    for(unsigned int i = 0; i < str_len; i++)
    {
        char c = str_ptr[i];
        if( c == 0 ) return true;
        if( c < 32 || c > 126 ) return false;
    }
    return false;
}
