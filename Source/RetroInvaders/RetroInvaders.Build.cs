using UnrealBuildTool;

public class RetroInvaders : ModuleRules
{
    public RetroInvaders(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "AudioMixer"
        });
    }
}
