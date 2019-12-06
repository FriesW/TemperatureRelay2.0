import asyncio
import packet
import udp_server


def handler(data):
    print(packet.parse_packet(data))
    return packet.make_ack_packet()


if __name__ == '__main__':

    udp_server.create_udp_server(handler)

    loop = asyncio.get_event_loop()
    print('running!')
    loop.run_forever()
