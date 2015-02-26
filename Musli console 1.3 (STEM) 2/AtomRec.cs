using System;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	class AtomRec : IComparable
	{
		public double x,y,z,DW,ads,occ ;
		public int type ;
		public int sn ; // slice number
		public int type_ ; // number of potential curve
		public AtomRec () {} 
		public AtomRec (double X, double Y, double Z, double TYPE, double DW_, double ADS)
		{
			x=X;y=Y;z=Z;type=(int)TYPE;DW=DW_;ads=ADS;occ=1 ;
		}
		public AtomRec (double X, double Y, double Z, double TYPE, double DW_, double ADS, double OCC)
		{
			x=X;y=Y;z=Z;type=(int)TYPE;DW=DW_;ads=ADS;occ=OCC ;
		}
		public int CompareTo(object obj)
		{
			if(this.z<((AtomRec)obj).z) return -1 ;
			if(this.z==((AtomRec)obj).z) return 0 ;
			return 1 ;
		}

	}
}
