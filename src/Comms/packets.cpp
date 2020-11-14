//packet.cpp
#include "packets.h"

PacketHeader::PacketHeader() {}
PacketHeader::~PacketHeader() {}
PacketHeader::PacketHeader(uint8_t packet_type, uint32_t packet_size) : packet_len{packet_size}, type{packet_type} {}

PacketHeader::PacketHeader(const uint8_t* data) {
	int step = 0;
	for (int i = 0; i < header_size; ++i) {
		const auto b = data[i];
		switch (step) {
		case 0:
			if (data[i] != 0xAF) // Look for the start byte in data
			{
				step = 0;
				continue;
			}
			this->start_byte = b;
			step++;
			break;
			
		case 1:
			for (int j = sizeof(packet_len) - 1; j >= 0 ; j--) {
				this->packet_len |= data[i+(sizeof(packet_len) - j + 1)] << j*8;
			}
			i+=sizeof(packet_len); // We've read sizeof(packet_len) bytes
			step++;
			break;
		case 2:
			this->type = b;
			step++;
			break;
		case 3:
			this->source = b;
			step++;
			break;
		case 4:
			this->destination = b;
			step++;
			break;
		case 5:
			this->src_interface = b;
			step++;
			break;
		}
	}
}

void PacketHeader::serialize(std::vector<uint8_t>& buf) {
    buf.push_back(start_byte);
	Packet::serialize_uint32_t(packet_len, buf);
	buf.push_back(type);
    buf.push_back(source);
	buf.push_back(destination);	
	buf.push_back(src_interface);
}


void TelemetryPacket::serialize(std::vector<uint8_t>& buf) {
	header.serialize(buf);

	// TODO: Maybe come up with a better way to serialize?
	// Serialize the float variables into bytes
	Packet::serialize_float(x, buf);
	Packet::serialize_float(y, buf);
	Packet::serialize_float(z, buf);
	Packet::serialize_float(ax, buf);
	Packet::serialize_float(ay, buf);
	Packet::serialize_float(az, buf);
	Packet::serialize_float(vx, buf);
	Packet::serialize_float(vy, buf);
	Packet::serialize_float(vz, buf);
	Packet::serialize_uint32_t(system_time, buf);
	buf.push_back(lora_rssi);
	// TODO: Add end of packet byte??
}

void CommandPacket::serialize(std::vector<uint8_t>& buf) {
	header.serialize(buf);
	buf.push_back(command);
}

void DetailedAllPacket::serialize(std::vector<uint8_t>& buf) {
	header.serialize(buf);

	Packet::serialize_float(ax, buf);
	Packet::serialize_float(ay, buf);
	Packet::serialize_float(az, buf);

	Packet::serialize_float(gx, buf);
	Packet::serialize_float(gy, buf);
	Packet::serialize_float(gz, buf);

	Packet::serialize_float(mx, buf);
	Packet::serialize_float(my, buf);
	Packet::serialize_float(mz, buf);

	Packet::serialize_float(gps_lat, buf);
	Packet::serialize_float(gps_long, buf);
	Packet::serialize_float(gps_speed, buf);
	Packet::serialize_float(gps_alt, buf);

	Packet::serialize_float(baro_alt, buf);
	Packet::serialize_float(baro_temp, buf);
	Packet::serialize_float(baro_press, buf);

	buf.push_back(batt_volt);
	buf.push_back(batt_percent);

	Packet::serialize_uint32_t(system_time, buf);
}

DetailedAllPacket::DetailedAllPacket(const uint8_t* data) {
	header = PacketHeader(data); // Deserialize header
	const uint8_t* packet_start = data + header.header_size; // Shift to the end of the header
	for (int i = 0; i < header.packet_len; i++) {
		switch (i)
		{
		case 0:
			Packet::deserialize_float(ax, packet_start + i*sizeof(float));
			break;
		case 1:
			Packet::deserialize_float(ay, packet_start + i*sizeof(float));
			break;
		case 2:
			Packet::deserialize_float(az, packet_start + i*sizeof(float));
			break;
		case 3:
			Packet::deserialize_float(gx, packet_start + i*sizeof(float));
			break;
		case 4:
			Packet::deserialize_float(gy, packet_start + i*sizeof(float));
			break;
		case 5:
			Packet::deserialize_float(gz, packet_start + i*sizeof(float));
			break;
		case 6:
			Packet::deserialize_float(mx, packet_start + i*sizeof(float));
			break;
		case 7:
			Packet::deserialize_float(my, packet_start + i*sizeof(float));
			break;
		case 8:
			Packet::deserialize_float(mz, packet_start + i*sizeof(float));
			break;
		case 9:
			Packet::deserialize_float(gps_lat, packet_start + i*sizeof(float));
			break;
		case 10:
			Packet::deserialize_float(gps_long, packet_start + i*sizeof(float));
			break;
		case 11:
			Packet::deserialize_float(gps_speed, packet_start + i*sizeof(float));
			break;
		case 12:
			Packet::deserialize_float(gps_alt, packet_start + i*sizeof(float));
			break;
		case 13:
			batt_volt = *(packet_start + i*sizeof(float));
			break;
		case 14:
			batt_percent = *(packet_start + i*sizeof(float) + 1);
			break;
		case 15:
			for (int j = 0; j < sizeof(uint32_t); j++) {
				system_time |= uint32_t(data[i]) << 8*(sizeof(uint32_t) - j - 1); 
			}
			i += sizeof(uint32_t);
			break;
		default:
			break;
		}
	}
}

CommandPacket::CommandPacket(const uint8_t* data) {
	header = PacketHeader(data); // Deserialize header

	command = data[header.header_size]; // Get the first byte which is not the header
}

CommandPacket::~CommandPacket() {}

TelemetryPacket::TelemetryPacket(const uint8_t* data) {
	header = PacketHeader(data); // Deserialize the header
	const uint8_t* packet_start = data + header.header_size; // Shift to the end of the header
	for (int i = 0; i < header.packet_len; i++) { // First 8 bytes is header, start with 9th byte
		switch (i)
		{
		case 0:
			Packet::deserialize_float(x, packet_start + i*sizeof(float));
			break;
		case 1:
			Packet::deserialize_float(y, packet_start + i*sizeof(float));
			break;
		case 2:
			Packet::deserialize_float(z, packet_start + i*sizeof(float));
			break;
		case 3:
			Packet::deserialize_float(ax, packet_start + i*sizeof(float));
			break;
		case 4:
			Packet::deserialize_float(ay, packet_start + i*sizeof(float));
			break;
		case 5:
			Packet::deserialize_float(az, packet_start + i*sizeof(float));
			break;
		case 6:
			Packet::deserialize_float(vx, packet_start + i*sizeof(float));
			break;
		case 7:
			Packet::deserialize_float(vy, packet_start + i*sizeof(float));
			break;
		case 8:
			Packet::deserialize_float(vz, packet_start + i*sizeof(float));
			break;
		case 9:
			for (int j = 0; j < sizeof(uint32_t); j++) {
				system_time |= uint32_t(data[i]) << 8*(sizeof(uint32_t) - j - 1); 
			}
			i += sizeof(uint32_t);
		case 10 + sizeof(uint32_t):
			lora_rssi = data[i];
			break;
		default:
			break;
		}
	}
}


TelemetryPacket::TelemetryPacket() {}
TelemetryPacket::~TelemetryPacket() {}
void Packet::serialize_float(const float f, std::vector<uint8_t> &buf) {
	uint8_t data[sizeof(float)];
	memcpy(data, &f, sizeof f);

	for (int i = 0; i < sizeof(float); i++) {
		buf.push_back(data[i]);
	}
}

void Packet::serialize_uint32_t(const uint32_t &n, std::vector<uint8_t> &buf) {
	for (int i = sizeof(uint32_t) - 1; i>=0; i--) {
		buf.push_back(n >> 8*i);
	}
}

void Packet::serialize_floats(const float* nums, int num_floats, std::vector<uint8_t> &buf) {
	for (int i = 0; i < num_floats; i++) {
		Packet::serialize_float(nums[i], buf);
	}
}

void Packet::deserialize_float(float &f, const uint8_t* bytes) {
	memcpy(&f, bytes, sizeof(float));
}