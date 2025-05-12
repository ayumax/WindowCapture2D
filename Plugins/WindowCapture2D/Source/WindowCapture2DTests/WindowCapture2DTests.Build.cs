// Copyright 2019 ayumax. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class WindowCapture2DTests : ModuleRules
{
	public WindowCapture2DTests
		(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		ShadowVariableWarningLevel = WarningLevel.Warning;
		bEnableExceptions = true;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"WindowCapture2D",
				"AutomationController",
				"FunctionalTesting",
				"Slate",
				"SlateCore",
				"RenderCore",
				"UMG",
				"RHI"
			}
		);

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("EditorStyle");
		}

		PublicSystemLibraries.AddRange(new string[] { "shlwapi.lib", "runtimeobject.lib", "D3D11.lib" });

		PrivateIncludePaths.Add(Path.Combine(
			Target.WindowsPlatform.WindowsSdkDir!,
			"Include",
			Target.WindowsPlatform.WindowsSdkVersion!,
			"cppwinrt"));
	}
}