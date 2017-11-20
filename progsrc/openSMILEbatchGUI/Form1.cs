using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Diagnostics;

namespace openSMILEbatchGUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button4_Click(object sender, EventArgs e)
        {

        }

        private void checkedListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void buildSelFileListFromFolder()
        {
            fileListBox.Items.Clear();
            // iterate through directory
            if (System.IO.Directory.Exists(selectedFolder.Text))
            {
                string[] files = System.IO.Directory.GetFiles(@selectedFolder.Text, fileExtensionFilter.Text);
                foreach (string f in files)
                {
                    string fname = System.IO.Path.GetFileName(f);
                    fileListBox.Items.Add(fname);
                }
            }
            selectAllFiles(true);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (selectedFolder.Text != "")
            {
                folderBrowserDialog1.SelectedPath = selectedFolder.Text;
            }
            else
            {
                folderBrowserDialog1.SelectedPath = System.IO.Directory.GetCurrentDirectory();
            }
            folderBrowserDialog1.RootFolder = Environment.SpecialFolder.MyComputer;
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                selectedFolder.Text = folderBrowserDialog1.SelectedPath;
                buildSelFileListFromFolder();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            workDirectory.Text = System.IO.Path.GetFullPath("."); // System.IO.Directory.GetCurrentDirectory();
            refreshConfigFiles();
            MessageBox.Show("Please set work directory to the root (trunk) directory of the openSMILE package!");
        }

        private void refreshConfigFiles()
        {
            string cd = workDirectory.Text + "\\config";
            if (System.IO.Directory.Exists(cd))
            {
                string[] files = System.IO.Directory.GetFiles(@cd, "*.conf");
                string defaultConfig = "";
                foreach (string f in files)
                {
                    string fname = System.IO.Path.GetFileName(f);
                    configurations.Items.Add(fname);
                    if (fname == "GenevaExtended.conf")
                    {
                        defaultConfig = fname;
                    }
                    if (defaultConfig == "" && fname == "IS09_emotion.conf")
                    {
                        defaultConfig = fname;
                    }
                }
                // default: Geneva Extended, if available, else IS09 emotion
                configurations.Text = defaultConfig;
            }
            else
            {
                //MessageBox.Show("Directory with openSMILE configuration files (" + cd + ") does not exist. Please change the 'Work directory' setting accordingly!");
            }
        }

        private void button11_Click(object sender, EventArgs e)
        {
            refreshConfigFiles();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            selectedFolder.Text = System.IO.Path.GetFullPath(selectedFolder.Text);
            buildSelFileListFromFolder();
        }

        private void selectAllFiles(bool state = true)
        {
            for (int i = 0; i < fileListBox.Items.Count; i++)
            {
                fileListBox.SetItemChecked(i, state);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            selectAllFiles(true);    
        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            selectAllFiles(false);    
        }

        private void initProgress(int N)
        {
            progressBar1.Maximum = N;
            progressBar1.Minimum = 0;
            progressBar1.Step = 1;
            progressBar1.Value = 0;
        }

        private void logProgress(int i, int N)
        {
            progressBar1.Value = i;
            progressLabel.Text = "Processing file " + i.ToString() + " of " + N.ToString() + " ...";
        }

        private void logOutput(string line)
        {
            logBox.AppendText(line + "\n");
        }

        private int runSmileBinary(string workpath, string binary, string options)
        {
            try
            {
                // run opensmile
                Process p = new System.Diagnostics.Process();
                p.EnableRaisingEvents = true;
//                p.StartInfo.FileName = smilePath;
                //p.StartInfo.Arguments = smileArgs;
                p.StartInfo.FileName = "wrapper.exe";
                p.StartInfo.Arguments = binary + " " + options;
                p.StartInfo.WorkingDirectory = workpath;

                p.StartInfo.UseShellExecute = false;
                p.StartInfo.CreateNoWindow = true;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardError = true;
                //p.OutputDataReceived += new DataReceivedEventHandler(p_OutputDataReceived);
                p.ErrorDataReceived += new DataReceivedEventHandler(p_ErrorDataReceived);
                p.Exited += new EventHandler(p_Exited);

                // delete old log file
                if (System.IO.File.Exists(logfile))
                {
                    System.IO.File.Delete(logfile);
                }
                if (p.Start())
                {
                    return p.Id;
                }
                else
                {
                    return 0;
                }
            }
            catch (Win32Exception e)
            {
                MessageBox.Show("Could not run wrapper.exe to invoke " + binary + ": " + e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return 0;
            }
        }

        List<string> filesToProcessQueue;
        int nextFileIndex = 0;
        int fileSelCount = 0;
        string logfile = "";

        private void p_ErrorDataReceived(object sendingProcess,
            DataReceivedEventArgs errLine)
        {
            // Write the error text to the file if there is something 
            // to write and an error file has been specified. 

            if (!String.IsNullOrEmpty(errLine.Data))
            {
                this.Invoke(new MethodInvoker(delegate
                {
                    logOutput(errLine.Data);
                }));
            }
        }

        // a smile process has finished
        void p_Exited(object sender, EventArgs e)
        {
            // get exit code from process object
            Process p = (Process)sender;
            // load log file:
            string txt = File.ReadAllText(logfile);
            this.Invoke(new MethodInvoker(delegate
            {
                logOutput(txt + "\n");
            }));
            if (p.ExitCode != 0)
            {
                // error running smile...
                this.Invoke(new MethodInvoker(delegate
                {
                    progressLabel.Text = "An error occurred, see log for details.";
                    prepareUiAfterAnalysisRun();
                }));
                // exit the loop

                /*int idx = ad.getIndex();
                listView1.Invoke(new MethodInvoker(delegate
                {
                    listView1.Items[idx].Checked = false;
                    listView1.Items[idx].SubItems[2].Text = "error";
                    listView1.Items[idx].ForeColor = Color.Gray;
                }
                ));*/
            }
            else
            {
                // run next file in loop
                this.Invoke(new MethodInvoker(delegate
                {
                    processNextFile();
                }));
            }
            p.Dispose();
        }

        private string getLabels(string filename)
        {
            if (labels == null)
            {
                return "";
            }
            string basename = System.IO.Path.GetFileNameWithoutExtension(filename);
            string opts = "";
            string[] vals = (string[])labels[basename];
            if (vals != null)
            {
                for (int i = 1; i < vals.Count(); i++)
                {
                    opts += "-" + classnames[i] + " " + vals[i] + " ";
                }
                //MessageBox.Show(opts);
            }
            return opts;
        }
        
        private void processNextFile()
        {
            bool next = true;
            while (next)
            {
                logProgress(nextFileIndex, fileSelCount);
                if (nextFileIndex < fileSelCount)
                {
                    string f = filesToProcessQueue[nextFileIndex];
                    nextFileIndex++;
                    this.Invoke(new MethodInvoker(delegate
                    {

                        // get labels
                        string extraoptions = getLabels(f);
                        if (labelFile.Text != "" && extraoptions == "")
                        {
                            logOutput("Skipping file '" + f + "'. Not in label list.\n");
                        }
                        else
                        {
                            logOutput("Processing file '" + f + "'.\n");
                            next = false;
                            // run openSMILE
                            runOpenSMILE(f, extraoptions);
                        }
                    }
                    ));
                }
                else
                {
                    this.Invoke(new MethodInvoker(delegate
                    {
                        progressLabel.Text = "Done.";
                        prepareUiAfterAnalysisRun();
                    }));
                    next = false;
                }
            }
        }

        System.Collections.Hashtable labels;
        string[] classnames;

        private void runOpenSMILE(string filename, string extraoptions="")
        {
            string binary = System.IO.Path.GetFullPath(workDirectory.Text + "\\SMILExtract_Release.exe");
            logfile = System.IO.Path.GetFullPath(workDirectory.Text + "\\smile.log");
            string options = "-C \"" + System.IO.Path.GetFullPath(workDirectory.Text + "\\config\\" + configurations.Text) + "\" ";
            options += "-logfile \"" + logfile + "\" ";
            string wavFilename = System.IO.Path.GetFullPath(filename);
            options += "-I \"" + wavFilename + "\" ";
            options += "-instname \"" + Path.GetFileName(wavFilename) + "\" ";
            string arffOutFile = System.IO.Path.GetFullPath(arffOutName.Text);
            string csvOutFile = System.IO.Path.GetFullPath(csvOutName.Text);
            string lldCsvOutFile = System.IO.Path.GetFullPath(lldCsvOutName.Text);
            if (haveArffOut.Checked)
            {
                options += "-O \"" + arffOutFile + "\" ";
                if (arffAppend.Checked)
                {
                    options += "-appendarff 1 ";  // note: this is the default
                }
                else
                {
                    options += "-appendarff 0 ";
                }
            }
            else
            {
                options += "-O ? ";
            }
            if (haveCsvOut.Checked)
            {
                options += "-csvoutput \"" + csvOutFile + "\" ";
                if (csvAppend.Checked)
                {
                    options += "-appendcsv 1 ";
                }
                else
                {
                    options += "-appendcsv 0 ";
                }
            }
            else
            {
                options += "-csvoutput ? ";
            }
            if (haveLldCsvOut.Checked)
            {
                options += "-lldcsvoutput \"" + lldCsvOutFile + "\" ";
                if (lldCsvAppend.Checked)
                {
                    options += "-appendcsvlld 1 ";
                }
                else
                {
                    options += "-appendcsvlld 0 ";
                }
            }
            else
            {
                options += "-lldcsvoutput ? ";
            }
            /*
             * if (!createLogFile.Checked)
            {
                options += "-nologfile ";
            }
             * */
            options += "-l 2 ";
            options += extraoptions + " ";
            logBox.AppendText("Running command: '" + binary + " " + options + "'\n");
            if (runSmileBinary(System.IO.Path.GetFullPath(workDirectory.Text), binary, options) == 0)
            {
                prepareUiAfterAnalysisRun();
            }
        }

        private void prepareUiForAnalysisRun()
        {
            // disable all controls that should not be changed during the analysis run
            groupBox1.Enabled = false;
            selectedFolder.Enabled = false;
            button1.Enabled = false;
            button2.Enabled = false;
            button3.Enabled = false;
            button4.Enabled = false;
            button5.Enabled = true;
            fileListBox.Enabled = false;
        }

        private void prepareUiAfterAnalysisRun()
        {
            groupBox1.Enabled = true;
            selectedFolder.Enabled = true;
            button1.Enabled = true;
            button2.Enabled = true;
            button3.Enabled = true;
            button4.Enabled = true;
            button5.Enabled = false;
            fileListBox.Enabled = true;
        }

        private int getNumFilesSelected()
        {
            int fileSelCount = 0;
            for (int i = 0; i < fileListBox.Items.Count; i++)
            {
                if (fileListBox.GetItemChecked(i))
                {
                    fileSelCount++;
                }
            }
            return fileSelCount;
        }

        private void runAnalysisLoop()
        {
            fileSelCount = getNumFilesSelected();
            if (fileSelCount > 0)
            {
                prepareUiForAnalysisRun();
                initProgress(fileSelCount);
                // TODO: thread this!
                filesToProcessQueue = new List<string>();
                for (int i = 0; i < fileListBox.Items.Count; i++)
                {
                    if (fileListBox.GetItemChecked(i))
                    {
                        string f = selectedFolder.Text + "\\" + fileListBox.Items[i].ToString();
                        filesToProcessQueue.Add(f);
                    }
                }
                // in append mode, delete existing files
                if (haveArffOut.Checked && arffAppend.Checked && File.Exists(arffOutName.Text))
                {
                    DialogResult dlrs = MessageBox.Show("The ARFF output file already exists. The existing file will be deleted now. Proceed?", "Overwrite files?", MessageBoxButtons.YesNo);
                    if (dlrs == DialogResult.Yes)
                    {
                        File.Delete(arffOutName.Text);
                    }
                    else
                    {
                        prepareUiAfterAnalysisRun();
                        return;
                    }
                    // TODO: show a warning dialogue and ask for overwrite yes/no
                }
                if (haveCsvOut.Checked && csvAppend.Checked && File.Exists(csvOutName.Text))
                {
                    DialogResult dlrs = MessageBox.Show("The CSV output file already exists. The existing file will be deleted now. Proceed?", "Overwrite files?", MessageBoxButtons.YesNo);
                    if (dlrs == DialogResult.Yes)
                    {
                        File.Delete(csvOutName.Text);
                    }
                    else
                    {
                        prepareUiAfterAnalysisRun();
                        return;
                    }
                }
                if (haveLldCsvOut.Checked && lldCsvAppend.Checked && File.Exists(lldCsvOutName.Text))
                {
                    DialogResult dlrs = MessageBox.Show("The LLD CSV output file already exists. The existing file will be deleted now. Proceed?", "Overwrite files?", MessageBoxButtons.YesNo);
                    if (dlrs == DialogResult.Yes)
                    {
                        File.Delete(lldCsvOutName.Text);
                    }
                    else
                    {
                        prepareUiAfterAnalysisRun();
                        return;
                    }
                }
                nextFileIndex = 0;
                processNextFile();  // start with the first
            }
            else
            {
                MessageBox.Show("No files selected for processing! Please select some files in the box on the left (use double click, or the 'select all' button)");
            }
        }

        private bool loadLabelList()
        {
            if (labelFile.Text == "")
            {
                return true;
            }
            if (System.IO.File.Exists(labelFile.Text))
            {
                string[] labelLines = File.ReadAllLines(@labelFile.Text);
                string[] labelVals = labelLines[0].Split(';');
                classnames = new string[labelLines.Count()];
                classnames[0] = "Filename";
                for (int i = 1; i < labelVals.Count(); i++)
                {
                    classnames[i] = labelVals[i];
                }
                labels = new System.Collections.Hashtable();
                for (int i = 1; i < labelLines.Count(); i++) 
                {
                  string[] labelValues = labelLines[i].Split(';');
                  string filename = System.IO.Path.GetFileNameWithoutExtension(labelValues[0]);
                  labels.Add(filename, labelValues);
                }
                return true;
            }
            else
            {
                MessageBox.Show("ERROR: Label file not found! Please leave field empty if you do not have a label file.");
            }
            return false;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (loadLabelList())
            {
                runAnalysisLoop();
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            if (saveFileDialogArff.ShowDialog() == DialogResult.OK) 
            {
                arffOutName.Text = saveFileDialogArff.FileName;
                haveArffOut.Checked = true;
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            if (saveFileDialogLldCsv.ShowDialog() == DialogResult.OK) 
            {
                lldCsvOutName.Text = saveFileDialogLldCsv.FileName;
                haveLldCsvOut.Checked = true;
            }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            if (saveFileDialogCsv.ShowDialog() == DialogResult.OK) 
            {
                csvOutName.Text = saveFileDialogCsv.FileName;
                haveCsvOut.Checked = true;
            }

        }

        private void fileListBox_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (getNumFilesSelected() > 0)
            {
                button1.Enabled = true;
            }
            else
            {
                button1.Enabled = false;
            }
        }

        private void workDirectory_TextChanged(object sender, EventArgs e)
        {
            refreshConfigFiles();
        }

        private void button10_Click(object sender, EventArgs e)
        {
            folderBrowserDialog2.RootFolder = Environment.SpecialFolder.MyComputer;
            folderBrowserDialog2.SelectedPath = workDirectory.Text;
            if (folderBrowserDialog2.ShowDialog() == DialogResult.OK)
            {
                workDirectory.Text = folderBrowserDialog2.SelectedPath;
            }
            if (System.IO.Directory.Exists(workDirectory.Text))
            {
                System.IO.Directory.CreateDirectory(workDirectory.Text + "\\data");
            }
        }

        private void haveLldCsvOut_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void button5_Click(object sender, EventArgs e)
        {
            filesToProcessQueue.Clear();
        }

        private void button11_Click_1(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                labelFile.Text = openFileDialog1.FileName;
            }
        }

        private void labelFile_TextChanged(object sender, EventArgs e)
        {

        }

        Form2 form2 = null;
        private void button12_Click(object sender, EventArgs e)
        {
            if (form2 == null)
            {
                form2 = new Form2();
            }
            form2.Show();
        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void button13_Click(object sender, EventArgs e)
        {
            workDirectory.Text = System.IO.Path.GetFullPath(workDirectory.Text);
            refreshConfigFiles();
            if (arffOutName.Text == "")
            {
                arffOutName.Text = workDirectory.Text + "\\data\\output.arff";
            }
            if (csvOutName.Text == "")
            {
                csvOutName.Text = workDirectory.Text + "\\data\\output.csv";
            }
            if (lldCsvOutName.Text == "")
            {
                lldCsvOutName.Text = workDirectory.Text + "\\data\\output_lld.csv";
            }
        }

        

    }
}
