# e-Paper network image
ESP32 / Arduino application to retrieve an image from the network/web and repeatedly refresh on a Waveshare e-Paper. 

## Introduction
Following the instructions on the pages listed under ['Hardware'](#hardware), I managed to get some functionality working, but not all on my 7.5inch(B) V3 version of the e-Paper.

My goal was to write a WIFI client on the ESP32 that retrieves a picture from a web location (that I generate / update using a separate device) and displays it, goes to sleep for a given interval, and repeatedly wakes up to refresh the image.

The WIFI demo of the ESP32 driver board seemed a good start, but didn't work correctly for my version of the e-Paper. Also, the protocol used for sending/receiving images over WIFI was unnecessarily complicated for my use. Hence, I created a custom file format, a python script to convert images to this format, and a client application to display (and refresh) the image on the e-Paper.

### Hardware
* https://www.waveshare.com/wiki/7.5inch_e-Paper_HAT_(B)_Manual
* https://www.waveshare.com/wiki/E-Paper_ESP32_Driver_Board

### Software
E-Paper_ESP32_Driver_Board_Code\examples\esp32-waveshare-epd\examples\epd7in5b_V2-demo\ from https://files.waveshare.com/upload/5/50/E-Paper_ESP32_Driver_Board_Code.7z

## Approach
Store images in the format described under [Programming Principle](https://www.waveshare.com/wiki/7.5inch_e-Paper_HAT_(B)_Manual#Programming_Principle) with minor meta information, such that they can be retrieved by the ESP32 in that format and forwarded to the e-Paper without further processing.
### Protocol
Define the .br.raw format as:
* 2 bytes to store the number of pixels per channel (N)
* 1 separator byte of high bits (\xff)
* N bytes representing the black-white channel, each bit represents 0-black, 1-white
* N bytes representing the red-white channel, each bit represents 0-red, 1-white

Each channel is a concatenation of pixels over columns, then over rows.

## Notes
Note that the HttpClient from https://github.com/arduino-libraries/ArduinoHttpClient works well to strip off the header, but somehow does not read the body of my GET requests, hence I only use it to strip off the http-header and fall back to reading directly from the underlying WIFI client for reading the request body.

The WIFI demo from the ESP32 driver board does not process the red channel properly (adds some white noise pattern of additional red pixels). Upon reusing the underlying local functionality for loading, showing, initializing etc. I found that it does not work well with the V3 7.5inch e-Paper. It loads one channel well, but the other color keeps stuck at a 'busy' state, not proceeding to 'busy released'.

## Use
* Create a .rb.raw image using the provided python script, and place it at a reachable network location.
* Insert the details of your WIFI network and the network address where the image can be found, in network.h.
* Using the [Arduino IDE](https://www.arduino.cc/en/software), compile/upload using board "ESP32 Dev Module".
