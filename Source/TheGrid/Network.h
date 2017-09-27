#pragma once

#include "NetworkPackets.h"
#include <enet/enet.h>

class SToCPacketHandler {
public:
	virtual void handleConnect() = 0;
	virtual void handleDisconnect() = 0;
	virtual void handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet) = 0;
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
	void sendPacket(ProtobufMessagePacket* payload, bool reliable = false);
private:
	SToCPacketHandler* _packetHandler;

	ENetHost* _enetHost;
	ENetPeer* _peer;

	bool keepConnection;
	bool netwokLoopRunning;
};

void networkLoopOnClient(void* client);