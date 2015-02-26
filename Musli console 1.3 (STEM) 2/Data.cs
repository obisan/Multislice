using System;
using System.IO;

namespace MusliDebug
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe class Data
	{
		string FileName ;
		public double* data ;
		public uint length ;

		public Data(string fname)
		{
			data=null ;
			FileName=fname ;
		}
		
		public bool Restore()
		{
			if(data==null) return false ;
			FileRW fN ;
			if((fN=FileRW.Open(FileName,FileRW.Action.Open,FileRW.Access.Read))!=null)
			{
				uint nw ;
				nw=fN.Read(data,length) ;
				fN.Close() ;
				if(nw!=length)
				{ 
					return false ;
				}
				return true ;
			}
			else 
			{
				return false ;
			}
		}
		public bool Store()
		{
			if(data==null) return false ;
			FileRW fN ;
			string lwd ;
			lwd=Path.GetDirectoryName(FileName) ;
			if(!Directory.Exists(lwd))
				try 
				{
					Directory.CreateDirectory(lwd) ;
				} 
				catch (Exception e) 
				{
					Console.WriteLine("Can't create directory "+lwd+":\n"+e.ToString());
					return false;
				} 
			if((fN=FileRW.Open(FileName,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
			{
				uint nw ;
				nw=fN.Write(data,length) ;
				fN.Close() ;
				if(nw!=length)
				{ 
					return false ;
				}
				return true ;
			}
			else 
			{
				return false ;
			}
		}
	}
}
