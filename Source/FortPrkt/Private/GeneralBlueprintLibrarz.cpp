// Fill out your copyright notice in the Description page of Project Settings.

#include "GeneralBlueprintLibrarz.h"

void UGeneralBlueprintLibrarz::PrintStringToScreen(FString String)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, String);
}
