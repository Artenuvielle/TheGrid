// Fill out your copyright notice in the Description page of Project Settings.

#include "LifeCounterActor.h"
#include "WallCollisionActor.h"

ALifeCounterActor::ALifeCounterActor()
{
	PrimaryActorTick.bCanEverTick = true;
	_lifeCount = 0;
	_rects = TArray<ULifeDisplayRectComponent*>();
}

void ALifeCounterActor::Init(PlayerFaction faction)
{
	_faction = faction;
	int tileNum = lifeCounterMaxLife * (lifeCounterMaxLife + 1) / 2;
	float tileSize = 240.f / tileNum;
	int usedTiles = 0;
	for (int i = 0; i < lifeCounterMaxLife; i++) {
		float factionBackwardPos;
		float currentTiles = lifeCounterMaxLife - i;
		float currentTileSize = currentTiles * tileSize;
		float offset = usedTiles * tileSize + currentTileSize / 2;
		if (faction == userFaction) {
			factionBackwardPos = -135 - offset;
		}
		else {
			factionBackwardPos = -660 + offset;
		}
		ULifeDisplayRectComponent* lifeRectComponent = NewObject<ULifeDisplayRectComponent>(this);
		lifeRectComponent->RegisterComponent();
		lifeRectComponent->Init(faction, 50, currentTileSize - 8, FVector::UpVector * 2 + FVector::ForwardVector * -factionBackwardPos);
		_rects.Add(lifeRectComponent);
		usedTiles += currentTiles;
	}

	setLifeCount(0);
}

void ALifeCounterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (int i = 0; i < _rects.Num(); i++) {
		if (_rects[i]->getFilled() != (i < _lifeCount)) {
			_rects[i]->setFilled(i < _lifeCount);
		}
	}
}

void ALifeCounterActor::setLifeCount(int newCount)
{
	int nextCount = FMath::Max(0, FMath::Min(lifeCounterMaxLife, newCount));
	if (nextCount < _lifeCount) {
		AWallCollisionActor* collisionActor = GetWorld()->SpawnActor<AWallCollisionActor>(AWallCollisionActor::StaticClass());
		collisionActor->Init(_faction,
			FVector::ForwardVector * (_faction == userFaction ? -WALL_BACKWARD_MAX : -WALL_BACKWARD_MIN) + WALL_UP_MID * FVector::UpVector,
			scoreAnimationSize, (_faction == userFaction ? COLLISION_WALL_BACKWARD : COLLISION_WALL_FORWARD));
	}
	_lifeCount = nextCount;
}

int ALifeCounterActor::getLifeCount()
{
	return _lifeCount;
}
