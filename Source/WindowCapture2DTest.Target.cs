// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class WindowCapture2DTestTarget : TargetRules
{
	public WindowCapture2DTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.Add("WindowCapture2DTest");
	}
}
