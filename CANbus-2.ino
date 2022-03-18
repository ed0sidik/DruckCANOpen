/* Sample Code for communication with Druck's CANbus Pressure Sensor

  Author : Edwin Sidik

  Ver.0 First Release 20-Feb 2021

  Ver.1 18-Mar 2022
  - If else statement added to avoid 0 being written to NodeID by mistake.
  - Read and change FIR setting added.

  This sample code is free, you can redistribute it and/or modify it with your own responsibility

  The code is intended to be used in Arduino Uno or MEGA with CAN-bus Shield
*/

#include <mcp_can.h>
#include <SPI.h>
#include "dps_can.h"

#define Pressure    0x01
#define Temperature 0x02
#define SerialNo    0x03
#define NodeIDReg   0x04
#define BitRateReg  0x05
#define FieldValue  0x06
#define FIR         0x07

unsigned long rxId;
byte len;
byte rxBuf[36];
// Command to read pressure

long Pre_millis;

// Default Node ID
int NodeID = 2;

// Default Bit Rate
int BitRate = 250;

//MCP_CAN CAN0(10);// CAN0 CS: pin 10
//MCP_CAN CAN1(9); // CAN1 CS: pin 9
MCP_CAN CAN1(10);

void setup() {
  // put your setup code here, to run once:
  Serial.setTimeout(7000);
  Serial.begin(9600);

  // init CAN0 bus, baudrate: 250kbps@8MHz
/*  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_16MHZ) == CAN_OK){
    Serial.println("CAN0: Init OK!");
    CAN0.setMode(MCP_NORMAL);
  } else{ 
    Serial.println("CAN0: Init Fail!");
  }
*/
  // init CAN1 bus, baudrate: 125kbps@16MHz
  if(BitRate == 125){
    if(CAN1.begin(MCP_ANY, CAN_125KBPS, MCP_16MHZ) == CAN_OK){
      Serial.println("CAN1: Init OK!");
      CAN1.setMode(MCP_NORMAL);
    }else{ 
      Serial.println("CAN1: Init Fail!");
    }  
  }
  // init CAN1 bus, baudrate: 250kbps@16MHz
  if(BitRate == 250){
    if(CAN1.begin(MCP_ANY, CAN_250KBPS, MCP_16MHZ) == CAN_OK){
      Serial.println("CAN1: Init OK!");
      CAN1.setMode(MCP_NORMAL);
    }else{ 
      Serial.println("CAN1: Init Fail!");
    }  
  }


  Pre_millis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int count=0; 
/*  if(millis()-Pre_millis > 100){ // Period: 100ms
    CAN1.sendMsgBuf(0x602, 0, 8, txBuf0);
    //CAN1.sendMsgBuf(0x101, 0, 24, txBuf1);
    Serial.println("Read Pressure");
    Pre_millis=millis();
  }
  delay(100);
  if(CAN1.checkReceive()==CAN_MSGAVAIL){
    CAN1.readMsgBuf(&rxId, &len, rxBuf);
    Serial.print("Recive ID: ");
    Serial.print(rxId, HEX);
    Serial.print(" Data: ");
    for(byte i = 0; i<len; i++){
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  */

  if(Serial.available() > 0){
    // Read serial until LF
    String input = Serial.readStringUntil(10);
    //Serial.flush();
    input.trim();
    if(input == "rp"){ // Read Pressure
      Serial.print("Pressure : ");
      Serial.print(calcFloat(can_read(Pressure)),4);
      Serial.println(" bar");
    }
    else if(input == "rt"){ // Read Temperature
      Serial.print("Temperature : ");
      Serial.print(calcFloat(can_read(Temperature)),2);
      Serial.println(" deg.C");
    }
    else if(input == "rs"){ // Read S/N
      Serial.print("S/N : ");
      Serial.println(can_read(SerialNo),DEC);
    }
    else if(input == "ri"){ // Read NodeID
      Serial.print("NodeID : ");
      Serial.println(can_read(NodeIDReg),DEC);
    }
    else if(input == "ci"){ // Change NodeID
      Serial.println("Change NodeID. Please type NodeID and press Enter");
      String input = Serial.readStringUntil(10);
      input.trim();
      if(input == "0"){
        Serial.println("Invalid number");
        //input == "2";
      }
      else{
        can_write(NodeIDReg, input.toInt());
        can_save();
        Serial.println("Power cycle the sensor to enable the change.");
      }
    }
    else if(input == "si"){ // Set NoteID to be read
      Serial.println("Set NodeID. Please type NodeID and press Enter");
      String input = Serial.readStringUntil(10);
      input.trim();
      NodeID = input.toInt();
      Serial.print("NodeID to be read is ");
      Serial.println(NodeID);
    }
    else if(input == "rb"){ // Read Bit Rate
      Serial.print("Bit Rate : ");
      Serial.println(can_read(BitRateReg),DEC);
    }
    else if(input == "cb"){ // Change Client Bit Rate
      Serial.println("Change Bit Rate. Please type Bit Rate and press Enter");
      String input = Serial.readStringUntil(10);
      input.trim();
      can_write(BitRateReg, input.toInt());
      can_save();
      Serial.println("Power cycle the sensor to enable the change.");
    }
    else if(input == "sb"){ // Set Host Bit Rate
      Serial.println("Set Host Bit Rate. Please type Bit Rate and press Enter");
      String input = Serial.readStringUntil(10);
      input.trim();
      BitRate = input.toInt();
      Serial.print("Host Bit Rate is set to ");
      Serial.print(BitRate);
      Serial.println(" kpbs");
      setup();
    }
    else if(input == "rst"){ // Reset to Factory Default Setup
      Serial.println("Reset Client to Factory Default Setup");
      can_reset();
      Serial.println("Power cycle the sensor to enable the change.");
    }
    else if(input == "rfv"){ // Read Field Value1
      Serial.print("Field Value1 : ");
      Serial.print(calcFloat(can_read(FieldValue)),4);
      Serial.println(" mbar");
    }
    else if(input == "rf"){ // Read selected FIR filter
      Serial.print("Selected FIR filter : ");
      Serial.println(can_read(FIR),DEC);
    }
    else if(input == "cf"){ // Change FIR filter
      Serial.println("Change FIR filter. Please type FIR no. and press Enter");
      String input = Serial.readStringUntil(10);
      input.trim();
      can_write(FIR, input.toInt());
      delay(100);
      if(CAN1.checkReceive()==CAN_MSGAVAIL){
        CAN1.readMsgBuf(&rxId, &len, rxBuf);
        Serial.print("Recive ID: ");
        Serial.print(rxId, HEX);
        Serial.print(" Data: ");
        for(byte i = 0; i<len; i++){
          Serial.print(rxBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
      }
    }
    else Serial.println("Unrecognized Command");
  }

  delay(1000);
  
}

long can_read(byte reg){
  int i;
  long read_val = 0;
  if (reg == 0x01){
    //CAN1.sendMsgBuf(0x602, 0, 8, txBuf0);
    CAN1.sendMsgBuf(Read_Pres.CAN_ID + NodeID, Read_Pres.EXT, Read_Pres.LEN, Read_Pres.txBuf);  
  }
  else if (reg == 0x02){
    //CAN1.sendMsgBuf(0x602, 0, 8, txBuf1);
    CAN1.sendMsgBuf(Read_Temp.CAN_ID + NodeID, Read_Temp.EXT, Read_Temp.LEN, Read_Temp.txBuf);
  }
  else if (reg == 0x03){
    CAN1.sendMsgBuf(Read_SN.CAN_ID + NodeID, Read_SN.EXT, Read_SN.LEN, Read_SN.txBuf);
  }
  else if (reg == 0x04){
    CAN1.sendMsgBuf(Read_NodeID.CAN_ID + NodeID, Read_NodeID.EXT, Read_NodeID.LEN, Read_NodeID.txBuf);
  }
  else if (reg == 0x05){
    CAN1.sendMsgBuf(Read_Bit_Rate.CAN_ID + NodeID, Read_Bit_Rate.EXT, Read_Bit_Rate.LEN, Read_Bit_Rate.txBuf);
  }
  else if (reg == 0x06){
    CAN1.sendMsgBuf(Read_Field_Val.CAN_ID + NodeID, Read_Field_Val.EXT, Read_Field_Val.LEN, Read_Field_Val.txBuf);
  }
  else if (reg == 0x07){
    CAN1.sendMsgBuf(Read_FIR.CAN_ID + NodeID, Read_FIR.EXT, Read_FIR.LEN, Read_FIR.txBuf);
  }
  delay(100);
  if(CAN1.checkReceive()==CAN_MSGAVAIL){
    CAN1.readMsgBuf(&rxId, &len, rxBuf);
  }
  // Read data are stored in the last 4 bytes of rxBuf, LSB first
  for(i=0; i<4; i++){
    read_val <<= 8;
    read_val += rxBuf[7-i];
  }
  return read_val;
}

void can_write(byte reg, byte newVal){
  if (reg == 0x04){
    // Change current NodeID to new NodeID
    Change_NodeID.txBuf[4] = newVal;
    CAN1.sendMsgBuf(Change_NodeID.CAN_ID + NodeID, Change_NodeID.EXT, Change_NodeID.LEN, Change_NodeID.txBuf);
  }
  else if (reg == 0x05){
    Change_Bit_Rate.txBuf[4] = newVal;
    CAN1.sendMsgBuf(Change_Bit_Rate.CAN_ID + NodeID, Change_Bit_Rate.EXT, Change_Bit_Rate.LEN, Change_Bit_Rate.txBuf);
  }
  else if (reg == 0x07){
    Change_FIR.txBuf[4] = newVal;
    CAN1.sendMsgBuf(Change_FIR.CAN_ID + NodeID, Change_FIR.EXT, Change_FIR.LEN, Change_FIR.txBuf);
  }
}

void can_save(){
  // Save all parameter
  CAN1.sendMsgBuf(Save_All_Param.CAN_ID + NodeID, Save_All_Param.EXT, Save_All_Param.LEN, Save_All_Param.txBuf);
}

void can_reset(){
  // Reset Sensor to Factory Default Setup
  CAN1.sendMsgBuf(Reset_to_Factory_Default.CAN_ID + NodeID, Reset_to_Factory_Default.EXT, Reset_to_Factory_Default.LEN, Reset_to_Factory_Default.txBuf);
}

double calcFloat(long floatLong)
{
  int j, sign;
  long exponent = 0, fraction = 0;
  double mantissa, fract, finalVal;
  float exponentFloat;
  
  if (bitRead(floatLong,31)== 1)
    sign=-1;
  else
    sign=1;

  for (j=0; j<8; j++)
  {
    bitWrite(exponent,j,bitRead(floatLong,j+23)); 
  }

  for (j=0; j<23; j++)
  {
    bitWrite(fraction,j,bitRead(floatLong,j));
  }

  mantissa = 0;
  fract = 0.5;
  for (j=0; j<23; j++)
  {
    if(bitRead(fraction,22-j) == 1)
    {
      mantissa = mantissa + fract;
    }
    fract = fract / 2;
  }
   
  mantissa = mantissa + 1;
  exponentFloat = float(exponent);
  finalVal = sign * mantissa * pow(2.0,(exponentFloat - 127.0));
   
  return finalVal;
}
