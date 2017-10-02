// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "common.h"
#include "LifeDisplayRectComponent.h"
#include "LifeCounterActor.generated.h"

UCLASS()
class THEGRID_API ALifeCounterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALifeCounterActor();
	void Init(PlayerFaction type);
	virtual void Tick(float DeltaTime) override;
	void setLifeCount(int);
	int getLifeCount();
private:
	int _lifeCount;
	TArray<ULifeDisplayRectComponent*> _rects;
};
