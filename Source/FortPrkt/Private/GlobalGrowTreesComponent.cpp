// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobalGrowTreesComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "OrganismCppBaseClass.h"
#include "DrawDebugHelpers.h "

DEFINE_LOG_CATEGORY_STATIC(LogCell_MasterGrower, Log, All);

DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ DivideCells"), STAT_FortPrktDivideCells, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ DrawCells"), STAT_FortPrktDrawCells, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ CalcTraits"), STAT_FortPrktCalcTraits, STATGROUP_FortPrkt);

DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves"), STAT_FortPrktRayTraceLeaves, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:InLoop"), STAT_FortPrktTrace_InLoop, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:LineTrace"), STAT_FortPrktTrace_LineTrace, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:HitInstMesh"), STAT_FortPrktTrace_HitInstMesh, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:InstIsLeave"), STAT_FortPrktTrace_TextLeaves, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:InstIsNoLeave"), STAT_FortPrktTrace_TextNoLeaves, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:GotComponent"), STAT_FortPrktTrace_GotComponent, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceLeaves&Wind:CantFindComponent"), STAT_FortPrktTrace_CantFindComponent, STATGROUP_FortPrkt);

DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ RayTraceWind"), STAT_FortPrktRayTraceWind, STATGROUP_FortPrkt);

DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ WaterOverlaps"), STAT_FortPrktWaterOverlaps, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ WaterOverlaps:ActualCheck"), STAT_FortPrktWaterOverlaps_ActualCheck, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ WaterOverlaps:HitInstMesh"), STAT_FortPrktWaterOverlaps_HitInstMesh, STATGROUP_FortPrkt);
DECLARE_CYCLE_STAT(TEXT("FortPrkt ~ WaterOverlaps:InstIsCell"), STAT_FortPrktWaterOverlaps_TextCell, STATGROUP_FortPrkt);


// Sets default values for this component's properties
UGlobalGrowTreesComponent::UGlobalGrowTreesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RayTraceAngleXMin = -15.0f;
	RayTraceAngleXStep = 3.0f;

	RayTraceAngleYMin = -15.0f;
	RayTraceAngleYStep = 3.0f;

	RayTraceIterationsX = 100;
	RayTraceIterationsY = 100;
	

	WindTraceIterationsX = 100;
	WindTraceIterationsY = 100;

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
			SCOPE_CYCLE_COUNTER(STAT_FortPrktDivideCells);

			TArray<UActorComponent*> TreeInfosArray = root->GetOwner()->GetComponentsByClass(UTreeInformationHolder::StaticClass());
			UTreeInformationHolder* TreeInfoOfRoot = TreeInfosArray.Num() > 0 ? Cast<UTreeInformationHolder>(TreeInfosArray[0]) : nullptr;


			const TArray<UActorComponent*> CellsOfRoot = root->GetOwner()->GetComponentsByClass(UTreeCellComponent::StaticClass());

			root->bCurrentlyStatic = false;
			if (TreeInfoOfRoot && TreeInfoOfRoot->MaxCellsInTreeBase > 0)
			{
				TreeInfoOfRoot->DebugCurrentNrOfCells = CellsOfRoot.Num(); //TODO outcomment for non-debugging
				// purposes

				const float MaxNumberOfCellsThisTick = FMath::Min(
					FMath::Pow(TreeInfoOfRoot->MaxCellsInTreeRuntimeValue, TreeInfoOfRoot->AllowedCellsExponent),
					TreeInfoOfRoot->MaxCellsHardUpperLimit
				);
				//MaxNumberOfCellsThisTick = FMath::Pow(TreeInfoOfRoot->MaxCellsInTreeRuntimeValue, TreeInfoOfRoot->AllowedCellsExponent);

				if (MaxNumberOfCellsThisTick < CellsOfRoot.Num())
				{
					root->bCurrentlyStatic = true;

					//ignore water overlap if static
					if (TreeInfoOfRoot->LeavesInstanceComponent->IsValidLowLevel())
					{
						TreeInfoOfRoot->LeavesInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

					}
					
					if (TreeInfoOfRoot->TrunksInstanceComponent->IsValidLowLevel())
					{
						TreeInfoOfRoot->TrunksInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);

					}
				}
			}

			if (root->bCurrentlyStatic == false)
			{
				if (TreeInfoOfRoot->LeavesInstanceComponent->IsValidLowLevel())
				{
					TreeInfoOfRoot->LeavesInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

				}

				if (TreeInfoOfRoot->TrunksInstanceComponent->IsValidLowLevel())
				{
					TreeInfoOfRoot->TrunksInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

				}

				for (UActorComponent* cell : CellsOfRoot)
				{
					(Cast<UTreeCellComponent>(cell))->divideCell();
				}
			}

			//After divide cells
			if (TreeInfoOfRoot != nullptr)
			{
				TreeInfoOfRoot->MaxCellsInTreeRuntimeValue = TreeInfoOfRoot->MaxCellsInTreeBase;

			}
		}
	}

	//draw all cells
	for (UDesignatedRootCellComponent* root : RootsOfCellsToGrow)
	{
		SCOPE_CYCLE_COUNTER(STAT_FortPrktDrawCells);

		if (root != nullptr && root->bCurrentlyStatic == false)
		{
			root->drawCellRecursively();

			
			TArray<UActorComponent*> TreeInfosArray = root->GetOwner()->GetComponentsByClass(UTreeInformationHolder::StaticClass());
			UTreeInformationHolder* TreeInfoOfRoot = TreeInfosArray.Num() > 0 ? Cast<UTreeInformationHolder>(TreeInfosArray[0]) : nullptr;
			if (TreeInfoOfRoot != nullptr)
			{
				TreeInfoOfRoot->CurrentLeafMalusMultiplier = 
					(TreeInfoOfRoot->CurrentLeafMalusMultiplier + TreeInfoOfRoot->LeafCurrentTickInfluence) /
					(TreeInfoOfRoot->LeafCurrentTickInfluence + 1.0f);
			}
		}
	}

	//calculate CellTraits for all cells

	{
		SCOPE_CYCLE_COUNTER(STAT_FortPrktRayTraceLeaves);

		RayTraceToLeaves();
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_FortPrktWaterOverlaps);

		HandleWaterOverlaps();
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_FortPrktRayTraceWind);

		RayTraceWind();
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_FortPrktCalcTraits);

		CalculateCellStateTraits();

	}
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
				UTreeCellComponent* child = Cast<UTreeCellComponent>(rootChild);
					
				if (child != nullptr)
				{
					child->CalcBurdenRecursively();
					child->CalcWindBurdenRecursively();
				}

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
			SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_InLoop);

			const FVector RelativeXAxis = FVector::CrossProduct(LineTraceDirectionVector, FVector(1.0f, 1.0f, 1.0f)).GetSafeNormal();// RotFromZ.RotateVector(FVector(1.0f, 0.0f, 0.0f));
			//const FVector RelativeYAxis = RotFromZ.RotateVector(FVector(0.0f, 1.0f, 0.0f));

			const FVector CurrentTraceEnd = LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis).RotateAngleAxis(y * RayTraceAngleYStep, LineTraceDirectionVector.GetSafeNormal());
			//const FVector CurrentTraceEnd = FVector::ZeroVector;
			FHitResult Rslt = FHitResult();
			FCollisionQueryParams Params = FCollisionQueryParams();
			//Params.TraceTag = TraceTag; //comment in to show raytrace debug lines
			Params.bTraceAsyncScene = false;
			const FVector LineTraceOriginWorld = LightTraceOriginPosition + GetOwner()->GetActorLocation();
			const FVector LineTraceEndWorld = CurrentTraceEnd + GetOwner()->GetActorLocation();

			//TODO this is actually raytracing ECC_Static rofl; if change consider UTreeInformationHolder::BeginPlay()
			
			{
				SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_LineTrace);
				World->LineTraceSingleByChannel(Rslt, LineTraceOriginWorld, LineTraceEndWorld, ECollisionChannel::ECC_WorldStatic, Params, FCollisionResponseParams());

			}
			if (Rslt.bBlockingHit == true)
			{
				//UE_LOG(LogCell_MasterGrower, Log, TEXT("Light LineTrace Hit Something: %s, Number: %u"),
				//	*GetNameSafe(Rslt.Component.Get()),
				//	Rslt.Item);

				UInstancedStaticMeshComponent* HitComponent = Cast<UInstancedStaticMeshComponent>(Rslt.Component.Get());

				if (HitComponent != nullptr)
				{
					SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_HitInstMesh);
					//TODO implement system so we can hit another cell than trees? Identify with name?
					if (GetNameSafe(HitComponent) == TEXT("LeavesMeshInstance"))
					{
						SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_TextLeaves);
						
						AOrganismCppBaseClass* HitComponentOwnerAsOrganism = Cast<AOrganismCppBaseClass>(HitComponent->GetOwner());

						if (HitComponentOwnerAsOrganism)
						{
							UTreeInformationHolder* HitTreeInfos = Cast<UTreeInformationHolder>(HitComponentOwnerAsOrganism->InfoOfThis);

							if (HitTreeInfos)
							{
								UTreeCellComponent* HitTreeCellComponent = HitTreeInfos->LeavesArrayThisIteration[Rslt.Item].Get();

								if (HitTreeCellComponent)
								{
									SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_GotComponent);
									HitTreeCellComponent->LightThisIteration++;

									if (HitTreeInfos->MaxCellsInTreeBase > 0)
									{
										HitTreeInfos->MaxCellsInTreeRuntimeValue += HitTreeInfos->AllowedCellsPerLightHitBonus;
										HitTreeInfos->CurrentLeafMalusMultiplier = HitTreeInfos->CurrentLeafMalusMultiplier * ( 1 - HitTreeInfos->LeafMalusMultPerHit);
									}


									if (HitTreeInfos->bShowLightRaycastHitMarkers)
									{
										DrawDebugPoint(GetWorld(), Rslt.ImpactPoint, 10, FColor(220, 100, 30), true, 3.0f);
									}
								}
								else
								{
									SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_CantFindComponent);
									//TODO actually fix this instead of declaring it just Verbose
									UE_LOG(LogCell_MasterGrower, Verbose, TEXT("Lighttrace: Can't find Leave TreeCellComponent according to number %u for hit actor: %s, LeavesArray.Num = %u, hit Name: %s"),
										Rslt.Item,
										*GetNameSafe(Rslt.Actor.Get()),
										HitTreeInfos->LeavesArrayThisIteration.Num(),
										*GetNameSafe(HitTreeInfos->LeavesArrayThisIteration[Rslt.Item].Get()));
								}
							}
						}

					}

					else
					{
						if (GetNameSafe(HitComponent) != TEXT("StaticMeshInstance"))
						{
							UE_LOG(LogCell_MasterGrower, Log, TEXT("Lighttrace hit InstancedStaticMesh != 'LeavesMeshInstance' and 'StaticMeshInstance', is this supposed to happen? Actor: %s, Component: %s"),
								*GetNameSafe(HitComponent->GetOwner()),
								*GetNameSafe(HitComponent));
						}
					}
				}
					
				//TODO this is bullshit
				//Cast<UTreeCellComponent>(Rslt.Actor.Get())->LightThisIteration++;
			}

/*	
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
				//LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis) + GetOwner()->GetActorLocation(),
				LineTraceEndWorld,
				FColor(255, 0, 0),
				true,
				0.5f,
				0,
				3.0f
			); 

*/
		}
	}

}

void UGlobalGrowTreesComponent::RayTraceWind()
{
	if (WindTraceOriginBottomLeft == FVector::ZeroVector || WindTraceOriginXDir == FVector::ZeroVector ||
		WindTraceOriginYDir == FVector::ZeroVector ||
		WindTraceEndOffset == FVector::ZeroVector)
	{
		//in case a StateString was reached that isn't in the Map, do safety behavior 
		UE_LOG(LogCell_MasterGrower, Log, TEXT("Cannot raycast wind, as origin square is missdefined or WindTraceEndOffset zero"));

		return;
	}

	UWorld* World = GetOwner()->GetWorld();
	const FName TraceTag("SightTrace");

	World->DebugDrawTraceTag = TraceTag;

	for (int32 x = 0; x < WindTraceIterationsX; x++)
	{
		for (int32 y = 0; y < WindTraceIterationsY; y++)
		{
			SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_InLoop);

			const FVector CurrentTraceOriginWorld = WindTraceOriginBottomLeft +
				((WindTraceOriginXDir * x) / WindTraceIterationsX) +
				((WindTraceOriginYDir * y) / WindTraceIterationsY) +
				GetOwner()->GetActorLocation(); //to get to global
			
			const FVector CurrentTraceEndWorld = CurrentTraceOriginWorld + WindTraceEndOffset;

			FHitResult Rslt = FHitResult();
			FCollisionQueryParams Params = FCollisionQueryParams();
			//Params.TraceTag = TraceTag; //comment in to show raytrace debug lines
			Params.bTraceAsyncScene = false;

			//TODO this is actually raytracing ECC_Static rofl; if change consider UTreeInformationHolder::BeginPlay()

			{
				SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_LineTrace);
				World->LineTraceSingleByChannel(Rslt, CurrentTraceOriginWorld, CurrentTraceEndWorld, ECollisionChannel::ECC_WorldStatic, Params, FCollisionResponseParams());

			}
			if(Rslt.bBlockingHit == true)
			{
				UInstancedStaticMeshComponent* HitComponent = Cast<UInstancedStaticMeshComponent>(Rslt.Component.Get());

				if (HitComponent != nullptr)
				{
					SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_HitInstMesh);
					//TODO implement system so we can hit another cell than trees? Identify with name?
					if (GetNameSafe(HitComponent) == TEXT("LeavesMeshInstance") || GetNameSafe(HitComponent) == TEXT("StaticMeshInstance"))
					{
						bool bHitIsLeave = true;
						if (GetNameSafe(HitComponent) == TEXT("StaticMeshInstance"))
						{
							bHitIsLeave = false;
						}

						SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_TextLeaves);

						AOrganismCppBaseClass* HitComponentOwnerAsOrganism = Cast<AOrganismCppBaseClass>(HitComponent->GetOwner());

						if (HitComponentOwnerAsOrganism)
						{
							UTreeInformationHolder* HitTreeInfos = Cast<UTreeInformationHolder>(HitComponentOwnerAsOrganism->InfoOfThis);

							if (HitTreeInfos)
							{
								UTreeCellComponent* HitTreeCellComponent;
		
								if (bHitIsLeave)
								{
									HitTreeCellComponent = HitTreeInfos->LeavesArrayThisIteration[Rslt.Item].Get();
								}
								else
								{
									HitTreeCellComponent = HitTreeInfos->TrunkArrayThisIteration[Rslt.Item].Get();
								}
								

								if (HitTreeCellComponent)
								{
									SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_GotComponent);
									HitTreeCellComponent->LocalWindForceBurden++;

									if (HitTreeInfos->bShowWindRaycastHitMarkers)
									{
										DrawDebugPoint(GetWorld(), Rslt.ImpactPoint, 10, FColor(122, 0, 179), true, 3.0f);
									}
								}
								else
								{
									SCOPE_CYCLE_COUNTER(STAT_FortPrktTrace_CantFindComponent);
									//TODO actually fix this instead of declaring it just Verbose
									UE_LOG(LogCell_MasterGrower, Verbose, TEXT("Windtrace: Can't find %s TreeCellComponent according to number %u for hit actor: %s, According Array.Num = %u, hit Name: %s"),
										bHitIsLeave ? TEXT("Leaf") : TEXT("Trunk"),
										Rslt.Item,
										*GetNameSafe(Rslt.Actor.Get()),
										bHitIsLeave ? HitTreeInfos->LeavesArrayThisIteration.Num() : HitTreeInfos->TrunkArrayThisIteration.Num(),
										bHitIsLeave ? *GetNameSafe(HitTreeInfos->LeavesArrayThisIteration[Rslt.Item].Get()) : *GetNameSafe(HitTreeInfos->TrunkArrayThisIteration[Rslt.Item].Get())
									);
								}
							}
						}

					}

					else
					{
						UE_LOG(LogCell_MasterGrower, Log, TEXT("Windtrace hit InstancedStaticMesh != 'LeavesMeshInstance' and 'StaticMeshInstance', is this supposed to happen? Actor: %s, Component: %s"),
							*GetNameSafe(HitComponent->GetOwner()),
							*GetNameSafe(HitComponent));
					}
				}
			}


			/* DrawDebugLine(
				World,
				CurrentTraceOriginWorld,
				//LineTraceDirectionVector.RotateAngleAxis(x * RayTraceAngleXStep, RelativeXAxis) + GetOwner()->GetActorLocation(),
				CurrentTraceEndWorld,
				FColor(0, 0, 255),
				true,
				0.5f,
				0,
				3.0f
			);
			*/
		}
	}
}

void UGlobalGrowTreesComponent::HandleWaterOverlaps()
{
	for (UWaterCheckForOverlapsComponent* Comp: WaterComponentsToCheck)
	{
		TArray<FOverlapResult> Results = TArray<FOverlapResult>();

		{
			SCOPE_CYCLE_COUNTER(STAT_FortPrktWaterOverlaps_ActualCheck);
			Comp->CheckForOverlapingMeshInstances(Results);
		}

		for (FOverlapResult r : Results)
		{
			UE_LOG(LogCell_MasterGrower, VeryVerbose, TEXT("Water: Check for %s, hit Component: %s and index: %u"),
				*GetNameSafe(Comp),
				*GetNameSafe(r.Component.Get()),
				r.ItemIndex
			);

			UInstancedStaticMeshComponent* HitComponent = Cast<UInstancedStaticMeshComponent>(r.Component.Get());

			if (HitComponent != nullptr)
			{
				SCOPE_CYCLE_COUNTER(STAT_FortPrktWaterOverlaps_HitInstMesh);

				//TODO implement system so we can hit another cell than trees? Identify with name?
				if (GetNameSafe(HitComponent) == TEXT("LeavesMeshInstance") || GetNameSafe(HitComponent) == TEXT("StaticMeshInstance"))
				{
					SCOPE_CYCLE_COUNTER(STAT_FortPrktWaterOverlaps_TextCell);

					AOrganismCppBaseClass* HitComponentOwnerAsOrganism = Cast<AOrganismCppBaseClass>(HitComponent->GetOwner());

					if (HitComponentOwnerAsOrganism)
					{
						UTreeInformationHolder* HitTreeInfos = Cast<UTreeInformationHolder>(HitComponentOwnerAsOrganism->InfoOfThis);

						if (HitTreeInfos)
						{
							if (HitTreeInfos->MaxCellsInTreeBase > 0)
							{
								HitTreeInfos->MaxCellsInTreeRuntimeValue += HitTreeInfos->AllowedCellsPerWaterHitBonus;
							}
						}
					}
				}

				else
				{
					UE_LOG(LogCell_MasterGrower, Log, TEXT("Water collision hit InstancedStaticMesh != 'LeavesMeshInstance' and 'StaticMeshInstance', is this supposed to happen? Actor: %s, Component: %s"),
						*GetNameSafe(HitComponent->GetOwner()),
						*GetNameSafe(HitComponent));
				}

			}
		}
	}
}
