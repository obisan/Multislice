using System;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public class MusliRandom : System.Random
	{
		public double Mean,StDevX,StDevY ;
		public MusliRandom()
		{
			Mean=0 ;
			StDevX=StDevY=1 ;
		}
		public MusliRandom(int seed): base(seed)
		{
			Mean=0 ;
			StDevX=StDevY=1 ;
		}
		public double NextGauss12()
		{
			return ((NextDouble()+NextDouble()+
				NextDouble()+NextDouble()+
				NextDouble()+NextDouble()+
				NextDouble()+NextDouble()+
				NextDouble()+NextDouble()+
				NextDouble()+NextDouble())-6)*StDevX+Mean ;

		}
		public double NextGauss()
		{
			double s,x,y;
			do
			{
				x=NextDouble()*2.0-1.0 ;
				y=NextDouble()*2.0-1.0 ;
				s=x*x+y*y ;
			} while(s>=1||s==0) ;
			s=Math.Sqrt(-2.0*Math.Log(s)/s);
			return s*x*StDevX+Mean ;
		}
		public void Vobble(ref double xx,ref double yy)
		{
			double s,x,y;
			do
			{
				x=NextDouble()*2.0-1.0 ;
				y=NextDouble()*2.0-1.0 ;
				s=x*x+y*y ;
			} while(s>=1||s==0) ;
			s=Math.Sqrt(-2.0*Math.Log(s)/s);
			xx+=x*s*StDevX ;
			yy+=y*s*StDevY ;
		}
	}
}
