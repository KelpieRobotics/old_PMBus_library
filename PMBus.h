/*
  PMBus.h - Library for getting values over the PMBus protocol for a Murata DCE0133V2.
  Created by Mohamad Ali Jarkas, May 11, 2022.
*/
#ifndef PMBus_h
#define PMBus_h

#include "Arduino.h"
#include "Wire.h"

class PMBus
{
    public:
        unsigned int data_request(unsigned char device_addr, unsigned char command, int expected_length);
        float lin16_to_dec(unsigned int received_word, unsigned int exponent);
        float lin11_to_dec(unsigned int received_word);
        float vin_request();
        float vout_request();
        float temp_request();
        //float current_in_request(); //Not available  
        float current_out_request();
        //float power_in_request(); //Not available
        float power_out_request();
    private: 
        // FIXME: update code to use this
        int _device_addr;
};

#endif