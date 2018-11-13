#include "FortPrkt.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "TreeCellComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogFortPAutomationTests, Log, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFortPDummyTest, "Praktikum.General.DummyTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFortPTestTreeAngles, "Praktikum.Trees.TestTreeSubstance", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

UWorld* GetWorld()
{
	if (GEngine)
	{
		if (FWorldContext* World = GEngine->GetWorldContextFromPIEInstance(0))
		{
			return World->World();
		}
	}
	return nullptr;
}

bool FFortPDummyTest::RunTest(const FString& Parameters)
{
    //return true to see if tests in this file run at all
	return false;
}

bool FFortPTestTreeAngles::RunTest(const FString& Parameters)
{
	//return true to see if tests in this file run at all
	//GetWorld()->GetWorld()->SpawnActor<AActor>(FVector::ZeroVector, FRotator::ZeroRotator);

	return false;
}

/*
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMCDummyTest, "MountainClimbing.Movement.Working", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMCDummyTest::RunTest(const FString& Parameters)
{
	//return true to see if tests in this file run at all
	return true;
}
*/
