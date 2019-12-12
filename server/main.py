import asyncio
import server_sensor


if __name__ == '__main__':

    server_sensor.setup()

    loop = asyncio.get_event_loop()
    print('running!')
    loop.run_forever()
