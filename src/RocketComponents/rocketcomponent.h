#pragma once

#include <iostream>
#include <unistd.h>
#include <string>

#include "Helpers/bitwiseflagmanager.h"

#include <ArduinoJson.h>

#include "rocketcomponenttype.h"

#include "Storage/logController.h"


enum class COMPONENT_STATUS_FLAGS:uint16_t{
    NOMINAL = 0,
    DISARMED = (1<<0),
    ERROR_NORESPONSE = (1<<1),
    ERROR_CONTINUITY = (1<<2),
    ERROR_PINS = (1<<3),
    ERROR_I2C = (1<<4),
    ERROR = (1<<5) //generic error
};

using component_status_flags_t = uint16_t;
// using system_flag_t = std::underlying_type<SYSTEM_FLAG>::type;

struct RocketComponentState : public BitwiseFlagManager<COMPONENT_STATUS_FLAGS,component_status_flags_t>
{

    uint32_t lastNewStateUpdateTime;
    uint32_t lastNewStateRequestTime;

    void newFlag(COMPONENT_STATUS_FLAGS flag) override final
    {
        if (flag == COMPONENT_STATUS_FLAGS::NOMINAL)
        {
            BitwiseFlagManager::_statusString = 0;
            return;
        }
        BitwiseFlagManager::newFlag(flag);
    };

    /**
     * @brief Allows tracking of the state of remote components such as
     * networked components
     * 
     * @param remoteState 
     */
    void trackRemoteStatus(uint16_t remoteState)
    {
        BitwiseFlagManager::_statusString = remoteState;
    }
};

class RocketComponent{
    public:
        RocketComponent(uint8_t id,RocketComponentTypes::TYPES componentType,LogController& logcontroller):
        _id(id),
        _componentType(componentType),
        _logcontroller(logcontroller)
        {};
        
        /**
         * @brief Returns a const piinter to the component state for external objects to 
         * get the last received state of the component.
         * 
         * @return const RocketComponentState* 
         */
        const RocketComponentState* getState(){return p_getState();};
        virtual void updateState() = 0;
        virtual bool flightCheck(uint32_t netRetryInterval,std::string handler);
        virtual ~RocketComponent() = 0;

        uint8_t getID(){return _id;};
        RocketComponentTypes::TYPES getComponentType(){return _componentType;};

    protected:
        const uint8_t _id;
        const RocketComponentTypes::TYPES _componentType;

        /**
         * @brief Returns a non const pointer to the component state allowing parent class
         * to update state of component
         * 
         * @return RocketComponentState* 
         */
        virtual RocketComponentState* p_getState() = 0;


        LogController& _logcontroller;
            

};

