#pragma once
/**
 * @file hypnos.h
 * @author Kiran de Silva
 * @brief The HyPnOs engine implementation, this engine is a throttable hybird engine whihc is only netowkred
 *  so only networked components are used throughout. 
 * @version 1
 * @date 2022-08-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "engine.h"

#include <ArduinoJson.h>
#include <memory>

#include <librrc/rocketactuator.h>
#include <librrc/networkactuator.h>
#include <librrc/networksensor.h>

struct HypnosState:public EngineState{

};


class Hypnos:public Engine{
    public:
        Hypnos(uint8_t id,JsonObjectConst engineConfig,addNetworkCallbackFunction_t addNetworkCallbackF,RnpNetworkManager& networkmanager,uint8_t handlerServiceID,LogController& logcontroller);

        void updateState() override{};

        uint8_t flightCheck() override;

        void update() override {}; 

        void armEngine() override;

        void execute(int32_t func) override;

        /**
         * @brief Control the engine throttle by varying valve displacement
         * 
         * @param u single value
         */
        void control(std::vector<float> u) override{};


    private:

        HypnosState _state;
        EngineState* getStatePtr()override {return &_state;};

        //key components of hypnos
        NetworkActuator _igniter;
        //default firing time
        uint16_t _igniterFiringTime = 1000;

        NetworkActuator _oxidiserValve;
        //default positions
        uint16_t _oxidiserValveClosed = 0;
        uint16_t _oxidiserValveFullyOpen = 180;
        uint16_t _oxidiserValvePreIgnition = 115;
        
        NetworkActuator _ventValve;
        //default positions
        uint16_t _ventValveClosed = 0;
        uint16_t _ventValveOpen = 180;

        NetworkSensor _chamberPressure; 
        static constexpr uint16_t _chamberPressurePollDelta = 500;
        uint32_t _chamberPressurePrevTime;

        NetworkSensor _tankPressure; 
        static constexpr uint16_t _tankPressurePollDelta = 500;
        uint32_t _tankPressurePrevTime;

        // NetworkSensor _temperature; 
        // static constexpr uint16_t _temperaturePollDelta = 500;
        // uint32_t _temperature

        //ignition parameters
        static constexpr uint16_t _preIgnitionDelay = 500;
        static constexpr uint16_t _postIgnitionDelay = 700;
        static constexpr uint16_t _throttleLockout = 4000;
        
        void ignite() override{};
        void shutdown() override{};
        void vent(); // fully vent main ox tank - do we also want to stop venting at some point?
        
        static constexpr uint16_t _networkRetryInterval = 5000;
        static constexpr uint16_t _componentStateExpiry = 1000;

        /**
         * @brief Extension of the exectuor for Hypnos, check for conflicts with the base ENGINE_EXECTUE
         * 
         */
        enum class HYPNOS_EXECTUE:uint8_t{ 
            VENT = 2
        };

        class NetworkSensorManager{
            public:
                NetworkSensorManager(NetworkSensor& sensor,uint16_t polldelta,uint16_t timeout):
                _sensor(sensor),_polldelta(polldelta)
                {};

            private:
                NetworkSensor& _sensor;
                uint16_t _polldelta;
        };

        void heartbeat();

};

