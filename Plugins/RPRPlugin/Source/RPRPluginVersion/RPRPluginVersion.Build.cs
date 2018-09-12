/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

/// <summary>
/// Primary runtime module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPRPluginVersion : ModuleRules
{
    private const int RPRPluginVersion_Major = 0;
    private const int RPRPluginVersion_Minor = 8;

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
