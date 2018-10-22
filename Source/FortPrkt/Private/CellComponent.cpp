// Fill out your copyright notice in the Description page of Project Settings.

#include "CellComponent.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogCell, VeryVerbose, All);

// Sets default values for this component's properties
UCellComponent::UCellComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	NewCellClass = this->GetClass();
	StateString = TEXT("@@@");
	// ...
}


// Called when the game starts
void UCellComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCellComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<FString> UCellComponent::GetDivideSubstrings(FString InString)
{
	TArray<FString> OutArray = TArray<FString>();

	OutArray.Add(TEXT("@@"));
	OutArray.Add(TEXT("@@"));

	return OutArray;
}

bool UCellComponent::shouldDivideHorizontally()
{
	return false;
}

void UCellComponent::divideCell()
{
	if (shouldDivideHorizontally())
	{
		divideCellHorizontally();
	}
	else
	{
		divideCellVertically();
	}
}

void UCellComponent::divideCellHorizontally()
{
	TArray<UCellComponent*> OutArray = TArray<UCellComponent*>();

	const TArray<FString> Strings = GetDivideSubstrings(StateString);

	UE_LOG(LogCell, Verbose, TEXT("DividingCellHorz:, %s, StateString: %s #Substrings: %i"),
		*GetNameSafe(this),
		*StateString,
		Strings.Num());

	for (int32 Index = 0; Index < Strings.Num(); ++Index)
	{
		UCellComponent* NewCell = NULL;

		if (Index < Strings.Num() - 1) //for new cells
		{
			NewCell = NewObject<UCellComponent>(GetOwner(), NewCellClass, NAME_None, RF_NoFlags, nullptr, false, nullptr);
			NewCell->SetMobility(EComponentMobility::Movable);
			NewCell->SetupAttachment(this);
			NewCell->RegisterComponent();

			NewCell->StateString = Strings[Index];

			AttachedCellParent->AddNewCellChild(NewCell);

			OutArray.Add(NewCell);
		}
		else //for this cell that is going to remain a cell but might change it's type
		{
			NewCell = this;
			StateString = Strings[Index];
		}

		UE_LOG(LogCell, VeryVerbose, TEXT("DividingCellHoriz, %s, Created New Cell: %s with Nr: %i and StateString: %s, for parent: %s"),
			*GetNameSafe(this),
			*GetNameSafe(NewCell),
			Index,
			*(NewCell->StateString),
			*GetNameSafe(NewCell->AttachedCellParent)
		);
	}

}

void UCellComponent::divideCellVertically()
{
	TArray<UCellComponent*> OutArray = TArray<UCellComponent*>();
	UCellComponent* LastCell = NULL;

	const TArray<FString> Strings = GetDivideSubstrings(StateString);

	UE_LOG(LogCell, Verbose, TEXT("DividingCellVert: %s, StateString: %s #Substrings: %i"),
		*GetNameSafe(this),
		*StateString,
		Strings.Num());

	for (int32 Index = 0; Index < Strings.Num(); ++Index)
	{
		UCellComponent* NewCell = NULL;

		if (Index < Strings.Num() - 1) //for new cells
		{
			NewCell = NewObject<UCellComponent>(GetOwner(), NewCellClass, NAME_None, RF_NoFlags, nullptr, false, nullptr);
			NewCell->SetMobility(EComponentMobility::Movable);
			NewCell->SetupAttachment(this);
			NewCell->RegisterComponent();

			NewCell->StateString = Strings[Index];

			if (Index == 0) //if we are creating the first new cell, replace the reference to this in the parent to the new cell
			{
				int32 IndexInParent = -42;
				if (AttachedCellParent != NULL)
				{
					AttachedCellParent->AttachedCellChildren.Find(this, IndexInParent);
					AttachedCellParent->AddNewCellChild(NewCell, IndexInParent);
				}

			}

			else
			{
				UE_LOG(LogCell, VeryVerbose, TEXT("DividingCellVert:, %s, AddNewCellChild to LastCel: %s "),
					*GetNameSafe(LastCell));
				LastCell->AddNewCellChild(NewCell);
			}

			LastCell = NewCell;
			OutArray.Add(NewCell);
		}
		else //for this cell that is going to remain a cell but might change it's type
		{
			NewCell = this;

			//check this as we might be the only cell without any LastCell existing
			if (LastCell != nullptr)
			{
				LastCell->AddNewCellChild(this);
			}
			
			StateString = Strings[Index];
		}

		UE_LOG(LogCell, VeryVerbose, TEXT("DividingCellVert:, %s, Created New Cell: %s with Nr: %i and StateString: %s for parent: %s"),
			*GetNameSafe(this),
			*GetNameSafe(NewCell),
			Index,
			*(NewCell->StateString),
			*GetNameSafe(NewCell->AttachedCellParent)
		);
	}
}

bool UCellComponent::AddNewCellChild(UCellComponent* CellToAdd, int32 AtPosition /*= -42*/)
{
	if (AtPosition < 0)
	{
		AttachedCellChildren.Add(CellToAdd);
	}

	else
	{
		AttachedCellChildren[AtPosition] = CellToAdd;
	}

	CellToAdd->AttachedCellParent = this;
	
	//do we need this? I think not
	// CellToAdd->SetupAttachment(this);

	return true;
}

void UCellComponent::drawCellRecursively()
{
	//do actual visuals here, before recursive call!
		//no visuals in base implementation

	UE_LOG(LogCell, VeryVerbose, TEXT("Drawing children of cell:, %s, NrChildren: %i"),
		*GetNameSafe(this),
		AttachedCellChildren.Num());

	//call the children
	for (UCellComponent* c : AttachedCellChildren)
	{
		c->drawCellRecursively();
	}
}

TArray<FString> UCellComponent::GetMapDivideStrings(FString InString, TMap<FString, FString>* ReplacementMap, FString Separator)
{
	const FString* rawValueString = ReplacementMap->Find(InString); //find value in our TMap

	TArray<FString> OutArray = TArray<FString>();

	if (rawValueString != nullptr)
	{
		rawValueString->ParseIntoArray(OutArray, *Separator, true);
	}
	
	else
	{
		OutArray.Add(TEXT("@"));
	}

	return OutArray;
}

void UCellComponent::InitWithString(FString InString)
{

}

void UCellComponent::InitFromStateString()
{
	InitWithString(StateString);
}
