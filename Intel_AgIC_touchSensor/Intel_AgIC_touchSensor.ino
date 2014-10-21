#include <Wire.h>
#include "MPR121.h"
MPR121 touchSensor;

void setup() {
  Serial.begin(115200);
  touchSensor.setup();
  // put your setup code here, to run once:

}

void loop() {
  int ch[12];
  int i = 0;
  for(i = 0;i<12;i++){
      ch[i] = touchSensor.readElectrodeData(i);
      Serial.print(ch[i]);
  }  
  Serial.print("\r\n");
  delay(0.01);    
  // put your main code here, to run repeatedly: 
  
}
