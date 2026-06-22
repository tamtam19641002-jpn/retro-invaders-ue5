#include "RetroInvadersHUD.h"
#include "RetroInvadersPlayerController.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

FVector2D ARetroInvadersHUD::P(float X, float Y) const
{
    return FVector2D(OffsetX + X * ScaleX, OffsetY + Y * ScaleY);
}

void ARetroInvadersHUD::Pixel(float X, float Y, float W, float H, const FLinearColor& Color)
{
    const FVector2D Pos = P(X, Y);
    DrawRect(Color, Pos.X, Pos.Y, W * ScaleX, H * ScaleY);
}

void ARetroInvadersHUD::DrawPlayer(float X, float Y)
{
    const FLinearColor Green(0.22f, 1.0f, 0.36f, 1.0f);
    Pixel(X - 5, Y - 16, 10, 6, Green);
    Pixel(X - 13, Y - 10, 26, 6, Green);
    Pixel(X - 25, Y - 4, 50, 10, Green);
    Pixel(X - 19, Y + 6, 38, 5, Green);
}

void ARetroInvadersHUD::DrawInvader(float X, float Y, int32 Type, int32 Frame)
{
    const FLinearColor C = Type == 2 ? FLinearColor(0.95f, 0.35f, 0.95f) :
                           Type == 1 ? FLinearColor(0.25f, 0.95f, 1.0f) :
                                       FLinearColor(0.45f, 1.0f, 0.35f);
    Pixel(X - 14, Y - 10, 28, 5, C);
    Pixel(X - 20, Y - 5, 40, 11, C);
    Pixel(X - 25, Y + 1, 10, 10, C);
    Pixel(X + 15, Y + 1, 10, 10, C);
    Pixel(X - 7, Y - 3, 4, 5, FLinearColor::Black);
    Pixel(X + 3, Y - 3, 4, 5, FLinearColor::Black);
    if (Frame == 0)
    {
        Pixel(X - 17, Y + 11, 7, 5, C);
        Pixel(X + 10, Y + 11, 7, 5, C);
    }
    else
    {
        Pixel(X - 25, Y + 11, 7, 5, C);
        Pixel(X + 18, Y + 11, 7, 5, C);
    }
}

void ARetroInvadersHUD::DrawUfo(float X, float Y)
{
    const FLinearColor Red(1.0f, 0.22f, 0.18f, 1.0f);
    const FLinearColor Amber(1.0f, 0.68f, 0.18f, 1.0f);
    Pixel(X - 10, Y - 12, 20, 5, Amber);
    Pixel(X - 20, Y - 7, 40, 6, Red);
    Pixel(X - 30, Y - 1, 60, 8, Red);
    Pixel(X - 20, Y + 7, 40, 5, Amber);
    Pixel(X - 14, Y + 12, 7, 4, Red);
    Pixel(X + 7, Y + 12, 7, 4, Red);
}

void ARetroInvadersHUD::DrawBunker(const FRetroBunker& Bunker)
{
    const FLinearColor Green(0.20f, 0.85f, 0.28f);
    const float Left = Bunker.Position.X - FRetroBunker::Columns * FRetroBunker::CellSize * 0.5f;
    const float Top = Bunker.Position.Y - FRetroBunker::Rows * FRetroBunker::CellSize * 0.5f;
    for (int32 Row = 0; Row < FRetroBunker::Rows; ++Row)
    {
        for (int32 Column = 0; Column < FRetroBunker::Columns; ++Column)
        {
            const int32 Index = Row * FRetroBunker::Columns + Column;
            if (Bunker.Cells.IsValidIndex(Index) && Bunker.Cells[Index] != 0)
            {
                Pixel(Left + Column * FRetroBunker::CellSize,
                      Top + Row * FRetroBunker::CellSize,
                      FRetroBunker::CellSize, FRetroBunker::CellSize, Green);
            }
        }
    }
}

void ARetroInvadersHUD::DrawCentered(const FString& Text, float Y, float TextScale, const FLinearColor& Color)
{
    UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
    float XL = 0.0f, YL = 0.0f;
    GetTextSize(Text, XL, YL, Font, TextScale);
    DrawText(Text, Color, Canvas->SizeX * 0.5f - XL * 0.5f, P(0, Y).Y, Font, TextScale, false);
}

void ARetroInvadersHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;
    ARetroInvadersPlayerController* Game = Cast<ARetroInvadersPlayerController>(GetOwningPlayerController());
    if (!Game) return;

    const float UniformScale = FMath::Min(Canvas->SizeX / 800.0f, Canvas->SizeY / 600.0f);
    ScaleX = UniformScale;
    ScaleY = UniformScale;
    OffsetX = (Canvas->SizeX - 800.0f * UniformScale) * 0.5f;
    OffsetY = (Canvas->SizeY - 600.0f * UniformScale) * 0.5f;

    DrawRect(FLinearColor::Black, 0, 0, Canvas->SizeX, Canvas->SizeY);
    Pixel(20, 36, 760, 2, FLinearColor(0.1f, 0.42f, 0.2f));
    Pixel(20, 565, 760, 2, FLinearColor(0.1f, 0.42f, 0.2f));

    if (Game->GameState == ERetroGameState::Title)
    {
        DrawCentered(TEXT("RETRO"), 105, 2.0f * UniformScale, FLinearColor(0.25f, 0.95f, 1.0f));
        DrawCentered(TEXT("INVADERS"), 165, 2.35f * UniformScale, FLinearColor(0.95f, 0.35f, 0.95f));

        DrawInvader(320, 285, 0, 0);
        DrawInvader(400, 285, 1, 1);
        DrawInvader(480, 285, 2, 0);

        Pixel(300, 345, 200, 55, FLinearColor(0.95f, 0.35f, 0.95f));
        Pixel(305, 350, 190, 45, FLinearColor(0.015f, 0.025f, 0.025f));
        DrawCentered(TEXT("START"), 354, 1.25f * UniformScale, FLinearColor::White);
        if ((static_cast<int32>(FPlatformTime::Seconds() * 2.0) & 1) == 0)
        {
            DrawCentered(TEXT("CLICK OR PRESS ENTER"), 408, 0.72f * UniformScale, FLinearColor(0.55f, 0.85f, 0.62f));
        }
        DrawCentered(TEXT("A/D OR ARROWS : MOVE"), 455, 0.74f * UniformScale, FLinearColor(0.35f, 1.0f, 0.45f));
        DrawCentered(TEXT("SPACE : FIRE   F11 : DISPLAY   ESC : QUIT"), 485, 0.68f * UniformScale, FLinearColor(0.35f, 1.0f, 0.45f));
        DrawCentered(TEXT("PROCEDURAL ARCADE SOUND"), 530, 0.62f * UniformScale, FLinearColor(0.55f, 0.70f, 0.62f));

        for (int32 Y = 45; Y < 565; Y += 4)
        {
            Pixel(20, static_cast<float>(Y), 760, 1, FLinearColor(0.0f, 0.0f, 0.0f, 0.17f));
        }
        return;
    }

    UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
    DrawText(FString::Printf(TEXT("SCORE  %06d"), Game->Score), FLinearColor(0.25f, 1.0f, 0.40f),
             P(32, 12).X, P(32, 12).Y, Font, 1.45f * UniformScale, false);
    DrawText(FString::Printf(TEXT("HI  %06d"), Game->HighScore), FLinearColor(0.95f, 0.35f, 0.95f),
             P(330, 12).X, P(330, 12).Y, Font, 1.45f * UniformScale, false);
    DrawText(FString::Printf(TEXT("LIVES  %d"), Game->Lives), FLinearColor(0.25f, 0.95f, 1.0f),
             P(650, 12).X, P(650, 12).Y, Font, 1.45f * UniformScale, false);

    const int32 AnimationFrame = (FPlatformTime::Seconds() * 5.0) >=
        FMath::FloorToDouble(FPlatformTime::Seconds() * 5.0) + 0.5 ? 1 :
        (static_cast<int32>(FPlatformTime::Seconds() * 5.0) & 1);
    for (const FRetroInvader& Invader : Game->Invaders)
    {
        if (Invader.bAlive) DrawInvader(Invader.Position.X, Invader.Position.Y, Invader.Type, AnimationFrame);
    }

    if (Game->bUfoActive)
    {
        DrawUfo(Game->UfoX, Game->UfoY);
    }
    if (Game->UfoScoreTimer > 0.0f)
    {
        DrawText(FString::Printf(TEXT("+%d"), Game->LastUfoScore), FLinearColor(1.0f, 0.68f, 0.18f),
                 P(Game->UfoX - 18.0f, Game->UfoY + 22.0f).X,
                 P(Game->UfoX - 18.0f, Game->UfoY + 22.0f).Y,
                 Font, 1.0f * UniformScale, false);
    }

    for (const FRetroBunker& Bunker : Game->Bunkers)
    {
        DrawBunker(Bunker);
    }
    DrawPlayer(Game->PlayerX, 545);

    for (const FRetroBullet& Bullet : Game->Bullets)
    {
        const FLinearColor C = Bullet.bEnemy ? FLinearColor(1.0f, 0.34f, 0.20f) : FLinearColor::White;
        Pixel(Bullet.Position.X - 2, Bullet.Position.Y - 7, 4, 14, C);
    }

    for (int32 Y = 45; Y < 565; Y += 4)
    {
        Pixel(20, static_cast<float>(Y), 760, 1, FLinearColor(0.0f, 0.0f, 0.0f, 0.17f));
    }

    if (Game->GameState == ERetroGameState::Victory)
    {
        Pixel(165, 245, 470, 110, FLinearColor(0.01f, 0.02f, 0.02f, 0.95f));
        DrawCentered(TEXT("WAVE CLEAR!"), 258, 1.65f * UniformScale, FLinearColor(0.25f, 1.0f, 0.40f));
        DrawCentered(TEXT("PRESS R OR ENTER"), 312, 0.85f * UniformScale, FLinearColor::White);
    }
    else if (Game->GameState == ERetroGameState::GameOver)
    {
        Pixel(165, 245, 470, 110, FLinearColor(0.01f, 0.02f, 0.02f, 0.95f));
        DrawCentered(TEXT("GAME OVER"), 258, 1.65f * UniformScale, FLinearColor(1.0f, 0.25f, 0.20f));
        DrawCentered(TEXT("PRESS R OR ENTER"), 312, 0.85f * UniformScale, FLinearColor::White);
    }

    DrawText(TEXT("A/D or ARROWS: MOVE   SPACE: FIRE   F11: DISPLAY   ESC: QUIT"), FLinearColor(0.45f, 0.75f, 0.52f),
             P(165, 574).X, P(165, 574).Y, Font, 0.82f * UniformScale, false);
}
