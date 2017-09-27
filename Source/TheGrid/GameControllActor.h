// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "GameFramework/Pawn.h"
#include "PlayerActor.h"
#include "GameControllActor.generated.h"

class NetworkWorker;

UCLASS()
class THEGRID_API AGameControllActor : public APawn
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AGameControllActor();
	~AGameControllActor();
	void BeginPlay();
	void Tick(float);
	virtual void handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet);
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent);
	void requestGameStart();
	void updateTrigger(float value);

private:
	NetworkWorker* _networkWorker;
	USceneComponent* _headComponent, *_diskArmComponent, *_shieldArmComponent;
	APlayerActor* _userActor;
	APlayerActor* _enemyActor;
	PlayerFaction _setFaction;
	bool _gameRunning = false;
	int _userId = -1;

	void sendPositionInformation();
	void handleGameStateBroadcast(GameInformation information);
	void handlePlayerIdentification(PlayerInformation information);
	void handlePlayerPositionBroadcast(PlayerPosition information);
	void handlePlayerChangeLifeBroadcast(PlayerCounterInformation information);
	void handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation information);
	void handleDiskStatusBroadcast(DiskStatusInformation information);
	void handleDiskThrowBroadcast(DiskThrowInformation information);
	void handleDiskPositionBroadcast(DiskPosition information);
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
