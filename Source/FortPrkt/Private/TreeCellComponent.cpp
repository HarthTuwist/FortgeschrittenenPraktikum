// Fill out your copyright notice in the Description page of Project Settings.

#include "TreeCellComponent.h"
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

	
	if (ParentAsTreeCell == nullptr)
	{


		//we should never have no parent. At max parent should be a designatedRootCell (== not a tree cell)
		if (AttachedCellParent)
		{
			DrawOriginPosition = FVector(0.0f, 0.0f, 0.1f);
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
			float bNegativeRotation = 1.0f; //use this as a fake bool to multiply rotation if we rotate left
			float bDivergeFromParent = 0.0f; //use a fake bool here, too, for consistency. Signals that we we don't just want to grow into the direction of the parent

					

			if (StateString.Contains(OwnersTreeInfos->GrowRightMarker) || StateString.Contains((OwnersTreeInfos->GrowLeftMarker)))
			{
				bDivergeFromParent = 1.0f;

				if (StateString.Contains(OwnersTreeInfos->GrowLeftMarker)) //invert rotation if rotating left
				{
					bNegativeRotation = -1.0f;
				}
			}

			else //grow straight
			{
				//don't do anything here, bDivergeFromParent needs to be 0
			}

			RotationAngleX = ParentAsTreeCell->RotationAngleX + bNegativeRotation * bDivergeFromParent * OwnersTreeInfos->BaseBranchingAngleX;
			RotationAngleY = ParentAsTreeCell->RotationAngleY + bNegativeRotation * bDivergeFromParent * OwnersTreeInfos->BaseBranchingAngleY;

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

	DrawnComponent = NewObject<USplineMeshComponent>(GetOwner());
	DrawnComponent->SetMobility(EComponentMobility::Movable);
//	DrawnComponent->SetupAttachment(this);
	DrawnComponent->SetupAttachment(GetOwner()->GetRootComponent());
	DrawnComponent->RegisterComponent();
	

	//DrawnComponent->SetupAttachment(this);
	UE_LOG(LogCell, VeryVerbose, TEXT("DrawTreeCell, Name:, %s, StateString: %s OriginPos: %f, %f, %f , EndPos: %f, %f, %f"),
		*GetNameSafe(this),
		*StateString,
		DrawOriginPosition.X, DrawOriginPosition.Y, DrawOriginPosition.Z,
		DrawEndPosition.X, DrawEndPosition.Y, DrawEndPosition.Z);

	if (OwnersTreeInfos && OwnersTreeInfos->StaticMeshForVisuals->IsValidLowLevel())
	{
		DrawnComponent->SetStaticMesh(OwnersTreeInfos->StaticMeshForVisuals);
		DrawnComponent->SetStartPosition(DrawOriginPosition);
		DrawnComponent->SetEndPosition(DrawEndPosition);
		DrawnComponent->SetStartTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		DrawnComponent->SetEndTangent((DrawEndPosition - DrawOriginPosition).GetSafeNormal());
		
		float SingleScale = OwnersTreeInfos->StandardCellWidth;
		
		DrawnComponent->SetStartScale(FVector2D(SingleScale, SingleScale));
		DrawnComponent->SetEndScale(FVector2D(SingleScale, SingleScale));

		DrawnComponent->SetForwardAxis(OwnersTreeInfos->SplineMeshForwardAxis);
		DrawnComponent->SetSplineUpDir(OwnersTreeInfos->SplineMeshUpDir);
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
}
