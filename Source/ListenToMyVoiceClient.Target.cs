// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

//[SupportedPlatforms(UnrealPlatformClass.Client)]
public class ListenToMyVoiceClientTarget : TargetRules
{
  public ListenToMyVoiceClientTarget(TargetInfo Target) {
    Type = TargetType.Client;
    //bUsesSteam = true;
  }
  //
  // TargetRules interface.
  //
  public override void SetupBinaries(TargetInfo Target,
                                     ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
                                     ref List<string> OutExtraModuleNames) {
    OutExtraModuleNames.Add("ListenToMyVoice");
  }
}