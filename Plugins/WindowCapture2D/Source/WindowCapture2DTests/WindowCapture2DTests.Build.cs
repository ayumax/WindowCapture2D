// Copyright 2019 ayumax. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class WindowCapture2DTests : ModuleRules
{
	public WindowCapture2DTests
		(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		ShadowVariableWarningLevel = WarningLevel.Warning;
		CppStandard = CppStandardVersion.Cpp20;
		bEnableExceptions = true;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"AutomationController",
				"FunctionalTesting",
				"Slate",
				"SlateCore",
				"RenderCore",
				"UMG",
				"RHI",
				"WindowCapture2D",
			}
		);

		var winrtDirectory = Path.Combine(ModuleDirectory, "..", "WindowCapture2D", "Private", "cppwinrt");
		if (Directory.Exists(winrtDirectory))
		{
			PrivateIncludePaths.Add(winrtDirectory);
		}

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("EditorStyle");
		}
	}
}