using System;
using System.Threading;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public class ProjectStarter
	{
		public MSProject tmp ;
		protected MemMan MM;
		protected int ExI ;
		static int ExN=0 ;
		public Thread t ;
		public bool result ;

		public ProjectStarter()
		{
			// 
			// TODO: Add constructor logic here
			//
			MM=new MemMan() ;
				result=true ;
				ExI=++ExN ;

		}

		public bool StartProc()
		{
			if(tmp==null) return false ;
			uint MemRequested, MemLockRequested	;
			t= new Thread(new ThreadStart(tmp.ForeWork)) ;
			t.Start() ;
			t.Join() ;
			if(!tmp.ForeWorkResult) return false ;
			MemRequested=tmp.MemRequested() ;
			MemLockRequested=tmp.MemLockRequested() ;

			MemMan.Compact() ;
			MemMan.MEMORYSTATUSEX MS ;
			MS=MemMan.MemoryStatus() ;
			try
			{
				MemMan.curProc.MaxWorkingSet=(IntPtr)MS.TotalPhys ;
				MemMan.curProc.MinWorkingSet=(IntPtr)(MemLockRequested+0x800000) ;
			}
			catch(Exception e)
			{
				Console.WriteLine("\n>>>>>>> "+e.Message) ;
				Console.WriteLine(">>>>>>> Can't take the ownership on "+((MemLockRequested+0x800000)/(1024*1024)).ToString("####0.0")+"Mb RAM") ;
				Console.WriteLine(">>>>>>> Probably the progamm is running not under an administrative account,\n>>>>>>> or too many other applications are running") ;
				Console.WriteLine(">>>>>>> or too many other applications are running.") ;
				Console.WriteLine(">>>>>>> Calculations will continue, but may slow down.") ;
			}
			
			
			if(!MM.Allocate(MemRequested)) 
			{ 
				Console.WriteLine(ExI+" ----> virtual allocation failed for "+MemRequested.ToString()+" bytes");
				return false;
			}

			// optionally lock the memory
			if(!MM.Lock(0,MemLockRequested))
			{
				Console.WriteLine(">>>>>>> Unable to lock the memory.") ;
				Console.WriteLine(">>>>>>> Calculations will continue, but will slow down.") ;
			}
			unsafe 
			{
				tmp.data=(double*)MM.BaseAddress ; 
			}
			t= new Thread(new ThreadStart(tmp.StartCalculations)) ;
			t.Start() ;
			t.Join() ;
			MM.Free() ;
			return true ;
			
		}
	
	}
}
