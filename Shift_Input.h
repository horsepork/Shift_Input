#ifndef SHIFT_INPUT_H
#define SHIFT_INPUT_H

#include "Arduino.h"
#include "BooleanInputBase.h"

class Shift_Input{
    public:
        Shift_Input(uint8_t _loadPin, uint8_t _clockPin, uint8_t _dataPin, uint16_t _numInputs){
            loadPin = _loadPin;
            clockPin = _clockPin;
            dataPin = _dataPin;
            numInputs = _numInputs;
            numShiftRegisters = ceil(numInputs / 8.0);
            rawInput = new uint8_t[numShiftRegisters];
            prevInput = new uint8_t[numShiftRegisters];
            debouncedInput = new uint8_t[numShiftRegisters];
            processedInput = new uint8_t[numShiftRegisters];
            debounceTimer = new uint32_t[numShiftRegisters];
            booleanBaseObject = new BooleanInputBase[numInputs];
        }
        BooleanInputBase *booleanBaseObject;
    
    private:
        uint8_t loadPin;
        uint8_t clockPin;
        uint8_t dataPin;
        uint32_t timer;
        uint8_t updateDelay = 10;
        uint8_t debounceTime = 5;
        uint16_t numInputs;
        uint8_t numShiftRegisters;
        uint8_t *rawInput;
        uint8_t *prevInput;
        uint8_t *debouncedInput;
        uint8_t *processedInput; // changes bit states according to input type
        uint32_t *debounceTimer;
        bool debug = false;
        bool updated = false;

    
    public:
        void begin(){
            pinMode(loadPin, OUTPUT);
            pinMode(clockPin, OUTPUT);
            pinMode(dataPin, INPUT);
            timer = millis();
        }

        bool update(){
            if(millis() - timer < updateDelay) return false;
            timer = millis();

            digitalWrite(loadPin, LOW);
            digitalWrite(clockPin, HIGH);
            delayMicroseconds(5);
            digitalWrite(loadPin, HIGH);
            for(int shiftRegIndex = 0; shiftRegIndex < numShiftRegisters; shiftRegIndex++){
                rawInput[shiftRegIndex] = shiftIn(dataPin, clockPin, LSBFIRST);
                Serial.println(rawInput[0]);
                if(rawInput[shiftRegIndex] = debouncedInput[shiftRegIndex]){
                    prevInput[shiftRegIndex] = rawInput[shiftRegIndex];
                    continue;
                }
                if(prevInput[shiftRegIndex] != rawInput[shiftRegIndex]){
                    prevInput[shiftRegIndex] = rawInput[shiftRegIndex];
                    debounceTimer[shiftRegIndex] = millis();
                }
                else if(millis() - debounceTimer[shiftRegIndex] > debounceTime){
                    debouncedInput[shiftRegIndex] = rawInput[shiftRegIndex];
                    Serial.print(shiftRegIndex);
                    Serial.print(" -- ");
                    Serial.println(rawInput[shiftRegIndex]);
                    processedInput[shiftRegIndex] = 0;
                    for(int bitIndex = 0; bitIndex < 8; bitIndex++){
                        uint8_t booleanBaseObjectIndex = bitIndex + shiftRegIndex * 8;
                        if(booleanBaseObjectIndex >= numInputs){
                            break;
                        }
                        booleanBaseObject[booleanBaseObjectIndex].setState(bitRead(debouncedInput[shiftRegIndex], bitIndex));
                        if(booleanBaseObject[booleanBaseObjectIndex].read()){
                            processedInput[shiftRegIndex] += power(2, bitIndex);
                        }
                    }
                    updated = true;
                }
            }
            if(updated){
                updated = false;
                return true;
            }
            return false;
        }

        void resetUpdateFlag(){
            updated = false;
        }

        uint8_t* read(){
            return getAllInputs();
        }

        uint8_t* getAllInputs(){
            return processedInput;
        }

        uint8_t getNumShiftRegisters(){
            return numShiftRegisters;
        }

        uint8_t readShiftRegister(uint8_t shiftRegisterIndex){
            if(shiftRegisterIndex >= numShiftRegisters){
                if(debug){
                    Serial.print(shiftRegisterIndex);
                    Serial.println(" exceeds the number of shift registers");
                }
                return 0;
            }
            return processedInput[shiftRegisterIndex];
        }

        bool read(uint16_t index){
            if(index >= numInputs){
                if(debug){
                    Serial.print(index);
                    Serial.println(" exceeds the number of inputs");
                }
                return false;
            }
            return booleanBaseObject[index].read();
        }

        bool read(uint8_t shiftRegisterIndex, uint8_t pinIndex){
            if(shiftRegisterIndex >= numShiftRegisters){
                
                return false;
            }
            if(pinIndex > 7){
                if(debug){
                    
                }
                return false;
            }
            return booleanBaseObject[shiftRegisterIndex * 8 + pinIndex].read();
        }

        void setDebugMode(bool d){
            debug = d;
            Serial.print("Shift input debugging ");
            debug ? Serial.println("on") : Serial.println("off");
        }

        void setUpdateDelay(uint8_t d){
            updateDelay = d;
            if(!debug) return;
            Serial.print("update delay -- ");
            Serial.println(updateDelay);
        }

        void setDebounceTime(uint8_t d){
            debounceTime = d;
            if(!debug) return;
            Serial.print("debounce time -- ");
            Serial.println(debounceTime);
        }

        uint16_t power(uint8_t base, uint8_t exponent){
            uint16_t returnVal = 1;
            for(int i = 0; i < exponent; i++){
                returnVal *= base;
            }
            return returnVal;
        }
};

#endif