using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IncludeFixer
{
    class IncludePathsFixer
    {
        public string ProjectPath { get; set; }
        public string EnginePath { get; set; }
        
        struct FileInfo
        {
            public string Filename;
            public string ModuleName;
            public string RelativePath; // Relative to the module
            public string DirectoryPath;
        }

        List<FileInfo> fileInfos;

        public IncludePathsFixer()
        {
            fileInfos = new List<FileInfo>();
        }

        public void Fix()
        {
            AnalyzePaths();
            FixPaths();
        }

        private void AnalyzePaths()
        {
            fileInfos.Clear();
            AnalyzeProjectPaths();
            AnalyzeEnginePaths();
        }

        private void AnalyzeProjectPaths()
        {
            AnalyzeProjectPluginsPaths();
        }

        private void AnalyzeEnginePaths()
        {
            string engineDirectory = Path.Combine(EnginePath, "Engine");
            string pluginsDirectory = Path.Combine(engineDirectory, "Plugins");
            string sourceDirectory = Path.Combine(engineDirectory, "Source");

            AnalyzeSourceDirectory(pluginsDirectory);
            AnalyzeSourceDirectory(sourceDirectory);
        }

        private void AnalyzeProjectPluginsPaths()
        {
            string pluginsPath = Path.Combine(ProjectPath, "Plugins");
            AnalyzeSourceDirectory(pluginsPath);
        }

        private void AnalyzeSourceDirectory(string sourcePath)
        {
            // Find modules
            string[] modulesPaths = Directory.GetFiles(sourcePath, "*.Build.cs", SearchOption.AllDirectories);

            for (int i = 0; i < modulesPaths.Length; ++i)
            {
                string modulePath = Path.GetDirectoryName(modulesPaths[i]);

                // Do not parse third parties!
                if (modulePath.Contains("ThirdParty"))
                {
                    continue;
                }

                string moduleName = Path.GetFileName(modulePath).Replace(".Build.cs", "");

                // Find header files...
                var moduleFilesQuery = Directory.EnumerateFiles(modulePath, "*.h", SearchOption.AllDirectories);
                foreach (var moduleFile in moduleFilesQuery)
                {
                    // Save each header file by module

                    FileInfo fi = new FileInfo();
                    fi.Filename = Path.GetFileName(moduleFile);
                    fi.DirectoryPath = Path.GetDirectoryName(moduleFile);
                    fi.ModuleName = moduleName;
                    fi.RelativePath = MakeModuleRelativePath(fi.DirectoryPath, modulePath);

                    fileInfos.Add(fi);
                }
            }
        }

        private string MakeModuleRelativePath(string directoryPath, string modulePath)
        {
            if (modulePath.Length == directoryPath.Length)
            {
                return ("");
            }

            string relativePath = directoryPath.Remove(0, modulePath.Length + 1);

            relativePath = RemoveFromStartIfPresent(relativePath, "Public");
            relativePath = RemoveFromStartIfPresent(relativePath, "Classes");
            relativePath = RemoveFromStartIfPresent(relativePath, "Private");
            relativePath = RemoveFromStartIfPresent(relativePath, @"\");

            relativePath = ConvertPathDelimitersToBackslashes(relativePath);

            return (relativePath);
        }

        private string RemoveFromStartIfPresent(string path, string item)
        {
            if (path.StartsWith(item))
            {
                return (path.Remove(0, item.Length));
            }
            return (path);
        }

        private void FixPaths()
        {
            string pluginsPath = Path.Combine(ProjectPath, "Plugins");
            string[] modulesPaths = Directory.GetFiles(pluginsPath, "*.Build.cs", SearchOption.AllDirectories);

            for (int i = 0; i < modulesPaths.Length; ++i)
            {
                string moduleDirectory = Path.GetDirectoryName(modulesPaths[i]);
                var moduleFileQuery = 
                    Directory.EnumerateFiles(moduleDirectory, "*.h", SearchOption.AllDirectories).Concat(
                    Directory.EnumerateFiles(moduleDirectory, "*.cpp", SearchOption.AllDirectories));

                foreach (var sourceFile in moduleFileQuery)
                {
                    FixSourceFile(sourceFile);
                }
            }
        }

        private void FixSourceFile(string filePath)
        {
            bool hasChanged = false;

            string[] fileContent = File.ReadAllLines(filePath);
            for (int i = 0; i < fileContent.Length; ++i)
            {
                if (fileContent[i].StartsWith("#include"))
                {
                    if (FixIncludeLine(ref fileContent[i]))
                    {
                        hasChanged = true;
                    }
                }
            }

            if (hasChanged)
            {
                File.WriteAllLines(filePath, fileContent);
                Console.WriteLine("Modified file : " + filePath);
            }
        }
        
        private bool FixIncludeLine(ref string includeLine)
        {
            string includePath = IsolateIncludePath(includeLine);

            // Do not support system includes
            if (includePath.StartsWith("<"))
            {
                return (false);
            }

            string includeFilename = Path.GetFileName(includePath);

            if (ManageException(ref includeLine, includePath, includeFilename))
            {
                return (true);
            }

            // Find include path among database
            // If found in database...
            // ... check that the relative path is set
            // - if set, skip this include line
            // - if not set, fix the line

            for (int i = 0; i < fileInfos.Count; ++i)
            {
                if (fileInfos[i].Filename == includeFilename)
                {
                    string includeFilePath = Path.GetDirectoryName(includePath);
                    if (includeFilePath != fileInfos[i].RelativePath)
                    {
                        string fixedPath = ConvertPathDelimitersToBackslashes(
                            Path.Combine(fileInfos[i].RelativePath, includeFilename));
                        includeLine = string.Format("#include \"{0}\"", fixedPath);
                        return (true);
                    }
                    else
                    {
                        return (false);
                    }
                }
            }

            return (false);
        }

        private string IsolateIncludePath(string includeLine)
        {
            string includePath = includeLine.Remove(0, "#include".Length).Trim();
            includePath = includePath.Replace("\"", "");
            return (includePath);
        }

        private bool ManageException(ref string includeLine, string includePath, string includeFilename)
        {
            if (includeFilename == "Version.h")
            {
                includeLine = "#include \"Runtime/Launch/Resources/Version.h\"";
                return (true);
            }
            return (false);
        }

        public bool CheckPaths()
        {
            return Directory.Exists(ProjectPath) && Directory.Exists(EnginePath);
        }

        private string ConvertPathDelimitersToBackslashes(string path)
        {
            return (path.Replace("\\", "/"));
        }
    }
}
