# esp8266_audio_visualizer

A simple audio visualizer using NodeMCU and WS2812B LED strip, inspired by [ESP32-8266-Audio-Spectrum-Display](https://github.com/G6EJD/ESP32-8266-Audio-Spectrum-Display).

## Parts Used

* ESP8266 development board like NodeMCU or Wemos. (Board I used: [link](https://www.ebay.de/itm/NodeMCU-V3-2-Arduino-ESP8266-ESP-12-E-Lua-CH340-WiFI-WLan-IoT-Lolin-Mini-Micro/252718027546?hash=item3ad72b131a:g:5GwAAOSwI7Fb3x~a))

* WS2812B led strip. (Strip I used (2m, 60led/m): [link](https://www.aliexpress.com/item/32820628563.htmlspm=a2g0s.9042311.0.0.2dfa4c4d764m75))

* 2X 10k resistor.

## Circuit

![Circuit](https://github.com/tjohn327/esp8266_audio_visualizer/raw/master/assets/espAudio_bb.png)

## Code

I used the Arduino IDE to program the NodeMCU. Here is a [guide](https://www.teachmemicro.com/intro-nodemcu-arduino/) describing how to setup the Arduino IDE to work with NodeMCU.

The following libraries need to be installed in the Arduino IDE:

* [FastLED](http://fastled.io/)

* [arduinoFFT](https://github.com/kosme/arduinoFFT)

Adjust the NUM_LEDS and colors to suit your lED strip.

The code was put together in an evening, so there is a whole lot of scope for improvement. Sorry about the spaghetti code.

## Demo

The ends of the LED strip responds to low frequencies, the middle part responds to the overall amplitude of the sound and high frequency sounds produce a mild sparkling effect.

[https://youtu.be/BhOv1-67Nhk](https://youtu.be/BhOv1-67Nhk)
