/*
  PMBus.cpp - Library for getting values over the PMBus protocol for a Murata DCE0133V2.
  Created by Mohamad Ali Jarkas, May 11, 2022.
  Documented by Juan Hiedra Primera, Octobe 1, 2022.
*/

#include "Arduino.h"
#include "PMBus.h"

/**
 * This function sends a READ @p command to an I2C device, and reads a number of bytes @p expected_length from it.
 *
 * @param device_addr the address of the device you want to communicate with
 * @param command the command to send to the device
 * @param expected_length the number of bytes you expect to receive from the device.
 *
 * @return the value of the data requested from the device.
 */
unsigned int PMBus::data_request(unsigned char device_addr, unsigned char command, int expected_length)
{
  unsigned char msb, lsb = 0; // Most and least significant bytes
  unsigned int received_word; //format: msblsb

  // Send command to step-down
  Wire.beginTransmission(device_addr);
  Wire.write(command);
  Wire.endTransmission(false);

  // Read data from step-down
  int bytes_returned = Wire.requestFrom((uint8_t) device_addr, (uint8_t) expected_length, (uint8_t) true);

  // Write bytes in Little Endian byte format to msb and lsb
  while (Wire.available()) {
    lsb = Wire.read(); // data_low is received first
    msb = Wire.read(); // data_high is received second
  }

  // Combine msb and lsb into a single Big Endian bit format word
  received_word = msb << 8 | lsb;

  // Return the received word
  return received_word;
}

// FIXME: only for case where mantissa is positive (should always be valid)
/**
 * This function converts a 16-bit word @p received_word (containing both the mantissa and exponent) to a decimal value.
 *
 * @param received_word the 16-bit word to convert, containing the mantissa and exponent.
 *
 * @return the decimal value of the 16-bit word.
 */
float PMBus::lin11_to_dec(unsigned int received_word)
{
    // Extract mantissa and exponent from received word

    // the mantissa is the last 11 bits of the received word (0x7FF = 11 1's)
    unsigned int mantissa = received_word & 0x7FF;

    // the exponent is the first 5 bits of the received word (shifted 11 bits to the right to isolate them)
    unsigned int exponent = received_word >> 11;

    // find the 2's complement value of exponent
    if (exponent >> 4) { // should always be the case 
      exponent = (~exponent + 1) & 0x1F; // find the 2's complement
    }

    float received_float = mantissa;

    // Calculate the decimal value of the received word
    // [NOTE: This could be done through shifting, but loses precision -Mohamad]
    for (int ii = 0; ii < exponent; ii++) {
      received_float = received_float / 2;
    }

    return received_float;
}

/**
 * This function converts a 16-bit word to a decimal (float) value, using the linear 16 format for the
 * input @p received_word.
 *
 * @param received_word An unsigned 16-bit word.
 * @param exponent The exponent of the number. Can be found in the lower 5 bits of the VOUT_MODE PMBus
 * value (the parameter).
 *
 * @return the floating point value of the @p received_word.
 */
float PMBus::lin16_to_dec(unsigned int received_word, unsigned int exponent){
    // 2's complement of exponent
    unsigned char updated_exponent = ((~exponent + 1) & 0x1F);
    
    float updated_word = received_word;

    // Calculate the decimal value of the received word by dividing the received word by 2^exponent
    for (int ii = 0; ii < updated_exponent; ii++) {
        updated_word = updated_word / 2;
    }

    return updated_word;
}

/**
 * This function requests the input voltage from the PMBus device and returns the value as a float (using LIN11 format).
 *
 * @return The voltage of the input power supply (in V).
 */
float PMBus::vin_request(){
    float vin = 0;
    return vin = lin11_to_dec(data_request(127, 0x88, 2));
}

/**
 * This function requests the output voltage from the PMBus device and converts it to a decimal value (using LIN16 format).
 *
 * @return The voltage output of the power supply (in V).
 */
float PMBus::vout_request(){
    float vout = 0;
    return vout = lin16_to_dec(data_request(127, 0x8B, 2), data_request(127, 0x20, 1));
}

/**
 * This function requests the temperature from the PMBus device and returns the temperature in degrees Celsius
 *
 * @return The temperature in degrees Celsius.
 */
float PMBus::temp_request(){
    float temp = 0;
    return temp = lin11_to_dec(data_request(127, 0x8D, 2));
}

/**
 * This function requests the current output from the PMBus device and returns it as a float
 *
 * @return The current output of the power supply (in Amps).
 */
float PMBus::current_out_request(){
    float i_out = 0;
    return i_out = lin11_to_dec(data_request(127, 0x8C, 2));
}

/**
 * This function requests the power output from the PMBus device and returns the value as a float
 *
 * @return The power output of the power supply (in W).
 */
float PMBus::power_out_request(){
    float p_out = 0;
    return p_out = lin11_to_dec(data_request(127, 0x96, 2));
}

// DEPRECATED

//float PMBus::current_in_request(){
//    float i_in = 0;
//    return i_in = lin11_to_dec(data_request(127, 0x89, 2));
//}
//
//
//float PMBus::power_in_request(){
//    float p_in = 0;
//    return p_in = lin11_to_dec(data_request(127, 0x97, 2));
//}