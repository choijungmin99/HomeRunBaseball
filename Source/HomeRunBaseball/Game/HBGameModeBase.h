#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HBGameModeBase.generated.h"
class AHBPlayerController;

/** 서버에만 존재하는 심판. 정답, 판정, 횟수, 승리/무승부/리셋을 맡습니다. */
UCLASS()
class HOMERUNBASEBALL_API AHBGameModeBase : public AGameModeBase
{
    GENERATED_BODY()
public:
    virtual void OnPostLogin(AController* NewPlayer) override;
    virtual void BeginPlay() override;
    virtual void Logout(AController* Exiting) override;
    FString GenerateSecretNumber();
    bool IsGuessNumberString(const FString& InNumberString);
    // 빈 문자열이면 유효합니다. 실패하면 입력자에게 보여 줄 안내를 반환합니다.
    FString GetGuessValidationMessage(const FString& InNumberString);
    FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);
    void PrintChatMessageString(AHBPlayerController* InChattingPlayerController, const FString& InChatMessageString);
    void IncreaseGuessCount(AHBPlayerController* InChattingPlayerController);
    void ResetGame();
    void JudgeGame(AHBPlayerController* InChattingPlayerController, int InStrikeCount);
protected:
    // 정답은 서버 GameMode에만 보관하며 클라이언트로 복제하지 않습니다.
    FString SecretNumberString;
    // 퇴장 후 다시 접속해도 플레이어 이름이 겹치지 않도록 접속 번호를 누적합니다.
    int32 NextPlayerNumber = 1;
    // 접속한 컨트롤러들에 결과 공지를 전달하고 횟수를 초기화합니다.
    UPROPERTY()
    TArray<TObjectPtr<AHBPlayerController>> AllPlayerControllers;
};
