Generic (AVR) code for ILI9341 on gitHub
https://gist.github.com/postmodern/ed6e670999f456ad9f13
  > postmodern/ili9341.c


Generic code for ILI9341

1) test on ESP32
    > format [v] + copyViaSerial some assets [x]
    > test mcu#2 code + wiring               [v]
    > test this code on same ESP32           [v]

2) try to compile on Omega2+

3) connect screen to Omega2+

4) check w/ an SD card (Cf SPI sharing)

________________________________________________________

When will have ~GPU & I2C controls reader
 > try to code a Lua impl. in cpp
   > try to code outrun
   > try to code simcityx
   > try to code gtrus

________________________________________________________




https://docs.onion.io/omega2-docs/first-time-setup.html#unboxing-and-getting-the-hardware-ready

Your Omega’s name is Omega-ABCD where ABCD are the last four digits from the sticker.

So the Omega from the picture above is named Omega-5931

Connect to the Omega’s WiFi Network

The Omega hosts it’s own WiFi network access point. Lets connect your computer to it. The WiFi network is named the same as your Omega and the default password is 12345678

The Setup Wizard

Open your favourite browser and navigate to http://omega-ABCD.local/ where ABCD are the same characters from the network name above. If the page doesn’t load, you can also browse to http://192.168.3.1

You have now arrived at the Setup Wizard:

Login with the Omega’s default credentials:

username: root
password: onioneer


