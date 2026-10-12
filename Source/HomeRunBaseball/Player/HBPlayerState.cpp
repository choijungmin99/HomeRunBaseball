#include "Player/HBPlayerState.h"
#include "Net/UnrealNetwork.h"

AHBPlayerState::AHBPlayerState()
    : PlayerNameString(TEXT("None")), CurrentGuessCount(0), MaxGuessCount(3)
{
    bReplicates = true;
}

void AHBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // Replicated 선언 외에 실제로 복제할 속성도 여기에 등록합니다.
    DOREPLIFETIME(ThisClass, PlayerNameString);
    DOREPLIFETIME(ThisClass, CurrentGuessCount);
    DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString AHBPlayerState::GetPlayerInfoString()
{
    // 발제의 [현재 시도 횟수 / 최대 시도 횟수] 표시입니다.
    return FString::Printf(TEXT("%s[%d/%d]"), *PlayerNameString, CurrentGuessCount, MaxGuessCount);
}
