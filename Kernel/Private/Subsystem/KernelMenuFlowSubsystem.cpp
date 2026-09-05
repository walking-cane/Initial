
#include "Subsystem/KernelMenuFlowSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKernelMenuFlowSubsystem::HostGame()
{
	const FName MapName(*GameMapPath.GetLongPackageName());
	if (MapName.IsNone()) { UE_LOG(LogTemp, Error, TEXT("[MenuFlow] GameMapPath 무효")); return; }
	UGameplayStatics::OpenLevel(this, MapName, true, TEXT("listen"));
}
 
void UKernelMenuFlowSubsystem::FindGames()
{
	UE_LOG(LogTemp, Warning, TEXT("[MenuFlow] FindGames — 4단계(Steam 세션)에서 구현"));
}