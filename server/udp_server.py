import asyncio
from config import SERVICE_PORT

def create_udp_server(cb):
    class UdpServerHandler:
        def connection_made(self, transport):
            self.transport = transport
        def datagram_received(self, data, addr):
            resp = cb(data)
            if type(resp) == bytes:
                self.transport.sendto(resp, addr)

    loop = asyncio.get_event_loop()
    listen = loop.create_datagram_endpoint( UdpServerHandler, local_addr=('0.0.0.0', SERVICE_PORT) )
    transport, protocol = loop.run_until_complete(listen)
