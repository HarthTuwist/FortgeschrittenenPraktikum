// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeInformationHolder.h"

UTreeInformationHolder::UTreeInformationHolder()
{

	StandardCellWidth = 0.5f;
	StandardDrawVector = FVector(0.0f, 0.0f, 50.0f);
	SplineMeshForwardAxis = ESplineMeshAxis::Z; //use upward axis to draw
	SplineMeshUpDir = FVector(1.0f, 0.0f, 0.0f); //not z, as we want to draw towards + z and splines can't do that for UE bugs or deeper reasons unbeknownst
	
	RootString = TEXT("A");
	DivideHorizMarker = TEXT("H");
	GrowLeftMarker = TEXT("L");
	GrowRightMarker = TEXT("R");

	BaseBranchingAngleX = 15.0f;
	BaseBranchingAngleY = 15.0f;
}
