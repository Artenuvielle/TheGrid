// Fill out your copyright notice in the Description page of Project Settings.

#include "GameControllActor.h"

#include <stdlib.h>
#include "LogStream.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"

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

PositionPacketType createPosition(FVector pos) {
	PositionPacketType ret;
	ret.set_x(pos.Y);
	ret.set_y(pos.Z);
	ret.set_z(-pos.X);
	return ret;
}

OrientationPacketType createOrientation(FQuat rot) {
	OrientationPacketType ret;
	ret.set_x(rot.Y);
	ret.set_y(rot.Z);
	ret.set_z(-rot.X);
	ret.set_w(rot.W);
	return ret;
}

AGameControllActor::AGameControllActor() : Super() {
	std::cout.rdbuf(&logStream);
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGameControllActor::BeginPlay()
{
	Super::BeginPlay();

	_networkWorker = new NetworkWorker("127.0.0.1", 13244);
	_userActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_enemyActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_userActor->Init(userFaction, false);
	_enemyActor->Init(enemyFaction, true);

	APawn* userPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<UCameraComponent*> cameras;
	userPawn->GetComponents<UCameraComponent>(cameras, true);
	_headComponent = (USceneComponent*)cameras[0];
	TArray<UMotionControllerComponent*> motionControllers;
	userPawn->GetComponents<UMotionControllerComponent>(motionControllers, true);
	for (int i = 0; i < motionControllers.Num(); i++) {
		if (motionControllers[i]->Hand == EControllerHand::Left) {
			_shieldArmComponent = (USceneComponent*)motionControllers[i];
		}
		else if (motionControllers[i]->Hand == EControllerHand::Right) {
			_diskArmComponent = (USceneComponent*)motionControllers[i];
		}
	}
}

void AGameControllActor::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	TArray<PacketInformation> packets = _networkWorker->getPacketInformation();
	int lastIndex = packets.Num() - 1;
	for (int i = 0; i <= lastIndex; i++) {
		handleSToCPacket(packets[i].peerId, packets[i].header, packets[i].serializedData);
	}

	if (_userId >= 0) {
		sendPositionInformation();
	}
}

void AGameControllActor::sendPositionInformation()
{
	PlayerPosition* pp = new PlayerPosition();
	pp->set_player_id(_userId);
	pp->set_faction_id(userFaction);
	PositionPacketType head_pos = createPosition(_userActor->getHeadPosition());
	PositionPacketType main_hand_pos = createPosition(_userActor->getDiskArmPosition());
	PositionPacketType off_hand_pos = createPosition(_userActor->getShieldArmPosition());
	OrientationPacketType head_rot = createOrientation(_userActor->getHeadRotation());
	OrientationPacketType main_hand_rot = createOrientation(_userActor->getDiskArmRotation());
	OrientationPacketType off_hand_rot = createOrientation(_userActor->getShieldArmRotation());
	pp->set_allocated_head_pos(&head_pos);
	pp->set_allocated_head_rot(&head_rot);
	pp->set_allocated_main_hand_pos(&main_hand_pos);
	pp->set_allocated_main_hand_rot(&main_hand_rot);
	pp->set_allocated_off_hand_pos(&off_hand_pos);
	pp->set_allocated_off_hand_rot(&off_hand_rot);
	_networkWorker->getClient()->sendPacket<PlayerPosition>(CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION, pp);
}

void AGameControllActor::requestGameStart()
{
	if (_userId >= 0) {
		UE_LOG(LogTemp, Display, TEXT("requesting game start"));
		GameInformation* gi = new GameInformation();
		gi->set_is_running(true);
		_networkWorker->getClient()->sendPacket(CTOS_PACKET_TYPE_START_GAME_REQUEST, gi, true);
	}
}

void AGameControllActor::updateTrigger(float value)
{
}

void AGameControllActor::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAction("MenuButtonLeft", EInputEvent::IE_Pressed, this, &AGameControllActor::requestGameStart);
	InputComponent->BindKey(EKeys::G, EInputEvent::IE_Pressed, this, &AGameControllActor::requestGameStart);
	InputComponent->BindAxis("RightTriggerAnalog", this, &AGameControllActor::updateTrigger);
}

void AGameControllActor::handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData)
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

void AGameControllActor::handleGameStateBroadcast(GameInformation* information)
{
	if (information->is_running() && !_gameRunning) {
		UE_LOG(LogTemp, Display, TEXT("game starting"));
		_gameRunning = true;
	}
	UE_LOG(LogTemp, Warning, TEXT("handleGameStateBroadcast"));
}

void AGameControllActor::handlePlayerIdentification(PlayerInformation* information)
{
	_userId = information->player_id();
	_setFaction = information->faction_id() == 0 ? userFaction : enemyFaction;
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerIdentification"));
}

void AGameControllActor::handlePlayerPositionBroadcast(PlayerPosition* information)
{
	APlayerActor* actor = _userActor;
	if (information->faction_id() != _setFaction)
	{
		actor = _enemyActor;
	}

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

void AGameControllActor::handlePlayerChangeLifeBroadcast(PlayerCounterInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerChangeLifeBroadcast"));
}

void AGameControllActor::handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handlePlayerChangeShieldChargeBroadcast"));
}

void AGameControllActor::handleDiskStatusBroadcast(DiskStatusInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleDiskStatusBroadcast"));
}

void AGameControllActor::handleDiskThrowBroadcast(DiskThrowInformation* information)
{
	UE_LOG(LogTemp, Warning, TEXT("handleDiskThrowBroadcast"));
}

void AGameControllActor::handleDiskPositionBroadcast(DiskPosition* information)
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

Client * NetworkWorker::getClient()
{
	return _client;
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
