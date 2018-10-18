// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeCellComponent.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, VeryVerbose, All);

UTreeCellComponent::UTreeCellComponent()
{
	StandardDrawVector = FVector(0.0f, 150.0f, 0.0f);
	NewCellClass = UTreeCellComponent::StaticClass();
}

TArray<FString> UTreeCellComponent::GetDivideSubstrings(FString InString)
{
	TArray<FString> OutArray = TArray<FString>();

	OutArray.Add(TEXT("A"));
	OutArray.Add(TEXT("A"));

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
	DrawEndPosition = DrawOriginPosition + StandardDrawVector;


	DrawnComponent = NewObject<USplineMeshComponent>(GetOwner());
	DrawnComponent->SetMobility(EComponentMobility::Movable);
//	DrawnComponent->SetupAttachment(this);
	DrawnComponent->SetupAttachment(GetOwner()->GetRootComponent());
	DrawnComponent->RegisterComponent();
	

	//DrawnComponent->SetupAttachment(this);

	if (StaticMeshForVisuals->IsValidLowLevel())
	{
		UE_LOG(LogCell, VeryVerbose, TEXT("DrawCell, Name:, %s, OriginPos: %f, %f, %f , EndPos: %f, %f, %f"),
			*GetNameSafe(this),
			DrawOriginPosition.X, DrawOriginPosition.Y, DrawOriginPosition.Z,
			DrawEndPosition.X, DrawEndPosition.Y, DrawEndPosition.Z);

		DrawnComponent->SetStaticMesh(StaticMeshForVisuals);
		DrawnComponent->SetStartPosition(DrawOriginPosition);
		DrawnComponent->SetEndPosition(DrawEndPosition);
		DrawnComponent->SetStartTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		DrawnComponent->SetEndTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		DrawnComponent->SetStartScale(FVector2D(2.0f, 2.0f));

		DrawnComponent->SetForwardAxis(ESplineMeshAxis::Z);

		//DrawnComponent->SetStartTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal()*150);
		//DrawnComponent->SetEndTangent((DrawOriginPosition - DrawEndPosition).GetSafeNormal()*150);
	}


	Super::drawCellRecursively();
}
