// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeInformationHolder.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, Log, All);

UTreeInformationHolder::UTreeInformationHolder()
{

	StandardCellWidth = 0.5f;

	StandardDrawVector = FVector(0.0f, 0.0f, 50.0f);
	StandardDrawDirection = FVector(0.0f, 0.0f, 1.0f);
	StandardDrawLength = 50.0f;
	ZAxisHeightOfDrawnMesh = 100.0f;

	SplineMeshForwardAxis = ESplineMeshAxis::Z; //use upward axis to draw
	SplineMeshUpDir = FVector(1.0f, 0.0f, 0.0f); //not z, as we want to draw towards + z and splines can't do that for UE bugs or deeper reasons unbeknownst
	


	RootString = TEXT("0");



	DivideHorizMarker = TEXT("H");
	GrowLeftMarker = TEXT("L");
	GrowRightMarker = TEXT("R");
	GrowForwardMarker = TEXT("F");
	GrowBackMarker = TEXT("B");

	BaseBranchingAngleX = 15.0f;
	BaseBranchingAngleY = 15.0f;

	LeaveGapMultiplier = 1.01f;

	bShowLightRaycastHitMarkers = false;
	bShowWindRaycastHitMarkers = false;
	bShowGrowCollisionMarkers = false;

	LeavesCollisionCheckMultiplier = 1.0f;

	MaxCellsInTreeBase = -5;
	AllowedCellsExponent = 1.0f;
	MaxCellsHardUpperLimit = 10000.0f;

	//Mesh Setup stuff
	TrunksInstanceComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("StaticMeshInstance"));
	LeavesInstanceComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LeavesMeshInstance"));

	CurrentLeafMalusMultiplier = 1.0f;
	LeafMalusMultPerHit = 0.0f;
	LeafMalusExponent = 1.0f;
	LeafCurrentTickInfluence = 1.0f;
}

void UTreeInformationHolder::BeginPlay()
{
	Super::BeginPlay();

	SetupValues();
}

void UTreeInformationHolder::SetupValues()
{
	TArray<FString> GenomeKeys;
	GenomeMap.GenerateKeyArray(GenomeKeys);
	for (FString F : GenomeKeys)
	{
		if (!GenomeMap.Contains(F))
		{
			CellDefMap.Add(F); //add default constructed Definitions
		}
	}

	if (StaticMeshForVisuals->IsValidLowLevel())
	{
		TrunksInstanceComponent->SetStaticMesh(StaticMeshForVisuals);

		TrunksInstanceComponent->bMultiBodyOverlap = true;
		TrunksInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//	TrunksInstanceComponent->SetCastShadow(false);
	}

	if (StaticMeshForLeaves->IsValidLowLevel())
	{
		LeavesInstanceComponent->SetStaticMesh(StaticMeshForLeaves);

		LeavesInstanceComponent->bMultiBodyOverlap = true;
		LeavesInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		LeavesInstanceComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	//	LeavesInstanceComponent->SetCastShadow(false);
	}

	MaxCellsInTreeRuntimeValue = MaxCellsInTreeBase;
}

void UTreeInformationHolder::MutateOrganism()
{
	/* BlindlyRandomCellDivides();

	TArray<FString> GenomeKeys;

	CellDefMap.GenerateKeyArray(GenomeKeys);
	for (FString F : GenomeKeys)
	{
		MutateCellDefProperties(F);
	}
	}*/

	CellDefMap.Empty();

	InitAndRandomTreeProperties();

	InitAndRandomPredefinedCells();
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

void UTreeInformationHolder::InitAndRandomPredefinedCells()
{
	FCellTypeDefinition CurrentType; 
	FCellDivideDefinition CurrentTimeDiv; 

/////////////0
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties


	//divide properties
	CurrentTimeDiv.DivideThreshold = 1;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("Static"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("Start"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("0"), CurrentType);

/////////////Static
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = 1.0f;

	CellDefMap.Add(TEXT("Static"), CurrentType);

/////////////Start
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties


	//divide properties
	CurrentTimeDiv.DivideThreshold = 1;
	CurrentTimeDiv.bDividesHorizontally = true;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("StartUp"));
	
	const int32 NumberOfRoots = FMath::RandRange(2, 7);
	for (int32 i = 0; i < NumberOfRoots; i++)
	{
		CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_K"));
	}


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("Start"), CurrentType);

/////////////StartUp
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = 1.0f;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 1;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("StaticUp"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("K"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("StartUp"), CurrentType);

/////////////StaticUp
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = 1.0f;

	//divide properties
	//..
	CellDefMap.Add(TEXT("StaticUp"), CurrentType);

/////////////K
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = 1.0f;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 1;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("H"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("K"), CurrentType);


	const float CorrelationOfF = FMath::RandRange(-0.05f, 0.2f);
/////////////F
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = CorrelationOfF;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 3;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F2"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F2"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("F"), CurrentType);

/////////////F2
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = CorrelationOfF;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 8;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("F2"), CurrentType);


/////////////F3
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = CorrelationOfF;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 13;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("F3"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("F3"), CurrentType);

/////////////H
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.bLEAVE_IsLeave = true;
	if (FMath::RandRange(0, 10) > 3)
	{
		CurrentType.bIgnoreCollisionCheck = true;
	}
	const float HNormalizationDivisor = StandardCellWidth * StandardDrawLength;
	CurrentType.WidthMultiplierX = FMath::RandRange(10.0f, 100.0f) / HNormalizationDivisor; 

	//divide properties
	CurrentTimeDiv.DivideThreshold = 15;
	CurrentTimeDiv.bDividesHorizontally = true;

	const int32 NumberOfDarkLeafChildren = FMath::RandRange(2, 4);
	for (int32 i = 0; i < NumberOfDarkLeafChildren; i++)
	{
		CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("K"));
	}

	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);

	//light divide
	FCellDivideDefinition HLightDivide = FCellDivideDefinition();
	HLightDivide.bDividesHorizontally = true;
	HLightDivide.DivideThreshold = 4;
	const int32 NumberOfLightLeafChildren = FMath::RandRange(3, 8);
	for (int32 i = 0; i < NumberOfLightLeafChildren; i++)
	{
		HLightDivide.ChildrenStateStrings.Add(TEXT("K"));
	}

	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIGHT, HLightDivide);

	CellDefMap.Add(TEXT("H"), CurrentType);

/////////////Down
	const float DownCorrelation = FMath::RandRange(-0.8f, -0.3f);
	const float DownHorCircleAngle = FMath::RandRange(30.0f, 60.0f);

	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = DownCorrelation;
	CurrentType.HorChlCircleAngle = DownHorCircleAngle;
	CurrentType.bAttachToGround = true;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 6;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_2"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_2"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("Down"), CurrentType);

/////////////D_K
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = DownCorrelation;
	CurrentType.HorChlCircleAngle = DownHorCircleAngle;
	CurrentType.bAttachToGround = true;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 2;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("Down"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_H"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("D_K"), CurrentType);

/////////////D_H
	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = DownCorrelation;
	CurrentType.HorChlCircleAngle = DownHorCircleAngle;
	CurrentType.bAttachToGround = true;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 3;
	CurrentTimeDiv.bDividesHorizontally = true;
	const int32 NumberOfDownChilds = FMath::RandRange(2, 5);
	for (int32 i = 0; i < NumberOfDownChilds; i++)
	{
		CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_K"));
	}


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("D_H"), CurrentType);

/////////////D_2

	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = DownCorrelation;
	CurrentType.HorChlCircleAngle = DownHorCircleAngle;
	CurrentType.bAttachToGround = true;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 7;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_3"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("D_2"), CurrentType);

/////////////D_3

	CurrentType = FCellTypeDefinition();
	CurrentTimeDiv = FCellDivideDefinition();

	//Cell properties
	CurrentType.CorrelationWithStandardDrawDirection = DownCorrelation;
	CurrentType.HorChlCircleAngle = DownHorCircleAngle;
	CurrentType.bAttachToGround = true;

	//divide properties
	CurrentTimeDiv.DivideThreshold = 12;
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_3"));
	CurrentTimeDiv.ChildrenStateStrings.Add(TEXT("D_3"));


	CurrentType.DivideMap.Add(EStateTraitEnum::TRAIT_LIFETIME, CurrentTimeDiv);
	CellDefMap.Add(TEXT("D_3"), CurrentType);
}

void UTreeInformationHolder::InitAndRandomTreeProperties()
{
	StandardCellWidth = FMath::Clamp(0.2f + FMath::RandRange(-0.15f, 0.6f), 0.0f, 1.0f);
	StandardDrawLength = 15.0f + FMath::RandRange(-12.0f, 15.0f);

	//LeavesCollisionCheckMultiplier = FMath::RandRange(0.1f, 1.0f);
	LeavesCollisionCheckMultiplier = FMath::RandRange(0.1f, 0.5f);
	LeaveGapMultiplier = 1.15f;
	LeafMalusMultPerHit = 0.01f;

	//LeafMalusExponent = 0.5f;
	LeafMalusExponent = FMath::RandRange(0.2f, 0.5f);

	LeafCurrentTickInfluence = 3.0f;

	MaxCellsInTreeBase = 1500.0f + FMath::RandRange(-500.0f, 500.0f);
	AllowedCellsPerLightHitBonus = 60.0f + FMath::RandRange(-30.0f, 30.0f);
	AllowedCellsPerWaterHitBonus = 100.0f + FMath::RandRange(-50.0f, 50.0f);
	AllowedCellsExponent = FMath::RandRange(0.5f, 1.0f);
	MaxCellsHardUpperLimit = 10000.0f;

	SetupValues();
}

void UTreeInformationHolder::CopyValuesOf(UTreeInformationHolder * Other)
{
	CellDefMap.Empty();

	StandardCellWidth = Other->StandardCellWidth;
	StandardDrawLength = Other->StandardDrawLength;

	LeavesCollisionCheckMultiplier = Other->LeavesCollisionCheckMultiplier;
	LeaveGapMultiplier = Other->LeaveGapMultiplier;
	LeafMalusMultPerHit = Other->LeafMalusMultPerHit;
	LeafMalusExponent = Other->LeafMalusExponent;
	LeafCurrentTickInfluence = Other->LeafCurrentTickInfluence;

	MaxCellsInTreeBase = Other->MaxCellsInTreeBase;
	AllowedCellsPerLightHitBonus = Other->AllowedCellsPerLightHitBonus;
	AllowedCellsPerWaterHitBonus = Other->AllowedCellsPerWaterHitBonus;
	AllowedCellsExponent = Other->AllowedCellsExponent;
	MaxCellsHardUpperLimit = Other->MaxCellsHardUpperLimit;

	SetupValues();



	TArray<FString> OtherGenomeKeys;
	Other->CellDefMap.GenerateKeyArray(OtherGenomeKeys);
	for (FString F : OtherGenomeKeys)
	{
		FCellTypeDefinition* OtherDef =  Other->CellDefMap.Find(F);

		FCellTypeDefinition OurDef = FCellTypeDefinition();

		OurDef.CorrelationWithStandardDrawDirection = OtherDef->CorrelationWithStandardDrawDirection;
		OurDef.HorChlCircleAngle = OtherDef->HorChlCircleAngle;
		OurDef.HorChlCircleVarianceAngle = OtherDef->HorChlCircleVarianceAngle;
		OurDef.DrawLengthMultiplier = OtherDef->DrawLengthMultiplier;
		OurDef.WidthMultiplierX = OtherDef->WidthMultiplierX;
		OurDef.bIgnoreCollisionCheck = OtherDef->bIgnoreCollisionCheck;
		OurDef.bAttachToGround = OtherDef->bAttachToGround;
		OurDef.bLEAVE_IsLeave = OtherDef->bLEAVE_IsLeave;

		TArray<EStateTraitEnum> OtherDefDivKeys;
		OtherDef->DivideMap.GenerateKeyArray(OtherDefDivKeys);
		for (EStateTraitEnum E : OtherDefDivKeys)
		{
			FCellDivideDefinition NewDivide = FCellDivideDefinition();
			NewDivide.DivideThreshold = OtherDef->DivideMap.Find(E)->DivideThreshold;
			NewDivide.bDividesHorizontally = OtherDef->DivideMap.Find(E)->bDividesHorizontally;

			for (FString OtherChild : OtherDef->DivideMap.Find(E)->ChildrenStateStrings)
			{
				NewDivide.ChildrenStateStrings.Add(OtherChild);
			}

			OurDef.DivideMap.Add(E, NewDivide);
		}
		
		CellDefMap.Add(F, OurDef);

	}
}
