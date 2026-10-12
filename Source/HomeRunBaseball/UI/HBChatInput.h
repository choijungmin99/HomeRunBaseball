#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "HBChatInput.generated.h"
class UEditableTextBox;
/** 챕터 1-1: WBP_ChatInput의 부모. 엔터 입력을 컨트롤러에 전달합니다. */
UCLASS()
class HOMERUNBASEBALL_API UHBChatInput : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
protected:
    // AddDynamic으로 연결할 함수에는 UFUNCTION이 필요합니다.
    UFUNCTION()
    void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
public:
    // BindWidget: WBP 안의 같은 이름인 EditableTextBox를 이 변수에 연결합니다.
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
