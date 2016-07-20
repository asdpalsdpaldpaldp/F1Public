namespace F1Loader
{
	partial class MainForm
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
			this.StartButton = new MetroFramework.Controls.MetroButton();
			this.LoadingBar = new MetroFramework.Controls.MetroProgressBar();
			this.LocalCheckbox = new MetroFramework.Controls.MetroCheckBox();
			this.LatestCheckbox = new MetroFramework.Controls.MetroCheckBox();
			this.StatusLabel = new MetroFramework.Controls.MetroLabel();
			this.MainFlowpanel = new System.Windows.Forms.FlowLayoutPanel();
			this.CheckboxFlowpanel = new System.Windows.Forms.FlowLayoutPanel();
			this.StatusFlowpanel = new System.Windows.Forms.FlowLayoutPanel();
			this.MainFlowpanel.SuspendLayout();
			this.CheckboxFlowpanel.SuspendLayout();
			this.StatusFlowpanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// StartButton
			// 
			this.StartButton.Location = new System.Drawing.Point(3, 3);
			this.StartButton.Name = "StartButton";
			this.StartButton.Size = new System.Drawing.Size(92, 29);
			this.StartButton.Style = MetroFramework.MetroColorStyle.White;
			this.StartButton.TabIndex = 0;
			this.StartButton.Text = "Start";
			this.StartButton.Theme = MetroFramework.MetroThemeStyle.Dark;
			this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
			// 
			// LoadingBar
			// 
			this.LoadingBar.HideProgressText = false;
			this.LoadingBar.Location = new System.Drawing.Point(3, 3);
			this.LoadingBar.Name = "LoadingBar";
			this.LoadingBar.Size = new System.Drawing.Size(229, 23);
			this.LoadingBar.Style = MetroFramework.MetroColorStyle.Black;
			this.LoadingBar.TabIndex = 1;
			this.LoadingBar.Theme = MetroFramework.MetroThemeStyle.Dark;
			this.LoadingBar.Click += new System.EventHandler(this.LoadingBar_Click);
			// 
			// LocalCheckbox
			// 
			this.LocalCheckbox.AutoSize = true;
			this.LocalCheckbox.Location = new System.Drawing.Point(3, 24);
			this.LocalCheckbox.Name = "LocalCheckbox";
			this.LocalCheckbox.Size = new System.Drawing.Size(75, 15);
			this.LocalCheckbox.TabIndex = 2;
			this.LocalCheckbox.Text = "Select File";
			this.LocalCheckbox.Theme = MetroFramework.MetroThemeStyle.Dark;
			this.LocalCheckbox.UseVisualStyleBackColor = true;
			this.LocalCheckbox.CheckedChanged += new System.EventHandler(this.LocalCheckbox_CheckedChanged);
			// 
			// LatestCheckbox
			// 
			this.LatestCheckbox.AutoSize = true;
			this.LatestCheckbox.Checked = true;
			this.LatestCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.LatestCheckbox.Location = new System.Drawing.Point(3, 3);
			this.LatestCheckbox.Name = "LatestCheckbox";
			this.LatestCheckbox.Size = new System.Drawing.Size(101, 15);
			this.LatestCheckbox.TabIndex = 3;
			this.LatestCheckbox.Text = "Embedded File";
			this.LatestCheckbox.Theme = MetroFramework.MetroThemeStyle.Dark;
			this.LatestCheckbox.UseVisualStyleBackColor = true;
			this.LatestCheckbox.CheckedChanged += new System.EventHandler(this.LatestCheckbox_CheckedChanged);
			// 
			// StatusLabel
			// 
			this.StatusLabel.Location = new System.Drawing.Point(3, 29);
			this.StatusLabel.Name = "StatusLabel";
			this.StatusLabel.Size = new System.Drawing.Size(229, 19);
			this.StatusLabel.TabIndex = 4;
			this.StatusLabel.Text = "Status: ";
			this.StatusLabel.Theme = MetroFramework.MetroThemeStyle.Dark;
			// 
			// MainFlowpanel
			// 
			this.MainFlowpanel.Controls.Add(this.StartButton);
			this.MainFlowpanel.Controls.Add(this.CheckboxFlowpanel);
			this.MainFlowpanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.MainFlowpanel.Location = new System.Drawing.Point(20, 60);
			this.MainFlowpanel.Name = "MainFlowpanel";
			this.MainFlowpanel.Size = new System.Drawing.Size(232, 47);
			this.MainFlowpanel.TabIndex = 5;
			// 
			// CheckboxFlowpanel
			// 
			this.CheckboxFlowpanel.Controls.Add(this.LatestCheckbox);
			this.CheckboxFlowpanel.Controls.Add(this.LocalCheckbox);
			this.CheckboxFlowpanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
			this.CheckboxFlowpanel.Location = new System.Drawing.Point(101, 3);
			this.CheckboxFlowpanel.Name = "CheckboxFlowpanel";
			this.CheckboxFlowpanel.Size = new System.Drawing.Size(103, 48);
			this.CheckboxFlowpanel.TabIndex = 6;
			// 
			// StatusFlowpanel
			// 
			this.StatusFlowpanel.Controls.Add(this.LoadingBar);
			this.StatusFlowpanel.Controls.Add(this.StatusLabel);
			this.StatusFlowpanel.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.StatusFlowpanel.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
			this.StatusFlowpanel.Location = new System.Drawing.Point(20, 107);
			this.StatusFlowpanel.Name = "StatusFlowpanel";
			this.StatusFlowpanel.Size = new System.Drawing.Size(232, 50);
			this.StatusFlowpanel.TabIndex = 6;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.ClientSize = new System.Drawing.Size(272, 177);
			this.Controls.Add(this.MainFlowpanel);
			this.Controls.Add(this.StatusFlowpanel);
			this.ImeMode = System.Windows.Forms.ImeMode.On;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MainForm";
			this.Resizable = false;
			this.ShowIcon = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.Style = MetroFramework.MetroColorStyle.Red;
			this.Text = "F1Public Loader";
			this.Theme = MetroFramework.MetroThemeStyle.Dark;
			this.Load += new System.EventHandler(this.Form1_Load);
			this.MainFlowpanel.ResumeLayout(false);
			this.CheckboxFlowpanel.ResumeLayout(false);
			this.CheckboxFlowpanel.PerformLayout();
			this.StatusFlowpanel.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private MetroFramework.Controls.MetroButton StartButton;
		private MetroFramework.Controls.MetroProgressBar LoadingBar;
		private MetroFramework.Controls.MetroCheckBox LocalCheckbox;
		private MetroFramework.Controls.MetroCheckBox LatestCheckbox;
		private MetroFramework.Controls.MetroLabel StatusLabel;
		private System.Windows.Forms.FlowLayoutPanel MainFlowpanel;
		private System.Windows.Forms.FlowLayoutPanel CheckboxFlowpanel;
		private System.Windows.Forms.FlowLayoutPanel StatusFlowpanel;
	}
}

