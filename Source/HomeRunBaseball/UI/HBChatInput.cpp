#include "UI/HBChatInput.h"
#include "Components/EditableTextBox.h"
#include "Player/HBPlayerController.h"

void UHBChatInput::NativeConstruct()
{
    Super::NativeConstruct();
    // 입력 확정 이벤트를 연결합니다. 이미 연결되었다면 중복 등록하지 않습니다.
    if (IsValid(EditableTextBox_ChatInput) &&
        !EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
    {
        EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
    }
}

void UHBChatInput::NativeDestruct()
{
    // 위젯이 제거되면 이벤트 연결도 해제합니다.
    if (IsValid(EditableTextBox_ChatInput) &&
        EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted))
    {
        EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
    }
    Super::NativeDestruct();
}

void UHBChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    // 포커스를 잃는 경우는 전송하지 않고 엔터를 눌렀을 때만 전송합니다.
    if (CommitMethod == ETextCommit::OnEnter)
    {
        AHBPlayerController* OwningHBPlayerController = Cast<AHBPlayerController>(GetOwningPlayer());
        if (IsValid(OwningHBPlayerController))
        {
            OwningHBPlayerController->SetChatMessageString(Text.ToString());
            EditableTextBox_ChatInput->SetText(FText());
        }
    }
}
