// Fill out your copyright notice in the Description page of Project Settings.

#include "GameEndingAnimationActor.h"
#include "Kismet/GameplayStatics.h"

UMaterial* AGameEndingAnimationActor::_winAnimationBlue = nullptr;
UMaterial* AGameEndingAnimationActor::_winAnimationOrange = nullptr;
UMaterial* AGameEndingAnimationActor::_loseAnimationBlue = nullptr;
UMaterial* AGameEndingAnimationActor::_loseAnimationOrange = nullptr;

AGameEndingAnimationActor::AGameEndingAnimationActor()
{
	if (!_winAnimationBlue) {
		_winAnimationBlue = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/win_animation_blue.win_animation_blue'"), NULL, LOAD_None, NULL);
		_winAnimationOrange = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/win_animation_orange.win_animation_orange'"), NULL, LOAD_None, NULL);
		_loseAnimationBlue = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/lose_animation_blue.lose_animation_blue'"), NULL, LOAD_None, NULL);
		_loseAnimationOrange = LoadObject<UMaterial>(NULL, TEXT("Material'/Game/Geometry/Meshes/lose_animation_orange.lose_animation_orange'"), NULL, LOAD_None, NULL);
	}
}

void AGameEndingAnimationActor::Init(PlayerFaction faction, bool hasWon)
{
	UMaterialInstanceDynamic* materialInstance;
	if (hasWon) {
		materialInstance = UMaterialInstanceDynamic::Create(faction == PLAYER_FACTION_BLUE ? _winAnimationBlue : _winAnimationOrange, this);
	}
	else {
		materialInstance = UMaterialInstanceDynamic::Create(faction == PLAYER_FACTION_BLUE ? _loseAnimationBlue : _loseAnimationOrange, this);
	}
	materialInstance->SetScalarParameterValue(FName("StartTime"), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
	createAnimationWithNormal(
		materialInstance, 270,
		FVector::ForwardVector * -WALL_BACKWARD_MID + FVector::UpVector * WALL_UP_MID + FVector::RightVector * WALL_RIGHT_MID,
		FQuat(FVector::RightVector, -90.f / 180.f * PI) * FQuat(FVector::UpVector, 90.f / 180.f * PI));
}
