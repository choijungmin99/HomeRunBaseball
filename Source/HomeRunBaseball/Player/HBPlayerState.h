#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HBPlayerState.generated.h"

/** 챕터 3-3: 서버가 정한 플레이어 이름과 시도 횟수를 클라이언트에 공유합니다. */
UCLASS()
class HOMERUNBASEBALL_API AHBPlayerState : public APlayerState
{
    GENERATED_BODY()
public:
    AHBPlayerState();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    FString GetPlayerInfoString();
public:
    UPROPERTY(Replicated)
    FString PlayerNameString;
    UPROPERTY(Replicated)
    int32 CurrentGuessCount;
    // 교재에서는 최대 횟수도 Replicated로 등록합니다. 한 라운드에 3번입니다.
    UPROPERTY(Replicated)
    int32 MaxGuessCount;
};
