# SEC DAQ
A 4-channel, 100 pF capacitance DAQ with active shielding designed for use with the Soft Elastomeric Capacitor (SEC) sensors but could be used for anything. 

<p align="center">
<img src="image.png" alt="drawing" width="450"/>
</p>
<p align="center">
</p>

This data acquisition can measure SECs in the range of 15-115 pF, with an asynchronous sampling speed of 400 S/s on a single channel.

This hardware was used in various papers, including:
1. Downey, Austin, Anna Laura Pisello, Elena Fortunati, Claudia Fabiani, Francesca Luzi, Luigi Torre, Filippo Ubertini, and Simon Laflamme. "Durability and weatherability of a styrene-ethylene-butylene-styrene (SEBS) block copolymer-based sensing skin for civil infrastructure applications." Sensors and Actuators A: Physical 293 (2019): 269-280.
1. Downey, Austin, Mohammadkazem Sadoughi, Simon Laflamme, and Chao Hu. "Fusion of sensor geometry into additive strain fields measured with sensing skin." Smart Materials and Structures 27, no. 7 (2018): 075033.
1. Yan, Jin, Austin Downey, Alessandro Cancelli, Simon Laflamme, An Chen, Jian Li, and Filippo Ubertini. "Concrete crack detection and monitoring using a capacitive dense sensor array." Sensors 19, no. 8 (2019): 1843.

## Hardware design notes

### v0.9.0
1. Contains all the older files. 
1. Contains the 2-board PCB design

### v0.9.1
1. This is stable release. 
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


## Licensing and Citation

[![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

This work is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg


Cite as:

@Misc{Downey2018SECDAQ,   
  author       = {Austin Downey},   
  howpublished = {GitHub},  
  title        = {{SEC} {DAQ} Open Source Hardware Design},   
  groups       = {{ARTS-L}ab},  
  url          = {https://github.com/ARTS-Laboratory/SEC-DAQ-Open-Source-Hardware-Design},  
}










