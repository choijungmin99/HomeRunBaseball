#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HBGameStateBase.generated.h"

/** 챕터 2-2: 모든 클라이언트에 존재하므로 접속 알림 Multicast를 맡습니다. */
UCLASS()
class HOMERUNBASEBALL_API AHBGameStateBase : public AGameStateBase
{
    GENERATED_BODY()
public:
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));
};
