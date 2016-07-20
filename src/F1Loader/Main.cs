using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace F1Loader
{
	sealed class TempFile : IDisposable
	{
		string path;
		public TempFile() : this(System.IO.Path.GetTempFileName()) { }

		public TempFile(string path)
		{
			if (string.IsNullOrEmpty(path)) throw new ArgumentNullException("path");
			this.path = path;
		}
		public string Path
		{
			get
			{
				if (path == null) throw new ObjectDisposedException(GetType().Name);
				return path;
			}
		}
		~TempFile() { Dispose(false); }
		public void Dispose() { Dispose(true); }
		private void Dispose(bool disposing)
		{
			if (disposing)
			{
				GC.SuppressFinalize(this);
			}
			if (path != null)
			{
				try { File.Delete(path); }
				catch { } // best effort
				path = null;
			}
		}
	}

	internal static class Main
	{
		//[DllImport("Crypt32.dll")]
		//public static extern bool CryptProtectData(ref BLOB pDataIn,
		//											string szDataDescr,
		//											ref BLOB pOptionalEntropy,
		//											ref int pvReserved,
		//											ref int pPromptStruct,
		//											long dwFlags,
		//											ref BLOB pDataOut);

		private static PathDllInjector pathInjector = PathDllInjector.GetInstance;

		private static ByteDllInjector byteInjector = ByteDllInjector.GetInstance;

		private static string fileName { get; set; }

		static public async Task<DllInjectionResult> waitForInjectionPath(string procName, string path)
		{
			return await Task.Run(() =>
			{
				DllInjectionResult res;
				while (true)
				{
					res = pathInjector.Inject(procName, path);
					if (res != DllInjectionResult.GameProcessNotFound)
						break;
				}
				return res;
			});
		}

		static public async Task<DllInjectionResult> waitForInjectionByte(string procName, byte[] bytes)
		{
			return await Task.Run(() =>
			{
				DllInjectionResult res;
				while (true)
				{
					res = byteInjector.Inject(procName, bytes);
					if (res != DllInjectionResult.GameProcessNotFound)
						break;
				}
				return res;
			});
		}

		static public string GetFile(bool openFile)
		{
			if(openFile)
			{
				OpenFileDialog file = new OpenFileDialog();

				file.Multiselect = false;
				file.CheckFileExists = true;
				file.CheckPathExists = true;
				file.Title = "Select F1Public Dll";
				file.Filter = "F1Public Hack (*.dll)|*.dll";
				file.ShowDialog();

				return file.FileName;
			}
			else
			{
				return Path.GetTempFileName();
			}
		}

		//static public byte[] GetFile()
		//{
		//	byte[] dll;

		//	string dll2 = (string)Properties.Resources.basehook.ToString();

		//	dll = Encoding.ASCII.GetBytes(dll2);

		//	return dll;
		//}
	}
}