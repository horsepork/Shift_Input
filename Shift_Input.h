#include "Arduino.h"

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
            debounceTimer = new unsigned long[numShiftRegisters];
        }
    
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

        void update(){
            if(millis() - timer < updateDelay) return;
            timer = millis();

            digitalWrite(loadPin, LOW);
            digitalWrite(clockPin, HIGH);
            delayMicroseconds(5);
            digitalWrite(loadPin, HIGH);
            for(int i = 0; i < numShiftRegisters; i++){
                rawInput[i] = shiftIn(dataPin, clockPin, LSBFIRST);
                if(rawInput[i] = debouncedInput[i]){
                    prevInput[i] = rawInput[i];
                    continue;
                }
                if(prevInput[i] != rawInput[i]){
                    prevInput[i] = rawInput[i];
                    debounceTimer[i] = millis();
                }
                else if(millis() - debounceTimer[i] > debounceTime){
                    debouncedInput[i] = rawInput[i];
                    updated = true; // flag only reset by getAllInputs
                }
            }
        }

        bool checkIfUpdated(){
            return updated;
        }

        uint8_t* getAllInputs(){
            updated = false;
            return debouncedInput;
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
            return debouncedInput[shiftRegisterIndex];
        }

        bool digitalRead(uint16_t index){
            if(index >= numInputs){
                if(debug){
                    Serial.print(index);
                    Serial.println(" exceeds the number of inputs");
                }
                return false;
            }
            return digitalRead(index / 8, index % 8);
        }

        bool digitalRead(uint8_t shiftRegisterIndex, uint8_t pinIndex){
            if(shiftRegisterIndex >= numShiftRegisters){
                
                return false;
            }
            if(pinIndex > 7){
                if(debug){
                    
                }
                return false;
            }
            return bitRead(debouncedInput[shiftRegisterIndex], pinIndex);
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
};