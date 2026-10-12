#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

/** 챕터 1-4, 1-5의 디버깅 도구. 게임 규칙을 처리하는 클래스는 아닙니다. */
class HomeRunBaseballFunctionLibrary
{
public:
    static void MyPrintString(const AActor* InWorldContextActor, const FString& InString,
        float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
    {
        if (IsValid(GEngine) && IsValid(InWorldContextActor))
        {
            // 교재처럼 클라이언트/리슨 서버는 화면 출력, 그 외는 로그 출력합니다.
            if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)
            {
                GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
            }
        }
    }

    static FString GetNetModeString(const AActor* InWorldContextActor)
    {
        FString NetModeString = TEXT("None");
        if (IsValid(InWorldContextActor))
        {
            const ENetMode NetMode = InWorldContextActor->GetNetMode();
            NetModeString = NetMode == NM_Client ? TEXT("Client") :
                (NetMode == NM_Standalone ? TEXT("StandAlone") : TEXT("Server"));
        }
        return NetModeString;
    }

    static FString GetRoleString(const AActor* InActor)
    {
        FString RoleString = TEXT("None");
        if (IsValid(InActor))
        {
            // LocalRole은 현재 PC, RemoteRole은 원격 PC에서의 역할입니다.
            const FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetLocalRole());
            const FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetRemoteRole());
            RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
        }
        return RoleString;
    }
};
