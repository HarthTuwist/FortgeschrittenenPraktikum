// Fill out your copyright notice in the Description page of Project Settings.

#include "DesignatedRootCellComponent.h"
#include "TreeCellComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, Log, All);

void UDesignatedRootCellComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AllOtherCells = GetOwner()->GetComponentsByClass(UCellComponent::StaticClass());

	UE_LOG(LogCell, VeryVerbose, TEXT("RootBeginPlay, Name:, %s, Owner: %s, LengthOf AlLotherCells(self included): %o"),
		*GetNameSafe(this),
		*GetNameSafe(GetOwner()),
		AllOtherCells.Num());

	AllOtherCells.Remove(this);
	for (UActorComponent* c : AllOtherCells)
	{
		AddNewCellChild(Cast<UCellComponent>(c));
	}
}

void UDesignatedRootCellComponent::drawCellRecursively()
{
	UE_LOG(LogCell, VeryVerbose, TEXT("RootDrawing, Name:, %s, NumChildren: %i"),
		*GetNameSafe(this),
		AttachedCellChildren.Num());


	//if we belong to a tree clear all instances before drawing children
	TArray<UActorComponent*> CompArray = GetOwner()->GetComponentsByClass(UTreeInformationHolder::StaticClass());
	if (CompArray.Num() > 0)
	{
		UTreeInformationHolder* OwnersTreeInfos = Cast<UTreeInformationHolder>(CompArray[0]);

		OwnersTreeInfos->LeavesInstanceComponent->ClearInstances();
		OwnersTreeInfos->TrunksInstanceComponent->ClearInstances();
		OwnersTreeInfos->TrunkArrayThisIteration.Empty();
		OwnersTreeInfos->LeavesArrayThisIteration.Empty();

	}

	for (UCellComponent* c : AttachedCellChildren)
	{
		c->drawCellRecursively();
	}
}

void UDesignatedRootCellComponent::divideCell()
{
	//do nothing here; don't divide the designated root
}
