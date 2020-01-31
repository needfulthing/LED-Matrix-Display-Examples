# LED-Matrix-Display-Examples
Code for various demos for an LED matrix panel.

# LED_Panel_64x32_Lava_Lamp_Effect.ino
Tested on a Wemos D1 Mini using the configuration from https://www.instructables.com/id/RGB-LED-Matrix-With-an-ESP8266/. The code still has a problem to produce flickering (erased screen between frames) without a recurring delay while writing pixels to the screen. Doing less delay starts the flicker with a down going frequency the less delays there are. This is not an issue of a late yield() call - the code also runs through with only one delay per frame, but then the output flickers.
