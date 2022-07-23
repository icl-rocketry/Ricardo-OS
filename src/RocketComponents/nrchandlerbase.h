/**
 * @file nrchandlerbase.h
 * @author Kiran de Silva
 * @brief Base class for the Networked Rocket Component (NRC) handler. Defines the basic required
 * command processing for NRC type components. Uses CRTP to reduce runtime polymorphism overhead 
 * as it is not expected to case to parent class ever. SHould be straightforward to change in 
 * the future or simply provide a runtime polymorphic derived class from this. Extended command handler 
 * implemented if a more complex componet is required.
 * @version 0.1
 * @date 2022-07-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <functional>

#include "rocketcomponent.h"

#include "packets/nrcpackets.h"
#include "Commands/commandpacket.h"

#include <rnp_networkmanager.h>

template<class Derived>
class NRCHandlerBase
{
public:
    NRCHandlerBase(RnpNetworkManager& networkmanager):
    _networkmanager(networkmanager)
    {};
    /*
     * @brief Returns network callback bound to instance
     *
     * @return std::function<void(packetptr_t)>
     */
    std::function<void(packetptr_t)> getThisNetworkCallback()
    {
        return [this](packetptr_t packetptr)
        { handlecommand(std::move(packetptr)); };
    };

protected:

    RnpNetworkManager& _networkmanager;
    uint8_t _componentState;

    void handlecommand(packetptr_t packetptr){
        //check packet is a command packet
        if (packetptr->header.type == static_cast<uint8_t>(NRCPacket::TYPES::NRC_COMMAND)){
            switch(static_cast<NRC_COMMAND_ID>(CommandPacket::getCommand(&packetptr))){
                case GETSTATE:
                {
                    static_cast<Derived*>(this)->getstate_impl(std::move(packetptr));
                    break;
                }
                case EXECTUE:
                {
                    if (_componentState == static_cast<uint8_t>(COMPONET_STATE::NOMINAL)){
                        static_cast<Derived*>(this)->exectue_impl(std::move(packetptr));
                    }
                    break:
                }
                case ARM:
                {
                    static_cast<Derived*>(this)->arm_imp(std::move(packetptr));
                    break:
                }
                case DISARM:
                {
                    static_cast<Derived*>(this)->disarm_impl(std::move(packetptr));
                    break;
                }
                default:
                {
                    static_cast<Derived*>(this)->extendedCommandHandler_impl(std::move(packetptr));
                    break;
                }
            }
        }
    }

    void getstate_impl(packetptr_t packetptr){};
    void execute_impl(packetptr_t packetptr){};
    void arm_impl(packetptr_t packetptr){};
    void disarm_impl(packetptr_t packetptr){};
    void extendedCommandHandler_impl(packetptr_t packetptr){};
     




};