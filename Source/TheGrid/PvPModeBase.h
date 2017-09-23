// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Network.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "GameFramework/GameModeBase.h"
#include "PvPModeBase.generated.h"

class NetworkWorker;

/**
 * 
 */
UCLASS()
class THEGRID_API APvPModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APvPModeBase();
	void InitGame(const FString &, const FString &, FString &);
	void Tick(float);
	virtual void handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData);

private:
	NetworkWorker* _networkWorker;

	void handleGameStateBroadcast(GameInformation* information);
	void handlePlayerIdentification(PlayerInformation* information);
	void handlePlayerPositionBroadcast(PlayerPosition* information);
	void handlePlayerChangeLifeBroadcast(PlayerCounterInformation* information);
	void handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation* information);
	void handleDiskStatusBroadcast(DiskStatusInformation* information);
	void handleDiskThrowBroadcast(DiskThrowInformation* information);
	void handleDiskPositionBroadcast(DiskPosition* information);
};

#define MAX_PACKETS_PER_TICK 20

struct PacketInformation {
	unsigned short peerId;
	SToCPacketType* header;
	std::string serializedData;
};

class NetworkWorker : public FRunnable, public SToCPacketHandler
{
public:
	NetworkWorker(const char *, short);
	virtual ~NetworkWorker();

	TArray<PacketInformation> getPacketInformation();

	virtual void handleConnect();
	virtual void handleDisconnect();
	virtual void handleSToCPacket(unsigned short peerId, SToCPacketType* header, std::string serializedData);

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
};
