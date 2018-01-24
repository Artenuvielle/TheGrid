// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelParameterSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class THEGRID_API ULevelParameterSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	FString ip;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	int32 port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Basic)
	bool startOwnServer;
	
	ULevelParameterSaveGame();
};
