#include "Arduino.h"
#include "Shift_Input.h"

#define LOAD_PIN 0
#define CLOCK_PIN 1
#define DATA_PIN 2
#define NUM_INPUTS 11 // every 8 inputs requires a new shift register, so this instance would require 2

Shift_Input shiftInputs(LOAD_PIN, CLOCK_PIN, DATA_PIN, NUM_INPUTS);

void setup(){
    shiftInputs.begin();
    shiftInputs.setDebugMode(true); // turns on debugging. Must be off for Unity comms
    shiftInputs.setUpdateDelay(20); // set time between update calls. Defaults to 10ms
    shiftInputs.setDebounceTime(10); // set debounce time. Defaults to 5ms
}

void loop(){
    // call every loop
    shiftInputs.update();

    // returns state of third pin on the second shift register
    shiftInputs.digitalRead(10); // passed value can't be equal to or greater than NUM_INPUTS

    // returns state of a pin of a given shift register
    shiftInputs.digitalRead(1, 0); // (pass shift register index first) returns state of first pin of the second shift register
                                   // first parameter can't exceed numShiftRegisters-1, second can't exceed 7
                                   
    // returns a byte with the state of all 8 pins on a given shift register
    // 1st, 2nd, and 6th pins high would result in 00100011
    shiftInputs.readShiftRegister(0); // 0 corresponds to first shift register in chain
    
    if(shiftInputs.checkIfUpdated()){ // updated flag is only reset to false by getAllInputs
        uint8_t *buf = shiftInputs.getAllInputs(); // buf is an array with the state of all inputs, with each member of the array
                                                   // corresponding to the state of a given shift register. 11 inputs would mean
                                                   // this array would have a size of 2, in this instance
                                                   
        for(int i = 0; i < shiftInputs.getNumShiftRegisters(); i++){ // getNumShiftRegisters() would return 2 here
            Serial.println(buf[i]);
        }
    }
}