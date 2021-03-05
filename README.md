# DruckCANOpen
Arduino library and sample code for communication with Druck's DPS8000-CANbus and DPS5000-CANbus pressure sensor

dps_can.h is library of byte stream to read/write data from/to DPS pressure sensor.

CANbus-2.ino is sample code to read/write data from/to DPS pressure sensor using terminal software

The library and sample code have been verified to work with Arduino UNO or MEGA and CAN-bus shield which utilizes MCP2515 and MCP2551 chips.
Below are CAN-bus shield vendors.
- Seeed Studio : www.seeedstudio.com/CAN-BUS-Shield-V2.html
- Keyestudio : www.keyestudio.com/products/keyestudio-can-bus-shield-mcp2515-chip-with-sd-socket-for-arduino-uno-r3-543

I recommend to use Seeed Studio, because there is little information on Keyestudio product. Keyestudio CAN-bus shield doesn't work on Arduino MEGA.
On Keyestudio CAN-bus shield, there is no Rdif mounted between CAN-H and CAN-L. You have to mount Rdif by yourself. 
Recommended Rdif value is 120Ω, but I use 330Ω instead and it works (Arduino UNO only).

MCP_CAN_lib MCP2515 CAN-bus controller library is required. It can be downloaded from repository below
https://github.com/coryjfowler/MCP_CAN_lib

Serial Command List

rp : Read Pressure

rt : Read Temperature

rs : Read S/N

ri : Read NodeID

ci : Change Sensor Node ID

si : Set NodeID to be read

rb : Read Bit Rate

cb : Change Sensor Bit Rate

sb : Set Host Bit Rate

rst : Reset Sensor to Factory Default Setup (don't execute on DPS8000)

rfv : Read Field Value1
