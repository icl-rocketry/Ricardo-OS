//process commands stored in command buffer and execute

#pragma once



#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <cstdarg>
#include <initializer_list>

#include "commands.h"
#include "rnp_packet.h"
#include "rnp_networkmanager.h"
#include <default_packets/simplecommandpacket.h>



class stateMachine;//forward declaration 
// typedef void (*handlerFunction)(const RnpPacketSerialized& packet);
using commandFunction_t = std::function<void(stateMachine&,const RnpPacketSerialized&)>;


class CommandHandler {
    public:
        CommandHandler(stateMachine* sm);
        
        std::function<void(std::unique_ptr<RnpPacketSerialized>)> getCallback();
 
        static constexpr uint8_t serviceID = static_cast<uint8_t>(DEFAULT_SERVICES::COMMAND); // serivce ID for network manager
        
        enum class PACKET_TYPES:uint8_t{
            SIMPLE = 0,
            MAGCAL = 10,
            MESSAGE_RESPONSE = 100,
            TELEMETRY_RESPONSE = 101
        };

        template<class T>
        void enable_commands(std::initializer_list<T> command_list) {
            for (auto command_id : command_list){
                _enabledCommands.set(command_id);
            }
        }

        template<class T>
        void disable_commands(std::initializer_list<T> command_list) {
            for (auto command_id : command_list){
                _enabledCommands.reset(command_id);
            }
        }

        void reset_commands() {
            _enabledCommands = _alwaysEnabledCommands();
        }

        

    private:
        stateMachine* _sm; //pointer to state machine

        const std::unordered_map<Commands::ID, commandFunction_t> _commandMap;

        std::bitset<256> _enabledCommands;

        constexpr std::bitset<256> _alwaysEnabledCommands() {
            
            std::bitset<256> bits;
            bits.set(8);
            return bits;
        };

        void handleCommand(std::unique_ptr<RnpPacketSerialized> packetptr);
        
        

};	
