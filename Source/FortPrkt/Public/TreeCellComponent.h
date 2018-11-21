// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "CellComponent.h"
#include "TreeInformationHolder.h"
#include "TreeCellComponent.generated.h"

/**
 * 
 */


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FORTPRKT_API UTreeCellComponent : public UCellComponent
{
	GENERATED_BODY()

	// Sets default values for this component's properties
	UTreeCellComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void CopyPropertiesFromParent(UCellComponent* Parent);

public:
		virtual void divideCell();

	//Draw Stuff
	TArray<FString> GetDivideSubstrings(FString InString) override;

	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		UStaticMeshComponent* DrawnComponent;

	virtual void drawCellRecursively() override;

	//where the draw, i.e. the SplineMesh, is going to start
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		FVector DrawOriginPosition;

	//where the draw, i.e the SplineMesh, is going to end
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		FVector DrawEndPosition;

	//how many degrees the growth of this cell is rotated around the X axis
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		float RotationAngleX;

	//how many degrees the growth of this cell is rotated around the Y axis
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		float RotationAngleY;

	//this is used by the static mesh instance to draw this cell
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		FTransform DrawTransform;

	virtual bool shouldDivideHorizontally() override;

	virtual void InitWithString(FString InString) override;

	//Calculate Raw Horizontal Divide Attached Children Vectors into RawHorizChilDrawVecs. TireWidth is variance in grow direction; freedom on the circle is always full 360°.
	//Return false if no calculation was made
	virtual bool CalcHorizDivChilVecs();

	//Get stored horizontal children. If not calculated yet, calculate it before returning
	UFUNCTION(Blueprintpure, Category = "Cellproperties")
		void GetRawHorizChilDrawVecs(TArray<FVector>& Vectors);

	//Static Mesh instance of this cell
	UPROPERTY(BlueprintReadOnly, Category = "Cellproperties")
		int32 InstancedMeshIdThisIteration;


	////////////////////
	//CellStateTraits, eg. properties that have an enum and can influence division
	////////////////////
public:
	//use this to set CellStateTraits.
	UFUNCTION(BlueprintCallable, Category = "CellStateTraits")
		void SetCellStateTrait(EStateTraitEnum TraitEnum, int32 NewValue);

	//use this to get CellStateTraits
	UFUNCTION(BlueprintCallable, Category = "CellStateTraits")
		int32 GetCellStateTrait(EStateTraitEnum TraitEnum);

//public to make this visible in C++ for calculation in Iteration
	UPROPERTY()
		int32 IterationsSinceCreation;

	//calculates the burden by this cell by calling this function of the children
	int32 CalcBurdenRecursively();

	UPROPERTY()
		int32 WeightBurden;

	UPROPERTY()
		int32 LightThisIteration;

private:
	//Empty if not calculated yet. Might contain more entries then there are children
	UPROPERTY()
		TArray<FVector> RawHorizChilDrawVecs;

	UPROPERTY()
		UTreeInformationHolder* OwnersTreeInfos;
};
