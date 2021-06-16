# Brain Connections Change log

5/14/20 Expanded the number of nodes, connections, orphans
Removed Words
5/20/20
mcu is running out of RAM. 
5/21/20
To reduce RAM usage I replaced the Adafruit Accelerometer library with sparkfun one.
There is still not enough space in SRAM. Converted the Serial Prints
to use F() to wrap strings to make it save into program memory instead of RAM.
Those two changes yielded enough space in SRAM.
5/21/20
Updated the Opto input pins to match the PCBs out of order pins.
5/26/20
Fixing connection segment bugs.
Added a Serial input from computer to trigger Nodes.
6/3/20
Added Individual OPTOtriggerValue for each OPTO
6/24/20 update
Wav trigger audio inputs set to these pins
const int Audio_1 = 8; // Idle Song - hold level high to loop
const int Audio_2 = 9; // Connection on 10 - edge triggered
const int Audio_3 = 10; // Node sound on 9 - edge triggered
const int Audio_4 = 11; // Win sound on 8 - edge triggered
const int Audio_5 = 12; // Start Song on 11 - edge triggered
7/15/20
Modified led type for sensorsout to neopixel GRB
Updated Sensor pins
Updated Audio out pins
11-23-20
Updated sensor trigger values to 350 on line#s105
4-14-21
Fixed Audio trigger pins to play win sound correctly
6-15-21
Updated accelZTriggerValue from 20 to 11 for a more responsive trigger
Fixed Node7 not dimming
