// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeCellComponent.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, VeryVerbose, All);

UTreeCellComponent::UTreeCellComponent()
{
	NewCellClass = UTreeCellComponent::StaticClass();
}

void UTreeCellComponent::BeginPlay()
{
	TArray<UActorComponent*> CompArray = GetOwner()->GetComponentsByClass(UTreeInformationHolder::StaticClass());
	if (CompArray.Num() > 0)
	{
		OwnersTreeInfos = Cast<UTreeInformationHolder>(CompArray[0]);

		if (StateString.Equals(TEXT("@@@")) && !OwnersTreeInfos->RootString.IsEmpty())
		{
			StateString = OwnersTreeInfos->RootString;
		}
		
	}

	else
	{
		OwnersTreeInfos = nullptr;
	}

	Super::BeginPlay();
}

TArray<FString> UTreeCellComponent::GetDivideSubstrings(FString InString)
{
	TArray<FString> OutArray = TArray<FString>();

	if (StateString.Contains(TEXT("H")))
	{
		OutArray.Add(TEXT("A"));
		OutArray.Add(TEXT("B"));
	}

	else
	{
		OutArray.Add(StateString);
		OutArray.Add(StateString);
	}

	return OutArray;
}

void UTreeCellComponent::drawCellRecursively()
{
	//throw away old visuals
	if (DrawnComponent)
	{
		DrawnComponent->DestroyComponent();
	}

	DrawOriginPosition = Cast<UTreeCellComponent>(AttachedCellParent) != NULL ? (Cast<UTreeCellComponent>(AttachedCellParent))->DrawEndPosition : FVector(0.0f, 0.0f, -1.0f);
	FVector DrawDirection = FVector::ZeroVector;
	if (StateString.Contains(TEXT("A")))
	{
		DrawDirection = OwnersTreeInfos ? (OwnersTreeInfos->StandardDrawVector).RotateAngleAxis(15, FVector(0.0f, 1.0f, 0.0f)) : FVector(-42.0f, -42.0f, -42.0f);
	}

	else
	{
		DrawDirection = OwnersTreeInfos ? (OwnersTreeInfos->StandardDrawVector).RotateAngleAxis(-15, FVector(0.0f, 1.0f, 0.0f)) : FVector(-42.0f, -42.0f, -42.0f);
	}
	DrawEndPosition = DrawOriginPosition + DrawDirection;


	DrawnComponent = NewObject<USplineMeshComponent>(GetOwner());
	DrawnComponent->SetMobility(EComponentMobility::Movable);
//	DrawnComponent->SetupAttachment(this);
	DrawnComponent->SetupAttachment(GetOwner()->GetRootComponent());
	DrawnComponent->RegisterComponent();
	

	//DrawnComponent->SetupAttachment(this);
	UE_LOG(LogCell, VeryVerbose, TEXT("DrawTreeCell, Name:, %s, StateString: %s OriginPos: %f, %f, %f , EndPos: %f, %f, %f"),
		*GetNameSafe(this),
		*StateString,
		DrawOriginPosition.X, DrawOriginPosition.Y, DrawOriginPosition.Z,
		DrawEndPosition.X, DrawEndPosition.Y, DrawEndPosition.Z);

	if (OwnersTreeInfos && OwnersTreeInfos->StaticMeshForVisuals->IsValidLowLevel())
	{
		DrawnComponent->SetStaticMesh(OwnersTreeInfos->StaticMeshForVisuals);
		DrawnComponent->SetStartPosition(DrawOriginPosition);
		DrawnComponent->SetEndPosition(DrawEndPosition);
		DrawnComponent->SetStartTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		DrawnComponent->SetEndTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		
		float SingleScale = OwnersTreeInfos->StandardCellWidth;
		
		DrawnComponent->SetStartScale(FVector2D(SingleScale, SingleScale));
		DrawnComponent->SetEndScale(FVector2D(SingleScale, SingleScale));

		DrawnComponent->SetForwardAxis(OwnersTreeInfos->SplineMeshForwardAxis);
		DrawnComponent->SetSplineUpDir(OwnersTreeInfos->SplineMeshUpDir);
	}


	Super::drawCellRecursively();
}

bool UTreeCellComponent::shouldDivideHorizontally()
{
	if (OwnersTreeInfos && !OwnersTreeInfos->DivideHorizMarker.IsEmpty()
		&& StateString.Contains(OwnersTreeInfos->DivideHorizMarker))
	{
		return true;
	}

	else
	{
		return false;
	}
	
}

void UTreeCellComponent::InitWithString(FString InString)
{
	Super::InitWithString(InString);
}
