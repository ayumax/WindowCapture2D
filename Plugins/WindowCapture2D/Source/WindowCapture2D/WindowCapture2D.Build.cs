// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class WindowCapture2D : ModuleRules
{
	public WindowCapture2D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Latest;
		bEnableExceptions = true;
		
		PublicDependencyModuleNames.AddRange(["Core"]);

		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"RenderCore",
			"UMG",
			"RHI"
		]);

		if (Target.Platform != UnrealTargetPlatform.Win64)
		{
			return;
		}
			
		PublicSystemLibraries.AddRange(["shlwapi.lib", "runtimeobject.lib", "D3D11.lib"]);
		
		PrivateIncludePaths.Add(Path.Combine(
			Target.WindowsPlatform.WindowsSdkDir!,
			"Include",
			Target.WindowsPlatform.WindowsSdkVersion!,
			"cppwinrt"));
	}
}
