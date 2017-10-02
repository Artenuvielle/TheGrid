// Fill out your copyright notice in the Description page of Project Settings.

#include "WallCollisionActor.h"

UStaticMesh* AWallCollisionActor::_planeMesh = nullptr;
UMaterial*   AWallCollisionActor::_blueCollisionMaterial = nullptr;
UMaterial*   AWallCollisionActor::_orangeCollisionMaterial = nullptr;

AWallCollisionActor::AWallCollisionActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_maxTime = 1.f;

	if (!_planeMesh) {
		_planeMesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"), NULL, LOAD_None, NULL);
		_blueCollisionMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/wall_collision_material_blue.wall_collision_material_blue'"), NULL, LOAD_None, NULL);
		_orangeCollisionMaterial = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/wall_collision_material_orange.wall_collision_material_orange'"), NULL, LOAD_None, NULL);
	}
}

void AWallCollisionActor::Init(PlayerFaction faction, FVector pos, float size, CollisionWall collisionWall)
{
	UE_LOG(LogTemp, Log, TEXT("creating collision at %s, size %f, wall %d"), *pos.ToString(), size, (int)collisionWall);
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
	if (_time >= _maxTime) {
		Destroy();
	}
}

void AWallCollisionActor::createAnimationsAtCollisionPoint(PlayerFaction faction, float size , FVector position, FVector normal, CollisionWall collisionWall)
{
	float distYTop = abs(WALL_RIGHT_MAX - position.Y);
	float distYBot = abs(WALL_RIGHT_MIN - position.Y);
	float distZTop = abs(WALL_UP_MAX - position.Z);
	float distZBot = abs(WALL_UP_MIN - position.Z);
	float distXTop = abs(-WALL_BACKWARD_MAX - position.X);
	float distXBot = abs(-WALL_BACKWARD_MIN - position.X);
	createAnimationWithNormal(faction, size, position, normal);
	switch (collisionWall)
	{
	case COLLISION_WALL_BACKWARD:
	case COLLISION_WALL_FORWARD:
		if (distYTop < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distYTop, WALL_RIGHT_MAX, position.Z);
			createAnimationWithNormal(faction, size, newPosition, -FVector::RightVector);
		}
		if (distYBot < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distYBot, WALL_RIGHT_MIN, position.Z);
			createAnimationWithNormal(faction, size, newPosition, FVector::RightVector);
		}
		if (distZTop < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distZTop, position.Y, WALL_UP_MAX);
			createAnimationWithNormal(faction, size, newPosition, -FVector::UpVector);
		}
		if (distZBot < size / 2) {
			FVector newPosition(position.X + (position.X > -WALL_BACKWARD_MID ? 1 : -1) * distZBot, position.Y, WALL_UP_MIN);
			createAnimationWithNormal(faction, size, newPosition, FVector::UpVector);
		}
		break;
	case COLLISION_WALL_RIGHT:
	case COLLISION_WALL_LEFT:
		if (distXTop < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MAX, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distXTop, position.Z);
			createAnimationWithNormal(faction, size, newPosition, FVector::ForwardVector);
		}
		if (distXBot < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MIN, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distXBot, position.Z);
			createAnimationWithNormal(faction, size, newPosition, -FVector::ForwardVector);
		}
		if (distZTop < size / 2) {
			FVector newPosition(position.X, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distZTop, WALL_UP_MAX);
			createAnimationWithNormal(faction, size, newPosition, -FVector::UpVector);
		}
		if (distZBot < size / 2) {
			FVector newPosition(position.X, position.Y + (position.Y > WALL_RIGHT_MID ? 1 : -1) * distZBot, WALL_UP_MIN);
			createAnimationWithNormal(faction, size, newPosition, FVector::UpVector);
		}
		break;
	case COLLISION_WALL_UP:
	case COLLISION_WALL_DOWN:
		if (distXTop < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MAX, position.Y, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distXTop);
			createAnimationWithNormal(faction, size, newPosition, FVector::ForwardVector);
		}
		if (distXBot < size / 2) {
			FVector newPosition(-WALL_BACKWARD_MIN, position.Y, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distXBot);
			createAnimationWithNormal(faction, size, newPosition, -FVector::ForwardVector);
		}
		if (distYTop < size / 2) {
			FVector newPosition(position.X, WALL_RIGHT_MAX, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distYTop);
			createAnimationWithNormal(faction, size, newPosition, -FVector::RightVector);
		}
		if (distYBot < size / 2) {
			FVector newPosition(position.X, WALL_RIGHT_MIN, position.Z + (position.Z > WALL_UP_MID ? 1 : -1) * distYBot);
			createAnimationWithNormal(faction, size, newPosition, FVector::RightVector);
		}
		break;
	}
}

void AWallCollisionActor::createAnimationWithNormal(PlayerFaction faction, float size, FVector position, FVector normal)
{
	UStaticMeshComponent* collisionPlane = NewObject<UStaticMeshComponent>(this);
	collisionPlane->RegisterComponent();
	collisionPlane->SetStaticMesh(_planeMesh);
	collisionPlane->SetMaterial(0, faction == userFaction ? _blueCollisionMaterial : _orangeCollisionMaterial);
	collisionPlane->SetWorldLocation(position + normal);
	collisionPlane->SetWorldRotation(FQuat::FindBetweenNormals(FVector::UpVector, normal));
	collisionPlane->SetWorldScale3D(FVector::RightVector * size * 0.01 + FVector::ForwardVector * size * 0.01);
}
