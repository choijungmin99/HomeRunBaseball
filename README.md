# HomeRunBaseball

Unreal Engine 5.5 C++로 만든 데디케이티드 서버 기반 멀티플레이 숫자 야구 과제입니다. 프로젝트명은 **HomeRunBaseball**, C++ 클래스 접두사는 **HB**입니다. 과제 발제의 필수 기능을 구현했습니다.

## 실행 방법

1. `HomeRunBaseball.uproject`를 Unreal Engine **5.5**로 엽니다. 모듈 재빌드 안내가 나오면 빌드합니다. Visual Studio 2022의 C++ 게임 개발 도구가 필요합니다.
2. `Content/HomeRunBaseball/Maps/Baseball` 레벨을 엽니다. 기본 시작 레벨로도 지정했습니다.
3. 플레이 메뉴에서 `Play as Client`, 플레이어 수 `2`를 확인합니다.
4. `Edit → Editor Preferences → Level Editor → Play`에서 `Launch Separate Server`를 체크하고 `Run Under One Process`를 체크 해제합니다.
5. `New Editor Window (PIE)`로 실행합니다. 설정 선택 직후 실행될 수 있습니다.
6. 두 게임 창 맨 아래 입력창에 메시지나 숫자를 입력하고 Enter를 누릅니다.

서버 로그 창 1개와 플레이어 창 2개가 실행됩니다. 서버는 플레이에 참가하지 않고 정답·시도 횟수·결과를 관리합니다. 게임 창에는 접속 공지, 채팅/판정 메시지, 입력창이 보입니다.

## 게임 규칙

- 정답은 **1~9의 중복 없는 세 자리 숫자**입니다.
- 플레이어는 **각자 3번** 시도할 수 있습니다. 먼저 정답을 맞힌 사람이 승리합니다.
- 숫자와 자리가 같으면 **S**, 숫자는 있지만 자리가 다르면 **B**입니다. 일치하는 숫자가 없으면 **OUT**입니다.
- OUT도 유효한 시도이므로 기회 1회를 사용합니다.
- 모든 플레이어가 3회를 사용했는데 승자가 없으면 무승부입니다.
- 승리/무승부 직후 새 정답을 뽑고 모든 시도 횟수를 0으로 초기화합니다.
- 공지는 `New game started.`를 함께 표시하므로 이미 새 판이 시작됐음을 알 수 있습니다.
- 이름은 접속 순서대로 Player1, Player2 등을 부여합니다.
- 턴제와 타이머는 도전 기능이므로 구현 범위에 포함하지 않았습니다.

정답이 `386`이라면 `123 → 0S1B`, `167 → 0S1B`, `386 → 3S0B`입니다.

## 입력 검사

**입력 전체**가 정확히 세 자리인지 검사합니다. 뒤 세 글자를 잘라 사용하지 않습니다.

| 입력 | 처리 | 시도 횟수 |
| --- | --- | --- |
| `123` | 정상 숫자 판정 | 1 증가 |
| `12`, `1234` | 3자리 숫자를 입력하라는 안내 | 유지 |
| `112` | 중복되지 않은 숫자를 입력하라는 안내 | 유지 |
| `012`, `0a1`, `abc` | 1~9 숫자만 입력하라는 안내 | 유지 |
| `abc123` | 채팅으로 전달하고 길이 오류 안내; 123으로 판정하지 않음 | 유지 |
| 빈 입력 | 입력이 비었다는 안내 | 유지 |
| 기회 소진 후 유효 숫자 | 다음 게임을 기다리라는 안내 | 유지 |

기본 멀티플레이 채팅도 유지합니다. 문자가 포함된 일반 메시지는 다른 플레이어에게 전달하며, 입력자에게는 유효한 숫자 시도가 아니라는 안내도 표시합니다. 오류 안내 자체는 입력자에게만 전달합니다.

## 시도 횟수 표시

PlayerState의 Getter는 `Player1[현재 시도 횟수/최대 시도 횟수]`를 반환합니다. 서버가 유효한 입력을 판정하고 횟수를 증가시킨 다음 메시지를 만듭니다.

첫 시도는 `Player1[1/3]: 123 -> 0S1B`처럼 표시됩니다. 이후 `[2/3]`, `[3/3]`입니다. 승리/무승부 후에는 실제 상태가 `[0/3]`으로 초기화됩니다. 클라이언트가 보낸 횟수를 신뢰하지 않고 서버의 값을 사용합니다.

## 구현 구조

| 클래스 | 역할 |
| --- | --- |
| `AHBGameModeBase` | 서버 정답 생성, 입력 검사, S/B/OUT 판정, 횟수 증가, 승리/무승부/리셋 |
| `AHBPlayerState` | 플레이어 이름과 현재/최대 횟수 저장 및 Property Replication, Getter |
| `AHBPlayerController` | 내 UI 생성, 입력 원문을 Server RPC로 전송, Client RPC 수신, 공지 복제 |
| `AHBGameStateBase` | NetMulticast RPC로 접속 알림 전달 |
| `UHBChatInput` | Enter 이벤트 연결과 입력 전달 |
| `HomeRunBaseballFunctionLibrary` | 화면/로그 출력, NetMode·NetRole 확인을 위한 C++ 라이브러리 |
| `AHBPawn` | 강의 NetRole 학습용 클래스. 최종 Default Pawn Class는 None |

### 입력부터 결과까지

1. 내 클라이언트 위젯이 Enter 입력을 받습니다.
2. PlayerController가 **입력 원문만** Server RPC로 전송합니다.
3. 서버의 GameMode가 입력 전체를 검사합니다.
4. 오류라면 안내를 보내고 횟수를 유지합니다. 유효하다면 S/B/OUT을 계산하고 해당 PlayerState의 횟수만 1 증가시킵니다.
5. 서버가 이름·증가된 횟수·입력·판정을 묶어 각 컨트롤러의 Client RPC로 전달합니다.
6. GameMode가 승리 또는 무승부를 한 번 판정합니다.
7. 서버가 각 PlayerController의 NotificationText를 설정하면 소유 클라이언트로 복제되고, WBP의 Text 바인딩으로 표시됩니다.
8. 결과 결정 후 정답과 라운드 횟수를 초기화합니다.

PlayerController는 서버와 해당 소유 클라이언트에 존재하므로 채팅은 각 컨트롤러의 Client RPC로 전달합니다. GameState는 모든 클라이언트에 존재하므로 접속 알림의 NetMulticast RPC를 맡습니다.

### 연결된 에셋

- `BP_HBGameMode`: PlayerController = `BP_HBPlayerController`, PlayerState = `BP_HBPlayerState`, GameState = `BP_HBGameState`, DefaultPawn = None.
- `BP_HBPlayerController`: ChatInputWidgetClass = `WBP_ChatInput`, NotificationTextWidgetClass = `WBP_NotificationText`.
- `WBP_ChatInput`: 부모 `UHBChatInput`, 입력 위젯 이름 `EditableTextBox_ChatInput`.
- `WBP_NotificationText`: Text 바인딩으로 Owning Player의 NotificationText를 읽습니다.
- `Baseball` 레벨: GameMode Override = `BP_HBGameMode`.

## 승리 장면을 쉽게 확인하는 방법

발제의 디버깅 가이드에 따라 게임 시작/리셋 시 서버 로그에 다음 형식으로 현재 정답을 출력합니다.

```text
[HomeRunBaseball] Secret number: 386
```

1. 서버 로그 창에서 **가장 마지막** `Secret number:` 줄을 찾습니다.
2. 시도가 남아 있는 게임 창에서 그 숫자를 입력하고 Enter를 누릅니다.
3. `3S0B`, 승자 공지와 자동 리셋을 확인합니다.
4. 리셋하면 서버 로그에 새 정답이 출력됩니다. 이전 정답으로 다음 판을 테스트하지 마세요.

정답 로그는 에디터/Development 테스트용입니다. Shipping 빌드에서는 출력하지 않습니다. 정답은 클라이언트에 복제하지 않습니다.

## 필수 기능 체크리스트

- [x] 개인 프로젝트명과 CX가 아닌 C++ 접두사
- [x] 기본 멀티플레이 채팅
- [x] 서버에서 1~9 중복 없는 세 자리 난수 생성
- [x] 정확한 길이, 문자/0, 중복 숫자 검사와 재입력 안내
- [x] S/B/OUT 문자열 판정
- [x] PlayerState의 플레이어별 3회 관리 및 복제
- [x] 현재/최대 횟수 Getter와 증가 후 표시
- [x] 기회 소진 후 추가 숫자 시도 차단
- [x] 공지 위젯 및 승자/무승부 판정
- [x] 결과 후 새 정답·라운드 상태 초기화
- [x] C++ 라이브러리와 구현 과정 README

## 공부와 검증

초보자를 위한 설명은 [공부용 코드 해설](공부용_코드해설.md)에 정리했습니다. 각 C++ 파일에도 한국어 주석을 달았습니다. 검증 자료는 `검증자료` 폴더를 참고하세요.

시연 영상은 별도 서버와 실제 두 클라이언트의 입력·결과·공지 화면을 자동 테스트에서 캡처해 편집한 MP4입니다. 정답 입력은 서버 로그의 정답을 사용해 승리 기능을 확인합니다.
