# SEC DAQ
A 4-channel, 100 pF capacitance DAQ with active shielding designed for Soft Elastomeric Capacitor (SEC) sensors.

<p align="center">
<img src="image.png" alt="drawing" width="450"/>
</p>
<p align="center">
</p>

This data acquisition can measure SECs in the range of 15-115 pF, with an asynchronous sampling speed of 400 S/s on a single channel.

## Hardware design notes

### v0.9.0
1. Contains all the older files. 
1. Contains the 2-board PCB design

### v0.9.1
1. This is the final version of the internal development. 
1. A 4-channel SEC DAC with isolated signal

### v0.9.2 (future version)
Improvements needed

#### High importance
1. The files needs to be cleaned up, and put back into a single KiCad file.
1. Compress the board layout so the DAQ sits inside the Arduino shield. (Reduces the wasted space)
1. Remove the ground tabs as adding a shield is a further work outside of these single box DAQs
1. Use a lower current draw opotcoupler, I think a CPC1301 should work. 
 
#### Medium importance
1. Move trigger housing into the board so it does not hang over.
1. Change the surface mount D-sub to a edge mount D-sub with signal/shield on top and ground on bottom. 
1. Add a switch for the time stamp so the DAQ only returns a time stamp if the switch in enabled. Will require a 14 pin IDC connector.

#### Low importance 
1. Enlarge the CDC filter capacitor solder pads
1. Add LED to the trigger circuit to help with trouble shooting (check current draw problems)

















