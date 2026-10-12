#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HBPlayerController.generated.h"
class UHBChatInput;
class UUserWidget;

/** 내 PC의 입력을 서버로 전달하고 서버가 보낸 채팅과 공지를 보여 줍니다. */
UCLASS()
class HOMERUNBASEBALL_API AHBPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    AHBPlayerController();
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    void SetChatMessageString(const FString& InChatMessageString);
    void PrintChatMessageString(const FString& InChatMessageString);

    // 챕터 2-2: 서버가 호출하면 이 컨트롤러를 소유한 클라이언트에서 실행됩니다.
    UFUNCTION(Client, Reliable)
    void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
    // 내 클라이언트가 호출하면 서버에서 실행됩니다. Reliable은 전달을 재시도합니다.
    UFUNCTION(Server, Reliable)
    void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
    // 교재처럼 BP_PlayerController의 Class Defaults에서 WBP를 지정합니다.
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UHBChatInput> ChatInputWidgetClass;
    UPROPERTY()
    TObjectPtr<UHBChatInput> ChatInputWidgetInstance;
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> NotificationTextWidgetClass;
    UPROPERTY()
    TObjectPtr<UUserWidget> NotificationTextWidgetInstance;
    FString ChatMessageString;

public:
    // 챕터 3-4: 서버가 바꾸면 소유한 클라이언트로 복제됩니다.
    // WBP_NotificationText의 Text 바인딩이 이 값을 읽습니다.
    UPROPERTY(Replicated, BlueprintReadOnly)
    FText NotificationText;
};
