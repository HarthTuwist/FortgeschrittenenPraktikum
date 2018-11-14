// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganismInformationHolder.h"
#include "Components/SplineMeshComponent.h"
#include "TreeInformationHolder.generated.h"

/**
 * 
 */

UENUM()
enum class EStateTraitEnum : uint8
{
	TRAIT_LIFETIME = 0 UMETA(DisplayName = "TimeSinceCreation"),

	FIRST_INVALID UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCellDivideDefinition
{
	GENERATED_USTRUCT_BODY()

public:

	//Trigger the encoded divide if the according CellState property is bigger than this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellDivideDefinition")
		int32 DivideThreshold;

	//false if division should occur vertically
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellDivideDefinition")
		bool bDividesHorizontally;

	//false if division should occur vertically
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellDivideDefinition")
		TArray<FString> ChildrenStateStrings;

	FCellDivideDefinition()
	{
		DivideThreshold = 100000000; //very high
		bDividesHorizontally = false;
		ChildrenStateStrings = TArray<FString>();
	}
};


USTRUCT(BlueprintType)
struct FCellTypeDefinition
{
	GENERATED_USTRUCT_BODY()

public:

	//1 if growing up, 0 if ignoring gravity, -1 if growing down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float CorrelationWithStandardDrawDirection;

	//always rotate around this besides everything else
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float AdditionalRotationAngleX;

	//always rotate around this besides everything else
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float AdditionalRotationAngleY;

	//the grow directions of AtachedCellChildrens are distributed equally in the provided ranges (all 0 if growing straight is wanted)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorizChildrenGrowFreedomX_Mean;
	//the grow directions of AtachedCellChildrens are distributed equally in the provided ranges (all 0 if growing straight is wanted)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorizChildrenGrowFreedomX_Variance;

	//Horizontal Children Circle Angle; The Angle between 0 and 180 that describes the circle on the sphere that is the outer limit of children grow vectors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorChlCircleAngle;

	//Horizontal Children Circle Angle Variance; The Angle between 0 and HorChlCircleAngle that describes the variance allowed towards straigth grow  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorChlCircleVarianceAngle;

	//the grow directions of AtachedCellChildrens are distributed equally in the provided ranges (all 0 if growing straight is wanted)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorizChildrenGrowFreedomY_Mean;
	//the grow directions of AtachedCellChildrens are distributed equally in the provided ranges (all 0 if growing straight is wanted)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorizChildrenGrowFreedomY_Variance;


	//multiplies StandardDrawLength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float DrawLengthMultiplier;

	//this map divides how any map is going to 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypDefinition")
		TMap<EStateTraitEnum, FCellDivideDefinition> DivideMap;

	//the cell divides at the next stage into this number of cells
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
	//	TArray<int32> ChildrenNumbers;

	FCellTypeDefinition()
	{
		CorrelationWithStandardDrawDirection = 0.0f;
		AdditionalRotationAngleX = 0.0f;
		AdditionalRotationAngleY = 0.0f;
		DrawLengthMultiplier = 1.0f;

		HorizChildrenGrowFreedomX_Variance =  45.0f;
		HorizChildrenGrowFreedomX_Mean = -22.5f;
		HorizChildrenGrowFreedomY_Variance = 0.0f; //45.0f;
		HorizChildrenGrowFreedomY_Mean = 0.0f; //-22.0f;

		HorChlCircleAngle = 15.0f;
		HorChlCircleVarianceAngle = 0.0f;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FORTPRKT_API UTreeInformationHolder : public UOrganismInformationHolder
{
	GENERATED_BODY()
	
		// Sets default values for this component's properties
		UTreeInformationHolder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Genome")
		float StandardCellWidth;

	//DEPRECATED this is UP grow direction * length of a cell in this direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FVector StandardDrawVector;

	//length of a cell in grow direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float StandardDrawLength;

	//Up Grow Direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FVector StandardDrawDirection;

	//height of the drawn mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float ZAxisHeightOfDrawnMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		TEnumAsByte<ESplineMeshAxis::Type> SplineMeshForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FVector SplineMeshUpDir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UStaticMesh* StaticMeshForVisuals;

	//if this string is in the state of the cell we divide horizontally
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FString DivideHorizMarker;

	//this is used to determine the angle between two branches 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float BaseBranchingAngleX;

	//this is used to determine the angle between two branches 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float BaseBranchingAngleY;

	//if this is in state string, this cell should grow left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FString GrowLeftMarker;

	//if this is in state string, this cell should grow right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FString GrowRightMarker;

	//if this is in state string, this cell should grow forward
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FString GrowForwardMarker;

	//if this is in state string, this cell should grow back
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FString GrowBackMarker;

	//TODO Change back to int32?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		TMap<FString, FCellTypeDefinition> CellDefMap;
};
