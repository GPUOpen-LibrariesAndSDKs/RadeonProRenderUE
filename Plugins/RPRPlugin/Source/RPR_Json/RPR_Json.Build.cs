// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using System.Collections.Generic;
using UnrealBuildTool;

public class RPR_Json : ModuleRules
{
    public string ThirdPartyDirectory
    { get { return ("ThirdParty/"); } }

    public RPR_Json(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory + "/" + ThirdPartyDirectory + "json/include",
            });
    }
}
