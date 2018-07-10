using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IncludeFixer
{
    class Program
    {
        private static string ProjectPath;
        private static string EnginePath;

        static void Main(string[] args)
        {
            ParseArguments(args);

            if (!string.IsNullOrWhiteSpace(ProjectPath) && 
                !string.IsNullOrWhiteSpace(EnginePath))
            {
                IncludePathsFixer includePathFixers = new IncludePathsFixer();
                includePathFixers.ProjectPath = ProjectPath;
                includePathFixers.EnginePath = EnginePath;

                if (includePathFixers.CheckPaths())
                {
                    includePathFixers.Fix();
                }
            }
        }

        static void ParseArguments(string[] args)
        {
            int cursor = 0;
            while (cursor < args.Length)
            {
                string arg = args[cursor];

                switch (arg)
                {
                    case "-p":
                        if (cursor + 1 < args.Length)
                        {
                            ProjectPath = args[cursor + 1];
                            ++cursor;
                        }
                        break;

                    case "-e":
                        if (cursor + 1 < args.Length)
                        {
                            EnginePath = args[cursor + 1];
                            ++cursor;
                        }
                        break;

                    case "-h":
                    case "/?":
                        DisplayHelp();
                        break;
                }

                ++cursor;
            }
        }

        static void DisplayHelp()
        {
            Console.WriteLine("usage : IncludeFixer -p project_path -e engine_path");
            Console.WriteLine("Fix include paths so they are all relative to their core.");
            Console.WriteLine("ie. #include \"UnrealTypeTraits.h\" will be changed to #include \"Templates/UnrealTypeTraits.h\".");
        }
    }
}
