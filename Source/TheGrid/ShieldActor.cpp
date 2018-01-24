// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldActor.h"

UStaticMesh* AShieldActor::_shieldMesh = nullptr;
UMaterial*   AShieldActor::_blueMaterial = nullptr;
UMaterial*   AShieldActor::_orangeMaterial = nullptr;
UMaterial*   AShieldActor::_blueMaterialOpaque = nullptr;
UMaterial*   AShieldActor::_orangeMaterialOpaque = nullptr;

// Sets default values
AShieldActor::AShieldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_shieldPosition = FVector();
	_shieldRotation = FQuat();
	_radius = shieldMinimumRadius;

	if (!_shieldMesh) {
		_shieldMesh           = getContent<UStaticMesh>("StaticMesh'/Game/Geometry/Meshes/shield.shield'");
		_blueMaterial         = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_blue.robot_material_blue'");
		_orangeMaterial       = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_orange.robot_material_orange'");
		_blueMaterialOpaque   = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_blue_opaque.robot_material_blue_opaque'");
		_orangeMaterialOpaque = getContent<UMaterial>("Material'/Game/Geometry/Meshes/robot_material_orange_opaque.robot_material_orange_opaque'");
	}
}

void AShieldActor::Init(PlayerFaction faction)
{
	_shieldMeshActor = spawnMeshActor(GetWorld(), _shieldMesh);
	_shieldMeshActor->GetStaticMeshComponent()->SetMaterial(0, faction == PLAYER_FACTION_BLUE ? _blueMaterialOpaque : _orangeMaterialOpaque);
	_shieldMeshActor->GetStaticMeshComponent()->SetMaterial(1, faction == PLAYER_FACTION_BLUE ? _blueMaterial : _orangeMaterial);
}

// Called every frame
void AShieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_shieldMeshActor->GetTransform().IsValid()) {
		if (_enemyDisk) {
			float distance = FVector::Dist(_enemyDisk->getPosition(), getPosition());
			if (distance < shieldGrowStartDistance) {
				if (distance < shieldGrowEndDistance) {
					_radius = shieldMaximumRadius;
				}
				else {
					float distanceScale = (1 + cos((distance - shieldGrowEndDistance) / (shieldGrowStartDistance - shieldGrowEndDistance) * PI)) / 2;
					_radius = shieldMinimumRadius + (shieldMaximumRadius - shieldMinimumRadius) * distanceScale;
				}
			}
			else {
				_radius = shieldMinimumRadius;
			}
		}
		float radiusScale = _radius / shieldMaximumRadius;

		_shieldMeshActor->SetActorLocation(_shieldPosition);
		_shieldMeshActor->SetActorRotation(_shieldRotation);
		_shieldMeshActor->SetActorScale3D(FVector(radiusScale, radiusScale, 1.0));
	}
}

void AShieldActor::setEnemyDisk(ADiskActor* enemyDisk)
{
	_enemyDisk = enemyDisk;
}

void AShieldActor::setPosition(FVector newPosition)
{
	_shieldPosition = newPosition;
}

FVector AShieldActor::getPosition()
{
	return _shieldPosition;
}

void AShieldActor::setRotation(FQuat newRotation)
{
	_shieldRotation = newRotation;
}

FQuat AShieldActor::getRotation()
{
	return _shieldRotation;
}

float AShieldActor::getRadius()
{
	return _radius;
}

void AShieldActor::setCharges(int newCharges)
{
	_charges = newCharges;
}

int AShieldActor::getCharges()
{
	return _charges;
}

void AShieldActor::reduceCharges()
{
	if (_charges > 0) {
		_charges--;
	}
}

void AShieldActor::refillCharges()
{
	_charges = shieldMaximumCharges;
}

bool AShieldActor::hasCharges()
{
	return _charges > 0;
}

