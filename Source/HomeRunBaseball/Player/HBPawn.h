#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HBPawn.generated.h"

/** 챕터 1-5의 NetRole 확인용 Pawn. 최종 게임의 Default Pawn Class는 None입니다. */
UCLASS()
class HOMERUNBASEBALL_API AHBPawn : public APawn
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
};
