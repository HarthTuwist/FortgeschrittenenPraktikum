// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeInformationHolder.h"

UTreeInformationHolder::UTreeInformationHolder()
{

	StandardCellWidth = 0.5f;

	StandardDrawVector = FVector(0.0f, 0.0f, 50.0f);
	StandardDrawDirection = FVector(0.0f, 0.0f, 1.0f);
	StandardDrawLength = 50.0f;
	ZAxisHeightOfDrawnMesh = 100.0f;

	SplineMeshForwardAxis = ESplineMeshAxis::Z; //use upward axis to draw
	SplineMeshUpDir = FVector(1.0f, 0.0f, 0.0f); //not z, as we want to draw towards + z and splines can't do that for UE bugs or deeper reasons unbeknownst
	
	RootString = TEXT("A");
	DivideHorizMarker = TEXT("H");
	GrowLeftMarker = TEXT("L");
	GrowRightMarker = TEXT("R");
	GrowForwardMarker = TEXT("F");
	GrowBackMarker = TEXT("B");

	BaseBranchingAngleX = 15.0f;
	BaseBranchingAngleY = 15.0f;

	LeaveGapMultiplier = 1.01f;

	bShowLightRaycastHitMarkers = false;
	bShowGrowCollisionMarkers = false;

	LeavesCollisionCheckMultiplier = 1.0f;

	//Mesh Setup stuff
	TrunksInstanceComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StaticMeshInstance"));
	LeavesInstanceComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LeavesMeshInstance"));

}

void UTreeInformationHolder::BeginPlay()
{
	Super::BeginPlay();

	TArray<FString> GenomeKeys;
	GenomeMap.GenerateKeyArray(GenomeKeys);
	for (FString F : GenomeKeys)
	{
		if(!GenomeMap.Contains(F))
		{
			CellDefMap.Add(F); //add default constructed Definitions
		}
	}

	if (StaticMeshForVisuals->IsValidLowLevel())
	{
		TrunksInstanceComponent->SetStaticMesh(StaticMeshForVisuals);

		TrunksInstanceComponent->bMultiBodyOverlap = true;
		TrunksInstanceComponent->SetCastShadow(false);
	}

	if (StaticMeshForLeaves->IsValidLowLevel())
	{
		LeavesInstanceComponent->SetStaticMesh(StaticMeshForLeaves);

		LeavesInstanceComponent->bMultiBodyOverlap = true;
		LeavesInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		LeavesInstanceComponent->SetCastShadow(false);
	}
}

void UTreeInformationHolder::MutateOrganism()
{
	BlindlyRandomCellDivides();

	TArray<FString> GenomeKeys;

	CellDefMap.GenerateKeyArray(GenomeKeys);
	for (FString F : GenomeKeys)
	{
		MutateCellDefProperties(F);
	}
}

void UTreeInformationHolder::MutateCellDefProperties(FString CellId)
{
	FCellTypeDefinition*  MutatedEntry = CellDefMap.Find(CellId);
	if (MutatedEntry == nullptr)
	{
		return;
	}

	MutatedEntry->CorrelationWithStandardDrawDirection = FMath::Clamp(MutatedEntry->CorrelationWithStandardDrawDirection + FMath::RandRange(-2.0f, 1.0f), 0.0f, 1.0f);
	MutatedEntry->HorChlCircleAngle = FMath::Clamp(MutatedEntry->HorChlCircleAngle + FMath::RandRange(0.0f, 90.0f), 0.0f, 90.0f);
	MutatedEntry->HorChlCircleVarianceAngle = FMath::Clamp(MutatedEntry->HorChlCircleVarianceAngle + FMath::RandRange(0.0f, MutatedEntry->HorChlCircleVarianceAngle), 0.0f, MutatedEntry->HorChlCircleVarianceAngle);
	//MutatedEntry->DrawLengthMultiplier == FMath::Clamp(MutatedEntry->HorChlCircleAngle + FMath::RandRange(-2.5f, 2.5f), 0.0f, MutatedEntry->HorChlCircleVarianceAngle);
}

void UTreeInformationHolder::MutateCellDivideProperties(FString CellId)
{

}

void UTreeInformationHolder::BlindlyRandomCellDivides()
{
	CellDefMap.Empty();
	TArray<FString> NewKeys = TArray<FString>();

	NewKeys.Add(TEXT("K"));
	NewKeys.Add(TEXT("A"));
	NewKeys.Add(TEXT("B"));
	NewKeys.Add(TEXT("C"));
	NewKeys.Add(TEXT("D"));

	for (FString F : NewKeys)
	{
		FCellTypeDefinition TypeDef = FCellTypeDefinition();
		FCellDivideDefinition DivDef = FCellDivideDefinition();
		
		while (FMath::RandRange(0, 100) > 20)
		{
			DivDef.ChildrenStateStrings.Add(NewKeys[FMath::RandRange(0, NewKeys.Num() - 1)]);
		}

		DivDef.DivideThreshold = FMath::RandRange(1, 3);

		//DivDef.DivideThreshold = FMath::RandRange(1, 7);

		if (FMath::RandRange(0,6) >= 5)
		{
			DivDef.bDividesHorizontally = true;
		}

	

		TypeDef.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, DivDef);

		CellDefMap.Add(F, TypeDef);
	}
}
