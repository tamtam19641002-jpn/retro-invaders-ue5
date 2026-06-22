#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RetroInvadersHUD.generated.h"

class ARetroInvadersPlayerController;
struct FRetroBunker;

UCLASS()
class RETROINVADERS_API ARetroInvadersHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    float ScaleX = 1.0f;
    float ScaleY = 1.0f;
    float OffsetX = 0.0f;
    float OffsetY = 0.0f;

    FVector2D P(float X, float Y) const;
    void Pixel(float X, float Y, float W, float H, const FLinearColor& Color);
    void DrawPlayer(float X, float Y);
    void DrawInvader(float X, float Y, int32 Type, int32 Frame);
    void DrawUfo(float X, float Y);
    void DrawBunker(const FRetroBunker& Bunker);
    void DrawCentered(const FString& Text, float Y, float Scale, const FLinearColor& Color);
};
