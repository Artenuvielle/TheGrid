#include "Network.h"

#include <iostream>
#include <string>

template<typename S> inline
S* deserialize(std::string serializedData) {
	S* ret = new S();
	ret->ParseFromString(serializedData);
	return ret;
}

Client::Client() {
	_enetHost = enet_host_create(NULL, 1, 2, 57600 / 8, 14400 / 8);
	if (_enetHost == NULL)
	{
		std::cerr << "An error occurred while trying to create an ENet client host" << std::endl;
		exit(EXIT_FAILURE);
	}
	_packetHandler = nullptr;
	_peer = nullptr;
	netwokLoopRunning = false;
}

Client::~Client() {
	enet_host_destroy(_enetHost);
}

void Client::setPacketHandler(SToCPacketHandler* handler) {
	_packetHandler = handler;
}

SToCPacketHandler* Client::getPacketHandler() {
	return _packetHandler;
}

bool Client::connect(const char* hostAdress, short port) {
	if (_peer != nullptr) {
		disconnect();
	}
	keepConnection = true;

	ENetAddress enetAddress;
	enet_address_set_host(&enetAddress, hostAdress);
	enetAddress.port = port;
	_peer = enet_host_connect(_enetHost, &enetAddress, 2, 0);
	if (_peer == NULL) {
		std::cerr << "No available peers for initiating an ENet connection." << std::endl;
		_peer = nullptr;
		return false;
	}
	ENetEvent event;
	if (enet_host_service(_enetHost, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		if (_packetHandler != nullptr) {
			_packetHandler->handleConnect();
		}
		std::cout << "Connection to server succeeded" << std::endl;
	}
	else {
		enet_peer_reset(_peer);
		_peer = nullptr;
		std::cout << "Connection to server failed" << std::endl;
		return false;
	}
	return true;
}

void Client::disconnect() {
	if (isConnected()) {
		keepConnection = false;
		ENetEvent event;

		enet_peer_disconnect(_peer, 0);
		while (enet_host_service(_enetHost, &event, 3000) > 0)
		{
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				enet_packet_destroy(event.packet);
			}
			else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				std::cout << "disconnect acknowledged by server" << std::endl;
				break;
			}
		}
		if (_packetHandler != nullptr) {
			_packetHandler->handleDisconnect();
		}
		enet_peer_reset(_peer);
		_peer = nullptr;
		std::cout << "Disconnection succeeded" << std::endl;
		while (netwokLoopRunning) {
			Sleep(10);
		}
	}
}

void Client::networkLoop() {
	if (isConnected() && !netwokLoopRunning) {
		netwokLoopRunning = true;
		ENetEvent event;
		ProtobufMessagePacket* packet;
		std::string serializedData;
		while (keepConnection) {
			while (enet_host_service(_enetHost, &event, 0) > 0) {
				switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE:
					packet = deserialize<ProtobufMessagePacket>(std::string(reinterpret_cast<const char*>(event.packet->data), event.packet->dataLength));
					if (_packetHandler != nullptr) {
						_packetHandler->handleSToCPacket(event.peer->incomingPeerID, packet);
					}
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					if (_packetHandler != nullptr) {
						_packetHandler->handleDisconnect();
					}
					std::cout << "Server closed connection" << std::endl;
					return;
				}
			}
		}
		netwokLoopRunning = false;
	}
}

void Client::sendPacket(ProtobufMessagePacket* payload, bool reliable) {
	std::string serializedPayload;
	payload->SerializeToString(&serializedPayload);
	ENetPacket* enetPacket = enet_packet_create(serializedPayload.c_str(), serializedPayload.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	enet_peer_send(_peer, 1, enetPacket);
}

bool Client::isConnected() {
	return _peer != nullptr;
}

void networkLoopOnClient(void* client) {
	reinterpret_cast<Client*>(client)->networkLoop();
}