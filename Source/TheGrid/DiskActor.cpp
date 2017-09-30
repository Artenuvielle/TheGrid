// Fill out your copyright notice in the Description page of Project Settings.

#include "DiskActor.h"

UStaticMesh* ADiskActor::_diskMesh = nullptr;
UMaterial*   ADiskActor::_blueMaterial = nullptr;
UMaterial*   ADiskActor::_orangeMaterial = nullptr;

ADiskActor::ADiskActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_state = DISK_STATE_READY;
	_diskPosition = FVector();
	_diskRotation = FQuat();

	if (!_diskMesh) {
		_diskMesh       = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/disk.disk'");
		_blueMaterial   = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_blue.robot_material_blue'");
		_orangeMaterial = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_orange.robot_material_orange'");
	}
}

void ADiskActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADiskActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_diskMeshActor->SetActorLocation(_diskPosition);
	_diskMeshActor->SetActorRotation(_diskRotation);
}

void ADiskActor::Init(PlayerFaction faction)
{
	_diskMeshActor = spawnMeshActor(GetWorld(), _diskMesh);
	_diskMeshActor->SetActorScale3D(FVector(diskRadius, diskRadius, diskHeight * 10 / 2));
	_diskMeshActor->GetStaticMeshComponent()->SetMaterial(0, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);
}

void ADiskActor::setPosition(FVector pos)
{
	if (_state == DISK_STATE_DRAWN) {
		_momentum = 0.9f * _momentum + (pos - _lastPositionWhileDrawn);
		_lastPositionWhileDrawn = pos;
	}
	_diskPosition = pos;
}

void ADiskActor::setRotation(FQuat rot)
{
	_diskRotation = rot;
}

FVector ADiskActor::getPosition()
{
	return _diskPosition;
}

FVector ADiskActor::getMomentum()
{
	return _momentum;
}

FQuat ADiskActor::getRotation()
{
	return _diskRotation;
}

DiskState ADiskActor::getState()
{
	return _state;
}

bool ADiskActor::startDraw(FVector position) {
	if (_state == DISK_STATE_READY) {
		_lastPositionWhileDrawn = position;
		_momentum = FVector::ZeroVector;
		_state = DISK_STATE_DRAWN;
		UE_LOG(LogTemp, Display, TEXT("started drawing a disk"));
		return true;
	}
	return false;
}

bool ADiskActor::endDraw(FVector position) {
	if (_state == DISK_STATE_DRAWN) {
		_state = DISK_STATE_FREE_FLY;
		_momentum.Normalize();
		UE_LOG(LogTemp, Display, TEXT("finished drawing a disk... LET IF FLYYYYYY"));
		notify(GAME_NOTIFICATION_DISK_THROWN);
		return true;
	}
	return false;
}

bool ADiskActor::forceReturn() {
	if (_state == DISK_STATE_FREE_FLY) {
		_state = DISK_STATE_RETURNING;
		return true;
	}
	return false;
}

bool ADiskActor::forceThrow(FVector position, FVector mom) {
	if (startDraw(position)) {
		_momentum = mom;
		if (endDraw(position)) {
			return true;
		}
	}
	return false;
}

bool ADiskActor::catchDisk()
{
	if (_state == DISK_STATE_RETURNING) {
		_state = DISK_STATE_READY;
		return true;
	}
	return false;
}