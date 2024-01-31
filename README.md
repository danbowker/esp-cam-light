## About the project
This repo contains code for an a web-cam 'busy' light built using a [ESP32](https://en.wikipedia.org/wiki/ESP32) or [ESP8266](https://en.wikipedia.org/wiki/ESP8266) dev board.

The code uses the wonderful [PlatformIO](https://platformio.org/) platform.

The code here monitors a web server that provides the status of the webcam and turn an LED strip on or off as appropriate.

I have an [accompanying repo](https://github.com/danbowker/WebCamServer/tree/master) that contains an example webserver that exposes the camera status from a Windows 11 machine.