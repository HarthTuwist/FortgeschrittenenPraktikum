// Fill out your copyright notice in the Description page of Project Settings.

#include "DesignatedRootCellComponent.h"

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

	for (UCellComponent* c : AttachedCellChildren)
	{



		c->drawCellRecursively();
	}
}

void UDesignatedRootCellComponent::divideCell()
{
	//do nothing her; don't divide the designated root
}
