//process commands stored in command buffer and execute

#pragma once



#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <cstdarg>

#include "commands.h"
#include "rnp_packet.h"
#include "rnp_networkmanager.h"
#include <default_packets/simplecommandpacket.h>



class stateMachine;//forward declaration 
typedef void (*handlerFunction)(const RnpPacketSerialized& packet);


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

        void enable_commands(const COMMANDS* cmds...) {
            va_list args;
            va_start(args, cmds);

            while (*args) {
                COMMANDS cmd = va_arg(args, COMMANDS);
                _enabled.set(cmd);
            }
        }

        void reset_commands(void) {
            _enabled = _always_enabled;
        }

    private:
        stateMachine* _sm; //pointer to state machine

        std::unordered_map<COMMANDS, handlerFunction> _handlers;
        std::bitset<255> _enabled;
        constexpr std::bitset<255> _always_enabled() {
            std::bitset bits;
            bits.set(...);
            ...
            return bits;
        };

        void handleCommand(std::unique_ptr<RnpPacketSerialized> packetptr);
        
        void LaunchCommand(const RnpPacketSerialized& packet);
        void ResetCommand(const RnpPacketSerialized& packet);
        void AbortCommand(const  RnpPacketSerialized& packet);
        void SetHomeCommand(const RnpPacketSerialized& packet);
        void StartLoggingCommand(const RnpPacketSerialized& packet);
        void StopLoggingCommand(const RnpPacketSerialized& packet);
        void TelemetryCommand(const RnpPacketSerialized& packet);
        void ClearFlashCommand(const RnpPacketSerialized& packet);
        void ClearSDCommand(const RnpPacketSerialized& packet);
        void PlaySongCommand(const RnpPacketSerialized& packet);
        void SkipSongCommand(const RnpPacketSerialized& packet);
        void ClearSongQueueCommand(const RnpPacketSerialized& packet);
        void ResetOrientationCommand(const RnpPacketSerialized& packet);
        void ResetLocalizationCommand(const RnpPacketSerialized& packet);
        void SetBetaCommand(const RnpPacketSerialized& packet);
        void CalibrateAccelGyroBiasCommand(const RnpPacketSerialized& packet);
        void CalibrateMagFullCommand(const RnpPacketSerialized& packet);
        void CalibrateBaroCommand(const RnpPacketSerialized& packet);
        void IgnitionCommand(const RnpPacketSerialized& packet);
        void EnterDebugCommand(const RnpPacketSerialized& packet);
        void EnterPreflightCommand(const RnpPacketSerialized& packet);
        // void EnterGroundstationCommand(const RnpPacketSerialized& packet);
        void EnterCountdownCommand(const RnpPacketSerialized& packet);
        void EnterFlightCommand(const RnpPacketSerialized& packet);
        void EnterRecoveryCommand(const RnpPacketSerialized& packet);
        void ExitDebugCommand(const RnpPacketSerialized& packet);
        void ExitToDebugCommand(const RnpPacketSerialized& packet);
        void EngineInfoCommand(const RnpPacketSerialized& packet);
        void SetThrottleCommand(const RnpPacketSerialized& packet);
        void PyroInfoCommand(const RnpPacketSerialized& packet);
        void FireInfoCommand(const RnpPacketSerialized& packet);
        void FreeRamCommand(const RnpPacketSerialized& packet);

};	
