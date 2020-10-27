//abstract class of all interfaces
#ifndef IFACE_H
#define IFACE_H
#include <Arduino.h>

class Iface{
    public:
        virtual void setup();
        virtual void send_packet(uint8_t* txpacket, size_t packet_len);
        virtual bool update();//update function 
        
        uint8_t _address;//address of interface
    

};

#endif