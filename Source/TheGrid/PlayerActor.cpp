// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerActor.h"

UStaticMesh* APlayerActor::_torsoMesh      = nullptr;
UStaticMesh* APlayerActor::_headMesh       = nullptr;
UStaticMesh* APlayerActor::_armMesh        = nullptr;
UMaterial*   APlayerActor::_blueMaterial   = nullptr;
UMaterial*   APlayerActor::_orangeMaterial = nullptr;

// Sets default values
APlayerActor::APlayerActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_headPosition = FVector();
	_headRotation = FQuat();
	_torsoPosition = FVector();
	_torsoRotation = FQuat();
	_diskArmPosition = FVector();
	_diskArmRotation = FQuat();
	_shieldArmPosition = FVector();
	_shieldArmRotation = FQuat();

	if (!_torsoMesh) {
		_torsoMesh      = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/robot_torso.robot_torso'");
		_headMesh       = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/robot_head.robot_head'");
		_armMesh        = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/robot_arm.robot_arm'");
		_blueMaterial   = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_blue.robot_material_blue'");
		_orangeMaterial = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_orange.robot_material_orange'");
	}
}

// Called when the game starts or when spawned
void APlayerActor::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerActor::Init(PlayerFaction faction, bool drawModel)
{
	ownFaction = faction;

	_discActor = GetWorld()->SpawnActor<ADiscActor>(ADiscActor::StaticClass());
	_discActor->Init(faction);

	if (drawModel)
	{
		_torsoActor = spawnMeshActor(GetWorld(), _torsoMesh);
		_torsoActor->SetActorScale3D(FVector(2.6, 2.6, 2.6));
		
		_headActor = spawnMeshActor(GetWorld(), _headMesh);
		_headActor->SetActorScale3D(FVector(2., 2., 2.));
		_headActor->GetStaticMeshComponent()->SetMaterial(1, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);

		_diskArmActor = spawnMeshActor(GetWorld(), _armMesh);
		_diskArmActor->SetActorScale3D(FVector(2., 2., 2.));
		_diskArmActor->GetStaticMeshComponent()->SetMaterial(1, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);

		_shieldArmActor = spawnMeshActor(GetWorld(), _armMesh);
		_shieldArmActor->SetActorScale3D(FVector(2., 2., 2.));
		_shieldArmActor->GetStaticMeshComponent()->SetMaterial(1, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);
	}
}

void APlayerActor::setFaction(PlayerFaction faction)
{
	ownFaction = faction;
}

PlayerFaction APlayerActor::getFaction()
{
	return ownFaction;
}

void APlayerActor::setHeadPosition(FVector pos)
{
	_headPosition = pos;
}

void APlayerActor::setHeadRotation(FQuat rot)
{
	_headRotation = rot;
}

void APlayerActor::setDiskArmPosition(FVector pos)
{
	_diskArmPosition = pos;
}

void APlayerActor::setDiskArmRotation(FQuat rot)
{
	_diskArmRotation = rot;
}

void APlayerActor::setShieldArmPosition(FVector pos)
{
	_shieldArmPosition = pos;
}

void APlayerActor::setShieldArmRotation(FQuat rot)
{
	_shieldArmRotation = rot;
}

FVector APlayerActor::getHeadPosition()
{
	return _headPosition;
}

FQuat APlayerActor::getHeadRotation()
{
	return _headRotation;
}

FVector APlayerActor::getDiskArmPosition()
{
	return _diskArmPosition;
}

FQuat APlayerActor::getDiskArmRotation()
{
	return _diskArmRotation;
}

FVector APlayerActor::getShieldArmPosition()
{
	return _shieldArmPosition;
}

FQuat APlayerActor::getShieldArmRotation()
{
	return _shieldArmRotation;
}

ADiscActor * APlayerActor::getDiscActor()
{
	return _discActor;
}

void APlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	updatePositions();

	if (_discActor->getState() == DISK_STATE_READY || _discActor->getState() == DISK_STATE_DRAWN) {
		FVector discArmUp   = _diskArmRotation.RotateVector(FVector::UpVector);
		FVector shieldArmUp = _shieldArmRotation.RotateVector(FVector::UpVector);
		_discActor->setDiscPosition(_diskArmPosition + discArmUp * 6.5);
		_discActor->setDiscRotation(_diskArmRotation);
	}
}

void APlayerActor::updatePositions()
{
	FVector headZAxisDirection = _headRotation.RotateVector(FVector::UpVector);
	_torsoPosition = _headPosition - headZAxisDirection * PLAYER_HEAD_SIZE - FVector(0.0, 0.0, PLAYER_TORSO_HEAD_OFFSET);

	if (_torsoActor) {
		FTransform torsoTransform = _torsoActor->GetTransform();
		FTransform headTransform = _headActor->GetTransform();
		FTransform diskArmTransform = _diskArmActor->GetTransform();
		FTransform shieldArmTransform = _shieldArmActor->GetTransform();

		torsoTransform.SetLocation(_torsoPosition);
		FVector headEulerAxisRotation = _headRotation.Euler();
		torsoTransform.SetRotation(FQuat(FVector(0.0, 0.0, 1.0), FMath::DegreesToRadians(headEulerAxisRotation.Z)));

		headTransform.SetLocation(_headPosition);
		headTransform.SetRotation(_headRotation);

		FVector diskArmForward = _diskArmRotation.RotateVector(FVector(-1.0, 0.0, 0.0));
		FVector shieldArmForward = _shieldArmRotation.RotateVector(FVector(-1.0, 0.0, 0.0));
		diskArmTransform.SetLocation(_diskArmPosition - diskArmForward * 7.5);
		diskArmTransform.SetRotation(_diskArmRotation);
		shieldArmTransform.SetLocation(_shieldArmPosition - shieldArmForward * 7.5);
		shieldArmTransform.SetRotation(_shieldArmRotation);

		_torsoActor->SetActorTransform(torsoTransform);
		_headActor->SetActorTransform(headTransform);
		_diskArmActor->SetActorTransform(diskArmTransform);
		_shieldArmActor->SetActorTransform(shieldArmTransform);
	}
}
