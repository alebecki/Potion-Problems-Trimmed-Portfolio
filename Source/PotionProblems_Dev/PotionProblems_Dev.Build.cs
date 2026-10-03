// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PotionProblems_Dev : ModuleRules
{

    public PotionProblems_Dev(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D", "UMG", "PaperZD", "ProceduralMeshComponent", "AkAudio"});
        // These modules are for online subsystem EOS (We can remove steam later if we need to)
        PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "VoiceChat", "EOSVoiceChat" });
        PrivateDependencyModuleNames.AddRange(new string[] { "AIModule", "Niagara" });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
