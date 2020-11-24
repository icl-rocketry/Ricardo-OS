#include "usb.h"

#include "config.h"
#include "iface.h"
#include "interfaces.h"

#include "Logging/systemstatus.h"

#include "../packets.h"

#include <memory>


USB::USB(Stream* stream,SystemStatus* systemstatus) :
_stream(stream),
_systemstatus(systemstatus)
{};

void USB::setup(){

};

void USB::send_packet(uint8_t* data, size_t size){ // From RICARDO to USB
    //maybe check if we can write using serial.avalibalewrite not sure what action we would take 
    //if we couldnt write though
    Serial.write(data,size);

};

void USB::get_packet(std::vector<std::shared_ptr<uint8_t>> *buf){
    //return if stream object is null
    if (_stream == nullptr) return;   

    while (_stream->available() > 0){
        //find and process any and all packets

        
        _firstByte = _stream->peek();
        
        if (_firstByte == 0xAF || _incompletePacketReceived){

            if(!_incompletePacketReceived){
                //reset timeoutcounter
                _timeoutCounter = 0;
                //read first bytes of stream to get packet header data into next elements in array
                _stream->readBytes(&_tmp_packet_data[0],_packetHeader_size);

                // delete previous instance of temporary packer header object - should be fine if _packetHeader_ptr is null
                //delete _packetHeader_ptr; 
                //create new instance of a packet header and give the first bytes of incoming packet to decode
                //_packetHeader_ptr = new PacketHeader(&_tmp_packet_data[0], _packetHeader_size);

                //create packet header object to decode packet header and retrieve packet size
                PacketHeader packetheader = PacketHeader(&_tmp_packet_data[0]);
                //get decoded packet length 
                _packet_len = packetheader.packet_len;


            };

            if (_packet_len > SERIAL_SIZE_RX){
                //This basically is a buffer overflow as the serial buffer wont be able to contain this size of packet
                //in theory this shouldn't happen unless someone does something wrong
                //TODO
                //handle oversized packets some how...
                //return nothing flush buffer automatically

            }else if (_packet_len - _packetHeader_size > _stream->available()){
                //minus packetheadersize to account for bytes read for header
                //we dont have the full packet to read 

                //increment timeoutcounter
                _timeoutCounter += 1;

                //return out of function so we can wait for new data without blocking - idk if this is a bad idea could cause a buffer overflow
                //potentially implement a timeout function so if the packet is formed wrong we dont get stuck here
                if (_timeoutCounter > MAX_SERIAL_TIMEOUT){
                    _incompletePacketReceived = false;
                }else{
                    _incompletePacketReceived = true;
                };
                
            }else{
                //only a single packet to read or multiple packets to read so we will only the first packet
                _incompletePacketReceived = false;

                //uint8_t* packet_ptr = new uint8_t[_packet_len]; // Allocate a new chunk of memory for the packet
                
                //create shared ptr with custom deleter
                std::shared_ptr<uint8_t> packet_ptr(new uint8_t[_packet_len], [](uint8_t *p) { delete[] p; });
                
                //deserialize packet header, modify source interface and reserialize.
                PacketHeader packetheader = PacketHeader(&_tmp_packet_data[0]);
                //update source interface
                packetheader.src_interface = static_cast<uint8_t>(Interface::USBSerial);
                //serialize packet header
                std::vector<uint8_t> modified_packet_header;
                packetheader.serialize(modified_packet_header);

                //copy data in modified_packet_header to packet container
                memcpy(packet_ptr.get(),modified_packet_header.data(),_packetHeader_size);
                //read bytes in stream buffer into the packet data array starting at the 8th index as header has been read out of stream buffer.
                //packet len has been decremented 8 as packet_len includes the packet header which is no longer in stream buffer
                _stream->readBytes((packet_ptr.get() + _packetHeader_size), (_packet_len - _packetHeader_size)); 

                //should add exceptioj checking here so we know if we have failed to properly read the data into the packet ptr
                buf->push_back(packet_ptr); // add pointer to packet immediately to buffer
            
            };
            
        }else{
            // read byte to clear byte in serial buffer
            _stream->read(); 
        };  
    };      

};   
