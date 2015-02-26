using System;
using System.Threading;

namespace MusliDebug
{
	/// <summary>
	/// 
	/// </summary>
	public class Starter
	{
		public Slice tmp ;
		protected MemMan MM;
		protected int ExI ;
		static int ExN=0 ;
		static uint MinWorkingMem=(uint)0x1000000 ; //minimal mem for large calcul, i.e. 1Kx1K should always run in RAM
		public Thread t ;

		public Starter()
		{
			// 
			// TODO: Add constructor logic here
			//
			MM=new MemMan() ;
			MemMan.MEMORYSTATUSEX MS ;
			MS=MemMan.MemoryStatus() ;
			MemMan.curProc.MaxWorkingSet=(IntPtr)MS.TotalPhys ;
			MemMan.curProc.MinWorkingSet=(IntPtr)((uint)MemMan.curProc.MaxWorkingSet*0.75) ;
			ExI=++ExN ;
		}

		public bool StartProc()
		{
			if(tmp==null) return false ;
			uint MemRequested, MemSupplied	;
			unsafe	{ MemRequested=tmp.M*tmp.N*(uint)2*sizeof(double) ; }
			
			if(!MM.Allocate(MemRequested)) 
			{ 
				Console.WriteLine(ExI+" ----> virtual allocation failed for "+MemRequested.ToString()+" bytes");
				return false;
				// тут зацикливается на светофор, если память занята просто другими экзекуторами
				// if(MemAlloc>=MemRequest) ....
			}
			Console.WriteLine(ExI+" ----> allocated "+MemRequested+" bytes, total "+MemMan.MemAlloc);

			if((MemSupplied=MM.LockAtLeast(0,MinWorkingMem,MemRequested))>=MemRequested)
			{   // установить параметры памяти в tmp
				Console.WriteLine(ExI+" ----> locked "+MemSupplied+" bytes, total "+MemMan.MemLock);
				unsafe 
				{
					tmp.sli=(double*)MM.LockAddress ; 
				}
				t= new Thread(new ThreadStart(tmp.StartCalculations)) ;
				t.Start() ;
				t.Join() ;
				tmp.FlushOld() ;
				MM.Free() ;
				return true ;
			}
			Console.WriteLine(ExI+" ----> failed to lock at least 16M out of "+MemRequested+" bytes");
			//здесь семафор, если память просто заблокирована другими экзекуторами
			// if(MemLock>=MinWorkingMem) ....
			return false ;
			
		}
	
	
	
	}
}
