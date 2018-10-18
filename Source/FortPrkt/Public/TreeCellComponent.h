// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineMeshComponent.h"
#include "CellComponent.h"
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

public:
	TArray<FString> GetDivideSubstrings(FString InString) override;
	UPROPERTY(BlueprintReadOnly, Category = "Cell")
		USplineMeshComponent* DrawnComponent;

	virtual void drawCellRecursively() override;
	
	//this is UP grow direction * length of a cell in this direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		FVector StandardDrawVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genome")
		UStaticMesh* StaticMeshForVisuals;

	//where the draw, i.e. the SplineMesh, is going to start
	UPROPERTY(BlueprintReadOnly, Category = "Genome")
		FVector DrawOriginPosition;

	//where the draw, i.e the SplineMesh, is going to end
	UPROPERTY(BlueprintReadOnly, Category = "Genome")
		FVector DrawEndPosition;
};
