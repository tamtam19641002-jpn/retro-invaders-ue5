#include "RetroInvadersPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"

namespace RetroRules
{
    constexpr float Width = 800.0f;
    constexpr float Height = 600.0f;
    constexpr float PlayerY = 545.0f;
}

ARetroInvadersPlayerController::ARetroInvadersPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    bShowMouseCursor = false;
}

void ARetroInvadersPlayerController::BeginPlay()
{
    Super::BeginPlay();
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    ResetGame();
}

void ARetroInvadersPlayerController::ResetGame(bool bKeepScore)
{
    if (!bKeepScore)
    {
        Score = 0;
        Lives = 3;
        Wave = 1;
    }
    PlayerX = 400.0f;
    GameState = ERetroGameState::Playing;
    Bullets.Reset();
    EnemyDirection = 1.0f;
    EnemyStepTimer = 0.0f;
    EnemyShotTimer = 0.5f;
    MarchTimer = 0.0f;
    BuildWave();
}

void ARetroInvadersPlayerController::BuildWave()
{
    Invaders.Reset();
    const int32 Columns = 11;
    const int32 Rows = 5;
    for (int32 Row = 0; Row < Rows; ++Row)
    {
        for (int32 Column = 0; Column < Columns; ++Column)
        {
            FRetroInvader Invader;
            Invader.Position = FVector2D(145.0f + Column * 51.0f, 105.0f + Row * 42.0f);
            Invader.Type = Row == 0 ? 2 : Row < 3 ? 1 : 0;
            Invaders.Add(Invader);
        }
    }
}

int32 ARetroInvadersPlayerController::GetAliveCount() const
{
    int32 Count = 0;
    for (const FRetroInvader& Invader : Invaders)
    {
        if (Invader.bAlive) ++Count;
    }
    return Count;
}

void ARetroInvadersPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    const bool bQuit = IsInputKeyDown(EKeys::Escape);
    if (bQuit && !bPreviousQuit)
    {
        UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
    }
    bPreviousQuit = bQuit;

    const bool bRestart = IsInputKeyDown(EKeys::R) || IsInputKeyDown(EKeys::Enter);
    if (GameState != ERetroGameState::Playing)
    {
        if (bRestart && !bPreviousRestart) ResetGame();
        bPreviousRestart = bRestart;
        return;
    }

    float Move = 0.0f;
    if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left)) Move -= 1.0f;
    if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) Move += 1.0f;
    PlayerX = FMath::Clamp(PlayerX + Move * 310.0f * DeltaTime, 45.0f, 755.0f);

    PlayerFireCooldown = FMath::Max(0.0f, PlayerFireCooldown - DeltaTime);
    const bool bFire = IsInputKeyDown(EKeys::SpaceBar);
    if (bFire && !bPreviousFire && PlayerFireCooldown <= 0.0f)
    {
        FirePlayerBullet();
    }
    bPreviousFire = bFire;

    UpdateBullets(DeltaTime);
    UpdateInvaders(DeltaTime);
    ResolveCollisions();
}

void ARetroInvadersPlayerController::FirePlayerBullet()
{
    FRetroBullet Bullet;
    Bullet.Position = FVector2D(PlayerX, RetroRules::PlayerY - 18.0f);
    Bullet.bEnemy = false;
    Bullets.Add(Bullet);
    PlayerFireCooldown = 0.20f;
    PlayShotSound();
}

void ARetroInvadersPlayerController::FireEnemyBullet()
{
    TArray<int32> BottomInvaders;
    for (int32 Column = 0; Column < 11; ++Column)
    {
        int32 Candidate = INDEX_NONE;
        float LowestY = -1.0f;
        for (int32 Index = 0; Index < Invaders.Num(); ++Index)
        {
            if (Invaders[Index].bAlive && Index % 11 == Column && Invaders[Index].Position.Y > LowestY)
            {
                Candidate = Index;
                LowestY = Invaders[Index].Position.Y;
            }
        }
        if (Candidate != INDEX_NONE) BottomInvaders.Add(Candidate);
    }

    if (BottomInvaders.Num() > 0)
    {
        const int32 Shooter = BottomInvaders[FMath::RandRange(0, BottomInvaders.Num() - 1)];
        FRetroBullet Bullet;
        Bullet.Position = Invaders[Shooter].Position + FVector2D(0.0f, 18.0f);
        Bullet.bEnemy = true;
        Bullets.Add(Bullet);
        PlayEnemyShotSound();
    }
}

void ARetroInvadersPlayerController::UpdateBullets(float DeltaTime)
{
    for (int32 Index = Bullets.Num() - 1; Index >= 0; --Index)
    {
        Bullets[Index].Position.Y += (Bullets[Index].bEnemy ? 225.0f : -470.0f) * DeltaTime;
        if (Bullets[Index].Position.Y < 48.0f || Bullets[Index].Position.Y > RetroRules::Height - 28.0f)
        {
            Bullets.RemoveAtSwap(Index);
        }
    }
}

void ARetroInvadersPlayerController::UpdateInvaders(float DeltaTime)
{
    const int32 Alive = GetAliveCount();
    if (Alive <= 0)
    {
        HighScore = FMath::Max(HighScore, Score);
        GameState = ERetroGameState::Victory;
        PlayExplosionSound();
        return;
    }

    const float SpeedFactor = 1.0f + (55 - Alive) * 0.045f + (Wave - 1) * 0.18f;
    const float MarchInterval = FMath::Max(0.075f, 0.52f / SpeedFactor);
    MarchTimer += DeltaTime;
    if (MarchTimer >= MarchInterval)
    {
        MarchTimer = 0.0f;
        PlayMarchSound();
    }

    bool bHitEdge = false;
    const float DeltaX = EnemyDirection * 34.0f * SpeedFactor * DeltaTime;
    for (FRetroInvader& Invader : Invaders)
    {
        if (!Invader.bAlive) continue;
        if (Invader.Position.X + DeltaX < 38.0f || Invader.Position.X + DeltaX > 762.0f)
        {
            bHitEdge = true;
            break;
        }
    }

    if (bHitEdge)
    {
        EnemyDirection *= -1.0f;
        for (FRetroInvader& Invader : Invaders)
        {
            if (Invader.bAlive) Invader.Position.Y += 15.0f;
        }
    }
    else
    {
        for (FRetroInvader& Invader : Invaders)
        {
            if (Invader.bAlive) Invader.Position.X += DeltaX;
        }
    }

    EnemyShotTimer -= DeltaTime;
    if (EnemyShotTimer <= 0.0f)
    {
        FireEnemyBullet();
        EnemyShotTimer = FMath::FRandRange(0.45f, 1.15f) / FMath::Sqrt(SpeedFactor);
    }

    for (const FRetroInvader& Invader : Invaders)
    {
        if (Invader.bAlive && Invader.Position.Y > 505.0f)
        {
            Lives = 0;
            HighScore = FMath::Max(HighScore, Score);
            GameState = ERetroGameState::GameOver;
            PlayExplosionSound();
            return;
        }
    }
}

void ARetroInvadersPlayerController::ResolveCollisions()
{
    for (int32 BulletIndex = Bullets.Num() - 1; BulletIndex >= 0; --BulletIndex)
    {
        FRetroBullet& Bullet = Bullets[BulletIndex];
        if (Bullet.bEnemy)
        {
            if (FMath::Abs(Bullet.Position.X - PlayerX) < 25.0f &&
                FMath::Abs(Bullet.Position.Y - RetroRules::PlayerY) < 18.0f)
            {
                Bullets.RemoveAtSwap(BulletIndex);
                --Lives;
                PlayExplosionSound();
                if (Lives <= 0)
                {
                    HighScore = FMath::Max(HighScore, Score);
                    GameState = ERetroGameState::GameOver;
                }
            }
            continue;
        }

        bool bDestroyed = false;
        for (FRetroInvader& Invader : Invaders)
        {
            if (Invader.bAlive && FMath::Abs(Bullet.Position.X - Invader.Position.X) < 20.0f &&
                FMath::Abs(Bullet.Position.Y - Invader.Position.Y) < 15.0f)
            {
                Invader.bAlive = false;
                Score += Invader.Type == 2 ? 30 : Invader.Type == 1 ? 20 : 10;
                HighScore = FMath::Max(HighScore, Score);
                PlayExplosionSound();
                bDestroyed = true;
                break;
            }
        }
        if (bDestroyed) Bullets.RemoveAtSwap(BulletIndex);
    }
}

void ARetroInvadersPlayerController::PlayTone(float StartFrequency, float EndFrequency, float Duration, float Volume, bool bNoise)
{
    constexpr int32 SampleRate = 22050;
    const int32 SampleCount = FMath::Max(1, FMath::RoundToInt(Duration * SampleRate));
    TArray<int16> Samples;
    Samples.SetNumUninitialized(SampleCount);
    uint32 NoiseState = 0x1234567u + FPlatformTime::Cycles();
    float Phase = 0.0f;

    for (int32 Index = 0; Index < SampleCount; ++Index)
    {
        const float T = static_cast<float>(Index) / static_cast<float>(SampleCount);
        const float Envelope = FMath::Square(1.0f - T);
        float Value = 0.0f;
        if (bNoise)
        {
            NoiseState = 1664525u * NoiseState + 1013904223u;
            Value = (static_cast<float>((NoiseState >> 16) & 0xFFFF) / 32767.5f) - 1.0f;
            Value *= 0.75f + 0.25f * FMath::Sin(T * 90.0f);
        }
        else
        {
            const float Frequency = FMath::Lerp(StartFrequency, EndFrequency, T);
            Phase += Frequency / SampleRate;
            Phase -= FMath::FloorToFloat(Phase);
            Value = Phase < 0.5f ? 1.0f : -1.0f;
        }
        Samples[Index] = static_cast<int16>(FMath::Clamp(Value * Envelope * Volume, -1.0f, 1.0f) * 32767.0f);
    }

    USoundWaveProcedural* WaveSound = NewObject<USoundWaveProcedural>(this);
    WaveSound->SetSampleRate(SampleRate);
    WaveSound->NumChannels = 1;
    WaveSound->Duration = Duration;
    WaveSound->SoundGroup = SOUNDGROUP_Effects;
    WaveSound->bLooping = false;
    WaveSound->QueueAudio(reinterpret_cast<const uint8*>(Samples.GetData()), Samples.Num() * sizeof(int16));
    GeneratedSounds.Add(WaveSound);
    if (GeneratedSounds.Num() > 24) GeneratedSounds.RemoveAt(0, GeneratedSounds.Num() - 24);
    UGameplayStatics::PlaySound2D(this, WaveSound, 1.0f);
}

void ARetroInvadersPlayerController::PlayShotSound()
{
    PlayTone(980.0f, 130.0f, 0.13f, 0.18f);
}

void ARetroInvadersPlayerController::PlayExplosionSound()
{
    PlayTone(0.0f, 0.0f, 0.30f, 0.25f, true);
}

void ARetroInvadersPlayerController::PlayMarchSound()
{
    const float Notes[4] = { 64.0f, 80.0f, 72.0f, 96.0f };
    PlayTone(Notes[MarchNote], Notes[MarchNote], 0.075f, 0.11f);
    MarchNote = (MarchNote + 1) % 4;
}

void ARetroInvadersPlayerController::PlayEnemyShotSound()
{
    PlayTone(240.0f, 520.0f, 0.16f, 0.10f);
}
