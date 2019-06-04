// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class WindowCapture2DTestTarget : TargetRules
{
	public WindowCapture2DTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "WindowCapture2DTest" } );
	}
}
