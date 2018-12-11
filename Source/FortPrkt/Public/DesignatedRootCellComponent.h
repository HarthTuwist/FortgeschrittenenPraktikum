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


	virtual void divideCell();
public:
	virtual void drawCellRecursively() override;

////////////////////////////////////Whole-Tree Properties, TODO  put these here for now (should change?)

	//when this is above limit, in UGlobalGrowTreesComponent::IterateOverRoots() the cells will not divide
	//UPROPERTY(BlueprintReadOnly, Category = "WholeOrganism")
	//	int32 NumberOfCellsInOrganism;
	//	don't actually do this this way
	

	//if true, no longer divide or redraw
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WholeOrganism")
		bool bCurrentlyStatic;
};
