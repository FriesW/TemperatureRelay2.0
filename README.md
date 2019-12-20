# TemperatureRelay

Measures and reports the temperature of a remote location. Results are viewable via a webpage.


## General Overview

Remote hardware consists of an Arduino programmed ESP8266 with a DS18B20 temperature sensor, and an Internet connection.

The server side consists of a Python script running SQLite, a UDP server, and a HTTP server.

The ESP8266 reads the temperature at regular intervals and sends its data to the UDP server.
SQLite database is used for persistent storage of datapoints.
The HTTP server provides a small collection of webpages.


## Features

* HTTP server
    * Dynamically generated page for viewing recent history
    * CSV download of data, with basic time filter
    * Status page with HTTP status codes based upon remote sensor timeout
* Remote sensor
    * Reliable and high accuracy digital sensor with integrated ADC
    * Configuration mode for in-field servicing and re-tasking
    * IP or domain identified reporting endpoint
    * LED indicators for current operational status and common faults
    * Serial over USB for debugging
    * Powered by 5v over USB
* Custom UDP protocol
    * Light network load
    * PSK HMAC-SHA256 for authentication of communication endpoints and data
    * Gracefully handles packet loss


## Web Service

A dynamically generated webpage is available at the root path. This page is human readable and shows relevant information.
The page contains an overview, a graph for recent temperatures over time, tables for daily, weekly, and monthly lists, and links for downloading CSV data.
Additionally, the webpage will remind the user when it is old and expired with a banner message. Note the webpage is a single resource, even including the _lack_ of favicon, and javascript is not used.

The path `/csv?t=[epoch]` provides a way to download data. All datapoints since `[epoch]` are provided in the output.
The output contains rows of epoch time of measurement and integer values of 128th degrees Celsius.

The path `/isup` can be used to monitor the health of the service.
If the remote sensor has recently made contact the HTTP status code `200` will be returned.
If the remote sensor has not made contact in the last 45 minutes the HTTP status code `503` will be returned (note other pages will still work).
A human readable text description is also provided in both cases.
This path is useful for automated monitoring tools or services, such as [UptimeRobot](https://uptimerobot.com/).


## Remote Sensor

The remote sensor has two modes: normal mode and configuration mode.
Normal mode collects temperature readings every 10 minutes and reports them to the configured endpoint via the configured wifi AP.
Configuration mode allows the changing of settings in-field with a second device capable of wifi STA (client mode) and running a basic web browser.

Normal mode is entered when the remote sensor is powered on and the button is not held.
If the button is held during normal mode operation, the LEDs will eventually illuminate
showing the status of the unit.

From case grill towards the button, illumination of each LED means:
1. Temperature is acquired
    * DS18B20 integrated circuit is operational
1. Local wifi AP is accessible
    * Connection to the local network was recently possible
1. Remote service IP is known
    * Reporting endpoint was successfully parsed as an IP address, or its DNS lookup was successful
1. Time is known
    * Sensor has conducted initial communication transaction and learned the time
1. Data successfully sent
    * Sensor was able to send a UDP packet into the local wifi network
1. ACK successfully received
    * Sensor has received a valid ACK in response to the last sent data

To enter configuration mode, apply power to the remote sensor with the button held.
When configuration mode is entered the status LEDs will blink continuously.
A wifi AP with SSID `Temp Sensor Config` will become available from the remote sensor.
The remote sensor will also run a HTTP server to provide a configuration web page and a DNS server to help find the configuration page.
Using a second device connect to the SSID and goto `http://192.168.4.1` or `http://neverssl.com` in a web browser.
The configuration page should now load.
Transitioning from configuration to normal mode currently requires a power cycle.

The following settings are configurable:
* Upstream Internet SSID and PSK
* Reporting endpoint domain, hostname, or IP address
* Reporting endpoint port
* Reporting endpoint service PSK


## UDP Protocol

The protocol is of client server relationship, where the remote sensor is the client and the reporting endpoint is the server.
Each transaction consists of a request packet sent from client to server, followed by an ACK packet sent from server to client.

The packet structure is as follows:

|  Name  |  Byte Size  |  Type  |  Repetition (`n`)  |
| ------ |:------:| ------ |:------:|
|  `hmac`  |  8  |  `char`  |  1  |
|  `version`  |  1  |  `uint8_t`  |  1  |
|  `const_id`  |  4  |  `uint32_t`  |  1  |
|  `nonce`  |  4  |  `uint32_t`  |  1  |
|  `epoch_time`  |  4  |  `uint32_t`  |  1  |
|  `sample`  |  2*`n`  |  `int16_t`  |  0-720  |

ACK packets will never have the `sample` field.

Authentication of data and endpoints is provided by a HMAC. The HMAC is the first 8 bytes of the `SHA256(PSK + packet)` digest,
where `packet` does not include the HMAC field.

The `version` field is currently `0x01`. The `const_id` field is currently `0x37AD`.

The `nonce` field is random data. It should not be used by the packet's recipient other then for the purposes of HMAC verification.

The `epoch_time` field is the epoch time of the earliest temperature reading in the `sample` field for request packets.
For ACK packets the `epoch_time` field is the epoch time at ACK packet transmission.

When client does not know the time, it will only issue request packets without the `sample` field and
with the `epoch_time` field set to 0.
The client will wait for an ACK packet, and use the time field from the ACK packet to learn the time.
After this the client may send request packets with the `sample` field.

The request packet contains an ordered list of temperature readings in the `sample` field.
Position 0 of the `sample` field is the first reading chronologically.
The `epoch_time` field is the epoch time at measurement of the first reading in the packet.

ACK packets are 49 bytes on the wire (20 ip + 8 udp + 21 proto), and request packets are usually 51 bytes on the wire (20 ip + 8 udp + 23 proto).
Usually a transaction occurs every 10 minutes.
Thus, for 30 days of continuous operation approximately 425 KB of data is transacted.

If configured to use a domain name for the remote endpoint, a DNS lookup occurs once every 24 hours.


<!--
## Software environment

TODO Python version, lib versions/source

TODO arduino versions, board manager version/source, lib versions/source
-->

## Hardware

BOM:
* ESP8266 NodeMCU
* DS18B20(+) temperature sensor
* 4.7 kOhm resistor
* 6 LEDs and current limiting resistors
* 1 momentary normally open push button
* An Internet connection available via a wifi AP

The 4.7 kOhm resistor is used as a pullup on the one-wire databus between the ESP and DS18B20.
The resistor should connect between 3.3v and the `PIN_D_BUS` pin. Wire the sensor according to its [datasheet](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf).
Consider placing the sensor with a few inches of physical separation from the other parts to prevent
their heat from influencing readings.

The push button is connected between the `PIN_BUTTON` pin and ground.
The button may be omitted, but then config mode cannot be entered and status cannot viewed.

The LEDs are connected between each `PIN_LEDS` pin and 3.3v in series with a pullup resistor.
The LEDs may be omitted, but then the status cannot be viewed.

Other ESP based modules could probably be used by modifying the pin assignments in `pin_map.h`.
Currently, no pins require interrupt capabilities, though `PIN_BUTTON` was purposefully select for this capability.

Any Internet connection may be used, but a mobile hotspot would be a flexible choice.
Due to the nature of the system, very little data is needed.
Thus an IoT dataplan service for the hotspot might be ideal, such as from [Hologram](https://hologram.io/).


## Case

The case is laser cut from 3mm plywood. A 2mm piece of acrylic with
[tape](https://www.staples.com/Post-it-Full-Adhesive-Roll-White-1-x-400-2650W/product_885359)
for diffusion
is placed between the LEDs and interior-top of the case.

The case is glued together except for the two farthest apart ends.
The ends are held together by a nut and a screw which is passed through the entire case.

<!-- TODO screw information. -->


## Future improvements

* Remote sensor
    * Better internal heat management
    * Use interrupt on button so status can be viewed instantly
    * Allow mode transitions without power cycle
    * Improved scheduler
    * Remove unused code
* Python server
    * General code cleanup, especially in webpage generation
    * Add gzip compression for webpage serving
    * Improved logging
    * Add password protection option
* Protocol improvements
    * Improve authentication method to prevent session hijacking on first communication
    * Encrypt transmissions to improve privacy


## Predecessor

This was created to replace [a prior](https://github.com/FriesW/TemperatureRelay) system.
The previous system had a variety of issues, such as
configuration being compiled into remote unit, difficulty debugging errors, unreliability of the sensor,
server side complexity, and reliance on third party server resources.
