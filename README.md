# nrf24l01-self-made-library-arduino

When I was working to build flight controller from scratch, I need a robust library and hardware to support rf communication.
I decided to use a readily available nrf module for this, but at the time I couldn't find any reliable library to do the heavy lifting for me.

And since I wanted to also desperately build the complete project from scratch, I wrote my own driver to communicate on SPI interface to the nrf24l01 module, to relay data to and fro the flight controller.

The code may be used as-is and with minor modification inside the void loop() function you can integrate it with your project.
