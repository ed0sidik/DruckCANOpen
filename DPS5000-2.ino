#include <Wire.h>
#include "dps5000.h"

float f = -0.02;
byte hex[4] = {0};
long hex32 = 0;

void setup() {
  // put your setup code here, to run once:

  Wire.begin(25,21);
//  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);
  Serial.setTimeout(3000);
//  Serial.println("DPS5000 Sensor Comms V1.0");
//  Serial.println("-------------------------");
  //Ping Device to check comms
  dpsVerify(DPS_ID);

  //Check Serial No
  //serialNo = dpsRead(DPS_ID,SERIAL_NO);
  //Serial.print("Serial No:");
  //Serial.println(serialNo);

}

void loop() {
  // put your main code here, to run repeatedly:
  byte error;
  int i;
  long statusWord, pressure, temp, offset, serialNo, lastCal, eDraw, type;
  double pressureFinal, tempFinal, offsetFinal;
  String dataString;
  boolean KeepReading = true;

  if(Serial.available() > 0){
    // Read serial until LF
    String input = Serial.readStringUntil(10);
    //Serial.flush();
    input.trim();
    if(input == "rp"){ // Read Pressure
      dpsTrigger(DPS_ID);
      pressure = dpsRead(DPS_ID,PRESSURE);
      pressureFinal = calcFloat(pressure);
      Serial.print("Pressure: ");
      Serial.print(pressureFinal,5);
      Serial.println(" bar");
    }
    else if(input == "rt"){ // Read Temperature
      dpsTrigger(DPS_ID);
      temp = dpsRead(DPS_ID,TEMP);
      tempFinal = calcFloat(temp);  
      Serial.print("Temp: ");
      Serial.print(tempFinal);
      Serial.println(" degC");
    }

    else if(input == "rc"){ // Read Pressure and Temperature Continuously
      while(KeepReading){
        //Trigger a Reading
        dpsTrigger(DPS_ID);
 
        //Read Pressure
        pressure = dpsRead(DPS_ID,PRESSURE);
        pressureFinal = calcFloat(pressure);
        Serial.print("Pressure: ");
        Serial.print(pressureFinal,5);
        Serial.println(" bar");
        Serial.print(",");

        //Read Temperature
        temp = dpsRead(DPS_ID,TEMP);
        tempFinal = calcFloat(temp);  
        Serial.print("Temp: ");
        Serial.print(tempFinal);
        Serial.println(" degC");
        if(Serial.available()>0){
          Serial.readStringUntil(10);
          KeepReading = false;
        }
        delay(1000);
       }
       KeepReading = true;
      }

    else if(input == "rz"){ // Read zero offset
      dpsTrigger(DPS_ID);
      offset = dpsRead(DPS_ID,PRESSURE_OFFSET);
      offsetFinal = calcFloat(offset);
      Serial.print("Offset : ");
      Serial.print(offsetFinal,5);
      Serial.println(" bar");
    }
    
    else if(input == "si"){ // Set I2C Address to be read
      Serial.println("Set Sensor Address to be read. Type address number and press Enter.");
      String input = Serial.readStringUntil(10);
      input.trim();
      if(input == ""){
        Serial.println("No input. Address Number to be read has not changed.");
      }
      else if( (input.toInt()>0) && (input.toInt()<128) ) {
        Serial.print("Sensor Address Number to be read is ");
        Serial.print(input);
        Serial.println(".");
        DPS_ID = input.toInt();
      }
      else{
        Serial.println("Input not valid. Address Number to be read has not changed.");
      }
    }

    else if(input == "ci"){ // Change Sensor Address
      Serial.println("Change Sensor Address. Type address number and press Enter.");
      String input = Serial.readStringUntil(10);
      input.trim();
      if(input == ""){
        Serial.println("No input. Sensor Address has not changed.");
      }
      else if( (input.toInt()>0) && (input.toInt()<128) ) {
        Serial.print("Sensor Address is changed to ");
        Serial.print(input);
        Serial.println(".");
        dpsWrite(DPS_ID, ACCESS_WORD, 4118); // Set WENB
        dpsWrite(DPS_ID, I2C_ADDRESS, input.toInt()); // Change Address
        dpsWrite(DPS_ID, STATUS, 32); // Save to flash memory, set bit 5 of STATUS register
        dpsWrite(DPS_ID, ACCESS_WORD, 0); // Clear WENB
        dpsWrite(DPS_ID, STATUS, 0x00008000); // Reset sensor, set bit 15 of STATUS register
      }
      else{
        Serial.println("Input not valid. Sensor Address Number has not changed.");
      }
    }

    else if(input == "za"){ // Sensor zero adjust
      Serial.println("Sensor zero adjustment");
      Serial.println("Zero offset will be erased, raw pressure reading will be displayed.");
      dpsWrite(DPS_ID, ACCESS_WORD, 4118); // Set WENB
      dpsWrite(DPS_ID, PRESSURE_OFFSET, 0); // Erase zero offset
      dpsWrite(DPS_ID, STATUS, 32); // Save to flash memory, set bit 5 of STATUS register
      dpsWrite(DPS_ID, ACCESS_WORD, 0); // Clear WENB
      dpsWrite(DPS_ID, STATUS, 0x00008000); // Reset sensor, set bit 15 of STATUS       
      delay(1000);
      pressure = dpsRead(DPS_ID,PRESSURE);
      pressureFinal = calcFloat(pressure);
      Serial.print("Pressure reading before adjustment is : ");
      Serial.print(pressureFinal,5);
      Serial.println(" bar");
      Serial.println("Please type current reference pressure in bar and press Enter.");
      delay(3000);
      
      input = Serial.readStringUntil(10);
      input.trim();

      if(input != ""){
        f = input.toFloat() - pressureFinal;
        //Serial.println(input.toFloat(), 5);
        Serial.println(input);
        Serial.print(f,5);
        Serial.println(" will be stored in PRESSURE_OFFSET register.");
        Serial.println("Please type y and Enter to proceed, or n and Enter to abort.");

        delay(3000);

        input = Serial.readStringUntil(10);
        input.trim();

        if(input == "y"){
          FloatToHex(f,hex);
          for(i=0;i<4;i++){
            hex32 = hex32<<8;
            hex32 += hex[3-i];
          }
          dpsWrite(DPS_ID, ACCESS_WORD, 4118); // Set WENB
          dpsWrite(DPS_ID, PRESSURE_OFFSET, hex32); // Adjust zero offset
          dpsWrite(DPS_ID, STATUS, 32); // Save to flash memory, set bit 5 of STATUS register
          dpsWrite(DPS_ID, ACCESS_WORD, 0); // Clear WENB
          dpsWrite(DPS_ID, STATUS, 0x00008000); // Reset sensor, set bit 15 of STATUS register
          Serial.println("Zero offset adjusted.");
        }
        else if(input == ""){
          Serial.println("Zero offset adjust aborted.");
        }
      }
/*      FloatToHex(f,hex);
      
      Serial.println(hex[3],HEX);
      Serial.println(hex[2],HEX);
      Serial.println(hex[1],HEX);
      Serial.println(hex[0],HEX);

      for(i=0;i<4;i++){
        hex32 = hex32<<8;
        hex32 += hex[3-i];

      }
      Serial.println(hex32);*/
    }
    
    else Serial.println("Unrecognized Command");
  }

}

//Functions
void dpsVerify(int dpsID)
{
  byte error;
  
  Wire.beginTransmission(dpsID);
  error = Wire.endTransmission();
  
    if (error == 0)
      {
        Serial.print("\nI2C device found at address 0x");
        if (dpsID<16) 
          Serial.print("0");
        Serial.print(dpsID, HEX);
        Serial.println("!");
      }
}

void dpsTrigger (int dpsID)
{
    byte error;
    long statusWord;
    //Trigger a Reading
    Wire.beginTransmission(dpsID);
      Wire.write(byte(0x00)); //write the address of the status word 
      Wire.write(byte(0x01)); //1 - LSB First
      Wire.write(byte(0x00)); //2
      Wire.write(byte(0x00)); //3 
      Wire.write(byte(0x00)); //4 - MSB Last
    
    error = Wire.endTransmission();
    if (error == 0)
        {
         delay(1); //Should see bit 0 change from 0 to 1 which show new reading is there
         statusWord = dpsRead(dpsID,STATUS);
         Serial.print("Status Word: ");
         Serial.print(statusWord,HEX); 
         Serial.print(",");
        }
}

long dpsRead(int dpsID, byte dpsAddress)
{
  byte stat, error;
  int i;
  long dpsData = 0;
  
  Wire.beginTransmission(dpsID);
  Wire.write(dpsAddress);
  error = Wire.endTransmission();

    if (error == 0)
      {
        Wire.requestFrom(dpsID,4);
        if (4 == Wire.available())
          {
            stat = Wire.read();
            for (i=0; i<8; i++)
             {
              bitWrite(dpsData,i,bitRead(stat,i));
             }
            
            stat = Wire.read();
            for (i=0; i<8; i++)
             {
              bitWrite(dpsData,i+8,bitRead(stat,i));
             }
             
            stat = Wire.read();
            for (i=0; i<8; i++)
             {
              bitWrite(dpsData,i+16,bitRead(stat,i));
             }
            
            stat = Wire.read();
            for (i=0; i<8; i++)
             {
              bitWrite(dpsData,i+24,bitRead(stat,i));
             }

          }
  return dpsData;
       }
}

long dpsWrite(int dpsID, byte dpsAddress, long dpsData){
  byte stat, error;
  int i;
  int data[4]; // 2 byte data array

  for(i=0; i<4; i++){
    data[i] = dpsData>>(i*8) & 0x00FF;  
  }
  
  Wire.beginTransmission(dpsID);
  Wire.write(dpsAddress);
  for (i=0; i<4; i++){
    Wire.write(data[i]);
  }
  delay(10);
  Wire.endTransmission();   
}

void FloatToHex(float f, byte* hex){
  byte* f_byte = reinterpret_cast<byte*>(&f);
  memcpy(hex, f_byte, 4);
}

/* long convFloat(double decVal){
  long V1d,V2d,V3d,V4d;
  char Buf[50];
  uint32_t ii = *((uint32_t*)&decVal);
  String st = String(ii,HEX);
   String V1 = st.substring(0,2);
    V1.toCharArray(Buf, 50);
    Serial.print('\n');
    V1d=HexAtoi(Buf);
    Serial.print(V1d);
  
    String V2 = st.substring(2,4);
    V2.toCharArray(Buf, 50);
    V2d=HexAtoi(Buf);

  
    String V3 = st.substring(4,6);
    V3.toCharArray(Buf, 50);
    V3d=HexAtoi(Buf);

  
    
    String V4 = st.substring(6,8);
    V4.toCharArray(Buf, 50);
    V4d=HexAtoi(Buf);
}*/

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
