import udp_server
import packet
import database as db
import time


def __handler(data):
    status, data = packet.parse_packet(data)
    print(status, data)
    if status:
        epoch, temps = data
        __process(epoch, temps)
        return packet.make_ack_packet()

def __process(epoch, temps):
    if epoch == 0:
        print("zeroed time")
        return
    if len(temps) == 0:
        print("zero length temp list")
        return
    if epoch < time.time() - 60 * 60 * 24 * 15: #15 days
        print("old time")
        return

    recent_t = 0
    recent = db.most_recent()
    if recent: recent_t, _ = recent

    for t in temps:
        if epoch < recent_t:
            print("time {:,} before most recent entry at {:,}".format(epoch, recent_t))
            continue
        if not db.insert(epoch, t):
            print("failed to insert:", epoch, t)
        epoch += 60 * 10

def setup():
    udp_server.create_udp_server(__handler)
