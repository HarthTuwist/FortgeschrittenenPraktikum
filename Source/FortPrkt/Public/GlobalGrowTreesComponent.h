// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DesignatedRootCellComponent.h"
#include "TreeCellComponent.h"
#include "GlobalGrowTreesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTPRKT_API UGlobalGrowTreesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGlobalGrowTreesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cell")
		TArray<UDesignatedRootCellComponent*> RootsOfCellsToGrow;

	UFUNCTION(BlueprintCallable, Category = "Cell")
		void IterateOverRoots();

protected:
	//calculates all CellStateTraits
	void CalculateCellStateTraits();
};
