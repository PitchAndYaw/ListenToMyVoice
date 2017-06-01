// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ListenToMyVoice : ModuleRules {

    public ListenToMyVoice(TargetInfo Target) {
        PublicDependencyModuleNames.AddRange(new string[] { "Core",
                                                            "CoreUObject",
                                                            "Engine",
                                                            "InputCore",
                                                            "OnlineSubsystem",
                                                            "OnlineSubsystemUtils",
                                                            "AIModule",
                                                            "GameplayTasks",
                                                            "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay",
                                                             "SteamVR",
                                                             "SteamVRController",
                                                             "FMODStudio",
                                                             "Slate",
                                                             "SlateCore" });
    }
}
