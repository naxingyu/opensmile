namespace openSMILEbatchGUI
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.button1 = new System.Windows.Forms.Button();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.fileListBox = new System.Windows.Forms.CheckedListBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button12 = new System.Windows.Forms.Button();
            this.button11 = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.labelFile = new System.Windows.Forms.TextBox();
            this.arffAppend = new System.Windows.Forms.CheckBox();
            this.csvAppend = new System.Windows.Forms.CheckBox();
            this.lldCsvAppend = new System.Windows.Forms.CheckBox();
            this.button10 = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.workDirectory = new System.Windows.Forms.TextBox();
            this.button9 = new System.Windows.Forms.Button();
            this.csvOutName = new System.Windows.Forms.TextBox();
            this.button8 = new System.Windows.Forms.Button();
            this.lldCsvOutName = new System.Windows.Forms.TextBox();
            this.button7 = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.arffOutName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.configurations = new System.Windows.Forms.ComboBox();
            this.haveCsvOut = new System.Windows.Forms.CheckBox();
            this.haveArffOut = new System.Windows.Forms.CheckBox();
            this.haveLldCsvOut = new System.Windows.Forms.CheckBox();
            this.button2 = new System.Windows.Forms.Button();
            this.selectedFolder = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.logBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.progressLabel = new System.Windows.Forms.Label();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.fileExtensionFilter = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.saveFileDialogArff = new System.Windows.Forms.SaveFileDialog();
            this.saveFileDialogCsv = new System.Windows.Forms.SaveFileDialog();
            this.saveFileDialogLldCsv = new System.Windows.Forms.SaveFileDialog();
            this.folderBrowserDialog2 = new System.Windows.Forms.FolderBrowserDialog();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button13 = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Enabled = false;
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(466, 378);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 39);
            this.button1.TabIndex = 0;
            this.button1.Text = "3. Start!";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // fileListBox
            // 
            this.fileListBox.FormattingEnabled = true;
            this.fileListBox.HorizontalScrollbar = true;
            this.fileListBox.Location = new System.Drawing.Point(15, 94);
            this.fileListBox.Name = "fileListBox";
            this.fileListBox.Size = new System.Drawing.Size(426, 409);
            this.fileListBox.TabIndex = 1;
            this.fileListBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.fileListBox_ItemCheck);
            this.fileListBox.SelectedIndexChanged += new System.EventHandler(this.checkedListBox1_SelectedIndexChanged);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(25, 42);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(373, 23);
            this.progressBar1.TabIndex = 2;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button13);
            this.groupBox1.Controls.Add(this.button12);
            this.groupBox1.Controls.Add(this.button11);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.labelFile);
            this.groupBox1.Controls.Add(this.arffAppend);
            this.groupBox1.Controls.Add(this.csvAppend);
            this.groupBox1.Controls.Add(this.lldCsvAppend);
            this.groupBox1.Controls.Add(this.button10);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.workDirectory);
            this.groupBox1.Controls.Add(this.button9);
            this.groupBox1.Controls.Add(this.csvOutName);
            this.groupBox1.Controls.Add(this.button8);
            this.groupBox1.Controls.Add(this.lldCsvOutName);
            this.groupBox1.Controls.Add(this.button7);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.arffOutName);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.configurations);
            this.groupBox1.Controls.Add(this.haveCsvOut);
            this.groupBox1.Controls.Add(this.haveArffOut);
            this.groupBox1.Controls.Add(this.haveLldCsvOut);
            this.groupBox1.Location = new System.Drawing.Point(466, 29);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(506, 274);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Options";
            // 
            // button12
            // 
            this.button12.Location = new System.Drawing.Point(406, 222);
            this.button12.Name = "button12";
            this.button12.Size = new System.Drawing.Size(53, 23);
            this.button12.TabIndex = 30;
            this.button12.Text = "Info...";
            this.button12.UseVisualStyleBackColor = true;
            this.button12.Click += new System.EventHandler(this.button12_Click);
            // 
            // button11
            // 
            this.button11.Location = new System.Drawing.Point(465, 247);
            this.button11.Name = "button11";
            this.button11.Size = new System.Drawing.Size(34, 21);
            this.button11.TabIndex = 29;
            this.button11.Text = "...";
            this.button11.UseVisualStyleBackColor = true;
            this.button11.Click += new System.EventHandler(this.button11_Click_1);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(28, 232);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(372, 13);
            this.label9.TabIndex = 28;
            this.label9.Text = "File selection list and labels (csv, with first column, filenames, and then label" +
    "s):";
            // 
            // labelFile
            // 
            this.labelFile.Location = new System.Drawing.Point(24, 248);
            this.labelFile.Name = "labelFile";
            this.labelFile.Size = new System.Drawing.Size(435, 20);
            this.labelFile.TabIndex = 27;
            this.labelFile.TextChanged += new System.EventHandler(this.labelFile_TextChanged);
            // 
            // arffAppend
            // 
            this.arffAppend.AutoSize = true;
            this.arffAppend.Checked = true;
            this.arffAppend.CheckState = System.Windows.Forms.CheckState.Checked;
            this.arffAppend.Location = new System.Drawing.Point(334, 195);
            this.arffAppend.Name = "arffAppend";
            this.arffAppend.Size = new System.Drawing.Size(167, 17);
            this.arffAppend.TabIndex = 26;
            this.arffAppend.Text = "Append to Functionals Arff file";
            this.arffAppend.UseVisualStyleBackColor = true;
            // 
            // csvAppend
            // 
            this.csvAppend.AutoSize = true;
            this.csvAppend.Checked = true;
            this.csvAppend.CheckState = System.Windows.Forms.CheckState.Checked;
            this.csvAppend.Location = new System.Drawing.Point(164, 195);
            this.csvAppend.Name = "csvAppend";
            this.csvAppend.Size = new System.Drawing.Size(172, 17);
            this.csvAppend.TabIndex = 25;
            this.csvAppend.Text = "Append to Functionals CSV file";
            this.csvAppend.UseVisualStyleBackColor = true;
            // 
            // lldCsvAppend
            // 
            this.lldCsvAppend.AutoSize = true;
            this.lldCsvAppend.Checked = true;
            this.lldCsvAppend.CheckState = System.Windows.Forms.CheckState.Checked;
            this.lldCsvAppend.Location = new System.Drawing.Point(24, 195);
            this.lldCsvAppend.Name = "lldCsvAppend";
            this.lldCsvAppend.Size = new System.Drawing.Size(138, 17);
            this.lldCsvAppend.TabIndex = 24;
            this.lldCsvAppend.Text = "Append to LLD CSV file";
            this.lldCsvAppend.UseVisualStyleBackColor = true;
            // 
            // button10
            // 
            this.button10.Location = new System.Drawing.Point(389, 45);
            this.button10.Name = "button10";
            this.button10.Size = new System.Drawing.Size(34, 21);
            this.button10.TabIndex = 23;
            this.button10.Text = "...";
            this.button10.UseVisualStyleBackColor = true;
            this.button10.Click += new System.EventHandler(this.button10_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(27, 49);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(109, 13);
            this.label5.TabIndex = 22;
            this.label5.Text = "1. Work directory:";
            this.label5.Click += new System.EventHandler(this.label5_Click);
            // 
            // workDirectory
            // 
            this.workDirectory.Location = new System.Drawing.Point(142, 46);
            this.workDirectory.Name = "workDirectory";
            this.workDirectory.Size = new System.Drawing.Size(241, 20);
            this.workDirectory.TabIndex = 21;
            this.workDirectory.Text = "..\\..\\..\\..\\";
            this.workDirectory.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.workDirectory.TextChanged += new System.EventHandler(this.workDirectory_TextChanged);
            // 
            // button9
            // 
            this.button9.Location = new System.Drawing.Point(465, 157);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(34, 21);
            this.button9.TabIndex = 20;
            this.button9.Text = "...";
            this.button9.UseVisualStyleBackColor = true;
            this.button9.Click += new System.EventHandler(this.button9_Click);
            // 
            // csvOutName
            // 
            this.csvOutName.Location = new System.Drawing.Point(150, 158);
            this.csvOutName.Name = "csvOutName";
            this.csvOutName.Size = new System.Drawing.Size(309, 20);
            this.csvOutName.TabIndex = 19;
            // 
            // button8
            // 
            this.button8.Location = new System.Drawing.Point(465, 135);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(34, 21);
            this.button8.TabIndex = 18;
            this.button8.Text = "...";
            this.button8.UseVisualStyleBackColor = true;
            this.button8.Click += new System.EventHandler(this.button8_Click);
            // 
            // lldCsvOutName
            // 
            this.lldCsvOutName.Location = new System.Drawing.Point(150, 136);
            this.lldCsvOutName.Name = "lldCsvOutName";
            this.lldCsvOutName.Size = new System.Drawing.Size(309, 20);
            this.lldCsvOutName.TabIndex = 17;
            // 
            // button7
            // 
            this.button7.Location = new System.Drawing.Point(465, 112);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(34, 21);
            this.button7.TabIndex = 16;
            this.button7.Text = "...";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new System.EventHandler(this.button7_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(32, 97);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(427, 13);
            this.label4.TabIndex = 7;
            this.label4.Text = "** The following options are only available if supported by the selected configur" +
    "ation file **";
            // 
            // arffOutName
            // 
            this.arffOutName.Location = new System.Drawing.Point(150, 113);
            this.arffOutName.Name = "arffOutName";
            this.arffOutName.Size = new System.Drawing.Size(309, 20);
            this.arffOutName.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(21, 24);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(146, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "openSMILE configuration file:";
            // 
            // configurations
            // 
            this.configurations.FormattingEnabled = true;
            this.configurations.Location = new System.Drawing.Point(175, 19);
            this.configurations.Name = "configurations";
            this.configurations.Size = new System.Drawing.Size(324, 21);
            this.configurations.TabIndex = 3;
            // 
            // haveCsvOut
            // 
            this.haveCsvOut.AutoSize = true;
            this.haveCsvOut.Location = new System.Drawing.Point(24, 161);
            this.haveCsvOut.Name = "haveCsvOut";
            this.haveCsvOut.Size = new System.Drawing.Size(97, 17);
            this.haveCsvOut.TabIndex = 2;
            this.haveCsvOut.Text = "Save CSV File:";
            this.haveCsvOut.UseVisualStyleBackColor = true;
            // 
            // haveArffOut
            // 
            this.haveArffOut.AutoSize = true;
            this.haveArffOut.Checked = true;
            this.haveArffOut.CheckState = System.Windows.Forms.CheckState.Checked;
            this.haveArffOut.Location = new System.Drawing.Point(24, 116);
            this.haveArffOut.Name = "haveArffOut";
            this.haveArffOut.Size = new System.Drawing.Size(102, 17);
            this.haveArffOut.TabIndex = 1;
            this.haveArffOut.Text = "Output file (Arff):";
            this.haveArffOut.UseVisualStyleBackColor = true;
            // 
            // haveLldCsvOut
            // 
            this.haveLldCsvOut.AutoSize = true;
            this.haveLldCsvOut.Location = new System.Drawing.Point(24, 139);
            this.haveLldCsvOut.Name = "haveLldCsvOut";
            this.haveLldCsvOut.Size = new System.Drawing.Size(120, 17);
            this.haveLldCsvOut.TabIndex = 0;
            this.haveLldCsvOut.Text = "Save LLD CSV File:";
            this.haveLldCsvOut.UseVisualStyleBackColor = true;
            this.haveLldCsvOut.CheckedChanged += new System.EventHandler(this.haveLldCsvOut_CheckedChanged);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(254, 6);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(187, 20);
            this.button2.TabIndex = 5;
            this.button2.Text = "Select folder to process ...";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // selectedFolder
            // 
            this.selectedFolder.Location = new System.Drawing.Point(15, 29);
            this.selectedFolder.Name = "selectedFolder";
            this.selectedFolder.Size = new System.Drawing.Size(426, 20);
            this.selectedFolder.TabIndex = 8;
            this.selectedFolder.Text = "..\\..\\..\\..\\example-audio";
            this.selectedFolder.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(163, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Folder with audio files to process:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 75);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(102, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "List of selected files:";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.logBox);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.progressLabel);
            this.groupBox2.Controls.Add(this.progressBar1);
            this.groupBox2.Location = new System.Drawing.Point(553, 323);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(419, 229);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Job progress";
            // 
            // logBox
            // 
            this.logBox.Location = new System.Drawing.Point(25, 97);
            this.logBox.Multiline = true;
            this.logBox.Name = "logBox";
            this.logBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.logBox.Size = new System.Drawing.Size(373, 115);
            this.logBox.TabIndex = 7;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 81);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(227, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "openSMILE console output and log messages:";
            // 
            // progressLabel
            // 
            this.progressLabel.AutoSize = true;
            this.progressLabel.Location = new System.Drawing.Point(22, 26);
            this.progressLabel.Name = "progressLabel";
            this.progressLabel.Size = new System.Drawing.Size(0, 13);
            this.progressLabel.TabIndex = 5;
            // 
            // button5
            // 
            this.button5.Enabled = false;
            this.button5.Location = new System.Drawing.Point(466, 453);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(75, 23);
            this.button5.TabIndex = 12;
            this.button5.Text = "Cancel";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // button6
            // 
            this.button6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button6.Location = new System.Drawing.Point(299, 67);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(142, 23);
            this.button6.TabIndex = 13;
            this.button6.Text = "2. Refresh Audio Files";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.button6_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(313, 531);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(104, 21);
            this.button4.TabIndex = 15;
            this.button4.Text = "Select none";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click_1);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(25, 531);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(103, 21);
            this.button3.TabIndex = 14;
            this.button3.Text = "Select all";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 52);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(96, 13);
            this.label7.TabIndex = 17;
            this.label7.Text = "File extension filter:";
            // 
            // fileExtensionFilter
            // 
            this.fileExtensionFilter.Location = new System.Drawing.Point(114, 49);
            this.fileExtensionFilter.Name = "fileExtensionFilter";
            this.fileExtensionFilter.Size = new System.Drawing.Size(61, 20);
            this.fileExtensionFilter.TabIndex = 18;
            this.fileExtensionFilter.Text = "*.wav";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(120, 75);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(150, 13);
            this.label8.TabIndex = 19;
            this.label8.Text = "Use double click to (de-)select";
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.DefaultExt = "csv";
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // button13
            // 
            this.button13.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button13.Location = new System.Drawing.Point(429, 45);
            this.button13.Name = "button13";
            this.button13.Size = new System.Drawing.Size(77, 21);
            this.button13.TabIndex = 31;
            this.button13.Text = "Refresh!";
            this.button13.UseVisualStyleBackColor = true;
            this.button13.Click += new System.EventHandler(this.button13_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(978, 564);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.fileExtensionFilter);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.selectedFolder);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.fileListBox);
            this.Controls.Add(this.button1);
            this.Name = "Form1";
            this.Text = "openSMILE batch processing GUI";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.CheckedListBox fileListBox;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox haveCsvOut;
        private System.Windows.Forms.CheckBox haveArffOut;
        private System.Windows.Forms.CheckBox haveLldCsvOut;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox selectedFolder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label progressLabel;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button9;
        private System.Windows.Forms.TextBox csvOutName;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.TextBox lldCsvOutName;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox arffOutName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox configurations;
        private System.Windows.Forms.Button button10;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox workDirectory;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.CheckBox arffAppend;
        private System.Windows.Forms.CheckBox csvAppend;
        private System.Windows.Forms.CheckBox lldCsvAppend;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox fileExtensionFilter;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.SaveFileDialog saveFileDialogArff;
        private System.Windows.Forms.SaveFileDialog saveFileDialogCsv;
        private System.Windows.Forms.SaveFileDialog saveFileDialogLldCsv;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog2;
        private System.Windows.Forms.TextBox logBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox labelFile;
        private System.Windows.Forms.Button button11;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button12;
        private System.Windows.Forms.Button button13;
    }
}

