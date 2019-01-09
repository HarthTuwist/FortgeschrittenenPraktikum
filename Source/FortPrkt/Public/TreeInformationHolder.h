// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganismInformationHolder.h"
#include "Components/SplineMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Containers/Array.h"
#include "TreeInformationHolder.generated.h"

/**
 * 
 */

//forward declarations
class UTreeCellComponent;


UENUM()
enum class EStateTraitEnum : uint8
{
	TRAIT_LIFETIME = 0 UMETA(DisplayName = "TimeSinceCreation"),
	TRAIT_WEIGHTBURDEN = 1 UMETA(DisplayName = "WeightBurden"),
	TRAIT_LIGHT = 2 UMETA(DisplayName = "Light"),
	TRAIT_FREEAGAIN = 3 UMETA(DisplayName = "NoMoreGroundAttachedTip"),

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

	//Horizontal Children Circle Angle; The Angle between 0 and 180 that describes the circle on the sphere that is the outer limit of children grow vectors
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorChlCircleAngle;

	//Horizontal Children Circle Angle Variance; The Angle between 0 and HorChlCircleAngle that describes the variance allowed towards straigth grow  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float HorChlCircleVarianceAngle;


	//multiplies StandardDrawLength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float DrawLengthMultiplier;

	//multiplies BOTH directions X and Y for now
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float WidthMultiplierX;

	//Bonus in cell size per cell burden
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		float WeightBurdenBonus;

	//ignores collision check on cells with no children
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		bool bIgnoreCollisionCheck;

	//if true this celltype will attach to ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		bool bAttachToGround;


	/////
	//////////////LEAVE Only Properties
	/////
	///// 
	///// 

	//basic bool that marks if cell is leave
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		bool bLEAVE_IsLeave;



	//this map defines the multiple ways this cell could divide
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CellTypeDefinition")
		TMap<EStateTraitEnum, FCellDivideDefinition> DivideMap;

	FCellTypeDefinition()
	{
		CorrelationWithStandardDrawDirection = 0.0f;
		AdditionalRotationAngleX = 0.0f;
		AdditionalRotationAngleY = 0.0f;
		DrawLengthMultiplier = 1.0f;

		HorChlCircleAngle = 15.0f;
		HorChlCircleVarianceAngle = 0.0f;
		WeightBurdenBonus = 0.0f;

		WidthMultiplierX = 1.0f;


		bAttachToGround = false;
		//leave stuff

		bLEAVE_IsLeave = false;
		bIgnoreCollisionCheck = false;
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

	virtual void SetupValues();

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

	//standard static mesh for visuals
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UStaticMesh* StaticMeshForVisuals;

	//specifically used for leaves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UStaticMesh* StaticMeshForLeaves;

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

	//StandardDrawUpVector is multiplied by this to determine total distance from leave to parent (should be bigger than 1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float LeaveGapMultiplier;

	//TODO Change back to int32?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		TMap<FString, FCellTypeDefinition> CellDefMap;


	///////////////////Mutation functions

	virtual void MutateOrganism() override;

	UFUNCTION(BlueprintCallable, Category = "Genome")
		virtual void MutateCellDefProperties(FString CellId);

	UFUNCTION(BlueprintCallable, Category = "Genome")
		virtual void MutateCellDivideProperties(FString CellId);

	UFUNCTION(BlueprintCallable, Category = "Genome")
		virtual void BlindlyRandomCellDivides();

	UFUNCTION(BlueprintCallable, Category = "Genome")
		virtual void InitAndRandomPredefinedCells();

	UFUNCTION(BlueprintCallable, Category = "Genome")
		virtual void InitAndRandomTreeProperties();


	//Component that stores static mesh instances for the trunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UInstancedStaticMeshComponent* TrunksInstanceComponent;

	//Component that stores static mesh instances for the leaves
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UInstancedStaticMeshComponent* LeavesInstanceComponent;

	//this maps the static mesh instance numbers to the trunk cells
	UPROPERTY()
		TArray<TWeakObjectPtr<UTreeCellComponent >> TrunkArrayThisIteration;

	//this maps the static mesh instance numbers to the trunk cells
	UPROPERTY()
		TArray<TWeakObjectPtr<UTreeCellComponent >> LeavesArrayThisIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		bool bShowLightRaycastHitMarkers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		bool bShowWindRaycastHitMarkers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		bool bShowGrowCollisionMarkers;

	//when Leaves check for collision, width will be multiplied by this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float LeavesCollisionCheckMultiplier;

	//runtime calculated value: makes the tree static if value is bigger than number of cells and MaxcellsInTreeBase > 0
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Genome")
		int32 MaxCellsInTreeRuntimeValue; //TODO does this belong in TreeInformationHolder ??

	//If > 0 : activates checking for max number of cells, with this as base addend
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		int32 MaxCellsInTreeBase;

	//how many additional cells the tree can have when 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		int32 AllowedCellsPerLightHitBonus;

	//how many additional cells the tree can have when 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		int32 AllowedCellsPerWaterHitBonus;

	//MaxCellsInTreeRuntimeValue is powered by this before comparison
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float AllowedCellsExponent;

	//Stop growing under all circumstances if this is smaller than the number of cells
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float MaxCellsHardUpperLimit;

	//Debug value to see how many cells in tree
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Genome")
		int32 DebugCurrentNrOfCells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Genome")
		bool bGrowBlockedByCollision;

	UFUNCTION(BlueprintCallable, Category = "Genome")
		void CopyValuesOf(UTreeInformationHolder* Other);

	//1 at start; gets lower when game gets zero; Not in Blueprint as 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float CurrentLeafMalusMultiplier;

	//(1 - this) is multiplied with CurrentCellSizeMultiplier for every Light Hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float LeafMalusMultPerHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float LeafMalusExponent;

	//used in UTreeCellComponent::ShouldWindBurdenKillThis(); higher value means more kills
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float WindBurdenKillMultiplier;

	//used in UTreeCellComponent::ShouldWindBurdenKillThis(); higher value means more kills
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float WindBurdenKillExponent;

	//the higher this is, the more CurrentLeafMalusMultiplier is reset towards 1 after each draw tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		float LeafCurrentTickInfluence;
};
