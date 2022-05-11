#include "PMBus.h"

PMBus object;

void setup() {
  Serial.begin(9600); 
  
  Wire.begin();// begin i2c comms
  Wire.setClock(400000); // set clock frequency, 400k is fast mode

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Vin vrms = ");
  Serial.println(object.vin_request());
  delay(1000);

  Serial.print("Vout vrms = ");
  Serial.println(object.vout_request());
  delay(1000);

  /*Serial.print("I_in A = ");
  Serial.println(object.current_in_request());
  delay(1000);
  */
  
  Serial.print("I_out A = ");
  Serial.println(object.current_out_request());
  delay(1000);

  Serial.print("temp c = ");
  Serial.println(object.temp_request());
  delay(1000);

  /*
  Serial.print("P_in W = ");
  Serial.println(object.power_in_request());
  delay(1000);
  */
  
  Serial.print("P_out W = ");
  Serial.println(object.power_out_request());
  delay(1000);


}
