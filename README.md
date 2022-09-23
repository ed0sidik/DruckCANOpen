# DruckCANOpen
Arduino library and sample code for communication with Druck's DPS8000-CANbus and DPS5000-CANbus pressure sensor

dps_can.h is library of byte stream to read/write data from/to DPS pressure sensor.

CANbus-2.ino is sample code to read/write data from/to DPS pressure sensor using terminal software

The library and sample code have been verified to work with Arduino UNO or MEGA and CAN-bus shield which utilizes MCP2515 and MCP2551 chips.
Below are CAN-bus shield vendors.
- Seeed Studio : www.seeedstudio.com/CAN-BUS-Shield-V2.html
- Keyestudio : www.keyestudio.com/products/keyestudio-can-bus-shield-mcp2515-chip-with-sd-socket-for-arduino-uno-r3-543

I recommend to use Seeed Studio, because there is only little information on Keyestudio product. Keyestudio CAN-bus shield doesn't work on Arduino MEGA.
On Keyestudio CAN-bus shield, there is no Rdif mounted between CAN-H and CAN-L. You have to mount Rdif by yourself. 
Recommended Rdif value is 120Ω, but I use 330Ω instead and it works (Arduino UNO only).
D-Sub 9 pin assignment : 3:GND, 2:CAN-LO, 7:CAN-HI

MCP_CAN_lib MCP2515 CAN-bus controller library is required. It can be downloaded from repository below
https://github.com/coryjfowler/MCP_CAN_lib

Serial Command List

rp : Read Pressure<br>
Command : rp &lt; CRLF &gt; <br>
Reply : Pressure : &lt; Pressure Value &gt; &lt; Pressure Unit &gt; &lt; CRLF &gt;

rt : Read Temperature<br>
Command : rt &lt; CRLF &gt; <br>
Reply : Temperature : &lt; Temperature Value &gt;&lt; Temperature Unit &gt;&lt; CRLF &gt;

rc : Read Pressure and Temperature continuously. Reading will stop when LF is received (Enter button is pressed)<br>
Command : rc &lt; CRLF &gt; <br>
Reply : Pressure: &lt; Pressure Value &gt; &lt; Pressure Unit &gt; , Temp: &lt; Temperature Value &gt;&lt; Temperature Unit &gt;&lt; CRLF &gt;

pn : Read Product No<br>
Command : pn &lt; CRLF &gt; <br>
Reply : Product No. : &lt; Product No. &gt;&lt; CRLF &gt;

rs : Read S/N<br>
Command : rs &lt; CRLF &gt; <br>
Reply : S/N : &lt; Serial Number &gt;&lt; CRLF &gt;

ri : Read NodeID<br>
Command : ri &lt; CRLF &gt; <br>
Reply : NodeID : &lt; NodeID &gt;&lt; CRLF &gt;

ci : Change Sensor NodeID<br>
Command : ci &lt; CRLF &gt; <br>
Reply : Change NodeID. Please type NodeID and press Enter <br>
Send : &lt; NodeID &gt;&lt; CRLF &gt; <br>
Reply : Power cycle the sensor to enable the change.
  
si : Set NodeID to be read
Command : si &lt; CRLF &gt; <br>
Reply : Set NodeID to be read. Please type NodeID and press Enter <br>
Send : &lt; NodeID &gt;&lt; CRLF &gt; <br>
Reply : NodeID to be read is &lt; NodeID &gt;&lt; CRLF &gt; <br>

rb : Read Bit Rate

cb : Change Sensor Bit Rate

sb : Set Host Bit Rate

rst : Reset Sensor to Factory Default Setup (don't execute on DPS8000)

rfv : Read Field Value1

rf : Read selected FIR filter

cf : Change FIR filter
