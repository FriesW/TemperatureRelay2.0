import struct
import hashlib
import os, time

KEY = b'test_key'
CONST_ID = 0x37AD

# | hmac | version | const_id | nonce | epoch time | readings  |
# | 8    | 1       | 4        | 4     | 4          | 2 * N     |

def parse_packet(data):
    try:

        # minimum length of 21, for pkt with 0 readings
        # overall packet must be odd in length
        if len(data) < 21 or len(data) % 2 != 1:
            return (False, 'bad size')

        format = '<8sBLLL'
        hmac, version, const_id, nonce, epoch = struct.unpack(format)

        if version != 1:
            return (False, 'unknown version')

        if const_id != CONST_ID:
            return (False, 'bad const id')

        m = hashlib.sha256()
        m.update(KEY)
        m.update(data[8:])
        m = m.digest()
        if m[:8] != hmac:
            return (False, 'bad hmac')

        temps = []
        for i in range(21, len(data)-1, 2):
            temps.append( struct.unpack_from('<H', data, offset = i) )

        return (True, temps)
    except:
        return (False, 'decode exception')


# | hmac | version | const_id | nonce | epoch time |
# | 8    | 1       | 4        | 4     | 4          |

def make_ack_packet():
    base = struct.pack('<BL4sL', 1, CONST_ID, os.urandom(4), int(time.time()))

    m = hashlib.sha256()
    m.update(KEY)
    m.update(base)
    m = m.digest()

    packet = m[:8] + base
    return packet
