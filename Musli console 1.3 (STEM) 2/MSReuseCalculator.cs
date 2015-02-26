using System;
using System.Collections;
using System.Runtime.InteropServices;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe class MSReuseCalculator : MSCalculator
	{
		public double*[] SliList ;
		public double*[] PropList ;
		public ArrayList ThSlices ; // thickness of each slice

		public MSReuseCalculator()
		{
		}
		override public bool RunCalculation()
		{
			double* buf1, buf2 ;
			double wr,wi ;
			double StartThickness=CurThickness ;
			DateTime start ;
			TimeSpan period ;
			int i, iSlice=0, iWave=0 ;
			if((ThWaves==null)||(ThWaves.Length==0)) return true ;
			
			if((double)ThWaves[iWave]<=CurThickness)
			{
				bool result=((LastSavedWave=SaveWav(CurThickness))!=null) ;
				iWave++ ;
				Console.WriteLine("\rWave at "+CurThickness.ToString("######0.00")+"A unloaded "+result+"                             ") ;
			}
			start=DateTime.Now ;
			while(iWave<ThWaves.Length)
			{
				iSlice=(int)(iSlice%NSlices);
				
				//Apply PG
				buf1=data ;
				buf2=SliList[iSlice] ;
				for(i=0;i<M*N*2;i+=2)
				{
					wr=*buf1 ;
					wi=*(buf1+1) ;
					*buf1=wr**buf2-wi**(buf2+1) ;
					*(buf1+1)=wr**(buf2+1)+wi**buf2 ;
					buf1++ ; buf1++ ;
					buf2++ ; buf2++ ;
				}

				//Apply Propagator
				SFT_direct.MakeSFT(data) ;
				buf1=data ;
				buf2=PropList[iSlice] ;
				for(i=0;i<M*N*2;i+=2)
				{
					wr=*buf1 ;
					wi=*(buf1+1) ;
					*buf1=wr**buf2-wi**(buf2+1) ;
					*(buf1+1)=wr**(buf2+1)+wi**buf2 ;
					buf1++ ; buf1++ ;
					buf2++ ; buf2++ ;
				}
				CurThickness+=(double)ThSlices[iSlice] ;
				if((double)ThWaves[iWave]<=CurThickness)
				{
					if(IsSaveSTEM)
					{
						SaveSTEM(iWave) ;
					}
					if(IsSaveDiffractions)
					{
						Console.WriteLine("\rDiffraction at "+CurThickness.ToString("######0.00")+"A unloaded "+(SaveDiffraction(CurThickness)!=null)+"                             ") ;
					}

				}
				SFT_backward.MakeSFT(data) ;
				period=DateTime.Now-start ;
				if(!IsSaveSTEM) Console.Write("\rPropagated "+CurThickness.ToString("######0.00")+"A at a speed of "+((double)((CurThickness-StartThickness)*TimeSpan.TicksPerSecond/period.Ticks*3600)).ToString("###0.000")+"A/h     ") ;
				if((double)ThWaves[iWave]<=CurThickness)
				{
					iWave++ ;
					if(IsSaveWaves)
					{
						Console.WriteLine("\rWave at "+CurThickness.ToString("######0.00")+"A unloaded "+((LastSavedWave=SaveWav(CurThickness))!=null)+"                             ") ;
					}

				}
				iSlice++ ;
			}
			return true ;
		}
	}
}
