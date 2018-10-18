// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CellComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTPRKT_API UCellComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCellComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Cell")
		virtual void divideCell(bool bDivideHorizontally = false);

	//the cells that are directly attached to this cell
	UPROPERTY(BlueprintReadOnly, Category = "Cell")
		TArray<UCellComponent*> AttachedCellChildren;

	//the cell this cell is attached to
	UPROPERTY(BlueprintReadOnly, Category = "Cell")
		UCellComponent* AttachedCellParent;

	//true if cell was actually added
	UFUNCTION()
		virtual bool AddNewCellChild(UCellComponent* CellToAdd, int32 AtPosition = -42);

	UFUNCTION(BlueprintCallable, Category = "Cell")
		virtual void drawCellRecursively();

	UPROPERTY(BlueprintReadWrite, Category = "Cell")
		UClass* NewCellClass;

//////Genome & String Stuff

	//this string represents the current state of the cell
	UPROPERTY(BlueprintReadWrite, Category = "Cell|Genome")
		FString StateString;

	//the string the first cell in an organism has
	UPROPERTY(BlueprintReadWrite, Category = "Cell|Genome")
		FString RootString;

	//returns the array of substrings that are used in a division to transform this cell into its children
	UFUNCTION(BlueprintPure, Category = "Cell|Genome")
		virtual TArray<FString> GetDivideSubstrings(FString InString);

	UFUNCTION(BlueprintCallable, Category = "Cell|Genome")
		virtual void InitWithString(FString InString);

	UFUNCTION(BlueprintCallable, Category = "Cell|Genome")
		void InitFromStateString();

	//this Map saves which strings are going to be new cells and with which StateString these cells are going to be initialized
	UPROPERTY(BlueprintReadWrite, Category = "Cell|Genome")
		TMap<FString, FString> GenomeMap;
};
