# OMSI's Brain Connections

## Next Steps
* update accelerometer code to use timers instead of delays
* update all the code to use timers instead of delays

## Other Todos
* consolodate notes
* write a good readme
* document High Level Design

## Known Bugs
none atm

# Flashing new firmware
The Itsybitsy32u4_5v is a litte finiky when it comes to flashing. The process that works most of the time for me is to double press the reset button
and then super quickly upload the code. Like within 300milliseconds or less. Maybe disabling serial.print()s would help? 

# Troubleshooting Tips
Removing and reinserting microcontroller is delicate and needs you to support the bottom of the mainboard with your hand.

## Fun Ideas
* Easter Egg Color pallette swap
* Sensor held on startup could change colors
* Handheld brain-connections game-thing