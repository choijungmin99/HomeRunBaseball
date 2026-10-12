#include "Game/HBGameModeBase.h"
#include "Game/HBGameStateBase.h"
#include "Player/HBPlayerController.h"
#include "Player/HBPlayerState.h"
#include "EngineUtils.h"

void AHBGameModeBase::OnPostLogin(AController* NewPlayer)
{
    Super::OnPostLogin(NewPlayer);
    // 챕터 3-3, 3-4: 접속 순서대로 이름을 정하고 접속 공지를 보냅니다.
    AHBPlayerController* HBPlayerController = Cast<AHBPlayerController>(NewPlayer);
    if (IsValid(HBPlayerController))
    {
        HBPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
        AllPlayerControllers.Add(HBPlayerController);
        AHBPlayerState* HBPS = HBPlayerController->GetPlayerState<AHBPlayerState>();
        if (IsValid(HBPS))
        {
            HBPS->PlayerNameString = TEXT("Player") + FString::FromInt(NextPlayerNumber++);
            AHBGameStateBase* HBGameStateBase = GetGameState<AHBGameStateBase>();
            if (IsValid(HBGameStateBase))
            {
                HBGameStateBase->MulticastRPCBroadcastLoginMessage(HBPS->PlayerNameString);
            }
        }
    }
}

void AHBGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    SecretNumberString = GenerateSecretNumber();
#if !UE_BUILD_SHIPPING
    // 발제의 테스트/디버깅 가이드: 서버 창에서 이 숫자를 읽어 승리 동작을 확인합니다.
    // 클라이언트로 정답을 복제하거나 화면에 표시하지 않습니다.
    UE_LOG(LogTemp, Display, TEXT("[HomeRunBaseball] Secret number: %s"), *SecretNumberString);
#endif
}

FString AHBGameModeBase::GenerateSecretNumber()
{
    // 챕터 3-2: 1~9에서 뽑은 숫자를 제거해 중복 없는 세 자리를 만듭니다.
    TArray<int32> Numbers;
    for (int32 i = 1; i <= 9; ++i)
    {
        Numbers.Add(i);
    }
    FMath::RandInit(static_cast<int32>(FDateTime::Now().GetTicks()));
    Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });
    FString Result;
    for (int32 i = 0; i < 3; ++i)
    {
        const int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
        Result.Append(FString::FromInt(Numbers[Index]));
        Numbers.RemoveAt(Index);
    }
    return Result;
}

FString AHBGameModeBase::GetGuessValidationMessage(const FString& InNumberString)
{
    // 발제 기준: 마지막 세 글자가 아니라 입력 전체를 검사합니다.
    if (InNumberString.IsEmpty())
    {
        return TEXT("입력 내용이 비어 있습니다. 3자리 숫자를 입력해주세요.");
    }
    if (InNumberString.Len() != 3)
    {
        return TEXT("3자리 숫자를 입력해주세요. 다시 입력하세요.");
    }
    TSet<TCHAR> UniqueDigits;
    for (TCHAR Digit : InNumberString)
    {
        if (Digit < '1' || Digit > '9')
        {
            return TEXT("1~9 사이의 숫자만 입력해주세요. 문자와 0은 사용할 수 없습니다.");
        }
        if (UniqueDigits.Contains(Digit))
        {
            return TEXT("중복되지 않은 숫자를 입력해주세요. 다시 입력하세요.");
        }
        UniqueDigits.Add(Digit);
    }
    return FString();
}

bool AHBGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
    return GetGuessValidationMessage(InNumberString).IsEmpty();
}

FString AHBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
    int32 StrikeCount = 0, BallCount = 0;
    for (int32 i = 0; i < 3; ++i)
    {
        if (InSecretNumberString[i] == InGuessNumberString[i])
        {
            // 숫자와 자리 모두 맞으면 스트라이크입니다.
            ++StrikeCount;
        }
        else
        {
            // 자리는 틀려도 정답 안에 숫자가 있으면 볼입니다.
            const FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
            if (InSecretNumberString.Contains(PlayerGuessChar))
            {
                ++BallCount;
            }
        }
    }
    if (StrikeCount == 0 && BallCount == 0)
    {
        return TEXT("OUT");
    }
    return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AHBGameModeBase::PrintChatMessageString(AHBPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
    if (!IsValid(InChattingPlayerController))
    {
        return;
    }
    AHBPlayerState* HBPS = InChattingPlayerController->GetPlayerState<AHBPlayerState>();
    if (!IsValid(HBPS))
    {
        return;
    }

    const FString ValidationMessage = GetGuessValidationMessage(InChatMessageString);
    if (!ValidationMessage.IsEmpty())
    {
        // 문자가 포함된 입력은 기본 채팅 기능으로 다른 플레이어에게도 전달합니다.
        // 숫자 야구의 유효한 시도로 인정하지 않으며, 입력자에게 재입력 안내도 보냅니다.
        // 예: hello는 채팅, abc123은 채팅 + 입력 오류입니다. 123으로 잘라 판정하지 않습니다.
        bool bContainsChatCharacter = false;
        for (TCHAR Character : InChatMessageString)
        {
            if (Character < '0' || Character > '9')
            {
                bContainsChatCharacter = true;
                break;
            }
        }
        if (bContainsChatCharacter && !InChatMessageString.TrimStartAndEnd().IsEmpty())
        {
            const FString ChatMessage = HBPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;
            for (TActorIterator<AHBPlayerController> It(GetWorld()); It; ++It)
            {
                if (IsValid(*It))
                {
                    It->ClientRPCPrintChatMessageString(ChatMessage);
                }
            }
        }
        // 오류 안내는 입력한 사람에게만 보냅니다. CurrentGuessCount는 바꾸지 않습니다.
        InChattingPlayerController->ClientRPCPrintChatMessageString(
            HBPS->GetPlayerInfoString() + TEXT(": \"") + InChatMessageString + TEXT("\" -> ") + ValidationMessage);
        return;
    }
    if (HBPS->CurrentGuessCount >= HBPS->MaxGuessCount)
    {
        InChattingPlayerController->ClientRPCPrintChatMessageString(
            HBPS->GetPlayerInfoString() + TEXT(": 기회를 모두 사용했습니다. 다음 게임을 기다려주세요."));
        return;
    }

    const FString JudgeResultString = JudgeResult(SecretNumberString, InChatMessageString);
    IncreaseGuessCount(InChattingPlayerController);
    // 서버의 증가된 값을 붙여 첫 시도부터 Player1[1/3]으로 표시합니다.
    // 클라이언트의 복제 도착 시점에 의존하지 않으므로 연속 입력에도 횟수가 정확합니다.
    const FString CombinedMessageString = HBPS->GetPlayerInfoString() + TEXT(": ")
        + InChatMessageString + TEXT(" -> ") + JudgeResultString;
    for (TActorIterator<AHBPlayerController> It(GetWorld()); It; ++It)
    {
        if (IsValid(*It))
        {
            It->ClientRPCPrintChatMessageString(CombinedMessageString);
        }
    }
    const int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
    // 결과를 모든 사람에게 전달한 뒤 한 번만 승리/무승부를 판정합니다.
    JudgeGame(InChattingPlayerController, StrikeCount);
}

void AHBGameModeBase::Logout(AController* Exiting)
{
    // 접속을 종료한 사람이 무승부 검사의 대상으로 남아 게임이 끝나지 않는 것을 막습니다.
    AllPlayerControllers.Remove(Cast<AHBPlayerController>(Exiting));
    Super::Logout(Exiting);
}

void AHBGameModeBase::IncreaseGuessCount(AHBPlayerController* InChattingPlayerController)
{
    AHBPlayerState* HBPS = InChattingPlayerController->GetPlayerState<AHBPlayerState>();
    if (IsValid(HBPS))
    {
        ++HBPS->CurrentGuessCount;
    }
}

void AHBGameModeBase::ResetGame()
{
    // 새 정답과 0회로 즉시 다음 게임을 시작합니다. 결과 공지는 남깁니다.
    SecretNumberString = GenerateSecretNumber();
#if !UE_BUILD_SHIPPING
    // 발제의 테스트/디버깅 가이드: 서버 창에서 이 숫자를 읽어 승리 동작을 확인합니다.
    // 클라이언트로 정답을 복제하거나 화면에 표시하지 않습니다.
    UE_LOG(LogTemp, Display, TEXT("[HomeRunBaseball] Secret number: %s"), *SecretNumberString);
#endif
    for (const auto& HBPlayerController : AllPlayerControllers)
    {
        if (!IsValid(HBPlayerController))
        {
            continue;
        }
        AHBPlayerState* HBPS = HBPlayerController->GetPlayerState<AHBPlayerState>();
        if (IsValid(HBPS))
        {
            HBPS->CurrentGuessCount = 0;
        }
    }
}

void AHBGameModeBase::JudgeGame(AHBPlayerController* InChattingPlayerController, int InStrikeCount)
{
    if (InStrikeCount == 3)
    {
        AHBPlayerState* HBPS = InChattingPlayerController->GetPlayerState<AHBPlayerState>();
        if (IsValid(HBPS))
        {
            // 교재처럼 모두에게 승자 이름을 알립니다. 나머지는 해당 라운드 패배입니다.
            const FString CombinedMessageString = HBPS->PlayerNameString + TEXT(" has won the game. New game started.");
            for (const auto& HBPlayerController : AllPlayerControllers)
            {
                if (IsValid(HBPlayerController))
                {
                    HBPlayerController->NotificationText = FText::FromString(CombinedMessageString);
                }
            }
            // [교재 최소 보정] 모든 공지 설정 후 라운드당 한 번만 초기화합니다.
            ResetGame();
        }
    }
    else
    {
        bool bIsDraw = true;
        for (const auto& HBPlayerController : AllPlayerControllers)
        {
            if (!IsValid(HBPlayerController))
            {
                continue;
            }
            AHBPlayerState* HBPS = HBPlayerController->GetPlayerState<AHBPlayerState>();
            if (IsValid(HBPS) && HBPS->CurrentGuessCount < HBPS->MaxGuessCount)
            {
                // 한 명이라도 남은 시도가 있으면 게임은 계속됩니다.
                bIsDraw = false;
                break;
            }
        }
        if (bIsDraw)
        {
            for (const auto& HBPlayerController : AllPlayerControllers)
            {
                if (IsValid(HBPlayerController))
                {
                    HBPlayerController->NotificationText = FText::FromString(TEXT("Draw... New game started."));
                }
            }
            ResetGame();
        }
    }
}
