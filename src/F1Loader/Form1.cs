using System;
using System.IO;
using System.Windows.Forms;

namespace F1Loader
{
	public partial class MainForm : MetroFramework.Forms.MetroForm
	{
		private string fileName = "";

		private bool tempFile = false;

		private string statusPrefix = "Status: ";

		private string[] statuses =
		{
			"Idle",
			"Loading embedded DLL",
			"Opening F1Public",
			"Waiting for TF2",
			"Injecting Hack",
			"Done!",
			"Invalid file Selected",
			"FAILED!"
		};

		private bool readyToInject;

		public MainForm()
		{
			InitializeComponent();

			LoadingBar.Step = 1;
			readyToInject = false;
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			StatusLabel.Text = statusPrefix + statuses[0];
		}

		private void LoadingBar_Click(object sender, EventArgs e)
		{
			LoadingBar.PerformStep();
		}

		private void LocalCheckbox_CheckedChanged(object sender, EventArgs e)
		{
			LatestCheckbox.Checked = !LocalCheckbox.Checked;
		}

		private void LatestCheckbox_CheckedChanged(object sender, EventArgs e)
		{
			LocalCheckbox.Checked = !LatestCheckbox.Checked;
		}

		private async void StartButton_Click(object sender, EventArgs e)
		{
			LoadingBar.Step = 0;
			LoadingBar.ProgressBarStyle = ProgressBarStyle.Marquee;

			if (LatestCheckbox.Checked)
			{
				// embedded resource
				StatusLabel.Text = statusPrefix + statuses[1];

				// call it to create a temporary file
				fileName = Main.GetFile(false);
				File.WriteAllBytes(fileName, F1Loader.Properties.Resources.basehook);
				tempFile = true;
			}
			else
			{
				// Local Install
				StatusLabel.Text = statusPrefix + statuses[2];

				// this should just be synchrounous
				fileName = Main.GetFile(true);

				if (fileName == "")
				{
					StatusLabel.Text = statusPrefix + statuses[6];
					return;
				}
			}

			LoadingBar.ProgressBarStyle = ProgressBarStyle.Continuous;
			LoadingBar.Step = 100;
			LoadingBar.PerformStep();

			if (!tempFile)
				StatusLabel.Text = statusPrefix + "Loaded file: " + fileName;
			else
				StatusLabel.Text = statusPrefix + "Loaded Embedded File";

			StartButton.Enabled = false;

			DllInjectionResult res;

			LoadingBar.Step = -100;
			LoadingBar.PerformStep();

			StatusLabel.Text = statusPrefix + statuses[4];
			LoadingBar.ProgressBarStyle = ProgressBarStyle.Marquee;


			res = await Main.waitForInjectionPath("hl2", fileName);

			switch (res)
			{
				case DllInjectionResult.DllNotFound:
					StatusLabel.Text = statusPrefix + statuses[6];
					break;

				case DllInjectionResult.GameProcessNotFound:
					return;
					break;

				case DllInjectionResult.InjectionFailed:
					StatusLabel.Text = statusPrefix + statuses[7];
					break;

				case DllInjectionResult.Success:
					StatusLabel.Text = statusPrefix + statuses[5];
					break;

				default:
					break;
			}

			LoadingBar.Step = 100;
			LoadingBar.PerformStep();
			LoadingBar.ProgressBarStyle = ProgressBarStyle.Continuous;
		}
	}
}