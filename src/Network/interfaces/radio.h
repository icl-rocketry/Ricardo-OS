#pragma once

#include <Arduino.h>
#include <SPI.h>

#include "Storage/systemstatus.h"
#include "Storage/logController.h"

#include <memory>
#include <vector>
#include <string>
#include <queue>

#include "rnp_interface.h"
#include "rnp_packet.h"



enum class RADIO_MODE:uint8_t{
            SIMPLE,
            TURN_TIMEOUT
};
struct RadioInterfaceInfo:public RnpInterfaceInfo{
    size_t sendBufferSize;
    bool sendBufferOverflow;

    RADIO_MODE mode;
    uint32_t prevTimeSent;

    int rssi;
    float snr;
    long freqError;
};


class Radio: public RnpInterface{
    public:

        

        Radio(SPIClass& spi, SystemStatus& systemstatus,LogController& logcontroller,RADIO_MODE mode=RADIO_MODE::SIMPLE,uint8_t id=2,std::string name="Radio");
        void setup() override;

        void sendPacket(RnpPacket& data) override;
        void update() override;
        const RnpInterfaceInfo* getInfo() override;
        
        



    private:
        
        SPIClass& _spi; //pointer to spi class 
        SystemStatus& _systemstatus; //pointer to system status object  
        LogController& _logcontroller;

        RadioInterfaceInfo _info;

        std::queue<std::vector<uint8_t> > _sendBuffer;
        size_t _currentSendBufferSize;

        bool _txDone;

        void getPacket();
        void checkSendBuffer();
        size_t send(std::vector<uint8_t> &data);
        void checkTx();
        void sendFromBuffer();

        static constexpr int turnTimeout = 250;
        bool _received;
        
  
};



