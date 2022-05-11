/*
  PMBus.cpp - Library for getting values over the PMBus protocol for a Murata DCE0133V2.
  Created by Mohamad Ali Jarkas, May 11, 2022.
*/

#include "Arduino.h"
#include "PMBus.h"

unsigned int PMBus::data_request(unsigned char device_addr, unsigned char command, int expected_length)
{
  unsigned char msb, lsb = 0;
  unsigned int received_word; //format: msblsb

  // Send command to step-down
  Wire.beginTransmission(device_addr);
  Wire.write(command);
  Wire.endTransmission(false);

  int bytes_returned = Wire.requestFrom((uint8_t) device_addr, (uint8_t) expected_length, (uint8_t) true);

  while (Wire.available()) {
    lsb = Wire.read(); // data_low is received first
    msb = Wire.read(); // data_high is received second
  }

  received_word = msb << 8 | lsb;  
  return received_word;
}


// FIXME: only for case where mantissa is positive (should always be valid)
float PMBus::lin11_to_dec(unsigned int received_word)
{
    unsigned int mantissa = received_word & 0x7FF;
    unsigned int exponent = received_word >> 11;
        
    if (exponent >> 4) { // should always be the case 
      exponent = (~exponent + 1) & 0x1F; //find the 2's complement
    }

    // could also be done with shifting but loses precision
    float received_float = mantissa;
    for (int ii = 0; ii < exponent; ii++){
      received_float = received_float / 2;
    }

    return received_float;
}

float PMBus::lin16_to_dec(unsigned int received_word, unsigned int exponent){
    unsigned char updated_exponent = ((~exponent + 1) & 0x1F);
    
    float updated_word = received_word;

    for (int ii = 0; ii < updated_exponent; ii++){
        updated_word = updated_word / 2;
    }

    return updated_word;
}

float PMBus::vin_request(){
    float vin = 0;
    return vin = lin11_to_dec(data_request(127, 0x88, 2));
}

float PMBus::vout_request(){
    float vout = 0;
    return vout = lin16_to_dec(data_request(127, 0x8B, 2), data_request(127, 0x20, 1));
}

float PMBus::temp_request(){
    float temp = 0;
    return temp = lin11_to_dec(data_request(127, 0x8D, 2));
}

/*float PMBus::current_in_request(){
    float i_in = 0;
    return i_in = lin11_to_dec(data_request(127, 0x89, 2));
}
*/

float PMBus::current_out_request(){
    float i_out = 0;
    return i_out = lin11_to_dec(data_request(127, 0x8C, 2));
}

/*float PMBus::power_in_request(){
    float p_in = 0;
    return p_in = lin11_to_dec(data_request(127, 0x97, 2));
}
*/

float PMBus::power_out_request(){
    float p_out = 0;
    return p_out = lin11_to_dec(data_request(127, 0x96, 2));
}

