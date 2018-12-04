// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeCellComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h "
#include "WorldCollision.h"
#include "Components/SceneComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogCell_Collision, Log, All);

UTreeCellComponent::UTreeCellComponent()
{
	NewCellClass = UTreeCellComponent::StaticClass();

	InstancedMeshIdThisIteration = -42;
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

void UTreeCellComponent::CopyPropertiesFromParent(UCellComponent * Parent)
{
	
	if (Parent == nullptr) //this should not happen
	{
		UE_LOG(LogCell, Warning, TEXT("CopyingPropertiesFrom Parent == nullptr for TreeCell: %s"),
			*StateString);

		return;
	}

	UTreeCellComponent* ParentAsTreeCell = Cast<UTreeCellComponent>(Parent);
	
	if (ParentAsTreeCell == nullptr) //other cell, e.g. root, is the parent
	{
		WeightBurden = 0;
	}

	else
	{
		WeightBurden = ParentAsTreeCell->WeightBurden;
	}

	//always set this properties to standard values
	IterationsSinceCreation = 0;
	LightThisIteration = 0;

}

void UTreeCellComponent::divideCell()
{

	if (OwnersTreeInfos == nullptr)
	{
		UE_LOG(LogCell, Warning, TEXT("No OwnersTreeInfos for cell: %s, StateString: %s, cancel divide"),
			*GetNameSafe(this),
			*StateString);

		return;
	}


	if (OwnersTreeInfos->bUseGenomeMapReplacement)
	{
		Super::divideCell();
	}

	else
	{
		FCellTypeDefinition* DefOfThis = OwnersTreeInfos->CellDefMap.Find(StateString);
		if (DefOfThis != nullptr)
		{
			for (uint8 i = 0; i < (uint8) EStateTraitEnum::FIRST_INVALID; i++)
			{
				FCellDivideDefinition* CurCellDivDef = DefOfThis->DivideMap.Find((EStateTraitEnum)i);

				if (CurCellDivDef != nullptr)
				{
					if (GetCellStateTrait((EStateTraitEnum)i) >= CurCellDivDef->DivideThreshold)
					{
						if (CurCellDivDef->bDividesHorizontally == true)
						{
							divideCellHorizontally(&CurCellDivDef->ChildrenStateStrings);
						}

						else
						{
							divideCellVertically(&CurCellDivDef->ChildrenStateStrings);
						}

						break;
					}
				}
			}
		}
	}

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

	//DrawnComponent = NewObject<USplineMeshComponent>(GetOwner());
/*	DrawnComponent = NewObject<UStaticMeshComponent>(GetOwner());
	DrawnComponent->SetMobility(EComponentMobility::Movable);
//	DrawnComponent->SetupAttachment(this);
	DrawnComponent->SetupAttachment(GetOwner()->GetRootComponent());
	DrawnComponent->RegisterComponent();*/

	//DrawnComponent->SetupAttachment(this);
	/*UE_LOG(LogCell, VeryVerbose, TEXT("DrawTreeCell, Name:, %s, StateString: %s OriginPos: %f, %f, %f , EndPos: %f, %f, %f"),
		*GetNameSafe(this),
		*StateString,
		DrawOriginPosition.X, DrawOriginPosition.Y, DrawOriginPosition.Z,
		DrawEndPosition.X, DrawEndPosition.Y, DrawEndPosition.Z);
		*/
	const FCellTypeDefinition* DefOfThis = OwnersTreeInfos->CellDefMap.Find(StateString);
	UStaticMesh* CellMesh = OwnersTreeInfos->StaticMeshForVisuals;
	bool bIsLeave = false;

	if (DefOfThis != nullptr && DefOfThis->bLEAVE_IsLeave)
	{
		CellMesh = OwnersTreeInfos->StaticMeshForLeaves;
		bIsLeave = true;

		//set collision channel 
//		DrawnComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
//		DrawnComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	int32 AddedInstanceID = -42;
	int32 AddedPointerID = -42;

	if (OwnersTreeInfos && CellMesh->IsValidLowLevel())
	{

		//DrawnComponent->SetStaticMesh(CellMesh);

		float SingleScale = OwnersTreeInfos->StandardCellWidth;
		//const FVector StandardDrawUpVector = OwnersTreeInfos->StandardDrawDirection * OwnersTreeInfos->StandardDrawLength * StandardDrawUpVector * ParentCellTransform.GetScale3D().Z;
		
		int32 ParentWeightBurden = 0;
	
		//Disable this for now
		//if (ParentAsTreeCell != nullptr)
		//{
		//	ParentWeightBurden = ParentAsTreeCell->WeightBurden;
		//}

		const FVector StandardDrawUpVector = OwnersTreeInfos->StandardDrawDirection * (OwnersTreeInfos->StandardDrawLength + ParentWeightBurden);
		const float StandardDrawScale = OwnersTreeInfos->StandardDrawLength / OwnersTreeInfos->ZAxisHeightOfDrawnMesh; //scale so that the height of the drawn mesh = StandardDrawLength
	
		//const float WeightThicknessBonus = FMath::Max(FMath::Pow(WeightBurden, 0.05) * 0.005, 1.0);
		const float WeightThicknessBonus = FMath::Max(FMath::Pow(WeightBurden, 0.6), 1.0f) * 0.005f ;

		const float CellWidthMultiplier = DefOfThis ? DefOfThis->WidthMultiplierX : 1.0f;

		const FVector Scale = FVector(
			OwnersTreeInfos->StandardCellWidth * StandardDrawScale * CellWidthMultiplier + WeightThicknessBonus,
			OwnersTreeInfos->StandardCellWidth * StandardDrawScale * CellWidthMultiplier + WeightThicknessBonus,
			StandardDrawScale);

		float PossibleLeaveGapMult = 1.0f;
		if (DefOfThis && DefOfThis->bLEAVE_IsLeave)
		{
			PossibleLeaveGapMult = OwnersTreeInfos->LeaveGapMultiplier;
		}

		float ParentTransformScaleZ = ParentAsTreeCell ? ParentAsTreeCell->DrawTransform.GetScale3D().Z : 0.0f; //if no root cell, no offset

		const FVector Location =
			ParentCellTransform.GetLocation()
			+ ParentCellTransform.GetRotation().RotateVector(OwnersTreeInfos->StandardDrawDirection * OwnersTreeInfos->ZAxisHeightOfDrawnMesh * ParentTransformScaleZ * PossibleLeaveGapMult);
		//	+ ParentCellTransform.GetRotation().RotateVector(StandardDrawUpVector * PossibleLeaveGapMult);

		FRotator ThisRot = FRotator::ZeroRotator;
		if (ParentAsTreeCell != nullptr && ParentAsTreeCell->AttachedCellChildren.Num() >= 2)
		{
			const int32 PosInParent = ParentAsTreeCell->AttachedCellChildren.Find(this);
			FVector GrowVectorOfThis;

			//handle being aligned to ground
// 			if (DefOfThis && DefOfThis->bAttachToGround &&
// 				(ParentAsTreeCell->AlignNormalForChilds != FVector::ZeroVector)
// 				)
 			if (false) //disable this for now as this does not work
			{
				const float AngleBetweenNeighbors = DefOfThis->HorChlCircleAngle * 2 / ParentAsTreeCell->AttachedCellChildren.Num();
				const float FinalAngle = (-DefOfThis->HorChlCircleAngle) + AngleBetweenNeighbors * PosInParent;
				GrowVectorOfThis = FVector(0.0f, 0.0f, 1.0f).RotateAngleAxis(FinalAngle , FVector(0.0f, 1.0f, 0.0f)); //Rotate Around Y, as in Local Space this is the normal of the impact

				ThisRot = UKismetMathLibrary::MakeRotFromZY(GrowVectorOfThis, FVector(0.0f, 1.0f, 0.0f)); //make a rotation corresponding to the Vector

				UE_LOG(LogCell, Log, TEXT("%s, drawing Vector in Local Space: %f, %f, %f, ResultRot: %f, %f, %f"),
					*GetNameSafe(this),
					GrowVectorOfThis.X, GrowVectorOfThis.Y, GrowVectorOfThis.Z,
					ThisRot.Roll, ThisRot.Pitch, ThisRot.Yaw);

			//	const FVector RelativeXAxis = FVector::CrossProduct(LineTraceDirectionVector, FVector(1.0f, 1.0f, 1.0f)).GetSafeNormal();


			}

			//handle not being aligned to ground
			else
			{
				//TODO why does this get copied if this is quasi-const?
				TArray<FVector> GrowVectorArray;
				ParentAsTreeCell->GetRawHorizChilDrawVecs(GrowVectorArray);



				if (!(GrowVectorArray.Num() > PosInParent))
				{
					UE_LOG(LogCell, Error, TEXT("Drawing TreeCell, to few parent grow Vectors, Name: %s, StateString: %s, ParentGrowVecs.Length: %u, wanted position: %u"),
						*GetNameSafe(this),
						*StateString,
						GrowVectorArray.Num(),
						PosInParent);
				}

				else
				{
					GrowVectorOfThis = GrowVectorArray[PosInParent];
					ThisRot = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, GrowVectorOfThis); //TODO I think this is wrong and works by accident
				}

			}

			

		}
		//else keep ZeroRotator(first cell in tree)
		//const FRotator EndRot = UKismetMathLibrary::ComposeRotators(ParentCellTransform.GetRotation().Rotator(), ThisRot);
		// 
		const float LerpValue = DefOfThis ? FMath::Abs(DefOfThis->CorrelationWithStandardDrawDirection) : 0.0f;
		FRotator LerpTarget;

		if (DefOfThis != nullptr && DefOfThis->CorrelationWithStandardDrawDirection < 0.0f)
		{
			LerpTarget = UKismetMathLibrary::MakeRotFromZX(StandardDrawUpVector * (-1), (StandardDrawUpVector * (-1)).RotateAngleAxis(90.0f, FVector(0.0f, 1.0f, 0.0f)));
		}

		else
		{
			LerpTarget = UKismetMathLibrary::MakeRotFromZX(StandardDrawUpVector , StandardDrawUpVector.RotateAngleAxis(90.0f, FVector(0.0f, 1.0f, 0.0f)));
		}

		FRotator EndRot = UKismetMathLibrary::ComposeRotators(ThisRot, ParentCellTransform.GetRotation().Rotator());
		EndRot.Roll = UKismetMathLibrary::Lerp(EndRot.Roll, LerpTarget.Roll, LerpValue);
		EndRot.Pitch = UKismetMathLibrary::Lerp(EndRot.Pitch, LerpTarget.Pitch, LerpValue);


		//DrawTransform = FTransform(FRotator(RotationAngleY,0.0f, RotationAngleX), Location, Scale);
		DrawTransform = FTransform(EndRot, Location, Scale);
		//DrawTransform = FTransform(UKismetMathLibrary::FindLookAtRotation(asdf, FVector::ZeroVector), Location, Scale);
		
		/*
		FRotator asdf = UKismetMathLibrary::MakeRotFromXZ(StandardDrawUpVector.RotateAngleAxis(90.0f, FVector(0.0f, 1.0f, 0.0f)), StandardDrawUpVector);
		FRotator xyz = UKismetMathLibrary::ComposeRotators(ThisRot, ParentCellTransform.GetRotation().Rotator());

		UE_LOG(LogCell, VeryVerbose, TEXT("Drawing TreeCell, Name:, %s, TEST MAKEROTFROMXZ %f, %f, %f composyRotators: %f, %f, %f, ParentCellRot: %f, %f, %f"),
			*GetNameSafe(this),
			asdf.Roll, asdf.Pitch, asdf.Yaw,
			xyz.Roll, xyz.Pitch, xyz.Yaw,
			ParentCellTransform.GetRotation().Rotator().Roll, ParentCellTransform.GetRotation().Rotator().Pitch, ParentCellTransform.GetRotation().Rotator().Yaw);
		*/

		UE_LOG(LogCell, VeryVerbose, TEXT("Drawing TreeCell, Name: %s, StateString: %s Loc: %f, %f, %f Scale: %f, %f, %f Rot: %f, %f, %f"),
			*GetNameSafe(this),
			*StateString,
			DrawTransform.GetLocation().X, DrawTransform.GetLocation().Y, DrawTransform.GetLocation().Z,
			DrawTransform.GetScale3D().X, DrawTransform.GetScale3D().Y, DrawTransform.GetScale3D().Z,
			DrawTransform.GetRotation().Rotator().Roll, DrawTransform.GetRotation().Rotator().Pitch, DrawTransform.GetRotation().Rotator().Yaw);



/////////////////////////////
///////////Handle Growing on Ground
/////////////////////////////
		if (DefOfThis && DefOfThis->bAttachToGround)
		{

			if (ParentAsTreeCell && (ParentAsTreeCell->AlignNormalForChilds != FVector::ZeroVector))
			{
// 				//TODO is it right to use StandardDrawUpVector here over just Z+ Axis?
// 				const FVector GrowDirection = DrawTransform.GetRotation().RotateVector(StandardDrawUpVector).GetSafeNormal();
// 				   FVector::VectorPlaneProject(GrowDirection, ParentAsTreeCell->AlignNormalForChilds); //TODO * standarddrawvector.length
// 
// 				const FRotator AlignCorrectedRotator =  UKismetMathLibrary::MakeRotFromZ(FVector::VectorPlaneProject(GrowDirection, ParentAsTreeCell->AlignNormalForChilds));
// 				//TODO does this work with yaw rotation as well?
// 				// 
// 				DrawTransform.SetRotation(AlignCorrectedRotator.Quaternion());

				const FVector GrowDirection = DrawTransform.GetRotation().RotateVector(StandardDrawUpVector).GetSafeNormal();

				const FRotator AlignCorrectedRotator = UKismetMathLibrary::MakeRotFromZ(FVector::VectorPlaneProject(GrowDirection, ParentAsTreeCell->AlignNormalForChilds));

				DrawTransform.SetRotation(AlignCorrectedRotator.Quaternion());
				
				
				
				//alternate: align only the parent vector

// 				const FVector GrowDirection = ParentCellTransform.GetRotation().RotateVector(StandardDrawUpVector);
// 				const FRotator AlignCorrectedRotator = UKismetMathLibrary::MakeRotFromZ(FVector::VectorPlaneProject(GrowDirection, ParentAsTreeCell->AlignNormalForChilds));
// 				DrawTransform.SetRotation(UKismetMathLibrary::ComposeRotators(ThisRot, AlignCorrectedRotator).Quaternion());


			}



			//calculate if drawing direction needs to change because of obstacle
			FHitResult Rslt = FHitResult();
			FCollisionQueryParams Params = FCollisionQueryParams();
			//Params.TraceTag = TraceTag; //comment in to show raytrace debug lines
			Params.bTraceAsyncScene = false;
			const FVector LineTraceEndWorld = DrawTransform.GetLocation() + DrawTransform.GetRotation().RotateVector(StandardDrawUpVector);
			GetWorld()->LineTraceSingleByChannel(Rslt, Location, LineTraceEndWorld, ECollisionChannel::ECC_GameTraceChannel1, Params, FCollisionResponseParams());

			if (Rslt.bBlockingHit)
			{
				AlignNormalForChilds = Rslt.ImpactNormal;

				//TODO is this enough?
				FVector NewScale = DrawTransform.GetScale3D();
				NewScale.Z = (Location - Rslt.Location).Size() / OwnersTreeInfos->ZAxisHeightOfDrawnMesh * 0.9;
				DrawTransform.SetScale3D(NewScale);
			}

			else if (ParentAsTreeCell)
			{
				AlignNormalForChilds = ParentAsTreeCell->AlignNormalForChilds;
			}

			else
			{
				AlignNormalForChilds = FVector::ZeroVector;
			}

			//DrawTransform . Rot 
			// use FVector::VectorPlaneProject
		}



/////////////////////////////
///////////Collision Check
/////////////////////////////

		bool bActuallyAddInstance  = true;

		if (AttachedCellChildren.Num() == 0 && DefOfThis && !DefOfThis->bIgnoreCollisionCheck ) //only check for cells without children whether they collide
		{		
			
			TArray <FOverlapResult > HitArray = TArray<FOverlapResult>();
			FCollisionObjectQueryParams CollisionObjectParams = FCollisionObjectQueryParams();
			CollisionObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
			FCollisionQueryParams QueryParams = FCollisionQueryParams();
			QueryParams.bFindInitialOverlaps = true;
			//const FName TraceTag("TrunkCollisionTag");
			//QueryParams.TraceTag = TraceTag;

			FCollisionShape OverlapShape;
			FVector ShapeLocation;

			if (!bIsLeave)
			{
				ShapeLocation = DrawTransform.GetLocation() + DrawTransform.GetRotation().RotateVector(StandardDrawUpVector / 2);
				const float CapsuleHalfHeigth = DrawTransform.GetScale3D().Z * OwnersTreeInfos->ZAxisHeightOfDrawnMesh / 2 * 0.8; //TOODO Remove magic number I
				const float CapsuleRadius = DrawTransform.GetScale3D().X * OwnersTreeInfos->ZAxisHeightOfDrawnMesh / 2;

				//const float CapsuleHalfHeigth = OwnersTreeInfos->ZAxisHeightOfDrawnMesh / 2;
				//const float CapsuleRadius = DrawTransform.GetScale3D().X * OwnersTreeInfos->ZAxisHeightOfDrawnMesh / 2;

				//GetWorld()->Overlapmulti(HitArray, DrawTransform.GetLocation(), DrawTransform.GetRotation());

				OverlapShape = FCollisionShape::MakeCapsule(CapsuleHalfHeigth, CapsuleRadius);

				if (OwnersTreeInfos->bShowGrowCollisionMarkers)
				{
					DrawDebugCapsule(
						GetWorld(),
						ShapeLocation,
						CapsuleHalfHeigth,
						CapsuleRadius,
						DrawTransform.GetRotation(),
						FColor(0, 0, 240),
						false,
						3.0f);
				}

			}

			else
			{
				ShapeLocation = DrawTransform.GetLocation();
				const float BoxHalfXY = DrawTransform.GetScale3D().X * OwnersTreeInfos->ZAxisHeightOfDrawnMesh / 2 * OwnersTreeInfos->LeavesCollisionCheckMultiplier;
				const FVector BoxHalfExtents = FVector(BoxHalfXY, BoxHalfXY, 0.1f); //TODO Remove magic number II

				OverlapShape = FCollisionShape::MakeBox(BoxHalfExtents);

				if (OwnersTreeInfos->bShowGrowCollisionMarkers)
				{
					DrawDebugBox(
						GetWorld(),
						ShapeLocation,
						BoxHalfExtents * 2,
						DrawTransform.GetRotation(),
						FColor(0, 240, 240),
						false,
						3.0f);
				}

			}

			GetWorld()->OverlapMultiByChannel(HitArray,
				ShapeLocation,
				DrawTransform.GetRotation(),
				ECollisionChannel::ECC_WorldStatic,
				OverlapShape,
				QueryParams);

			TArray<int32> InstanceIdsToIgnore = TArray<int32>();
				
			if (ParentAsTreeCell != nullptr)
			{
				InstanceIdsToIgnore.Add(ParentAsTreeCell->InstancedMeshIdThisIteration);

				for (UCellComponent* Sibling : ParentAsTreeCell->AttachedCellChildren)
				{
					const UTreeCellComponent* SiblingAsTreeCell = Cast<UTreeCellComponent>(Sibling);
					if (SiblingAsTreeCell != nullptr)
					{
						InstanceIdsToIgnore.Add(SiblingAsTreeCell->InstancedMeshIdThisIteration);
					}
						
				}
			}
				

			//TODO this does not check if overlap is actually static mesh instance, but this shouldn't be a problem?
			for (FOverlapResult Rslt : HitArray)
			{
				UE_LOG(LogCell_Collision, VeryVerbose, TEXT("Collided with: %s, Nr: %u"),
					*GetNameSafe(Rslt.Component.Get()),
					Rslt.ItemIndex);

				if (!InstanceIdsToIgnore.Contains(Rslt.ItemIndex))
				{
						if (!DefOfThis->bAttachToGround || Rslt.Actor.Get() == GetOwner()) //only self-collide if attach to ground
						{
							bActuallyAddInstance = false;
						}
				}

					
			}
		}

		if (bActuallyAddInstance)
		{
			if (bIsLeave)
			{
				AddedInstanceID = OwnersTreeInfos->LeavesInstanceComponent->AddInstance(DrawTransform);
				AddedPointerID = OwnersTreeInfos->LeavesArrayThisIteration.Add(this);
			}

			else
			{
				AddedInstanceID = OwnersTreeInfos->TrunksInstanceComponent->AddInstance(DrawTransform);
				AddedPointerID = OwnersTreeInfos->TrunkArrayThisIteration.Add(this);
			}

			InstancedMeshIdThisIteration = AddedInstanceID;
		}

		else
		{
			//destroy this if we get a collision
			//this->UnregisterComponent();

			DestroyComponent();
		}
	}

	UE_LOG(LogCell, VeryVerbose, TEXT("Drawing TreeCell, Name:, %s, StateString: %s, InstanceId: %u (%u) ,Loc: %f, %f, %f Scale: %f, %f, %f Rot: %f, %f, %f"),
		*GetNameSafe(this),
		*StateString,
		AddedInstanceID,
		AddedPointerID,
		DrawTransform.GetLocation().X, DrawTransform.GetLocation().Y, DrawTransform.GetLocation().Z,
		DrawTransform.GetScale3D().X, DrawTransform.GetScale3D().Y, DrawTransform.GetScale3D().Z,
		DrawTransform.GetRotation().Rotator().Roll, DrawTransform.GetRotation().Rotator().Pitch, DrawTransform.GetRotation().Rotator().Yaw);




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
	//NOTE: All values in local space (i.e. only Z+ and not StandardDrawVector)

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
		const FVector RowRotator = FVector(0.0f, 0.0f, 1.0f).RotateAngleAxis(DefOfThis->HorChlCircleAngle - DefOfThis->HorChlCircleVarianceAngle * r, FVector(1.0f, 0.0f, 0.0f)); //one axis orthogonal to StandardDrawDirection

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
		CalcHorizDivChilVecs();
	}

	Vectors = RawHorizChilDrawVecs;

}

void UTreeCellComponent::SetCellStateTrait(EStateTraitEnum TraitEnum, int32 NewValue)
{
	if (TraitEnum == EStateTraitEnum::TRAIT_LIFETIME)
	{
		IterationsSinceCreation = NewValue;
	}

	else if (TraitEnum == EStateTraitEnum::TRAIT_WEIGHTBURDEN)
	{
		WeightBurden = NewValue;
	}

	else if (TraitEnum == EStateTraitEnum::TRAIT_LIGHT)
	{
		LightThisIteration = NewValue;
	}
}

int32 UTreeCellComponent::GetCellStateTrait(EStateTraitEnum TraitEnum)
{
	if (TraitEnum == EStateTraitEnum::TRAIT_LIFETIME)
	{
		return IterationsSinceCreation;
	}

	else if (TraitEnum == EStateTraitEnum::TRAIT_WEIGHTBURDEN)
	{
		return WeightBurden;
	}

	else if (TraitEnum == EStateTraitEnum::TRAIT_LIGHT)
	{
		return LightThisIteration;
	}

	else
	{
		return -21;
	}
}

int32 UTreeCellComponent::CalcBurdenRecursively()
{
	int32 outValue = 0;

	for (UCellComponent* child : AttachedCellChildren)
	{
		outValue += Cast<UTreeCellComponent>(child)->CalcBurdenRecursively();
		outValue += 1; //add 1 as the cell has a weight, too
	}
	//if no children weight is 0
	
	WeightBurden = outValue;
	return outValue;
}
