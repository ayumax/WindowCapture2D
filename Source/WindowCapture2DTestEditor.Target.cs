// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class WindowCapture2DTestEditorTarget : TargetRules
{
	public WindowCapture2DTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("WindowCapture2DTest");
	}
}
