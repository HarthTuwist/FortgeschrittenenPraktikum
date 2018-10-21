// Fill out your copyright notice in the Description page of Project Settings.

#include "OrganismInformationHolder.h"


// Sets default values for this component's properties
UOrganismInformationHolder::UOrganismInformationHolder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RootString = TEXT(""); //intentionally leave this empty
	bUseGenomeMapReplacement = false;
	GenomeMapValueSeparator = TEXT(",");
	// ...
}


// Called when the game starts
void UOrganismInformationHolder::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UOrganismInformationHolder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

