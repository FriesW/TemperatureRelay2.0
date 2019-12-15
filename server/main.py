import asyncio
import uvloop
import server_sensor
import server_web


if __name__ == '__main__':

    asyncio.set_event_loop(uvloop.new_event_loop())

    server_sensor.setup()
    server_web.setup()

    loop = asyncio.get_event_loop()
    print('running!')
    loop.run_forever()
