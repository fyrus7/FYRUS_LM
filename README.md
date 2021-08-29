# FYRUS LM / Arduino Photography Light Meter
Incident Light Meter &amp; White Balance for Photography


# Highlite

1. Aperture Priority.
2. Shutter Priority.
3. Flash Mode.
4. ND Filter Mode.
5. Temperature Reading (White Balance).
6. EV Reading.
7. Lux Reading.
8. Battery Reading (appear after button is pressed to get reading)

- Aperture: f/1.0 to f/22.0
- Shutter: 1/10000 to 133 sec.
- ISO: 8 - 4 000 000



# What you need:

1. Arduino Nano.

2. 0.96" Oled Display.
- 7 pin or 4 pin both can use.
- i have write both the code. You just need to choose which to use.
- NOTE: 7 pin is faster but use more pin to soldered.

3. BH1750FVI Light Sensor with Dome.
 - (minor change in code if you use BH1750 without dome)

4. TCS34725 RGB Sensor.

5. 6x Tactile Button.

6. Lipo 1S 3.7v Battery.

7. (optional) Electronic enclosure case.

-

# to do :
- (i failed this due to the lack of knowledge)
- if anyone got the idea please update the code

- one tactile & one rotary switch with push button.

1. Instead of 6 button, make...
- ONE Tactile Button for Metering
- ONE (EC11) Rotary Switch with Push Button

2. Rotary Switch to control...
- plus.
- minus.
- single click to switch between aperture/shutter priority.
- double click to enter ISO/ND setting.
- long press to switch between Ambient/Flash mode.
- 


this Software is originally based from https://github.com/vpominchuk/lightmeter
