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
		virtual void divideCell();

	//return true if cell is supposed to divide horizontally, false if it should divide vertically
	UFUNCTION(BlueprintPure, Category = "Cell")
		virtual bool shouldDivideHorizontally();

	virtual void divideCellHorizontally(); //called by divideCell(bool bDivideHorizontally)
	virtual void divideCellVertically();//called by divideCell(bool bDivideHorizontally)

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

	//returns the array of substrings that are used in a division to transform this cell into its children
	UFUNCTION(BlueprintPure, Category = "Cell|Genome")
		virtual TArray<FString> GetDivideSubstrings(FString InString);

	//returns the array of substrings intended for division if following the GenomeMap replacement scheme
		virtual TArray<FString> GetMapDivideStrings(FString InString, TMap<FString, FString>* ReplacementMap, FString Separator = TEXT(","));

	UFUNCTION(BlueprintCallable, Category = "Cell|Genome")
		virtual void InitWithString(FString InString);

	UFUNCTION(BlueprintCallable, Category = "Cell|Genome")
		void InitFromStateString();

};
