// Fill out your copyright notice in the Description page of Project Settings.

#include "PvPModeBase.h"
#include <stdlib.h>
#include "LogStream.h"

LogStream logStream;

template<typename S> inline
S* deserialize(std::string serializedData) {
	S* ret = new S();
	ret->ParseFromString(serializedData);
	return ret;
}

APvPModeBase::APvPModeBase() : Super() {
	std::cout.rdbuf(&logStream);
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void APvPModeBase::InitGame(const FString & in1, const FString & in2, FString & in3)
{
	UE_LOG(LogTemp, Warning, TEXT("PvP Game initiating..."));

	_networkWorker = new NetworkWorker("127.0.0.1", 13244);

	Super::InitGame(in1, in2, in3);
}

void APvPModeBase::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	TArray<PacketInformation> packets = _networkWorker->getPacketInformation();
	int lastIndex = packets.Num() - 1;
	for (int i = 0; i <= lastIndex; i++) {
		handleSToCPacket(packets[i].peerId, packets[i].header, packets[i].serializedData);
	}
}

void APvPModeBase::handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData)
{
	switch (*header) {
	case STOC_PACKET_TYPE_GAME_STATE_BROADCAST:
		handleGameStateBroadcast(deserialize<GameInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_PLAYER_IDENTIFICATION:
		handlePlayerIdentification(deserialize<PlayerInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST:
		handlePlayerPositionBroadcast(deserialize<PlayerPosition>(serializedData));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST:
		handlePlayerChangeLifeBroadcast(deserialize<PlayerCounterInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST:
		handlePlayerChangeShieldChargeBroadcast(deserialize<PlayerCounterInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_DISK_STATUS_BROADCAST:
		handleDiskStatusBroadcast(deserialize<DiskStatusInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_DISK_THROW_BROADCAST:
		handleDiskThrowBroadcast(deserialize<DiskThrowInformation>(serializedData));
		break;
	case STOC_PACKET_TYPE_DISK_POSITION_BROADCAST:
		handleDiskPositionBroadcast(deserialize<DiskPosition>(serializedData));
		break;
	}
}

void APvPModeBase::handleGameStateBroadcast(GameInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleGameStateBroadcast"));
}

void APvPModeBase::handlePlayerIdentification(PlayerInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerIdentification"));
}

void APvPModeBase::handlePlayerPositionBroadcast(PlayerPosition* information)
{
	std::cout << "player " << information->player_id() << " position: Vec3f("
		<< information->head_pos().x() << ", "
		<< information->head_pos().y() << ", "
		<< information->head_pos().z() << ")" << std::endl;
}

void APvPModeBase::handlePlayerChangeLifeBroadcast(PlayerCounterInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerChangeLifeBroadcast"));
}

void APvPModeBase::handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerChangeShieldChargeBroadcast"));
}

void APvPModeBase::handleDiskStatusBroadcast(DiskStatusInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleDiskStatusBroadcast"));
}

void APvPModeBase::handleDiskThrowBroadcast(DiskThrowInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleDiskThrowBroadcast"));
}

void APvPModeBase::handleDiskPositionBroadcast(DiskPosition* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleDiskPositionBroadcast"));
}



NetworkWorker::NetworkWorker(const char *host, short port)
	: _host(host),
	_port(port)
{
	_client = new Client();
	_client->setPacketHandler(this);

	_packets.Reserve(MAX_PACKETS_PER_TICK);

	_thread = FRunnableThread::Create(this, TEXT("NetworkWorker"));
}

NetworkWorker::~NetworkWorker()
{
	_client->disconnect();
	delete _thread;
	delete _client;
}

TArray<PacketInformation> NetworkWorker::getPacketInformation()
{
	TArray<PacketInformation> arrayToReturn;
	_mutex.Lock();
		int lastIndex = _packets.Num() - 1;
		arrayToReturn.Reserve(lastIndex);
		for (int i = 0; i <= lastIndex; i++) {
			arrayToReturn.Add(_packets[0]);
			_packets.RemoveAt(0);
		}
	_mutex.Unlock();
	return arrayToReturn;
}

void NetworkWorker::handleConnect() {}

void NetworkWorker::handleDisconnect() {}

void NetworkWorker::handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData) {
	_mutex.Lock();
		while (_packets.Num() >= MAX_PACKETS_PER_TICK) {
			_mutex.Unlock();
			FPlatformProcess::Sleep(0.1);
			_mutex.Lock();
		}
		_packets.Add({ peerId, header, serializedData });
	_mutex.Unlock();
}

bool NetworkWorker::Init()
{
	if (_client->connect(_host, _port)) {
		UE_LOG(LogTemp, Warning, TEXT("connection established"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("could not connect"));
		return false;
	}
}

//Run
uint32 NetworkWorker::Run()
{
	FPlatformProcess::Sleep(0.03);
	if (_client->isConnected()) {
		UE_LOG(LogTemp, Warning, TEXT("network loop starting"));
		_client->networkLoop();
	}
	else {
		return -1;
	}
	return 0;
}

//stop
void NetworkWorker::Stop()
{
}