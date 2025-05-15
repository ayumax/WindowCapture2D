// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class WindowCapture2D : ModuleRules
{
	public WindowCapture2D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		bEnableExceptions = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core" });

		PrivateDependencyModuleNames.AddRange(new string[]{
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"RenderCore",
			"UMG",
			"RHI"
		});

		if (Target.Platform != UnrealTargetPlatform.Win64)
		{
			return;
		}

		PublicSystemLibraries.AddRange(new string[] { "shlwapi.lib", "runtimeobject.lib", "D3D11.lib" });

		var winrtDirectory = Path.Combine(ModuleDirectory, "Private", "cppwinrt");
		if (Directory.Exists(winrtDirectory))
		{
			PrivateIncludePaths.Add(winrtDirectory);
		}
	}
}
