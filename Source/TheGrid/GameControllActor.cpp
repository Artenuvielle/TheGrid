// Fill out your copyright notice in the Description page of Project Settings.

#include "GameControllActor.h"
#include "WallCollisionActor.h"
#include <stdlib.h>
#include "LogStream.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "LevelParameterSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplay.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"

#ifdef _simulate_
#include "Simulation.h"
#endif

#ifdef _logFrames_
#include <fstream>
std::ofstream logFile;
#endif

LogStream logStream;

FVector createVector(PositionPacketType position) {
	return FVector(-position.z(), position.x(), position.y());
}

FQuat createQuat(OrientationPacketType orientation) {
	return FQuat(-orientation.z(), orientation.x(), orientation.y(), -orientation.w());
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
	ret.set_w(-rot.W);
	return ret;
}

AGameControllActor::AGameControllActor() : Super() {
	std::cout.rdbuf(&logStream);
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	_gameEndingAnimation = nullptr;
	gameRunning = false;
	_userId = -1;

#ifdef _simulate_
	_simulateStartTime = 0;
#endif
#ifdef _logFrames_
	_triggerPush = 0.0;
	_startedLogging = false;
	logFile.open("tracking_log.txt");
#endif
}

AGameControllActor::~AGameControllActor()
{
	delete _networkWorker;
	if (_serverWorker != nullptr) {
		delete _serverWorker;
	}
#ifdef _logFrames_
	logFile.close();
#endif
}

void AGameControllActor::BeginPlay()
{
	Super::BeginPlay();
	FString configFileName = "config.ini";
	FString serverConfig = "Server";
	const char* ip = "127.0.0.1";
	short port = 13244;
	isSpectating = false;
	ULevelParameterSaveGame* LoadGameInstance = Cast<ULevelParameterSaveGame>(UGameplayStatics::CreateSaveGameObject(ULevelParameterSaveGame::StaticClass()));
	if (UGameplayStatics::DoesSaveGameExist(TEXT("levelParameters"), 0)) {
		LoadGameInstance = Cast<ULevelParameterSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("levelParameters"), 0));
	}
	ip = TCHAR_TO_ANSI(*(LoadGameInstance->ip));
	port = (short) LoadGameInstance->port;

	if (LoadGameInstance->startOwnServer) {
		_serverWorker = new ServerWorker(port);
	}
	else {
		_serverWorker = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Attempting to connect to %s:%d"), ip, port);
	_networkWorker = new NetworkWorker(ip, port);

	_userActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_enemyActor = GetWorld()->SpawnActor<APlayerActor>(APlayerActor::StaticClass());
	_userActor->Init(userFaction, false);
	_enemyActor->Init(enemyFaction, true);
	_userActor->getDiskActor()->attach(this);
	_enemyActor->getDiskActor()->attach(this);
	_userActor->getShieldActor()->setEnemyDisk(_enemyActor->getDiskActor());
	_enemyActor->getShieldActor()->setEnemyDisk(_userActor->getDiskActor());

	APawn* userPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TArray<UCameraComponent*> cameras;
	userPawn->GetComponents<UCameraComponent>(cameras, true);
	for (int i = 0; i < cameras.Num(); i++) {
		if (cameras[i]->IsActive()) {
			_headComponent = (USceneComponent*)cameras[i];
			_defaultCamera = cameras[i];
		}
		else {
			_spectatorCameraComponent = cameras[i];
		}
	}
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

void AGameControllActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGameControllActor::Tick(float deltaSeconds)
{
	_time += deltaSeconds;
	Super::Tick(deltaSeconds);
	TArray<PacketInformation> packets = _networkWorker->getPacketInformation();
	int lastIndex = packets.Num() - 1;
	for (int i = 0; i <= lastIndex; i++) {
		handleSToCPacket(packets[i].peerId, packets[i].packet);
	}

#ifdef _simulate_
	if (_simulateStartTime > 0) {
		SimStep simStep = getSimulationStep(_time - _simulateStartTime);
		_headComponent->SetWorldLocation(simStep.head_position);
		_headComponent->SetWorldRotation(simStep.head_orientation);
		_diskArmComponent->SetRelativeLocation(simStep.disk_arm_position);
		_diskArmComponent->SetRelativeRotation(simStep.disk_arm_orientation);
		_shieldArmComponent->SetRelativeLocation(simStep.shield_arm_position);
		_shieldArmComponent->SetRelativeRotation(simStep.shield_arm_orientation);
		_simTriggerPush = simStep.triggerPush;
	}
#endif

	if (!isSpectating) {
		_userActor->setHeadPosition(_headComponent->GetComponentLocation());
		_userActor->setHeadRotation(FQuat(_headComponent->GetComponentRotation()));
		_userActor->setDiskArmPosition(_diskArmComponent->GetComponentLocation());
		_userActor->setDiskArmRotation(FQuat(_diskArmComponent->GetComponentRotation()));
		_userActor->setShieldArmPosition(_shieldArmComponent->GetComponentLocation());
		_userActor->setShieldArmRotation(FQuat(_shieldArmComponent->GetComponentRotation()));

		if (_userId >= 0) {
			sendPositionInformation();
		}
	}

#ifdef _logFrames_
	if (_startedLogging) {
		FVector head_position = _headComponent->GetComponentLocation();
		FVector wand_position = _diskArmComponent->GetComponentLocation();
		FVector shield_position = _shieldArmComponent->GetComponentLocation();
		FQuat head_orientation = FQuat(_headComponent->GetComponentRotation());
		FQuat wand_orientation = FQuat(_diskArmComponent->GetComponentRotation());
		FQuat shield_orientation = FQuat(_shieldArmComponent->GetComponentRotation());
		logFile << "		simSteps.push(SimStep(" << (_time - _logStartTime) << ", " << _triggerPush << ", ";
		logFile << "FVector(" << head_position.X << ", " << head_position.Y << ", " << head_position.Z << "), ";
		logFile << "FQuat(" << head_orientation.X << ", " << head_orientation.Y << ", " << head_orientation.Z << ", " << head_orientation.W << "), ";
		logFile << "FVector(" << wand_position.X << ", " << wand_position.Y << ", " << wand_position.Z << "), ";
		logFile << "FQuat(" << wand_orientation.X << ", " << wand_orientation.Y << ", " << wand_orientation.Z << ", " << wand_orientation.W << "), ";
		logFile << "FVector(" << shield_position.X << ", " << shield_position.Y << ", " << shield_position.Z << "), ";
		logFile << "FQuat(" << shield_orientation.X << ", " << shield_orientation.Y << ", " << shield_orientation.Z << ", " << shield_orientation.W << ")));\n";
	}
#endif
}

void AGameControllActor::sendPositionInformation()
{
	if (isSpectating) {
		return;
	}
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
	ProtobufMessagePacket* packet = new ProtobufMessagePacket();
	packet->set_header(CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION);
	packet->set_allocated_player_position(pp);
	_networkWorker->getClient()->sendPacket(packet);
}

void AGameControllActor::requestGameStart()
{
	if (_userId >= 0) {
		UE_LOG(LogTemp, Display, TEXT("requesting game start"));
		GameInformation* gi = new GameInformation();
		gi->set_is_running(true);
		ProtobufMessagePacket* packet = new ProtobufMessagePacket();
		packet->set_header(CTOS_PACKET_TYPE_START_GAME_REQUEST);
		packet->set_allocated_game_information(gi);
		_networkWorker->getClient()->sendPacket(packet, true);
	}
}

void AGameControllActor::quitToMenu()
{
	if (!gameRunning) {
		UGameplayStatics::OpenLevel(GetWorld(), "MainMenuLevel");
	}
}

void AGameControllActor::updateTrigger(float value)
{
#ifdef _simulate_
	if (_simulateStartTime > 0) {
		value = _simTriggerPush;
	}
#endif
	if (value > 0.6) {
		if (_userActor->getDiskActor()->getState() == DISK_STATE_READY) {
			_userActor->getDiskActor()->startDraw(_userActor->getDiskActor()->getPosition());
		}
	}
	else if (value < 0.4) {
		if (_userActor->getDiskActor()->getState() == DISK_STATE_DRAWN) {
			_userActor->getDiskActor()->endDraw(_userActor->getDiskActor()->getPosition());
		}
	}
#ifdef _logFrames_
	_triggerPush = value;
#endif
}

bool AGameControllActor::observableUpdate(GameNotifications notification, Observable<GameNotifications>* src)
{
	switch (notification) {
	case GAME_NOTIFICATION_DISK_THROWN:
		if (src == _userActor->getDiskActor()) {
			DiskThrowInformation* dti = new DiskThrowInformation();
			dti->set_player_id(_userId);
			dti->set_faction_id(userFaction);
			PositionPacketType disk_pos = createPosition(_userActor->getDiskActor()->getPosition());
			PositionPacketType disk_momentum = createPosition(_userActor->getDiskActor()->getMomentum());
			dti->set_allocated_disk_pos(&disk_pos);
			dti->set_allocated_disk_momentum(&disk_momentum);
			ProtobufMessagePacket* packet = new ProtobufMessagePacket();
			packet->set_header(CTOS_PACKET_TYPE_PLAYER_THROW_INFORMATION);
			packet->set_allocated_disk_throw_information(dti);
			_networkWorker->getClient()->sendPacket(packet, true);
		}
		break;
	}
	return true;
}

void AGameControllActor::observableRevoke(GameNotifications notification, Observable<GameNotifications>* src)
{
}

#ifdef _simulate_
void AGameControllActor::startSimulation()
{
	initSimulation();
	_simulateStartTime = _time;
}
#endif

void AGameControllActor::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindKey(EKeys::Escape, EInputEvent::IE_Pressed, this, &AGameControllActor::quitToMenu);

	InputComponent->BindAction("MenuButtonLeft", EInputEvent::IE_Pressed, this, &AGameControllActor::requestGameStart);
	InputComponent->BindAction("LeftTrigger", EInputEvent::IE_Pressed, this, &AGameControllActor::requestGameStart);
	InputComponent->BindKey(EKeys::G, EInputEvent::IE_Pressed, this, &AGameControllActor::requestGameStart);

	InputComponent->BindAxis("RightTriggerAnalog", this, &AGameControllActor::updateTrigger);

#ifdef _simulate_
	InputComponent->BindKey(EKeys::X, EInputEvent::IE_Pressed, this, &AGameControllActor::startSimulation);
#endif
#ifdef _logFrames_
	InputComponent->BindAction("MenuButtonRight", EInputEvent::IE_Pressed, this, &AGameControllActor::switchLoggingOnOff);
#endif
}

#ifdef _logFrames_
void AGameControllActor::switchLoggingOnOff()
{
	_logStartTime = _time;
	_startedLogging = !_startedLogging;
}
#endif

void AGameControllActor::handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet)
{
	switch (packet->header()) {
	case STOC_PACKET_TYPE_GAME_STATE_BROADCAST:
		handleGameStateBroadcast(packet->game_information());
		break;
	case STOC_PACKET_TYPE_PLAYER_IDENTIFICATION:
		handlePlayerIdentification(packet->player_information());
		break;
	case STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST:
		handlePlayerPositionBroadcast(packet->player_position());
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST:
		handlePlayerChangeLifeBroadcast(packet->player_counter_information());
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST:
		handlePlayerChangeShieldChargeBroadcast(packet->player_counter_information());
		break;
	case STOC_PACKET_TYPE_DISK_STATUS_BROADCAST:
		handleDiskStatusBroadcast(packet->disk_status_information());
		break;
	case STOC_PACKET_TYPE_DISK_THROW_BROADCAST:
		handleDiskThrowBroadcast(packet->disk_throw_information());
		break;
	case STOC_PACKET_TYPE_DISK_POSITION_BROADCAST:
		handleDiskPositionBroadcast(packet->disk_position());
		break;
	case STOC_PACKET_TYPE_WALL_COLLISION_INFORMATION:
		handleWallCollisonInformation(packet->wall_collision_information());
		break;
	}
}

void AGameControllActor::handleGameStateBroadcast(GameInformation information)
{
	if (information.is_running()) {
		if (!gameRunning) {
			if (_gameEndingAnimation) {
				_gameEndingAnimation->Destroy();
				_gameEndingAnimation = nullptr;
			}
			UE_LOG(LogTemp, Log, TEXT("game starting"));
			gameRunning = true;
		}
	}
	else {
		gameRunning = false;
		if (information.has_winning_player_id()) {
			_gameEndingAnimation = GetWorld()->SpawnActor<AGameEndingAnimationActor>(AGameEndingAnimationActor::StaticClass());
			if (information.winning_player_id() == _userId) {
				_gameEndingAnimation->Init(userFaction, true);
				UE_LOG(LogTemp, Log, TEXT("user won"));
			}
			else {
				_gameEndingAnimation->Init(userFaction, false);
				UE_LOG(LogTemp, Log, TEXT("enemy won"));
			}
		}
	}
}

void AGameControllActor::handlePlayerIdentification(PlayerInformation information)
{
	_userId = information.player_id();
	_setFaction = information.faction_id() == 0 ? userFaction : enemyFaction;
	if (information.is_spectator()) {
		_userActor->setDrawState(true);
		isSpectating = true;
		_spectatorCameraComponent->SetActive(true);
		_defaultCamera->SetActive(false);
	} else {
		if (GEngine && GEngine->HMDDevice.Get()) {
			GEngine->HMDDevice.Get()->EnableHMD(true);
		}
	}
}

void AGameControllActor::handlePlayerPositionBroadcast(PlayerPosition information)
{
	APlayerActor* actor = _userActor;
	if (information.faction_id() != _setFaction)
	{
		actor = _enemyActor;
	}

	actor->setHeadPosition(createVector(information.head_pos()));
	actor->setHeadRotation(createQuat(information.head_rot()));
	actor->setDiskArmPosition(createVector(information.main_hand_pos()));
	actor->setDiskArmRotation(createQuat(information.main_hand_rot()));
	actor->setShieldArmPosition(createVector(information.off_hand_pos()));
	actor->setShieldArmRotation(createQuat(information.off_hand_rot()));
}

void AGameControllActor::handlePlayerChangeLifeBroadcast(PlayerCounterInformation information)
{
	if (information.faction_id() == _setFaction) {
		_userActor->getLifeCounterActor()->setLifeCount(information.counter());
	}
	else {
		_enemyActor->getLifeCounterActor()->setLifeCount(information.counter());
	}
}

void AGameControllActor::handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation information)
{
	if (information.faction_id() == _setFaction) {
		_userActor->getShieldActor()->setCharges(information.counter());
	}
	else {
		_enemyActor->getShieldActor()->setCharges(information.counter());
	}
}

void AGameControllActor::handleDiskStatusBroadcast(DiskStatusInformation information)
{
	if (information.faction_id() == _setFaction) {
		if (information.disk_status() == DISK_STATE_RETURNING) {
			if (_userActor->getDiskActor()->getState() == DISK_STATE_READY) {
				_userActor->getDiskActor()->startDraw(_userActor->getDiskActor()->getPosition());
			}
			if (_userActor->getDiskActor()->getState() == DISK_STATE_DRAWN) {
				_userActor->getDiskActor()->forceThrow(_userActor->getDiskActor()->getPosition(), FVector::ForwardVector);
			}
			_userActor->getDiskActor()->forceReturn();
		}
		else if(information.disk_status() == DISK_STATE_READY) {
			if (_userActor->getDiskActor()->getState() == DISK_STATE_DRAWN) {
				_userActor->getDiskActor()->forceThrow(_userActor->getDiskActor()->getPosition(), FVector::ForwardVector);
			}
			if (_userActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY) {
				_userActor->getDiskActor()->forceReturn();
			}
			_userActor->getDiskActor()->catchDisk();
		}
	}
	else {
		switch (information.disk_status())
		{
		case DISK_STATE_READY:
			UE_LOG(LogTemp, Log, TEXT("DISK_STATE_READY")); break;
		case DISK_STATE_DRAWN:
			UE_LOG(LogTemp, Log, TEXT("DISK_STATE_DRAWN")); break;
		case DISK_STATE_FREE_FLY:
			UE_LOG(LogTemp, Log, TEXT("DISK_STATE_FREE_FLY")); break;
		case DISK_STATE_RETURNING:
			UE_LOG(LogTemp, Log, TEXT("DISK_STATE_RETURNING")); break;
		default:
			break;
		}
		if (information.disk_status() == DISK_STATE_RETURNING) {
			if (_enemyActor->getDiskActor()->getState() == DISK_STATE_READY) {
				_enemyActor->getDiskActor()->startDraw(_enemyActor->getDiskActor()->getPosition());
			}
			if (_enemyActor->getDiskActor()->getState() == DISK_STATE_DRAWN) {
				_enemyActor->getDiskActor()->forceThrow(_enemyActor->getDiskActor()->getPosition(), FVector::ForwardVector);
			}
			_enemyActor->getDiskActor()->forceReturn();
		}
		else if (information.disk_status() == DISK_STATE_READY) {
			if (_enemyActor->getDiskActor()->getState() == DISK_STATE_DRAWN) {
				_enemyActor->getDiskActor()->forceThrow(_enemyActor->getDiskActor()->getPosition(), FVector::ForwardVector);
			}
			if (_enemyActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY) {
				_enemyActor->getDiskActor()->forceReturn();
			}
			_enemyActor->getDiskActor()->catchDisk();
		}
	}
}

void AGameControllActor::handleDiskThrowBroadcast(DiskThrowInformation information)
{
	if (information.faction_id() != _setFaction) {
		UE_LOG(LogTemp, Log, TEXT("DISK_THROW"));
		if (_enemyActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY) {
			_enemyActor->getDiskActor()->forceReturn();
		}
		if (_enemyActor->getDiskActor()->getState() == DISK_STATE_RETURNING) {
			_enemyActor->getDiskActor()->catchDisk();
		}
		_enemyActor->getDiskActor()->forceThrow(createVector(information.disk_pos()), createVector(information.disk_momentum()));
	} else {
		if (isSpectating) {
			if (_userActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY) {
				_userActor->getDiskActor()->forceReturn();
			}
			if (_userActor->getDiskActor()->getState() == DISK_STATE_RETURNING) {
				_userActor->getDiskActor()->catchDisk();
			}
			_userActor->getDiskActor()->forceThrow(createVector(information.disk_pos()), createVector(information.disk_momentum()));
		}
	}
}

void AGameControllActor::handleDiskPositionBroadcast(DiskPosition information)
{
	if (information.faction_id() == _setFaction) {
		if (_userActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY || _userActor->getDiskActor()->getState() == DISK_STATE_RETURNING) {
			_userActor->getDiskActor()->setPosition(createVector(information.disk_pos()));
			_userActor->getDiskActor()->setRotation(createQuat(information.disk_rot()));
		}
	}
	else {
		if (_enemyActor->getDiskActor()->getState() == DISK_STATE_FREE_FLY || _enemyActor->getDiskActor()->getState() == DISK_STATE_RETURNING) {
			_enemyActor->getDiskActor()->setPosition(createVector(information.disk_pos()));
			_enemyActor->getDiskActor()->setRotation(createQuat(information.disk_rot()));
		}
	}
}

void AGameControllActor::handleWallCollisonInformation(WallCollisonInformation information)
{
	AWallCollisionActor* collisionActor = GetWorld()->SpawnActor<AWallCollisionActor>(AWallCollisionActor::StaticClass());
	collisionActor->Init(information.faction_id() == _setFaction ? userFaction : enemyFaction, createVector(information.collision_pos()), collisionAnimationSize, information.collision_wall());
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

void NetworkWorker::handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet) {
	_mutex.Lock();
	while (_packets.Num() >= MAX_PACKETS_PER_TICK) {
		_mutex.Unlock();
		UE_LOG(LogTemp, Display, TEXT("Packet buffer overflowing"));
		FPlatformProcess::Sleep(0.1);
		_mutex.Lock();
	}
	_packets.Add({ peerId, packet });
	_mutex.Unlock();
}

bool NetworkWorker::Init()
{
	if (_client->connect(_host, _port)) {
		UE_LOG(LogTemp, Display, TEXT("connection established"));
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("could not connect"));
		return false;
	}
}

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

void NetworkWorker::Stop()
{
}

ServerWorker::ServerWorker(short port)
	: _port(port)
{
	_thread = FRunnableThread::Create(this, TEXT("ServerWorker"));
	FPlatformProcess::Sleep(0.5);
}

ServerWorker::~ServerWorker()
{
	_thread->Kill();
	delete _thread;
}

bool ServerWorker::Init()
{
	return true;
}

uint32 ServerWorker::Run()
{
	const FString serverPath = FPaths::ConvertRelativePathToFull(FPaths::GameContentDir()) + "Server/TheGridServer.exe";

	_handle = FPlatformProcess::CreateProc(*serverPath, *FString::FromInt(_port), false, true, false, nullptr, 0, nullptr, nullptr);
	return 0;
}

void ServerWorker::Stop()
{
	if (FPlatformProcess::IsProcRunning(_handle)) {
		FPlatformProcess::TerminateProc(_handle, true);
	}
}
