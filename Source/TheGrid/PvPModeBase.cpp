// Fill out your copyright notice in the Description page of Project Settings.

#include "PvPModeBase.h"
#include <stdlib.h>
#include "LogStream.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

LogStream logStream;

void drawGizmo(UWorld* world, FVector position, FQuat rotation, int length = 15, int width = 2) {
	DrawDebugLine(world, position, position + length * rotation.RotateVector(FVector::UpVector), FColor(0, 0, 255), false, 0.1, 0, width);
	DrawDebugLine(world, position, position + length * rotation.RotateVector(FVector::RightVector), FColor(0, 255, 0), false, 0.1, 0, width);
	DrawDebugLine(world, position, position + length * rotation.RotateVector(FVector::ForwardVector), FColor(255, 0, 0), false, 0.1, 0, width);
}

template<typename S> inline
S* deserialize(std::string serializedData) {
	S* ret = new S();
	ret->ParseFromString(serializedData);
	return ret;
}

FVector createVector(PositionPacketType position) {
	return FVector(-position.z(), position.x(), position.y());
}

FQuat createQuat(OrientationPacketType orientation) {
	return FQuat(-orientation.z(), orientation.x(), orientation.y(), orientation.w());
}

APvPModeBase::APvPModeBase() : Super() {
	std::cout.rdbuf(&logStream);
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void APvPModeBase::InitGame(const FString & in1, const FString & in2, FString & in3)
{
	Super::InitGame(in1, in2, in3);
	UE_LOG(LogTemp, Warning, TEXT("PvP Game initiating..."));

	_networkWorker = new NetworkWorker("127.0.0.1", 13244);
	_userActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_enemyActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_userActor->Init(userFaction, false);
	_enemyActor->Init(enemyFaction, true);
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
	setFaction = information->faction_id() == 0 ? userFaction : enemyFaction;
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerIdentification"));
}

void APvPModeBase::handlePlayerPositionBroadcast(PlayerPosition* information)
{
	APlayerActor* actor = _userActor;
	if (information->faction_id() != setFaction)
	{
		actor = _enemyActor;
	}

	FQuat mainHand = createQuat(information->main_hand_rot());
	std::cout << "x: " << mainHand.X << " y: " << mainHand.X << " z: " << mainHand.X << " w: " << mainHand.X << std::endl;

	actor->setHeadPosition(createVector(information->head_pos()));
	actor->setHeadRotation(createQuat(information->head_rot()));
	actor->setDiskArmPosition(createVector(information->main_hand_pos()));
	actor->setDiskArmRotation(createQuat(information->main_hand_rot()));
	actor->setShieldArmPosition(createVector(information->off_hand_pos()));
	actor->setShieldArmRotation(createQuat(information->off_hand_rot()));

	if (UWorld* g = GetWorld())
	{
		drawGizmo(GetWorld(), createVector(information->head_pos()), createQuat(information->head_rot()));
		drawGizmo(GetWorld(), createVector(information->main_hand_pos()), createQuat(information->main_hand_rot()));
		drawGizmo(GetWorld(), createVector(information->off_hand_pos()), createQuat(information->off_hand_rot()));
	}
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