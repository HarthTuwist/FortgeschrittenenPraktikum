// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalGrowTreesComponent.h"


// Sets default values for this component's properties
UGlobalGrowTreesComponent::UGlobalGrowTreesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGlobalGrowTreesComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetComponentTickEnabled(false);
	// ...
	
}


// Called every frame
void UGlobalGrowTreesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGlobalGrowTreesComponent::IterateOverRoots()
{
	//divide over all cells
	for (UDesignatedRootCellComponent* root : RootsOfCellsToGrow)
	{
		if (root != nullptr)
		{
			const TArray<UActorComponent*> CellsOfRoot = root->GetOwner()->GetComponentsByClass(UTreeCellComponent::StaticClass());

			for (UActorComponent* cell : CellsOfRoot)
			{
				(Cast<UTreeCellComponent>(cell))->divideCell();
			}
		}
	}

	//draw all cells
	for (UDesignatedRootCellComponent* root : RootsOfCellsToGrow)
	{
		if (root != nullptr)
		{
			root->drawCellRecursively();
		}
	}

	//calculate CellTraits for all cells
	
	CalculateCellStateTraits();


}

void UGlobalGrowTreesComponent::CalculateCellStateTraits()
{
	//needs to be another arrays, as dividing cells makes more cells

	for (UDesignatedRootCellComponent* root : RootsOfCellsToGrow)
	{
		if (root != nullptr)
		{
			const TArray<UActorComponent*> CellsOfRoot = root->GetOwner()->GetComponentsByClass(UTreeCellComponent::StaticClass());

			for (UActorComponent* cell : CellsOfRoot)
			{
				UTreeCellComponent* CellAsCell = Cast<UTreeCellComponent>(cell);

				//add one to time
				CellAsCell->IterationsSinceCreation++;
			}
	}
}
}

