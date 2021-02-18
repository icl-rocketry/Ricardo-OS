#include "state.h"
#include "stateMachine.h"
#include "Logging/tostring.h"

State::State(stateMachine* sm):
_sm(sm)
{};

State::~State(){
  // Compulsory virtual destructor definition, even if it's empty
};

void State::initialise(){
    time_entered_state = millis();
    _sm->systemstatus.new_message(_curr_stateID,"state entered: " + tostring(time_entered_state));
};



void State::exitstate(){
    time_exited_state = millis();
    time_duration_state = time_exited_state - time_entered_state;
     _sm->systemstatus.delete_message(_curr_stateID,"state exited: " + tostring(time_exited_state) + " state duration: " + tostring(time_duration_state));

};

