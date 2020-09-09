/*
State during flight portion of rocket controlling thrust output and relaying telemetry to ground. Limited commands avaliable
*/

#include "Arduino.h"
#include "state.h"
#include "stateMachine.h"

#ifndef FLIGHT_H
#define FLIGHT_H

class Flight: public State {
  public:
    void initialise();
    State* update();
};

#endif
