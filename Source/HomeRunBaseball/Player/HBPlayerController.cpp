#include "Player/HBPlayerController.h"
#include "UI/HBChatInput.h"
#include "Player/HBPlayerState.h"
#include "Game/HBGameModeBase.h"
#include "HomeRunBaseball.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AHBPlayerController::AHBPlayerController()
{
    bReplicates = true;
}

void AHBPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // 챕터 1-3: 서버의 다른 플레이어 컨트롤러에는 내 화면 UI를 만들지 않습니다.
    if (!IsLocalController())
    {
        return;
    }
    FInputModeUIOnly InputModeUIOnly;
    SetInputMode(InputModeUIOnly);
    bShowMouseCursor = true; // 초보 사용자도 입력창을 클릭할 수 있도록 마우스를 표시합니다.
    if (IsValid(ChatInputWidgetClass))
    {
        ChatInputWidgetInstance = CreateWidget<UHBChatInput>(this, ChatInputWidgetClass);
        if (IsValid(ChatInputWidgetInstance))
        {
            ChatInputWidgetInstance->AddToViewport();
        }
    }
    if (IsValid(NotificationTextWidgetClass))
    {
        NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
        if (IsValid(NotificationTextWidgetInstance))
        {
            NotificationTextWidgetInstance->AddToViewport();
        }
    }
}

void AHBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ThisClass, NotificationText);
}

void AHBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
    ChatMessageString = InChatMessageString;
    if (IsLocalController())
    {
        // 입력 원문만 전달합니다. 서버가 형식 검사와 판정 후 이름/횟수를 붙입니다.
        // 클라이언트가 보낸 이름이나 횟수를 서버가 신뢰하지 않게 하는 구조입니다.
        ServerRPCPrintChatMessageString(InChatMessageString);
    }
}

void AHBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
    // 저장 문자열이 아니라 RPC로 받은 매개변수를 출력해야 남의 채팅도 보입니다.
    // 챕터 1-4 마지막 예제처럼 NetMode 접두어 없이 실제 메시지를 출력합니다.
    HomeRunBaseballFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void AHBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    PrintChatMessageString(InChatMessageString);
}

void AHBPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
    // 챕터 3-2: 규칙은 서버에만 존재하는 GameMode에서 처리합니다.
    AHBGameModeBase* HBGM = Cast<AHBGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (IsValid(HBGM))
    {
        HBGM->PrintChatMessageString(this, InChatMessageString);
    }
}
