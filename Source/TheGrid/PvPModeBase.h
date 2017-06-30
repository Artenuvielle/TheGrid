// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PvPModeBase.generated.h"

/**
 * 
 */
UCLASS()
class THEGRID_API APvPModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void InitGame();
	
};
