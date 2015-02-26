using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.ComponentModel;


namespace MusliConsole
{
	/// <summary>
	/// Unsafe memory management wrapper
	/// </summary>
	unsafe public class MemMan
	{
		[DllImport("kernel32")]
		static private extern void GetSystemInfo(out SYSTEM_INFO SystemInfo	);
		
		[DllImport("kernel32")]
		static private extern bool GlobalMemoryStatusEx(out MEMORYSTATUSEX MStat);

		[DllImport("kernel32")]
		static private extern void* VirtualAlloc(
									void* lpAddress,
									uint dwSize,
									/*AllocationType*/uint flAllocationType,
									PageProtection flProtect
									);

		[DllImport("kernel32")]
		static private extern bool VirtualFree(
									void* lpAddress,
									uint dwSize,
									AllocationType dwFreeType
									);

		[DllImport("kernel32")]
		static private extern bool VirtualLock(
									void* lpAddress,
									uint dwSize
									);
		[DllImport("kernel32")]
		static private extern bool VirtualUnlock(
									void* lpAddress,
									uint dwSize
									);

		[DllImport("kernel32")]
		static private extern uint GetProcessHeap();

		[DllImport("kernel32")]
		static private extern uint HeapCompact(
									uint hHeap,
									uint dwFlags
									);
		[DllImport("kernel32")]
		static private extern void CopyMemory(
									void* Destination,
									void* Source,
									uint Length
									);

		[DllImport("kernel32")]
		static private extern void ZeroMemory(
									void* Destination,
									uint Length
									);
		[DllImport("kernel32")]
		static private extern bool HeapWalk(
									uint hHeap,
									out PROCESS_HEAP_ENTRY HeapEntr
									);

		[DllImport("kernel32")]
		static private extern uint VirtualQuery(
									void* lpAddress,
									out MEMORY_BASIC_INFORMATION lpBuffer,
									uint dwLength
									);
		[DllImport("kernel32")]
		static private extern void* HeapAlloc(
									uint hHeap,
									uint dwFlags,
									uint dwBytes
									);




		/// <summary>
		/// Static constructor to provide information services
		/// </summary>
		static MemMan()
		{
			GetSystemInfo(out SI) ;
			curProc = Process.GetCurrentProcess();
		}
		~MemMan()
		{
			Unlock() ;
			Free() ;
		}
		public static uint MemAlloc  ;
		public static uint MemLock ;

		#region System information properties
		static SYSTEM_INFO SI;
		static public string GetCurrentVersion()
		{
			return curProc.MainModule.FileVersionInfo.ProductVersion ;
		}
		static public uint NProc
		{
			get
			{
				return SI.NumberOfProcessors ;
			}
		}
		static public uint TotalMem
		{
			get
			{
				return SI.MaximumApplicationAddress-SI.MinimumApplicationAddress ;
			}
		}
		static public uint PageSize
		{
			get
			{
				return SI.PageSize ;
			}
		}
		static public uint Granularity
		{
			get
			{
				return SI.AllocationGranularity ;
			}
		}
		[StructLayout(LayoutKind.Sequential)]
			struct SYSTEM_INFO 
		{  
			public ushort ProcessorArchitecture;
			ushort Reserved;
			public uint PageSize;
			public uint MinimumApplicationAddress;
			public uint MaximumApplicationAddress;
			public uint ActiveProcessorMask;
			public uint NumberOfProcessors;
			uint ProcessorType;
			public uint AllocationGranularity;
			public ushort ProcessorLevel;
			public ushort ProcessorRevision;
		} 

		[StructLayout(LayoutKind.Sequential)]
			struct BLOCK
		{      
			public uint hMem;
			public uint dwReserved1;
			public uint dwReserved2;
			public uint dwReserved3;
		} 
		[StructLayout(LayoutKind.Sequential)]
			struct REGION
		{      
			public uint dwCommittedSize;
			public uint dwUnCommittedSize;
			public uint lpFirstBlock;
			public uint lpLastBlock;
		} 		
		[StructLayout(LayoutKind.Explicit)]
			struct DATA 
			{   
				[FieldOffset(0)] public BLOCK Block ;
				[FieldOffset(0)] public REGION Region ;
			}
		[StructLayout(LayoutKind.Sequential)]
			struct PROCESS_HEAP_ENTRY
		{  
			public void* lpData;
			public uint cbData;
			public byte cbOverhead;
			public byte iRegionIndex;
			public ushort wFlags;  
			public DATA data ;
		} 
		
		[StructLayout(LayoutKind.Sequential)]
			struct MEMORY_BASIC_INFORMATION 
		{
			public void* BaseAddress;
			public void* AllocationBase;
			public uint AllocationProtect;
			public uint RegionSize;
			public uint State;
			public uint Protect;
			public uint Type;
		}


		#endregion

		#region Unsafe memory status call and structure
		static public MEMORYSTATUSEX MemoryStatus() 
		{
			MEMORYSTATUSEX MS ;
			MS.Length=(uint)sizeof(MEMORYSTATUSEX) ;
			GlobalMemoryStatusEx(out MS);
			return MS ;
		}

		static public uint Compact()
		{
			return HeapCompact(GetProcessHeap(),0x00000001) ;
		}

		static public void HeapSearch()
		{
			PROCESS_HEAP_ENTRY EntrInfo ;
			int i=0 ;
			uint nbytes=0;
			EntrInfo.wFlags=1 ;
			EntrInfo.lpData=null ;
			while(HeapWalk(GetProcessHeap(),out EntrInfo))
			{
				//EntrInfo.wFlags=1 ;
				i++ ;
				nbytes+=EntrInfo.cbData+EntrInfo.cbOverhead ;
				Console.WriteLine("\n"+i+"  "+EntrInfo.cbData+"  "+nbytes+" "+EntrInfo.wFlags) ;
			}

		}
		static public uint GetHeap()
		{
			return GetProcessHeap() ;
		}

		static public void ScanVirtualMemory()
		{
			MEMORY_BASIC_INFORMATION EntrInfo ;
			int i=0 ;
			uint nbytes=0;
			while(VirtualQuery((void*)nbytes,out EntrInfo,(uint)sizeof(MEMORY_BASIC_INFORMATION))>0)
			{
				//EntrInfo.wFlags=1 ;
				i++ ;
				nbytes+=EntrInfo.RegionSize ;
				Console.WriteLine("\n"+i+"  "+EntrInfo.RegionSize+"  "+nbytes+" ") ;
			}

		}

		static public void CopyData(double* from, double* to, uint howmuch)
		{
			CopyMemory((void*)to,(void*)from,howmuch*sizeof(double)) ;
		}
		static public void ZeroData(double* data, uint num)
		{
			ZeroMemory((void*)data, (uint)(sizeof(double)*num)); 
		}
		[StructLayout(LayoutKind.Sequential)]
		public struct MEMORYSTATUSEX
		{
			public uint Length;
			public uint MemoryLoad;
			public ulong TotalPhys;
			public ulong AvailPhys;
			public ulong TotalPageFile;
			public ulong AvailPageFile;
			public ulong TotalVirtual;
			public ulong AvailVirtual;
			public ulong AvailExtendedVirtual;
		} 
		#endregion

		/// <summary>
		/// Holds a current process to access it's properties
		/// </summary>
		static public Process curProc ;

		#region Nonstatic members -> memory allocation

		void* BaseAddr=null, LockAddr=null ;
		uint BaseSize, LockSize ;
		public void* BaseAddress
		{
			get
			{
				return BaseAddr ;
			}
		}
		public void* LockAddress
		{
			get
			{
				return LockAddr ;
			}
		}
		public uint PhysSize
		{
			get
			{
				return LockSize ;
			}
		}
		public uint VirtSize
		{
			get
			{
				return BaseSize ;
			}
		}

		enum AllocationType:uint
		{
			Commit=0x1000,Reserve=0x2000,
			Decommit=0x4000,Release=0x8000,Free=0x10000,
			Private=0x20000,Mapped=0x40000,Reset=0x80000,
			TopDown=0x100000
		};
		enum PageProtection:uint
		{
			PAGE_NOACCESS=0x01,
			PAGE_READONLY=0x02,     
			PAGE_READWRITE=0x04,     
			PAGE_WRITECOPY=0x08,     
			PAGE_EXECUTE=0x10,     
			PAGE_EXECUTE_READ=0x20,     
			PAGE_EXECUTE_READWRITE=0x40,     
			PAGE_EXECUTE_WRITECOPY=0x80,     
			PAGE_GUARD=0x100,     
			PAGE_NOCACHE=0x200,     
			PAGE_WRITECOMBINE=0x400 
		};

		public bool Allocate(uint Size)
		{	if(BaseAddr!=null) return false;
			BaseAddr=VirtualAlloc(null,Size,(uint)AllocationType.Commit,PageProtection.PAGE_READWRITE	);
			if(BaseAddr==null) return false ;
			BaseSize=Size ;
			MemAlloc+=Size ;
			return true ;
		}
		public bool AllocateFromHeap(uint Size)
		{
				if(BaseAddr!=null) return false;
			BaseAddr=HeapAlloc(GetProcessHeap(),0,Size);
			if(BaseAddr==null) return false ;
			BaseSize=Size ;
			MemAlloc+=Size ;
			return true ;
		}

		public bool AllocateAt(uint From, uint Size)
		{
				if(BaseAddr!=null) return false;
			BaseAddr=VirtualAlloc((void*)From,Size,(uint)AllocationType.Commit,PageProtection.PAGE_READWRITE	);
			if(BaseAddr==null) return false ;
			BaseSize=Size ;
			MemAlloc+=Size ;
			return true ;
		}

		public uint AllocateAtLeast(uint leastSize, uint requestSize)
		{	
			if(BaseAddr!=null) return 0 ;
			bool result ;
			do // attempt to allocate as much allocated memory as system will allow
			{   
				result=Allocate(requestSize) ;
				if(!result) requestSize/=2 ;	
			} while((!result) && (requestSize>=leastSize)) ;
			return BaseSize ;
		}

		public bool Free()
		{	if(BaseAddr==null) return false ;
			bool result=VirtualFree(BaseAddr,0,AllocationType.Release) ;
			if(result) { BaseAddr=LockAddr=null ; MemAlloc-=BaseSize ; MemLock-=LockSize ; BaseSize=LockSize=0 ; }
			return result ;
		}
	
		public bool Lock(uint Offset,uint Size)
		{
			if(BaseAddr==null) return false ;
			if(LockAddr!=null) return false ;
			LockAddr=(void*)((byte*)BaseAddr+Offset) ;
			LockSize=Size ;
			bool result=VirtualLock(LockAddr,LockSize) ;
			if(!result) { LockAddr=null ; LockSize=0 ; }
			MemLock+=LockSize ;
			return result ;
		}

		public uint LockAtLeast(uint Offset,uint leastSize, uint requestSize)
		{
			if(BaseAddr==null) return 0 ;
			if(LockAddr!=null) return 0 ;
			LockAddr=(void*)((byte*)BaseAddr+Offset) ;
			LockSize=requestSize ;
			bool result ;
			do // attempt to lock as much allocated memory as system will allow
			{   
			 result=VirtualLock(LockAddr,LockSize) ;
				if(!result) LockSize/=2 ;	
			} while((!result) && (LockSize>=requestSize)) ;
			if(!result) { LockAddr=null ; LockSize=0 ; }
			MemLock+=LockSize ;
			return LockSize ;
		}
		public bool Unlock()
		{
			if(BaseAddr==null) return false ;
			if(LockAddr==null) return false ;
			bool result=VirtualUnlock(LockAddr,LockSize) ;
			if(result) { MemLock-=LockSize ; LockAddr=null ; LockSize=0 ; }
			
			return result ;
		}	
		#endregion

	}
}
