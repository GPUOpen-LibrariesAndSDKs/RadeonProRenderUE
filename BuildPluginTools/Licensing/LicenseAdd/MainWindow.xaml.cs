using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace LicenseAdd
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const string OutputLogFilename = "output_log.txt";
        private string CopyrightTextBlock;

        private FileStream outputLogStream;
        private StreamWriter outputLogWriter;

        public MainWindow()
        {
            InitializeComponent();
            UISourceDir.Text = System.IO.Path.Combine(Directory.GetCurrentDirectory(), @"..\..\Plugins\RPRPlugin\Source");
            UICopyrightDir.Text = System.IO.Path.Combine(Directory.GetCurrentDirectory(), "License.txt");
            outputLogStream = File.Open(OutputLogFilename, FileMode.Create);
            outputLogWriter = new StreamWriter(outputLogStream);
            outputLogWriter.AutoFlush = true;
        }

        private void UIBrowseSourcesButton_Click(object sender, RoutedEventArgs e)
        {
            using (var fbd = new FolderBrowserDialog())
            {
                fbd.SelectedPath = UISourceDir.Text;
                if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    UISourceDir.Text = fbd.SelectedPath;
                }
            }
        }

        private void UIBrowseCopyrightButton_Click(object sender, RoutedEventArgs e)
        {
            using (var fbd = new OpenFileDialog())
            {
                fbd.CheckFileExists = true;
                fbd.InitialDirectory = System.IO.Path.GetDirectoryName(UICopyrightDir.Text);
                fbd.FileName = System.IO.Path.GetFileName(UICopyrightDir.Text);
                if (fbd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    UICopyrightDir.Text = fbd.FileName;
                    CopyrightTextBlock = string.Empty;
                }
            }
        }

        private void UIAddLicense_Click(object sender, RoutedEventArgs e)
        {
            UIAddLicense.IsEnabled = false;

            LoadCopyrightTextBlock();
            
            string[] filters = UIFilters.Text.Split(',');
            for (int i = 0; i < filters.Length; ++i)
            {
                string[] files = Directory.GetFiles(UISourceDir.Text, filters[i], SearchOption.AllDirectories);
                for (int j = 0; j < files.Length; ++j)
                {
                    if (!IsExcludedPath(files[j]))
                    {
                        AddToLog("Add license to : " + files[j]);
                        AddLicenseIfRequired(files[j]);
                    }
                    else
                    {
                        AddToLog("Exclude file : " + files[j]);
                    }
                }
            }

            outputLogWriter.Flush();
            UIAddLicense.IsEnabled = true;
        }

        private bool IsExcludedPath(string path)
        {
            string[] dirs = UIExcluded.Text.Split(',');
            for (int i = 0; i < dirs.Length; ++i)
            {
                if (path.Contains(dirs[i]))
                {
                    return (true);
                }
            }
            return (false);
        }

        private void LoadCopyrightTextBlock()
        {
            if (string.IsNullOrWhiteSpace(CopyrightTextBlock))
            {
                CopyrightTextBlock = File.ReadAllText(UICopyrightDir.Text);
            }
        }

        private void AddLicenseIfRequired(string filePath)
        {
            string[] lines = File.ReadAllLines(filePath);

            lines = SearchAndEraseExistingCopyright(lines);
            lines = InsertCopyright(lines);

            File.WriteAllLines(filePath, lines);
        }

        private string[] InsertCopyright(string[] lines)
        {
            List<string> lineList = new List<string>(lines);

            if (!string.IsNullOrWhiteSpace(CopyrightTextBlock))
            {
                lineList.Insert(0, CopyrightTextBlock);
            }

            return (lineList.ToArray());
        }

        private string[] SearchAndEraseExistingCopyright(string[] lines)
        {
            // Find the word "copyright"
            for (int i = 0; i < lines.Length; ++i)
            {
                // If "copyright" found...
                if (lines[i].IndexOf("copyright", 0, StringComparison.OrdinalIgnoreCase) >= 0)
                {
                    int blockStart, blockEnd;
                    GetCommentBlockAround(lines, i, out blockStart, out blockEnd);
                    return EraseLines(lines, blockStart, blockEnd);
                }
            }

            return (lines);
        }

        private string[] EraseLines(string[] lines, int blockStart, int blockEnd)
        {
            List<string> lineList = new List<string>(lines);

            lineList.RemoveRange(blockStart, (blockEnd + 1) - blockStart);
            return (lineList.ToArray());
        }

        private void GetCommentBlockAround(string[] lines, int blockElementIndex, out int blockStart, out int blockEnd)
        {
            blockStart = blockEnd = blockElementIndex;

            for (int i = blockElementIndex; i >= 0 && IsCommentedLine(lines[i]); --i)
            {
                blockStart = i;
            }

            for (int i = blockElementIndex; i < lines.Length && IsCommentedLine(lines[i]); ++i)
            {
                blockEnd = i;
            }
        }

        private bool IsCommentedLine(string line)
        {
            string trimedLine = line.Trim();
            return 
                trimedLine.StartsWith("/*") || 
                trimedLine.StartsWith("//") || 
                trimedLine.EndsWith("*/") || 
                trimedLine.StartsWith("*");
        }

        private void AddToLog(string msg)
        {
            outputLogWriter.Write(msg + System.Environment.NewLine);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            outputLogStream.Close();
        }
    }
}
