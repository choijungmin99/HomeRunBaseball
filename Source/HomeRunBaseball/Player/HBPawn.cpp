#include "Player/HBPawn.h"
#include "HomeRunBaseball.h"

void AHBPawn::BeginPlay()
{
    Super::BeginPlay();
    const FString CombinedString = FString::Printf(TEXT("HBPawn::BeginPlay() %s [%s]"),
        *HomeRunBaseballFunctionLibrary::GetNetModeString(this), *HomeRunBaseballFunctionLibrary::GetRoleString(this));
    HomeRunBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void AHBPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    const FString CombinedString = FString::Printf(TEXT("HBPawn::PossessedBy() %s [%s]"),
        *HomeRunBaseballFunctionLibrary::GetNetModeString(this), *HomeRunBaseballFunctionLibrary::GetRoleString(this));
    HomeRunBaseballFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
