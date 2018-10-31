// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeInformationHolder.h"

UTreeInformationHolder::UTreeInformationHolder()
{

	StandardCellWidth = 0.5f;

	StandardDrawVector = FVector(0.0f, 0.0f, 50.0f);
	StandardDrawDirection = FVector(0.0f, 0.0f, 1.0f);
	StandardDrawLength = 50.0f;
	ZAxisHeightOfDrawnMesh = 100.0f;

	SplineMeshForwardAxis = ESplineMeshAxis::Z; //use upward axis to draw
	SplineMeshUpDir = FVector(1.0f, 0.0f, 0.0f); //not z, as we want to draw towards + z and splines can't do that for UE bugs or deeper reasons unbeknownst
	
	RootString = TEXT("A");
	DivideHorizMarker = TEXT("H");
	GrowLeftMarker = TEXT("L");
	GrowRightMarker = TEXT("R");
	GrowForwardMarker = TEXT("F");
	GrowBackMarker = TEXT("B");

	BaseBranchingAngleX = 15.0f;
	BaseBranchingAngleY = 15.0f;
}

void UTreeInformationHolder::BeginPlay()
{
	Super::BeginPlay();

	TArray<FString> GenomeKeys;
	GenomeMap.GenerateKeyArray(GenomeKeys);
	for (FString F : GenomeKeys)
	{
		if(!GenomeMap.Contains(F))
		{
			CellDefMap.Add(F); //add default constructed Definitions
		}
	}

}
