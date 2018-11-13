// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeCellComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, VeryVerbose, All);

UTreeCellComponent::UTreeCellComponent()
{
	NewCellClass = UTreeCellComponent::StaticClass();
}

void UTreeCellComponent::BeginPlay()
{
	TArray<UActorComponent*> CompArray = GetOwner()->GetComponentsByClass(UTreeInformationHolder::StaticClass());
	if (CompArray.Num() > 0)
	{
		OwnersTreeInfos = Cast<UTreeInformationHolder>(CompArray[0]);

		if (StateString.Equals(TEXT("@@@")) && !OwnersTreeInfos->RootString.IsEmpty())
		{
			StateString = OwnersTreeInfos->RootString;
		}
		
	}

	else
	{
		OwnersTreeInfos = nullptr;
	}

	Super::BeginPlay();
}

TArray<FString> UTreeCellComponent::GetDivideSubstrings(FString InString)
{
	//if we are using GenomeMap scheme, calculate children by this
	if (OwnersTreeInfos->bUseGenomeMapReplacement)
	{
		TArray<FString> ArrayFromMap = GetMapDivideStrings(InString, &OwnersTreeInfos->GenomeMap, OwnersTreeInfos->GenomeMapValueSeparator);
		if (ArrayFromMap.Num()> 0 && ArrayFromMap[0] != TEXT("@"))
		{
			return ArrayFromMap;
		}

		else
		{
			//in case a StateString was reached that isn't in the Map, do safety behavior 
			UE_LOG(LogCell, Warning, TEXT("GetMapDivideStrings returned @ for TreeCell: %s, StateString: %s, keeping StateString"),
				*GetNameSafe(this),
				*StateString);

			ArrayFromMap.Empty();
			ArrayFromMap.Add(StateString);

			return ArrayFromMap;
		}
	}

	//else
	TArray<FString> OutArray = TArray<FString>();

	if (StateString.Contains(TEXT("H")))
	{
		OutArray.Add(TEXT("A"));
		OutArray.Add(TEXT("B"));
	}

	else
	{
		OutArray.Add(StateString);
		OutArray.Add(StateString);
	}

	return OutArray;
}

void UTreeCellComponent::drawCellRecursively()
{
	//throw away old visuals
	if (DrawnComponent)
	{
		DrawnComponent->DestroyComponent();
	}

	UTreeCellComponent* ParentAsTreeCell = Cast<UTreeCellComponent>(AttachedCellParent);
	FTransform ParentCellTransform; //the transform of our parent or 0 if it does not exist
	
	if (ParentAsTreeCell == nullptr)
	{


		//we should never have no parent. At max parent should be a designatedRootCell (== not a tree cell)
		if (AttachedCellParent)
		{
			DrawOriginPosition = FVector(0.0f, 0.0f, 0.1f);

			ParentCellTransform = FTransform(GetOwner()->GetActorRotation(), GetOwner()->GetActorLocation()); //identity transform
		}

		else
		{
			UE_LOG(LogCell, Error, TEXT("TreeCell: %s can't divide, no parent, StateString: %s"),
				*GetNameSafe(this),
				*StateString);
			return;
		}
		
	}

	else
	{
		DrawOriginPosition = ParentAsTreeCell->DrawEndPosition;
		ParentCellTransform = ParentAsTreeCell->DrawTransform;
	}

	FVector DrawDirection = FVector::ZeroVector;

	if (OwnersTreeInfos != nullptr)
	{	
		DrawDirection = OwnersTreeInfos->StandardDrawVector;

		if (ParentAsTreeCell == nullptr) //if we not have a TreeCell as parent but something else (usually the designated root)
		{
			RotationAngleX = 0.0f;
			RotationAngleY = 0.0f;


			//grow straight up into the air, so don't change DrawDirection
		}
		else 
		{
			float bNegativeXRotation = 1.0f; //use this as a fake bool to multiply rotation if we rotate left
			float bNegativeYRotation = 1.0f; //use this as a fake bool to multiply rotation if we rotate backwards
			float XDivergionFromParent = 0.0f; //the diversion from the parent angle in X axis
			float YDivergionFromParent = 0.0f; //the diversion from the parent angle in > axis
			
			/*
			//X Divergion
			if (StateString.Contains(OwnersTreeInfos->GrowRightMarker))
			{
				XDivergionFromParent = CountStringInString(&OwnersTreeInfos->GrowRightMarker, &StateString);
			}

			else if (StateString.Contains(OwnersTreeInfos->GrowLeftMarker))
			{
				XDivergionFromParent = CountStringInString(&OwnersTreeInfos->GrowLeftMarker, &StateString);
				bNegativeXRotation = -1.0f;
			}

			//Y Divergion
			if (StateString.Contains(OwnersTreeInfos->GrowForwardMarker))
			{
				YDivergionFromParent = CountStringInString(&OwnersTreeInfos->GrowForwardMarker, &StateString);
			}

			else if (StateString.Contains(OwnersTreeInfos->GrowBackMarker))
			{
				YDivergionFromParent = CountStringInString(&OwnersTreeInfos->GrowBackMarker, &StateString);
				bNegativeYRotation = -1.0f;
			}
			 
			// if grow straight
				//don't do anything here, angles need to be 0

			*/

			//////////////
			const int32 NumParentChildren = ParentAsTreeCell->AttachedCellChildren.Num();
			const FCellTypeDefinition* ParentDef = OwnersTreeInfos->CellDefMap.Find(ParentAsTreeCell->StateString);

			if (NumParentChildren > 1)
			{
				const int32 PosInParentsChildren = ParentAsTreeCell->AttachedCellChildren.Find(this);
				
	
				if (ParentDef != nullptr)
				{
					XDivergionFromParent = ParentDef->HorizChildrenGrowFreedomX_Variance / (NumParentChildren - 1) * PosInParentsChildren + ParentDef->HorizChildrenGrowFreedomX_Mean;
					YDivergionFromParent = ParentDef->HorizChildrenGrowFreedomY_Variance / (NumParentChildren - 1) * PosInParentsChildren + ParentDef->HorizChildrenGrowFreedomY_Mean;
				}
			}

			const FCellTypeDefinition* DefOfThis = OwnersTreeInfos->CellDefMap.Find(StateString);
			if (DefOfThis != nullptr)
			{

				XDivergionFromParent += OwnersTreeInfos->CellDefMap.Find(StateString)->AdditionalRotationAngleX;
				YDivergionFromParent += OwnersTreeInfos->CellDefMap.Find(StateString)->AdditionalRotationAngleY;


				RotationAngleX = ParentAsTreeCell->RotationAngleX +  XDivergionFromParent;
				RotationAngleY = ParentAsTreeCell->RotationAngleY + YDivergionFromParent;


				//add correlation with standardDrawDirection
				float LerpTarget = 0.0f;
				if (DefOfThis->CorrelationWithStandardDrawDirection >= 0)
				{
					LerpTarget = 0.0f;
				}
				else
				{
					
					LerpTarget = 180.0f;
				}

				//TODO mache das auch 3D fähig
				RotationAngleX = FMath::Lerp(RotationAngleX, LerpTarget, FMath::Abs(DefOfThis->CorrelationWithStandardDrawDirection));
				//RotationAngleY = FMath::Lerp(RotationAngleY, LerpTarget, FMath::Abs(DefOfThis->CorrelationWithStandardDrawDirection));
			}
			//////////////

			else
			{
				RotationAngleX = ParentAsTreeCell->RotationAngleX + XDivergionFromParent;
				RotationAngleY = ParentAsTreeCell->RotationAngleY + YDivergionFromParent;
			}

		


			//actually rotate DrawDirection
			DrawDirection = DrawDirection.RotateAngleAxis(RotationAngleX, FVector(1.0f, 0.0f, 0.0f));
			DrawDirection = DrawDirection.RotateAngleAxis(RotationAngleY, FVector(0.0f, 1.0f, 0.0f));






		}

		DrawEndPosition = DrawOriginPosition + DrawDirection;

	}

	else //a dummy value in case OwnersTreeInfos doesn't exist
	{
		DrawDirection = FVector(-42.0f, -42.0f, -42.0f);
	}

	//DrawnComponent = NewObject<USplineMeshComponent>(GetOwner());
	DrawnComponent = NewObject<UStaticMeshComponent>(GetOwner());
	DrawnComponent->SetMobility(EComponentMobility::Movable);
//	DrawnComponent->SetupAttachment(this);
	DrawnComponent->SetupAttachment(GetOwner()->GetRootComponent());
	DrawnComponent->RegisterComponent();
	

	//DrawnComponent->SetupAttachment(this);
	/*UE_LOG(LogCell, VeryVerbose, TEXT("DrawTreeCell, Name:, %s, StateString: %s OriginPos: %f, %f, %f , EndPos: %f, %f, %f"),
		*GetNameSafe(this),
		*StateString,
		DrawOriginPosition.X, DrawOriginPosition.Y, DrawOriginPosition.Z,
		DrawEndPosition.X, DrawEndPosition.Y, DrawEndPosition.Z);
		*/

	if (OwnersTreeInfos && OwnersTreeInfos->StaticMeshForVisuals->IsValidLowLevel())
	{
		DrawnComponent->SetStaticMesh(OwnersTreeInfos->StaticMeshForVisuals);
		/*
		DrawnComponent->SetStartPosition(DrawOriginPosition);
		DrawnComponent->SetEndPosition(DrawEndPosition);
		DrawnComponent->SetStartTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		DrawnComponent->SetEndTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		
		float SingleScale = OwnersTreeInfos->StandardCellWidth;
		
		DrawnComponent->SetStartScale(FVector2D(SingleScale, SingleScale));
		DrawnComponent->SetEndScale(FVector2D(SingleScale, SingleScale));

		DrawnComponent->SetForwardAxis(OwnersTreeInfos->SplineMeshForwardAxis);
		DrawnComponent->SetSplineUpDir(OwnersTreeInfos->SplineMeshUpDir);
	*/
		float SingleScale = OwnersTreeInfos->StandardCellWidth;
		//const FVector StandardDrawUpVector = OwnersTreeInfos->StandardDrawDirection * OwnersTreeInfos->StandardDrawLength * StandardDrawUpVector * ParentCellTransform.GetScale3D().Z;
		const FVector StandardDrawUpVector = OwnersTreeInfos->StandardDrawDirection * OwnersTreeInfos->StandardDrawLength;
	
		const FVector Scale = FVector(
			OwnersTreeInfos->StandardCellWidth * OwnersTreeInfos->StandardDrawLength / OwnersTreeInfos->ZAxisHeightOfDrawnMesh,
			OwnersTreeInfos->StandardCellWidth * OwnersTreeInfos->StandardDrawLength / OwnersTreeInfos->ZAxisHeightOfDrawnMesh,
			OwnersTreeInfos->StandardDrawLength / OwnersTreeInfos->ZAxisHeightOfDrawnMesh);

		const FVector Location =
			ParentCellTransform.GetLocation()
			+ ParentCellTransform.GetRotation().RotateVector(StandardDrawUpVector);

		FRotator ThisRot = FRotator::ZeroRotator;
		if (ParentAsTreeCell != nullptr && ParentAsTreeCell->AttachedCellChildren.Num()>2)
		{
			TArray<FVector> GrowVectorArray;
			ParentAsTreeCell->GetRawHorizChilDrawVecs(GrowVectorArray);
			const int32 PosInParent = ParentAsTreeCell->AttachedCellChildren.Find(this);
			if (GrowVectorArray.Num() > PosInParent)
			{
				const FVector GrowVectorOfThis = GrowVectorArray[PosInParent];
				ThisRot = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, GrowVectorOfThis);
			}
			else
			{
				UE_LOG(LogCell, Error, TEXT("Drawing TreeCell, to few parent grow Vectors, Name: %s, StateString: %s, ParentGrowVecs.Length: %u, wanted position: %u"),
					*GetNameSafe(this),
					*StateString,
					GrowVectorArray.Num(),
					PosInParent);
			}
			
			//Rotation = FRotator(0.0f, 0.0f, 0.0f).Quaternion();
		}
		//else keep ZeroRotator(first cell in tree)
		const FRotator EndRot = UKismetMathLibrary::ComposeRotators(ParentCellTransform.GetRotation().Rotator(), ThisRot);

		//DrawTransform = FTransform(FRotator(RotationAngleY,0.0f, RotationAngleX), Location, Scale);
		DrawTransform = FTransform(EndRot, Location, Scale);
		//DrawTransform = FTransform(UKismetMathLibrary::FindLookAtRotation(asdf, FVector::ZeroVector), Location, Scale);

		UE_LOG(LogCell, VeryVerbose, TEXT("Drawing TreeCell, Name:, %s, StateString: %s Loc: %f, %f, %f Scale: %f, %f, %f Rot: %f, %f, %f"),
			*GetNameSafe(this),
			*StateString,
			DrawTransform.GetLocation().X, DrawTransform.GetLocation().Y, DrawTransform.GetLocation().Z,
			DrawTransform.GetScale3D().X, DrawTransform.GetScale3D().Y, DrawTransform.GetScale3D().Z,
			DrawTransform.GetRotation().Rotator().Roll, DrawTransform.GetRotation().Rotator().Pitch, DrawTransform.GetRotation().Rotator().Yaw);

		DrawnComponent->SetWorldTransform(DrawTransform);
		DrawnComponent->SetCastShadow(false);
	}



	Super::drawCellRecursively();
}

bool UTreeCellComponent::shouldDivideHorizontally()
{
	if (OwnersTreeInfos && !OwnersTreeInfos->DivideHorizMarker.IsEmpty()
		&& StateString.Contains(OwnersTreeInfos->DivideHorizMarker))
	{
		return true;
	}

	else
	{
		return false;
	}
	
}

void UTreeCellComponent::InitWithString(FString InString)
{
	Super::InitWithString(InString);

	RawHorizChilDrawVecs.Empty();
}

bool UTreeCellComponent::CalcHorizDivChilVecs()
{
	//TODO fix this, as this does not seem to do exactly what it should be doing
	RawHorizChilDrawVecs.Empty();

	if (OwnersTreeInfos == nullptr)
	{
		return false;
	}

	const FCellTypeDefinition* DefOfThis = OwnersTreeInfos->CellDefMap.Find(StateString);
	if (DefOfThis == nullptr)
	{
		return false;
	}

	//TODO erlaube auch Varianten, die nur einen Halbkreis und keinen Kreis beschreiben?
	const float sqrt3 = 1.73205080757;
	const int32 NrAttachedChilds = AttachedCellChildren.Num();

	int32 NumberOfRows = 1;

	if (DefOfThis->HorChlCircleVarianceAngle != 0)
	{
		//There are somewhat complicated maths behind this. Look it up in the paper documentation
		NumberOfRows = FMath::CeilToInt(FMath::Sqrt(2 / sqrt3 * DefOfThis->HorChlCircleVarianceAngle / 360 * NrAttachedChilds));
		NumberOfRows = FMath::Sqrt(NrAttachedChilds);
		if (NumberOfRows < 1)
		{
			NumberOfRows = 1;
		}
	}
	//else keep it at 1
	
	const int32 ElementsPerRow = FMath::CeilToInt(NrAttachedChilds / NumberOfRows); //use ceil to get to the nearest angle-"rectangle" that contain all elements

	for (int32 r = 0; r < NumberOfRows; r++)
	{
		DefOfThis->HorChlCircleVarianceAngle;
		const FVector RowRotator = FVector(0.0f, 0.0f, 1.0f).RotateAngleAxis(DefOfThis->HorChlCircleAngle - DefOfThis->HorChlCircleVarianceAngle *r, FVector(1.0f, 0.0f, 0.0f)); //one axis orthogonal to StandardDrawDirection

		for (int32 e = 0; e < ElementsPerRow; e++)
		{
			const FVector NewEntry = RowRotator.RotateAngleAxis(e * 360 / ElementsPerRow, FVector(0.0f, 0.0f, 1.0f)); //rotate around Z Axis
		//	NewEntry = NewEntry.
			

		//	NewEntry =  NewEntry.RotateAngleAxis()
			RawHorizChilDrawVecs.Add(NewEntry);
		}
	}

	return true;
}

void UTreeCellComponent::GetRawHorizChilDrawVecs(TArray<FVector>& Vectors)
{
	if (RawHorizChilDrawVecs.Num() == 0)
	{
		CalcHorizDivChilVecs(); //TODO put actual value here
	}

	Vectors = RawHorizChilDrawVecs;

}
