# Retro Invaders UE5

A small retro fixed-screen shooter built from scratch with Unreal Engine 5.5 and C++.

The game uses simple HUD primitives for its pixel-art presentation and generates all arcade sound effects at runtime. No external art or audio assets are required.

## Features

- 55-enemy moving formation
- Player and enemy projectiles
- Score, high score, and three lives
- Increasing movement tempo as enemies are defeated
- Procedurally generated shot, explosion, enemy-shot, and four-note march sounds
- CRT-style scanline overlay
- Windows gamepad-free keyboard controls

## Controls

| Key | Action |
| --- | --- |
| `A` / `D` or arrow keys | Move |
| `Space` | Fire |
| `R` or `Enter` | Restart after game over or victory |
| `Esc` | Quit |

## Requirements

- Unreal Engine 5.5
- Visual Studio 2022 Build Tools with the C++ workload
- Windows 10 or Windows 11

## Open In Unreal Engine

1. Clone this repository.
2. Right-click `RetroInvaders.uproject` and select **Generate Visual Studio project files**.
3. Open the project with Unreal Engine 5.5.
4. Build the `RetroInvadersEditor` target if prompted.
5. Press **Play**.

## Command-Line Build

```powershell
& 'C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat' `
  RetroInvadersEditor Win64 Development `
  '-Project=C:\path\to\RetroInvaders.uproject' -WaitMutex
```

## Notes

This is an original retro shooter inspired by the fixed-screen arcade genre. It does not include assets, audio, or code from the original Space Invaders game.
