// Fill out your copyright notice in the Description page of Project Settings.

#include "PvPModeBase.h"

#include "NetworkPackets.pb.h"

void APvPModeBase::InitGame()
{
	DiskPosition dpos;
	PositionPacketType pos;
	dpos.set_allocated_disk_pos(&pos);
	dpos.set_faction_id(0);
	dpos.set_player_id(0);
}
