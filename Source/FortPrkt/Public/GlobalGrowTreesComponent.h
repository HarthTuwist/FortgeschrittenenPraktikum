// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DesignatedRootCellComponent.h"
#include "TreeCellComponent.h"
#include "WaterCheckForOverlapsComponent.h"
#include "GlobalGrowTreesComponent.generated.h"


//For UE4 Profiler ~ Stat Group
DECLARE_STATS_GROUP(TEXT("FortPrkt"), STATGROUP_FortPrkt, STATCAT_Advanced);


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		TArray<UDesignatedRootCellComponent*> RootsOfCellsToGrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		TArray<UWaterCheckForOverlapsComponent*> WaterComponentsToCheck;

	UFUNCTION(BlueprintCallable, Category = "CellGrow")
		void IterateOverRoots();

//light ray trace stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		FVector LightTraceOriginPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		FVector LightTraceUnrotatedEndPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		float RayTraceAngleXMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		float RayTraceAngleXStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		float RayTraceAngleYMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		float RayTraceAngleYStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		int32 RayTraceIterationsX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellGrow")
		int32 RayTraceIterationsY;

	UFUNCTION(BlueprintCallable)
	void RayTraceToLeaves();
protected:
	//calculates all CellStateTraits
	void CalculateCellStateTraits();

	//handles water overlapped static mesh instances
	void HandleWaterOverlaps();
	
};
