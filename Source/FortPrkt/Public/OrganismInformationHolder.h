// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrganismInformationHolder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTPRKT_API UOrganismInformationHolder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrganismInformationHolder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//this Map saves which strings are going to be new cells and with which StateString these cells are going to be initialized
	UPROPERTY(BlueprintReadWrite, Category = "Cell|Genome")
		TMap<FString, FString> GenomeMap;

	//the string the first cell in an organism has
	UPROPERTY(BlueprintReadWrite, Category = "Cell|Genome")
		FString RootString;
		
	
};
