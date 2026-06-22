#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RetroInvadersPlayerController.generated.h"

class USoundWaveProcedural;

struct FRetroInvader
{
    FVector2D Position = FVector2D::ZeroVector;
    bool bAlive = true;
    int32 Type = 0;
};

struct FRetroBullet
{
    FVector2D Position = FVector2D::ZeroVector;
    bool bEnemy = false;
};

struct FRetroBunker
{
    static constexpr int32 Columns = 14;
    static constexpr int32 Rows = 8;
    static constexpr float CellSize = 5.0f;

    FVector2D Position = FVector2D::ZeroVector;
    TArray<uint8> Cells;
};

UENUM()
enum class ERetroGameState : uint8
{
    Title,
    Playing,
    Victory,
    GameOver
};

UCLASS()
class RETROINVADERS_API ARetroInvadersPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARetroInvadersPlayerController();
    virtual void BeginPlay() override;
    virtual void PlayerTick(float DeltaTime) override;

    float PlayerX = 400.0f;
    int32 Score = 0;
    int32 HighScore = 0;
    int32 Lives = 3;
    int32 Wave = 1;
    bool bUfoActive = false;
    float UfoX = -60.0f;
    float UfoY = 72.0f;
    int32 LastUfoScore = 0;
    float UfoScoreTimer = 0.0f;
    TArray<FRetroInvader> Invaders;
    TArray<FRetroBullet> Bullets;
    TArray<FRetroBunker> Bunkers;
    ERetroGameState GameState = ERetroGameState::Playing;

    int32 GetAliveCount() const;

private:
    float PlayerFireCooldown = 0.0f;
    float EnemyDirection = 1.0f;
    float EnemyStepTimer = 0.0f;
    float EnemyShotTimer = 0.0f;
    float MarchTimer = 0.0f;
    int32 MarchNote = 0;
    bool bPreviousFire = false;
    bool bPreviousRestart = false;
    bool bPreviousQuit = false;
    bool bPreviousMouse = false;
    bool bPreviousFullscreen = false;
    float UfoSpawnTimer = 0.0f;
    float UfoSoundTimer = 0.0f;
    int32 UfoSoundNote = 0;

    UPROPERTY()
    TArray<TObjectPtr<USoundWaveProcedural>> GeneratedSounds;

    void ResetGame(bool bKeepScore = false);
    void BuildWave();
    void BuildBunkers();
    void FirePlayerBullet();
    void FireEnemyBullet();
    void UpdateBullets(float DeltaTime);
    void UpdateInvaders(float DeltaTime);
    void ResolveCollisions();
    bool DamageBunkerAt(const FVector2D& HitPosition, bool bEnemyBullet);
    void UpdateUfo(float DeltaTime);
    void StartPlayingFromTitle();
    void PlayTone(float StartFrequency, float EndFrequency, float Duration, float Volume, bool bNoise = false);
    void PlayShotSound();
    void PlayExplosionSound();
    void PlayMarchSound();
    void PlayEnemyShotSound();
    void PlayUfoSound();
};
