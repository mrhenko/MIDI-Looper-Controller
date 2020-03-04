# MIDI-to-LOOPER #

or maybe LOOPER-to-MIDI is a hardware and software solution for controlling the __Looper__ plugin in Ableton Live. The hardware is an Arduino Micro powered foot controller with footswitch and RGB LED. The idea is to make a version with two footswitches and two LEDs, enabling control over two separate Looper instances. During this part of the development process though, I'm just using one footswitch and one LED making it a single channel version.

The footswitch sends a MIDI Note On command when being stepped on and it's supposed to be mapped to the big multi function button on the Looper plugin. The Max4Live plugin then watches the state of the Looper (*Stop*, *Rec*, *Play* or *Overdub*) and when the state changes it communicates that to the pedal using MIDI CC. The pedal then changes the color of the LED to correspond with the state[^1].

[^1]: * Stop = not lit at all
* Rec = red
* Play = green
* Overdub = yellow