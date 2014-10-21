#include "MPR121.h"



MPR121::MPR121(){
    //Wire.begin();
}

void MPR121::writeRegister(char commandAddress, char command){
  Wire.beginTransmission(MPR121_ADDR);
  Wire.write(commandAddress);
  Wire.write(command);
  char errorno = Wire.endTransmission();
  if(errorno != 0){
    this->i2cError(I2C_ERROR_WRITE | errorno);
  }  
}

char MPR121::readRegister(char commandAddress){
  Wire.beginTransmission(MPR121_ADDR);
  Wire.write(commandAddress);
  Wire.endTransmission(false);
  char bytesReceived;
  bytesReceived = Wire.requestFrom(MPR121_ADDR,1);
  if(bytesReceived != 1){
    this->i2cError(I2C_ERROR_READ |   I2C_RECEIVED_BYTES_ERROR);
  }
  char readData = Wire.read();
  return readData;
}


char MPR121::readTouchStatus(char upperNLower){
  return readRegister(upperNLower);
}

int MPR121::readElectrodeData(char ch){
  char lowerAddress = ch*2 + ELE0_ELECTRODE_FILTERED_DATA_LSB;
  char upperAddress = ch*2 + ELE0_ELECTRODE_FILTERED_DATA_MSB;
  
  unsigned char lowerData = readRegister(lowerAddress);
  unsigned char upperData = readRegister(upperAddress);
  
  return ((int)upperData)<<8 | lowerData;
}

void MPR121::setup(){
  Wire.begin();
  writeRegister(ELECTRODE_CONFIGURATION,0x00);  
  writeRegister(AFE_CONFIGURATION1,0x1F);
  writeRegister(AFE_CONFIGURATION2,0x20);
  writeRegister(ELECTRODE_CONFIGURATION,0x0C);
}

void MPR121::calibrate(){
  electrodeCurrentCalibrate();
  thresholdCalibrate();
}

void MPR121::i2cError(unsigned char errorno){
  Serial.write("i2c error 0x");
  Serial.println(errorno,HEX);
}

void MPR121::electrodeCurrentCalibrate(){
  char overcurrent;
  int electrodeData;
  
  for(char i=63;i>0;i--){
    overcurrent = 0;
    
    writeRegister(ELECTRODE_CONFIGURATION,0x00);  
    writeRegister(AFE_CONFIGURATION1,i);
    writeRegister(ELECTRODE_CONFIGURATION,0x0C);
    delay(1);
      
    for(char j=0;j<12;j++){
      electrodeData = readElectrodeData(j);
      if(electrodeData > 900){
        overcurrent = 1;
      }
    }
    if(overcurrent == 0){
      break;
    }
  }
}

void MPR121::thresholdCalibrate(){
  int electrodeData;
  char baselineValue;
  
  for(char i=0;i<12;i++){
    writeRegister(ELECTRODE_CONFIGURATION,0x4C);
    delay(10);
    electrodeData = readElectrodeData(i);
    writeRegister(ELECTRODE_CONFIGURATION,0x40);
    
    baselineValue = (char)((2*electrodeData/5)>>2);
    writeRegister(ELE0_BASELINE_VALUE+i,baselineValue);
    
    Serial.print(i,DEC);
    Serial.print(":");
    Serial.print(electrodeData,DEC);
    Serial.print(",");
    Serial.println(baselineValue,DEC);    
  }
  writeRegister(ELECTRODE_CONFIGURATION,0x4C);
}

