// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneralBlueprintLibrarz.generated.h"

/**
 * 
 */
UCLASS()
class FORTPRKT_API UGeneralBlueprintLibrarz : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Print")
		static void PrintStringToScreen(FString String);
	
	
};
