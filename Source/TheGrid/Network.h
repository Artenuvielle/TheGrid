#pragma once

#include "NetworkPackets.h"
#include <enet/enet.h>

class SToCPacketHandler {
public:
	virtual void handleConnect() = 0;
	virtual void handleDisconnect() = 0;
	virtual void handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData) = 0;
};

class Client {
public:
	Client();
	~Client();

	void setPacketHandler(SToCPacketHandler* handler);
	SToCPacketHandler* getPacketHandler();

	bool connect(const char* hostAdress, short port);
	void disconnect();

	bool isConnected();
	void networkLoop();
	template<typename S> void sendPacket(CToSPacketType header, S* data, bool reliable = false);
private:
	SToCPacketHandler* _packetHandler;

	ENetHost* _enetHost;
	ENetPeer* _peer;

	bool keepConnection;
	bool netwokLoopRunning;
};

void networkLoopOnClient(void* client);

#include "Network.inl"