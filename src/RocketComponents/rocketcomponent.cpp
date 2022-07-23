#include "rocketcomponent.h"

#include <string>

RocketComponent::~RocketComponent(){};

//Returns true if there is an error !!
bool RocketComponent::flightCheck(uint32_t networkRetryInterval,std::string handler){
    const RocketComponentState* currentState = this->getState();
    if (currentState == nullptr){
        #ifdef _RICDEBUG
        throw std::runtime_error("flightCheck() called on base class instance without a derived class!");
        #endif
        _logcontroller.log("flightCheck() called on base class instance without a derived class!");
        return 1; 
    }

    const uint8_t cid = this->getID();

    if (currentState->lastNewStateRequestTime > currentState->lastNewStateUpdateTime) //component hasnt sent new update
    {
        uint32_t requestInterval = millis() - currentState->lastNewStateRequestTime;
        if (requestInterval > networkRetryInterval)
        { //maybe packet got lost on the network? might indicate something more serious however
            _logcontroller.log(handler + " Component: " + std::to_string(cid) + " not responding!");
            //update state of component to no response error
            this->p_getState()->newFlag(COMPONENT_STATUS_FLAGS::ERROR_NORESPONSE);
            // try requesting an update again
            this->updateState(); 
        }
        return 1;
    }
    else if (!this->p_getState()->flagSet(COMPONENT_STATUS_FLAGS::NOMINAL))
    {
        _logcontroller.log(handler + " Component: " + std::to_string(cid) + " responded with error: " + std::to_string(currentState->getStatus()));
        return 1;
    }
    return 0;
}