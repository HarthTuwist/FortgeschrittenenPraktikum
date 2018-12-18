// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrganismCppBaseClass.generated.h"

class UOrganismInformationHolder;

UCLASS()
class FORTPRKT_API AOrganismCppBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrganismCppBaseClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "WholeOrganism")
		UOrganismInformationHolder* InfoOfThis;
	
};
