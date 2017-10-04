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
		_diskMesh       = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Geometry/Meshes/disk.disk'"), NULL, LOAD_None, NULL);
		_blueMaterial   = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/robot_material_blue.robot_material_blue'"), NULL, LOAD_None, NULL);
		_orangeMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/robot_material_orange.robot_material_orange'"), NULL, LOAD_None, NULL);
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
	_faction = faction;

	_rightTrailActor = GetWorld()->SpawnActor<ALightTrailActor>(ALightTrailActor::StaticClass());
	_leftTrailActor = GetWorld()->SpawnActor<ALightTrailActor>(ALightTrailActor::StaticClass());

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
	else if (_state == DISK_STATE_FREE_FLY || _state == DISK_STATE_RETURNING) {
		FVector diskOffset = _diskRotation.RotateVector(FVector::RightVector * (diskRadius - 1));
		_rightTrailActor->addPoint(_diskPosition + diskOffset);
		_leftTrailActor->addPoint(_diskPosition - diskOffset);
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

ALightTrailActor * ADiskActor::getRightTrailActor()
{
	return _rightTrailActor;
}

ALightTrailActor * ADiskActor::getLeftTrailActor()
{
	return _leftTrailActor;
}

bool ADiskActor::startDraw(FVector position) {
	if (_state == DISK_STATE_READY && gameRunning) {
		_lastPositionWhileDrawn = position;
		_momentum = FVector::ZeroVector;
		_state = DISK_STATE_DRAWN;
		UE_LOG(LogTemp, Log, TEXT("started drawing a disk"));
		return true;
	}
	return false;
}

bool ADiskActor::endDraw(FVector position) {
	if (!gameRunning && _state == DISK_STATE_DRAWN) {
		_state = DISK_STATE_READY;
	}
	if (_state == DISK_STATE_DRAWN) {
		FVector diskOffset = _diskRotation.RotateVector(FVector::RightVector * (diskRadius - 1));
		_rightTrailActor->Init(_faction, position + diskOffset);
		_leftTrailActor->Init(_faction, position - diskOffset);
		_state = DISK_STATE_FREE_FLY;
		_momentum.Normalize();
		UE_LOG(LogTemp, Log, TEXT("finished drawing a disk... LET IF FLYYYYYY"));
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
		_rightTrailActor->endTrail();
		_leftTrailActor->endTrail();
		_rightTrailActor = GetWorld()->SpawnActor<ALightTrailActor>(ALightTrailActor::StaticClass());
		_leftTrailActor = GetWorld()->SpawnActor<ALightTrailActor>(ALightTrailActor::StaticClass());
		_state = DISK_STATE_READY;
		return true;
	}
	return false;
}