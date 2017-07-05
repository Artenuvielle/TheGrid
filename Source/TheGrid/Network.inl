#include <string>

struct PacketWraper {
	void* packetPointer;
	int size;
};

template<typename S> inline
PacketWraper combineHeaderAndPayload(CToSPacketType header, S* payload) {
	PacketWraper wraper;
	std::string serializedPayload;
	payload->SerializeToString(&serializedPayload);
	wraper.size = serializedPayload.size() + sizeof(CToSPacketType);
	void* packetData = malloc(wraper.size);
	memcpy(packetData, &header, sizeof(CToSPacketType));
	memcpy(reinterpret_cast<unsigned char *>(packetData) + sizeof(CToSPacketType), serializedPayload.c_str(), serializedPayload.size());
	wraper.packetPointer = packetData;
	return wraper;
}

template<typename S> inline
void Client::sendPacket(CToSPacketType header, S* payload, bool reliable) {
	PacketWraper wraper = combineHeaderAndPayload(header, payload);
	ENetPacket* packet = enet_packet_create(wraper.packetPointer, wraper.size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_peer_send(_peer, 1, packet);
	free(wraper.packetPointer);
}