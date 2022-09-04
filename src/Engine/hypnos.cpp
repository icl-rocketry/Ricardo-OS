#include "hypnos.h"

Hypnos::Hypnos(uint8_t id,JsonObjectConst engineConfig,addNetworkCallbackFunction_t addNetworkCallbackF,RnpNetworkManager& networkmanager,uint8_t handlerServiceID,LogController& logcontroller):
Engine(id,engineConfig,addNetworkCallbackF,networkmanager,handlerServiceID,logcontroller)
{};

void Hypnos::updateState(){//force update all components
    _igniter.updateState();
    _oxidiserValve.updateState();
    _ventValve.updateState();
    _chamberPressure.updateState();
    _tankPressure.updateState();
}

void Hypnos::execute(int32_t func){
    switch(func){
        case static_cast<uint8_t>(HYPNOS_EXECTUE::VENT):
        {
            // Kiran is a poo
            vent();
            break;
        }
        default:
        {
            Engine::execute(func);
            break;
        }
    }
}

void Hypnos::vent() {
    log("Venting Started");
    _oxidiserValve.execute(_oxidiserValveClosed);
    _ventValve.execute(_ventValveOpen);
}

void Hypnos::armEngine(){
    _ventValve.arm();
    _oxidiserValve.arm();
    _igniter.arm();
};

void Hypnos::shutdown(){
    Engine::shutdown();
    _oxidiserValve.execute(_oxidiserValveClosed); // close main ox valve
    _state.runState = static_cast<uint8_t>(ENGINE_RUN_STATE::SHUTDOWN);
}

void Hypnos::ignite(){
    Engine::ignite();
    _state.runState = static_cast<uint8_t>(ENGINE_RUN_STATE::IGNITION);
    _ventValve.execute(_ventValveClosed);
    log("oxidiser valve to pre-ignition");
    _oxidiserValve.execute(_oxidiserValvePreIgnition);
}

void Hypnos::update(){
    if (_state.runState == static_cast<uint8_t>(ENGINE_RUN_STATE::IGNITION)){//if we are still in ignition phase
        if (millis() - _state.ignitionTime > _preIgnitionDelay){ // ignite igniter
            log("firing igniter");
            _igniter.execute(_igniterFiringTime);
        }else if (millis() - _state.ignitionTime > _postIgnitionDelay+_preIgnitionDelay){
            log("oxidiser valve to fully open");
            _oxidiserValve.execute(_oxidiserValveFullyOpen);
            _state.runState = static_cast<uint8_t>(ENGINE_RUN_STATE::RUNNING);
        }
    }

}

void Hypnos::control(std::vector<float> u){
    if (_state.runState == static_cast<uint8_t>(ENGINE_RUN_STATE::RUNNING)){//if the engine is running
        if (millis() - _state.ignitionTime > _throttleLockout){
            //do control
            log("control input" + std::to_string(u[0]));
        }
    }
}

