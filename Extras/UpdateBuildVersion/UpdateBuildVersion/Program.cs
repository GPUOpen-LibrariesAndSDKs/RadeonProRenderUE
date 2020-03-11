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

﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UpdateBuildVersion
{
    class Program
    {
        private const string RPRPluginBuildVersionDefName = "RPR_PLUGIN_BUILD_VERSION";
        private const string RPRPluginBuildGUIDDefName = "RPR_PLUGIN_BUILD_GUID";

        static void Main(string[] args)
        {
            if (args.Length == 1)
            {
                string filePath = args[0];
                SetRPRPluginVersion(filePath);
            }
        }

        private static void SetRPRPluginVersion(string versionFilePath)
        {
            int buildVersion = -1;
            string buildGuid = "Unknown";

            string[] lines = File.ReadAllLines(versionFilePath);
            for (int i = 0; i < lines.Length; ++i)
            {
                string line = lines[i];
                
                SetValueIfValidLine(ref line, RPRPluginBuildVersionDefName, (Value) =>
                {
                    buildVersion = int.Parse(Value) + 1;
                    return buildVersion.ToString();
                });

                SetValueIfValidLine(ref line, RPRPluginBuildGUIDDefName, (Value) =>
                {
                    buildGuid = Guid.NewGuid().ToString();
                    return EscapeText(buildGuid);
                });

                lines[i] = line;
            }
            File.WriteAllLines(versionFilePath, lines);

            Console.WriteLine("RPR Plugin Build {0}:{1}", buildVersion, buildGuid);
        }

        private static bool SetValueIfValidLine(ref string Line, string ExpectedDefName, Func<string, string> SetValue)
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

        private static string JoinLineChunks(string[] LineChunks)
        {
            string line = LineChunks[0];
            for (int i = 1; i < LineChunks.Length; ++i)
            {
                line += " " + LineChunks[i];
            }
            return (line);
        }

        private static string EscapeText(string text)
        {
            return ("TEXT(\"" + text + "\")");
        }
    }
}
