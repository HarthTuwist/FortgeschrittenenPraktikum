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

public:

	//Draw Stuff
	TArray<FString> GetDivideSubstrings(FString InString) override;
	UPROPERTY(BlueprintReadOnly, Category = "Cell")
		USplineMeshComponent* DrawnComponent;

	virtual void drawCellRecursively() override;

	//where the draw, i.e. the SplineMesh, is going to start
	UPROPERTY(BlueprintReadOnly, Category = "Genome")
		FVector DrawOriginPosition;

	//where the draw, i.e the SplineMesh, is going to end
	UPROPERTY(BlueprintReadOnly, Category = "Genome")
		FVector DrawEndPosition;


	virtual bool shouldDivideHorizontally() override;

	virtual void InitWithString(FString InString) override;

private:
	UPROPERTY()
		UTreeInformationHolder* OwnersTreeInfos;
};
