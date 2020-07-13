// Fill out your copyright notice in the Description page of Project Settings.

#include "WallCollisionActor.h"
#include "Kismet/GameplayStatics.h"

UMaterial*   AWallCollisionActor::_blueCollisionMaterial = nullptr;
UMaterial*   AWallCollisionActor::_orangeCollisionMaterial = nullptr;

AWallCollisionActor::AWallCollisionActor() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	_maxTime = 1.f;

	if (!_blueCollisionMaterial) {
		_blueCollisionMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/wall_collision_material_blue.wall_collision_material_blue'"), NULL, LOAD_None, NULL);
		_orangeCollisionMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/wall_collision_material_orange.wall_collision_material_orange'"), NULL, LOAD_None, NULL);
	}
}

void AWallCollisionActor::Init(PlayerFaction faction, FVector pos, float size, CollisionWall collisionWall)
{
	switch (collisionWall)
	{
	case COLLISION_WALL_RIGHT:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X, pos.Y - 1.f, pos.Z), -FVector::RightVector, collisionWall);
		break;
	case COLLISION_WALL_LEFT:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X, pos.Y + 1.f, pos.Z), FVector::RightVector, collisionWall);
		break;
	case COLLISION_WALL_UP:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X, pos.Y, pos.Z - 1.f), -FVector::UpVector, collisionWall);
		break;
	case COLLISION_WALL_DOWN:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X, pos.Y, pos.Z + 1.f), FVector::UpVector, collisionWall);
		break;
	case COLLISION_WALL_BACKWARD:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X + 1.f, pos.Y, pos.Z), FVector::ForwardVector, collisionWall);
		break;
	case COLLISION_WALL_FORWARD:
		createAnimationsAtCollisionPoint(faction, size, FVector(pos.X - 1.f, pos.Y, pos.Z), -FVector::ForwardVector, collisionWall);
		break;
	}
}

void AWallCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_time += DeltaTime;
	if (material != nullptr) {
		material->SetScalarParameterValue(FName("Now"), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
	}
	if (_time >= _maxTime) {
		Destroy();
	}
}

void AWallCollisionActor::createAnimationsAtCollisionPoint(PlayerFaction faction, float size , FVector position, FVector normal, CollisionWall collisionWall)
{
	material = UMaterialInstanceDynamic::Create(faction == PLAYER_FACTION_BLUE ? _blueCollisionMaterial : _orangeCollisionMaterial, this);
	material->SetScalarParameterValue(FName("Now"), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
	material->SetScalarParameterValue(FName("StartTime"), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
	float distYTop = abs(WALL_RIGHT_MAX - position.Y);
	float distYBot = abs(WALL_RIGHT_MIN - position.Y);
	float distZTop = abs(WALL_UP_MAX - position.Z);
	float distZBot = abs(WALL_UP_MIN - position.Z);
	float distXTop = abs(-WALL_BACKWARD_MAX - position.X);
	float distXBot = abs(-WALL_BACKWARD_MIN - position.X);
	createAnimationWithNormal(material, size, position, normal);
	switch (collisionWall)
	{
	case COLLISION_WALL_BACKWARD:
	case COLLISION_WALL_FORWARD:
		if (distYTop < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distYTop, WALL_RIGHT_MAX, position.Z);
			createAnimationWithNormal(material, size, newPosition, -FVector::RightVector);
		}
		if (distYBot < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distYBot, WALL_RIGHT_MIN, position.Z);
			createAnimationWithNormal(material, size, newPosition, FVector::RightVector);
		}
		if (distZTop < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distZTop, position.Y, WALL_UP_MAX);
			createAnimationWithNormal(material, size, newPosition, -FVector::UpVector);
		}
		if (distZBot < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distZBot, position.Y, WALL_UP_MIN);
			createAnimationWithNormal(material, size, newPosition, FVector::UpVector);
		}
		break;
	case COLLISION_WALL_RIGHT:
	case COLLISION_WALL_LEFT:
		if (distXTop < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MAX, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distXTop, position.Z);
			createAnimationWithNormal(material, size, newPosition, FVector::ForwardVector);
		}
		if (distXBot < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MIN, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distXBot, position.Z);
			createAnimationWithNormal(material, size, newPosition, -FVector::ForwardVector);
		}
		if (distZTop < size / 2) {
			FVector newPosition(position.X, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distZTop, WALL_UP_MAX);
			createAnimationWithNormal(material, size, newPosition, -FVector::UpVector);
		}
		if (distZBot < size / 2) {
			FVector newPosition(position.X, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distZBot, WALL_UP_MIN);
			createAnimationWithNormal(material, size, newPosition, FVector::UpVector);
		}
		break;
	case COLLISION_WALL_UP:
	case COLLISION_WALL_DOWN:
		if (distXTop < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MAX, position.Y, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distXTop);
			createAnimationWithNormal(material, size, newPosition, FVector::ForwardVector);
		}
		if (distXBot < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MIN, position.Y, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distXBot);
			createAnimationWithNormal(material, size, newPosition, -FVector::ForwardVector);
		}
		if (distYTop < size / 2) {
			FVector newPosition(position.X, WALL_RIGHT_MAX, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distYTop);
			createAnimationWithNormal(material, size, newPosition, -FVector::RightVector);
		}
		if (distYBot < size / 2) {
			FVector newPosition(position.X, WALL_RIGHT_MIN, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distYBot);
			createAnimationWithNormal(material, size, newPosition, FVector::RightVector);
		}
		break;
	}
}
