// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "PlayerActor.h"
#include "GameEndingAnimationActor.h"
#include "Observer.h"
#include "common.h"
#include "GameControllActor.generated.h"

class NetworkWorker;
class ServerWorker;

UCLASS()
class THEGRID_API AGameControllActor : public APawn, public Observer<GameNotifications>
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGameControllActor();
	~AGameControllActor();
	void BeginPlay();
	void EndPlay(const EEndPlayReason::Type EndPlayReason);
	void Tick(float);
	virtual void handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet);
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent);
	void requestGameStart();
	void quitToMenu();
	void updateTrigger(float value);
	bool observableUpdate(GameNotifications notification, Observable<GameNotifications>* src) override;
	void observableRevoke(GameNotifications notification, Observable<GameNotifications>* src) override;

#ifdef _simulate_
	void startSimulation();
#endif
#ifdef _logFrames_
	void switchLoggingOnOff();
#endif

private:
	float _time;
	NetworkWorker* _networkWorker;
	ServerWorker* _serverWorker;
	USceneComponent* _headComponent, *_diskArmComponent, *_shieldArmComponent;
	APlayerActor* _userActor;
	APlayerActor* _enemyActor;
	AGameEndingAnimationActor* _gameEndingAnimation;
	PlayerFaction _setFaction;
	UCameraComponent* _spectatorCameraComponent;
	UCameraComponent* _defaultCamera;
	bool isSpectating;
	int _userId;

	void sendPositionInformation();
	void handleGameStateBroadcast(GameInformation information);
	void handlePlayerIdentification(PlayerInformation information);
	void handlePlayerPositionBroadcast(PlayerPosition information);
	void handlePlayerChangeLifeBroadcast(PlayerCounterInformation information);
	void handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation information);
	void handleDiskStatusBroadcast(DiskStatusInformation information);
	void handleDiskThrowBroadcast(DiskThrowInformation information);
	void handleDiskPositionBroadcast(DiskPosition information);
	void handleWallCollisonInformation(WallCollisonInformation information);

#ifdef _simulate_
	float _simulateStartTime;
	float _simTriggerPush;
#endif
#ifdef _logFrames_
	float _triggerPush;
	float _logStartTime;
	bool _startedLogging;
#endif
};


#define MAX_PACKETS_PER_TICK 512

struct PacketInformation {
	unsigned short peerId;
	ProtobufMessagePacket* packet;
};

class NetworkWorker : public FRunnable, public SToCPacketHandler
{
public:
	NetworkWorker(const char *, short);
	virtual ~NetworkWorker();

	TArray<PacketInformation> getPacketInformation();
	Client* getClient();

	virtual void handleConnect();
	virtual void handleDisconnect();
	virtual void handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet);

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
private:

	TArray<PacketInformation> _packets;
	FCriticalSection _mutex;
	Client* _client;
	FRunnableThread* _thread;
	const char* _host;
	short _port;
	bool gameRunnning;
};

class ServerWorker : public FRunnable
{
public:
	ServerWorker(short);
	virtual ~ServerWorker();

	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
private:
	short _port;
	FRunnableThread* _thread;
	FProcHandle _handle;
};
