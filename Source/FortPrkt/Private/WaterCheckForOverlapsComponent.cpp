// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterCheckForOverlapsComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell_Water, Log, All);

// Sets default values for this component's properties
UWaterCheckForOverlapsComponent::UWaterCheckForOverlapsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWaterCheckForOverlapsComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComponent != nullptr)
	{
		CollisionComponent->bMultiBodyOverlap = true;
	}
	
	
}


// Called every frame
void UWaterCheckForOverlapsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWaterCheckForOverlapsComponent::CheckForOverlapingMeshInstances(TArray<FOverlapResult>& OutArray)
{
	if (CollisionComponent == nullptr || !CollisionComponent->IsValidLowLevel())
	{
		UE_LOG(LogCell_Water, Error, TEXT("%s: no Collision Component"), *GetNameSafe(this));

		return;
	}

	//FCollisionObjectQueryParams CollisionObjectParams = FCollisionObjectQueryParams();
	//CollisionObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	//FCollisionQueryParams QueryParams = FCollisionQueryParams();
	//QueryParams.bFindInitialOverlaps = true;
	//const FName TraceTag("TrunkCollisionTag");
	//QueryParams.TraceTag = TraceTag;
	

	GetWorld()->ComponentOverlapMulti(OutArray, CollisionComponent, CollisionComponent->GetComponentLocation(), CollisionComponent->GetComponentRotation());

	//GetWorld()->ComponentOverlapMultiByChannel(OutArray, CollisionComponent, CollisionComponent->GetComponentLocation(), CollisionComponent->GetComponentRotation(),ECollisionChannel::ECC_GameTraceChannel2);
	//CollisionComponent->ComponentOverlapMulti(Results, GetWorld())

}

