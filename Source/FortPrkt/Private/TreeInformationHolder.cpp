// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeInformationHolder.h"

UTreeInformationHolder::UTreeInformationHolder()
{
	RootString = TEXT("A");
	StandardCellWidth = 0.5f;
	StandardDrawVector = FVector(0.0f, 0.0f, 50.0f);
	SplineMeshForwardAxis = ESplineMeshAxis::Z; //use upward axis to draw
	SplineMeshUpDir = FVector(1.0f, 0.0f, 0.0f); //not z, as we want to draw towards + z and splines can't do that for UE bugs or deeper reasons unbeknownst

	DivideHorizMarker = TEXT("H");
}
