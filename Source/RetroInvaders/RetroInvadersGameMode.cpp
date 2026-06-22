#include "RetroInvadersGameMode.h"
#include "RetroInvadersHUD.h"
#include "RetroInvadersPlayerController.h"

ARetroInvadersGameMode::ARetroInvadersGameMode()
{
    HUDClass = ARetroInvadersHUD::StaticClass();
    PlayerControllerClass = ARetroInvadersPlayerController::StaticClass();
    DefaultPawnClass = nullptr;
}
