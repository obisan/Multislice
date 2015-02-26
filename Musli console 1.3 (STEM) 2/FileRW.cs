using System;
using System.Runtime.InteropServices;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	unsafe public class FileRW
	{
		uint fhandle ;
		#region WinApi imports
		[DllImport("kernel32")]
		static private extern uint WriteFile(
			uint hFile,
			void* lpBuffer,
			uint nNumberOfBytesToWrite,
			uint* lpNumberOfBytesWritten,
			uint lpOverlapped
			);
		[DllImport("kernel32")]
		static private extern uint ReadFile(
			uint hFile,
			void* lpBuffer,
			uint nNumberOfBytesToRead,
			uint* lpNumberOfBytesRead,
			uint lpOverlapped
			);
		[DllImport("kernel32")]
		static private extern uint SetFilePointerEx(
			uint hFile,
			long liDistanceToMove,
			long* lpNewFilePointer,
			uint dwMoveMethod
			);

		public enum SeekFrom:uint {Begin=0,Current=1,End=2};

		
		[DllImport("kernel32")]
		static private extern uint GetLastError();
		[DllImport("kernel32")]
		static private extern uint CloseHandle(uint handle);
		[DllImport("kernel32")]
		static private extern uint CreateFile( string lpFileName,
			uint dwDesiredAccess,
			uint dwShareMode,
			uint sec,
			uint dwCreationDisposition,
			uint dwFlagsAndAttributes,
			uint hTemplateFile
			);
		public enum Access:uint {Read=0x80000000,Write=0x40000000,Execute=0x20000000,All=0x10000000};
		public enum Action:uint {CreateAlways=2,CreateIfNotExist=1,OpenOrCreate=4,Open=3,TruncateExisting=5};
		#endregion
		/// <summary>
		/// Private constructor to forbid direct creation
		/// Creation is only allowed by Open method
		/// </summary>
		/// <param name="i">It's a Win filehandle</param>
		private FileRW(uint i)
		{
			fhandle=i ;
		}
		/// <summary>
		/// Finalizer: close a file if it was not closed before
		/// </summary>
		~FileRW()
		{
			if(fhandle>0) CloseHandle(fhandle) ;
		}
		static public FileRW Open(string FileName, Action act, Access acc)
		{
			uint tmphandle=CreateFile(FileName,(uint)acc,0,0,(uint)act,0x00000080,0) ;
			if(tmphandle==System.UInt32.MaxValue) return null ;
			return new FileRW(tmphandle);
		}

		public void Close()
		{
			if(fhandle>0) CloseHandle(fhandle) ;
			fhandle=0 ;
		}
		public uint Write(byte[] ino , uint nelem)
		{
			uint nwritten ;
			fixed(void* p=&ino[0] )
			{
				WriteFile(fhandle, p, nelem, &nwritten , 0 );
			}
			return nwritten ;
		}
		public uint Write(byte* ino , uint nelem)
		{
			uint nwritten ;
			WriteFile(fhandle, (void* )ino, nelem, &nwritten , 0 );
			return nwritten ;
		}

		public uint Write(double[,] ino , uint nelem)
		{
			uint nwritten ;
			fixed(void* p=&ino[0,0] )
			{
				WriteFile(fhandle, p, nelem*sizeof(double), &nwritten , 0 );
			}
			return nwritten ;
		}
		public uint Read(double[,] ino , uint nelem)
		{
			uint nwritten ;
			fixed(void* p=&ino[0,0] )
			{
				ReadFile(fhandle, p, nelem*sizeof(double), &nwritten , 0 );
			}
			return nwritten ;
		}
		public uint Write(double* ino , uint nelem)
		{
			uint nwritten ;
			WriteFile(fhandle, ino, nelem*sizeof(double), &nwritten , 0 );
			return nwritten ;
		}	
		public uint Read(double* ino , uint nelem)
		{
			uint nwritten ;
			ReadFile(fhandle, ino, nelem*sizeof(double), &nwritten , 0 );
			return nwritten ;
		}	
		public uint Read(byte* ino , uint nelem)
		{
			uint nwritten ;
			ReadFile(fhandle, ino, nelem, &nwritten , 0 );
			return nwritten ;
		}	
		public long Seek(SeekFrom fr,long nbytes)
		{
			long where;
			SetFilePointerEx(fhandle,nbytes,&where,(uint)fr) ;
			return where ;
		}
	}
}
