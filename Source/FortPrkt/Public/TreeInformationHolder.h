// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OrganismInformationHolder.h"
#include "Components/SplineMeshComponent.h"
#include "TreeInformationHolder.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FORTPRKT_API UTreeInformationHolder : public UOrganismInformationHolder
{
	GENERATED_BODY()
	
		// Sets default values for this component's properties
		UTreeInformationHolder();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Genome")
		float StandardCellWidth;

	//this is UP grow direction * length of a cell in this direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FVector StandardDrawVector;

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
};
