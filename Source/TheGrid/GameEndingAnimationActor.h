// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "common.h"
#include "AnimationActor.h"
#include "GameEndingAnimationActor.generated.h"

/**
 * 
 */
UCLASS()
class THEGRID_API AGameEndingAnimationActor : public AAnimationActor
{
	GENERATED_BODY()

public:
	AGameEndingAnimationActor();
	void Init(PlayerFaction, bool);

private:
	static UMaterial* _winAnimationBlue;
	static UMaterial* _winAnimationOrange;
	static UMaterial* _loseAnimationBlue;
	static UMaterial* _loseAnimationOrange;
};
