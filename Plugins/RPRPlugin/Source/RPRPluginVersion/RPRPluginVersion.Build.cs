/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

/// <summary>
/// Primary runtime module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPRPluginVersion : ModuleRules
{
    private const int RPRPluginVersion_Major = 1;
    private const int RPRPluginVersion_Minor = 1;

    private const string RPRPluginMajorVersionDefName = "RPR_PLUGIN_MAJOR_VERSION";
    private const string RPRPluginMinorVersionDefName = "RPR_PLUGIN_MINOR_VERSION";

    // Assure to do this step only one time during a build
    // since the constructor of RPRPluginVersion can be called multiple times
    // during the same build
    private static bool HasFileVersionBeenSet = false;

    public RPRPluginVersion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(
			new string[] {
                "RPRPluginVersion/Public",
                "RPRPluginVersion/Private",
            }
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			});

        if (!HasFileVersionBeenSet)
        {
            SetRPRPluginVersion();
            HasFileVersionBeenSet = true;
        }
    }

    private void SetRPRPluginVersion()
    {
        string versionFilePath = Path.Combine(ModuleDirectory, "Public", "RPRPluginVersion.h");

        string[] lines = File.ReadAllLines(versionFilePath);
        for (int i = 0; i < lines.Length; ++i)
        {
            string line = lines[i];

            SetValueIfValidLine(ref line, RPRPluginMajorVersionDefName, (Value) => { return RPRPluginVersion_Major.ToString(); });
            SetValueIfValidLine(ref line, RPRPluginMinorVersionDefName, (Value) => { return RPRPluginVersion_Minor.ToString(); });

            lines[i] = line;
        }
        File.WriteAllLines(versionFilePath, lines);

        Console.WriteLine("RPR Plugin Version {0}.{1}", RPRPluginVersion_Major, RPRPluginVersion_Minor);
    }

    private bool SetValueIfValidLine(ref string Line, string ExpectedDefName, Func<string, string> SetValue)
    {
        if (Line.Contains(ExpectedDefName))
        {
            string[] lineChunks = Line.Split(' ');
            lineChunks[2] = SetValue(lineChunks[2]);
            Line = JoinLineChunks(lineChunks);
            return (true);
        }
        return (false);
    }

    private string JoinLineChunks(string[] LineChunks)
    {
        string line = LineChunks[0];
        for (int i = 1; i < LineChunks.Length; ++i)
        {
            line += " " + LineChunks[i];
        }
        return (line);
    }

    private string EscapeText(string text)
    {
        return ("TEXT(\"" + text + "\")");
    }
}
