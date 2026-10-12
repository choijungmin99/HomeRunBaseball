#include "Game/HBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HBPlayerController.h"

void AHBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
    // 교재는 데디케이티드 서버 환경이므로 서버에서는 화면 출력을 하지 않습니다.
    if (!HasAuthority())
    {
        AHBPlayerController* HBPC = Cast<AHBPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (IsValid(HBPC))
        {
            HBPC->PrintChatMessageString(InNameString + TEXT(" has joined the game."));
        }
    }
}
