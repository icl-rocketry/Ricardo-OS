#include <Arduino.h>
#include "groundstation.h"
#include "stateMachine.h"

#include "flags.h"

Groundstation::Groundstation(stateMachine* sm) : State(sm){
    _curr_stateID = system_flag::STATE_GROUNDSTATION;
};

void Groundstation::initialise(){
    State::initialise();
    
};

State* Groundstation::update(){
    
    return this;//loopy loop
};

void Groundstation::exitstate(){
    State::exitstate();
};