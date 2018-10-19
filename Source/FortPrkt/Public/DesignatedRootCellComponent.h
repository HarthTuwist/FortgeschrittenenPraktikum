// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CellComponent.h"
#include "DesignatedRootCellComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FORTPRKT_API UDesignatedRootCellComponent : public UCellComponent
{
	GENERATED_BODY()

	void BeginPlay() override;

	virtual void drawCellRecursively() override;

	virtual void divideCell();
};
