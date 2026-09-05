#include "UI/HUD/KernelHUD.h"

#include "CommonActivatableWidget.h"
#include "UI/BossWidget.h"
#include "Blueprint/UserWidget.h"
#include "Game/KernelGameState.h"

void AKernelHUD::BeginPlay()
{
	Super::BeginPlay();
	TryBindToGameState();
	
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;
	
	// 1. 에디터에서 보스 위젯 클래스를 제대로 넣었는지 확인
	if (BossWidgetClass)
	{
		// 2. 위젯을 메모리에 생성합니다.
		BossWidgetInstance = CreateWidget<UBossWidget>(GetWorld(), BossWidgetClass);
		
		if (BossWidgetInstance)
		{
			BossWidgetInstance->AddToViewport();
		}
	}
	
	if (GameplayLayoutClass)
	{
		GameplayLayout = CreateWidget(PC, GameplayLayoutClass);
		
		if (GameplayLayout)
		{
			GameplayLayout->AddToViewport(0);
		}
	}
}

void AKernelHUD::TryBindToGameState()
{
	AKernelGameState* GS = GetWorld()->GetGameState<AKernelGameState>();
	if (!GS)
	{
		// 클라 초기: GameState 복제 도착 전일 수 있음 (PlayerController 때 배운 그 패턴)
		GetWorldTimerManager().SetTimerForNextTick(this, &AKernelHUD::TryBindToGameState);
		return;
	}

	GS->OnGamePhaseChanged.AddUObject(this, &AKernelHUD::HandleGamePhase);
	HandleGamePhase(GS->GetGamePhase()); // 구독 전에 이미 전환됐을 수 있으니 현재값 즉시 처리
}

void AKernelHUD::HandleGamePhase(EKernelGamePhase NewPhase)
{
	switch (NewPhase)
	{
	case EKernelGamePhase::MapSelect:
		HideWidget(GameplayLayout);                                   /**  맵,캐릭터 선택 전에 Layout 전부 숨김처리. */
		ShowWidget(MapSelectWidgetInstance, MapSelectWidgetClass);    /** 선택한 레벨이 열리면 Layout 은 자동 복구됨  */
		break;

	case EKernelGamePhase::CharacterSelect:
		HideWidget(MapSelectWidgetInstance);                              // 맵 위젯 내림
		ShowWidget(CharacterSelectWidgetInstance, CharacterSelectWidgetClass);  // 캐릭터 위젯 올림
		break;

	case EKernelGamePhase::Departing:
		HideWidget(CharacterSelectWidgetInstance);                 // 출발 연출을 위해 UI 정리
		//TODO : 연출 게임 시작 로직 추가.
		break;

	default: break;
	}
}

/** 공통 헬퍼 — 생성 1회 후 재사용 */
void AKernelHUD::ShowWidget(TObjectPtr<UUserWidget>& Instance, TSubclassOf<UUserWidget> Class)
{
	if (!Instance && Class)
	{
		Instance = CreateWidget<UUserWidget>(GetOwningPlayerController(), Class);
	}
	if (!Instance) return;

	if (!Instance->IsInViewport()) Instance->AddToViewport(10);
	if (UCommonActivatableWidget* Act = Cast<UCommonActivatableWidget>(Instance))
	{
		Act->ActivateWidget(); // 입력 라우팅(Menu/NoCapture) 적용
	}
}

void AKernelHUD::HideWidget(TObjectPtr<UUserWidget>& Instance)
{
	if (!Instance) return;

	if (UCommonActivatableWidget* Act = Cast<UCommonActivatableWidget>(Instance))
	{
		Act->DeactivateWidget(); // 입력 반환 
	}
	Instance->RemoveFromParent();
}