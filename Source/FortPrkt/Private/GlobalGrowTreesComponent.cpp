// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalGrowTreesComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h "

DEFINE_LOG_CATEGORY_STATIC(LogCell_MasterGrower, Log, All);

// Sets default values for this component's properties
UGlobalGrowTreesComponent::UGlobalGrowTreesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RayTraceAngleXMin = -15.0f;
	RayTraceAngleYStep = 3.0f;

	RayTraceAngleYMin = -15.0f;
	RayTraceAngleYStep = 3.0f;

	RayTraceIterationsX = 100;
	RayTraceIterationsY = 100;
	
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


	RayTraceToLeaves();

}

void UGlobalGrowTreesComponent::CalculateCellStateTraits()
{
	//needs to be another arrays, as dividing cells makes more cells

	for (UDesignatedRootCellComponent* root : RootsOfCellsToGrow)
	{
		if (root != nullptr)
		{
			//iterate over cells
			const TArray<UActorComponent*> CellsOfRoot = root->GetOwner()->GetComponentsByClass(UTreeCellComponent::StaticClass());

			for (UActorComponent* cell : CellsOfRoot)
			{
				UTreeCellComponent* CellAsCell = Cast<UTreeCellComponent>(cell);

				//add one to time
				CellAsCell->IterationsSinceCreation++;
			}

			//calculate burden
			for (UCellComponent* rootChild : root->AttachedCellChildren)
			{
				Cast<UTreeCellComponent>(rootChild)->CalcBurdenRecursively();
			}
		}


	}
}

void UGlobalGrowTreesComponent::RayTraceToLeaves()
{
	if (LightTraceOriginPosition == FVector::ZeroVector || LightTraceUnrotatedEndPosition == FVector::ZeroVector || 
		LightTraceOriginPosition == LightTraceUnrotatedEndPosition)
	{			
		//in case a StateString was reached that isn't in the Map, do safety behavior 
		UE_LOG(LogCell_MasterGrower, Log, TEXT("Cannot raycast light, as Origin or End is Zero, or both are equal"));

		return;
	}

	UWorld* World = GetOwner()->GetWorld();
	const FName TraceTag("SightTrace");

	World->DebugDrawTraceTag = TraceTag;

	const FVector LineTraceDirectionVector = LightTraceUnrotatedEndPosition - LightTraceOriginPosition;

	const FRotator RotFromZ = UKismetMathLibrary::MakeRotFromZ(LineTraceDirectionVector);

	for (int32 x = 0; x < RayTraceIterationsX; x++)
	{
		for (int32 y = 0; y < RayTraceIterationsY; y++)
		{
			const FVector RelativeXAxis = RotFromZ.RotateVector(FVector(1.0f, 0.0f, 0.0f));
			const FVector RelativeYAxis = RotFromZ.RotateVector(FVector(0.0f, 1.0f, 0.0f));

			const FVector CurrentTraceEnd = LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis).RotateAngleAxis(y * RayTraceAngleYStep, LineTraceDirectionVector);
			//const FVector CurrentTraceEnd = FVector::ZeroVector;
			FHitResult Rslt = FHitResult();
			FCollisionQueryParams Params = FCollisionQueryParams();
		//	Params.TraceTag = TraceTag;
			Params.bTraceAsyncScene = false;
			const FVector LineTraceOriginWorld = LightTraceOriginPosition + GetOwner()->GetActorLocation();
			const FVector LineTraceEndWorld = CurrentTraceEnd + GetOwner()->GetActorLocation();
			World->LineTraceSingleByChannel(Rslt, LineTraceOriginWorld, LineTraceEndWorld, ECollisionChannel::ECC_Pawn, Params, FCollisionResponseParams());

			UE_LOG(LogCell_MasterGrower, Log, TEXT("Light Linetracing, LineTraceDir: %f, %f, %f, TraceCenter: %f, %f, %f, End: %f, %f, %f , hit: %s, XAxis: %f, %f, %f"),
				//LineTraceOriginWorld.X, LineTraceOriginWorld.Y, LineTraceOriginWorld.Z,
				LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis).X, LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis).Y, LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis).Z,
				LineTraceDirectionVector.X, LineTraceDirectionVector.Y, LineTraceDirectionVector.Z,
				LineTraceEndWorld.X, LineTraceEndWorld.Y, LineTraceEndWorld.Z,
				*GetNameSafe(Rslt.Actor.Get()),
				RelativeXAxis.X, RelativeXAxis.Y, RelativeXAxis.Z);

			
			DrawDebugLine(
				World,
				LineTraceOriginWorld,
				LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis) + GetOwner()->GetActorLocation(),
				//LineTraceEndWorld,
				FColor(255, 0, 0),
				true,
				30.0f,
				0,
				3.0f
			); 


		}
	}
}

