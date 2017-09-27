// Fill out your copyright notice in the Description page of Project Settings.

#include "DiscActor.h"

UStaticMesh* ADiscActor::_discMeshInner = nullptr;
UStaticMesh* ADiscActor::_discMeshOuter = nullptr;
UMaterial*   ADiscActor::_blueMaterial = nullptr;
UMaterial*   ADiscActor::_orangeMaterial = nullptr;

ADiscActor::ADiscActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_state = DISK_STATE_READY;
	_discPosition = FVector();
	_discRotation = FQuat();

	if (!_discMeshInner) {
		_discMeshInner = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/disk_blue_inner.disk_blue_inner'");
		_discMeshOuter = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/disk_blue_outer.disk_blue_outer'");
		_blueMaterial = getContent<UMaterial>("Material'/Game/Geometry/Meshes/blue/robotMaterialBlue.robotMaterialBlue'");
		_orangeMaterial = getContent<UMaterial>("Material'/Game/Geometry/Meshes/orange/robotMaterialOrange.robotMaterialOrange'");
	}
}

void ADiscActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADiscActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_discInnerActor->SetActorLocation(_discPosition);
	_discOuterActor->SetActorLocation(_discPosition);
	_discInnerActor->SetActorRotation(_discRotation);
	_discOuterActor->SetActorRotation(_discRotation);
}

void ADiscActor::Init(PlayerFaction faction)
{
	_discInnerActor = spawnMeshActor(GetWorld(), _discMeshInner);
	_discInnerActor->SetActorScale3D(FVector(diskRadius, diskRadius, diskHeight * 10 / 2));
	_discInnerActor->GetStaticMeshComponent()->SetMaterial(0, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);
	
	_discOuterActor = spawnMeshActor(GetWorld(), _discMeshOuter);
	_discOuterActor->SetActorScale3D(FVector(diskRadius, diskRadius, diskHeight * 10 / 2));
}

void ADiscActor::setDiscPosition(FVector pos)
{
	_discPosition = pos;
}

void ADiscActor::setDiscRotation(FQuat rot)
{
	_discRotation = rot;
}

FVector ADiscActor::getDiscPosition()
{
	return _discPosition;
}

FQuat ADiscActor::getDiscRotation()
{
	return _discRotation;
}

DiskState ADiscActor::getState()
{
	return _state;
}
