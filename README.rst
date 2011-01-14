Thermduino
==========

Thermduino is a simple arduino+python project which I hacked together in a few
days to set a rotary thermostat based on time and date.

The arduino is able to function without being connected to a computer once its
clock has been set (which is facilitated by connecting it to the computer running
the appropriate software). It can follow a set of programmes (rules) which state
what temperature it should set the thermostat at what time. If it looses power
it can remember what day it was last on and without computer assistance it will
do as best it can with its inaccurate time information.

The desktop client is an (extremely messily coded) wxWidgets GUI which provides
the ability to change the default program and to set the programmes and
calibration information on the device. It can be hidden into the system tray
where it will keep the clock on a connected arduino in time if it drifts more
than 60 seconds.

Programmes can be expressed using a natural-language-ish expression along the
lines of::
	
	10*c on saturdays and sundays
	20*c from 11/12/2012 4am to 1/1/2013

Code Quality (a disclaimer...)
------------------------------

The code in this project is very rough and many areas need refactoring and
documenting, particularly in the python client. Oh, and about that regular
expression...


