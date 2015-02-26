using System;
using System.Collections;
using System.Runtime.InteropServices;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe class MSUseOnceCalculator : MSCalculator
	{

		#region Lib imports

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_DW_cutpot(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_TV_cutpot(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_DT_DW_cutpot(double a,double b,double gam, double E,double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* dw, double* ads, double* occ,int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_DT_TV_cutpot(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads,double* occ, int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_DW_periodic(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_DT_DW_periodic(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* dw, double* ads, double* occ,int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_TV_periodic(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads,double* occ, int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_DT_TV_periodic(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads,double* occ, int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int ApplyPropagator(double a,double b,double gam,double thickness, double E,
			double Xtilt, double Ytilt, 
			uint N,uint M,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_DW_cutpot_oneatom(double a,double b,double gam, double K, double atrad,
			uint N,uint M,
			double xx,double yy, double ads, double occ,
			double *scatentry,uint Npoints, double dds,
			double* gli) ;

		#endregion

		double[] x,y,z,DW,ads,occ ;
		int[] type, type_ ;
		double CurSliThickness ;
		uint NAtoms ;
		public string SliceFilename ;
		public MSProject.PotTypes PotType ;
		public double atrad,dela,ds ;
		public uint CurveLength, NTypes ;
		public double* CurveHolder ;
		public double X_tilt_crystal, Y_tilt_crystal ;

		/// <summary>
		/// variable for pullZ potential
		/// </summary>
		uint nn;
		uint mm ;
		double *buf ;
		double lambda ; 
		double K ;
		SFT atSFT_forward ;
		SFT atSFT_backward ;
		MemMan atM ;
		int hanwidth ;
		double[] hanning ;
		uint nninner, mminner ;
		int offix,offiy ;



		public MSUseOnceCalculator()
		{
			// 
			// TODO: Add constructor logic here
			//
			nn=1;
			mm=1 ;
			buf=null ;
			K=0 ;
			atSFT_forward=null ;
			atSFT_backward=null ;
			atM=null ;
		}

		override public bool RunCalculation()
		{
			float progress ;
			DateTime start ;
			TimeSpan period ;
			double StartThickness=CurThickness ;

			int i, iSlice=0, iWave=0 ;

			if((ThWaves==null)||(ThWaves.Length==0)) return true ;
			if((double)ThWaves[iWave]<=CurThickness)
			{
				bool result=((LastSavedWave=SaveWav(CurThickness))!=null) ;
				iWave++ ;
				Console.WriteLine("\rWave at "+CurThickness.ToString("######0.00")+"A unloaded "+result+"                             ") ;
			}
			if((PotType==MSProject.PotTypes.PG_curve_TV_pullZ)||(PotType==MSProject.PotTypes.PG_curve_DW_pullZ))
			{
				hanwidth=8 ;
				hanning=new double[hanwidth] ;
				nn=Math.Min(N,(uint)Math.Pow(2,Math.Ceiling(Math.Log(((atrad+0.5)/a*N+hanwidth)/(1.0-2.0/hanwidth),2))+1)) ;
				mm=Math.Min(M,(uint)Math.Pow(2,Math.Ceiling(Math.Log(((atrad+0.5)/b*M+hanwidth)/(1.0-2.0/hanwidth),2))+1)) ;
				atSFT_forward=new SFT() ;
				atSFT_backward=new SFT() ;
				atM=new MemMan() ;
				if(!atM.Allocate(nn*mm*2*sizeof(double))) return false;
				if(!atM.Lock(0,nn*mm*2*sizeof(double))) return false;
				buf=(double*)atM.LockAddress ;
				atSFT_forward.MakePlan(buf,nn,mm,1) ;
				atSFT_backward.MakePlan(buf,nn,mm,-1) ;
				lambda=0.387818/Math.Sqrt(E/1000.0*(1.0+0.978459e-3*E/1000)) ; // E in V
				K=lambda*Math.Sqrt(1.0+Math.Pow((0.0242621/lambda),2)) ;
				nninner=(uint)Math.Min(nn,(uint)(atrad)/a*N*2+Math.Max(1,nn/2-hanwidth-(uint)(atrad)/a*N)) ;
				mminner=(uint)Math.Min(mm,(uint)(atrad)/b*M*2+Math.Max(1,mm/2-hanwidth-(uint)(atrad)/b*M)) ;
//				nninner=(uint)Math.Min(nn,(uint)(atrad)/a*N*2) ;
//				mminner=(uint)Math.Min(mm,(uint)(atrad)/b*M*2) ;
				offix=(int)(nn-nninner) ;
				offiy=(int)(mm-mminner)/2 ;
				for(i=0; i<hanwidth;i++)
					hanning[i]=(Math.Cos(i*Math.PI/hanwidth)+1)*0.5 ;

			}

			start=DateTime.Now ;
			while(iWave<ThWaves.Length)
			{
				iSlice=(int)(iSlice%NSlices)+1;

				
				// Load Slice
				if(!LoadSlice(iSlice)) return false ;
				// apply Phase Gratting
				if(NAtoms>0)
				{
					if((PotType==MSProject.PotTypes.PG_DT_TV_periodic)||
						(PotType==MSProject.PotTypes.PG_DT_TV_cut)||
						(PotType==MSProject.PotTypes.PG_curve_TV_periodic)||
						(PotType==MSProject.PotTypes.PG_curve_TV_cut)||
						(PotType==MSProject.PotTypes.PG_curve_TV_pullZ))
					{	// некорректно работает с gam!=90
						MusliRandom vibr=new MusliRandom() ;
						for(i=0;i<NAtoms;i++)
						{
							x[i]*=a;y[i]*=b ;
							vibr.StDevX=vibr.StDevY=Math.Sqrt(DW[i]*0.5)/(2.0*Math.PI) ;
							vibr.Vobble(ref x[i],ref y[i]) ;
							if(PotType==MSProject.PotTypes.PG_curve_TV_pullZ) z[i]+=vibr.NextGauss() ;
							if(x[i]<0) x[i]+=a ;
							if(x[i]>=a) x[i]-=a ;
							if(y[i]<0) y[i]+=b ;
							if(y[i]>=b) y[i]-=b ;

						}

					}
					fixed(double* _x=x,_y=y,_dw=DW,_ads=ads,_occ=occ)
					{
						fixed(int* _type_=type_,_type=type)  
						{
							switch(PotType)
							{
								case MSProject.PotTypes.PG_DT_DW_periodic    : 
									PG_rs_DT_DW_periodic(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_DT_DW_cut         :
									PG_rs_DT_DW_cutpot(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_curve_DW_periodic : 
									PG_rs_curve_DW_periodic(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,CurveHolder,CurveLength,dela,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_curve_DW_cut      : 
									PG_rs_curve_DW_cutpot(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,CurveHolder,CurveLength,dela,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_DT_TV_periodic    : 
									PG_rs_DT_TV_periodic(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_DT_TV_cut         :
									PG_rs_DT_TV_cutpot(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_curve_TV_periodic : 
									PG_rs_curve_TV_periodic(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,CurveHolder,CurveLength,dela,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_curve_TV_cut      : 
									PG_rs_curve_TV_cutpot(a,b,gam,E,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,CurveHolder,CurveLength,dela,data,&progress) ;
									break ;
								case MSProject.PotTypes.PG_curve_DW_pullZ :
									PG_rs_pullZ() ;
									break ;
								case MSProject.PotTypes.PG_curve_TV_pullZ :
									PG_rs_pullZ() ;
									break ;
								default :
									Console.WriteLine("Phase Gratting " +PotType.ToString()+" is not supported in "+this.ToString()) ;
									break ;
							}
						}
					}
				}

				//Apply Propagator
				SFT_direct.MakeSFT(data) ;
				ApplyPropagator(a, b, gam, CurSliThickness, E, X_tilt_crystal, Y_tilt_crystal,  N, M, data, &progress) ;
				CurThickness+=CurSliThickness ;
				if((double)ThWaves[iWave]<=CurThickness)
				{
					if(IsSaveSTEM)
					{
						SaveSTEM(iWave) ;
					}

					if(IsSaveDiffractions)
					{
						Console.WriteLine("\rDiffraction at "+CurThickness.ToString("######0.00")+"A unloaded "+(SaveDiffraction(CurThickness)!=null)+"                                   ") ;
					}

				}
				SFT_backward.MakeSFT(data) ;
				period=DateTime.Now-start ;
				if(!IsSaveSTEM) Console.Write("\rPropagated "+NAtoms+" atoms, at total "+CurThickness.ToString("######0.00")+"A at a speed of "+((double)((CurThickness-StartThickness)*TimeSpan.TicksPerSecond/period.Ticks*3600)).ToString("###0.0")+"A/h     ") ;
				if((double)ThWaves[iWave]<=CurThickness)
				{
					iWave++ ;
					if(IsSaveWaves)
					{
						Console.WriteLine("\rWave at "+CurThickness.ToString("######0.00")+"A unloaded "+((LastSavedWave=SaveWav(CurThickness))!=null)+"                                   ") ;
					}

				}


			} // calculation cycle
			return true ;
		}

		protected bool LoadSlice(int iSlice)
		{
			string lwd ;
			FileRW fN ;
			uint nr ;

			lwd=SliceFilename+iSlice.ToString("00000")+".slice_coo" ;
			if((fN=FileRW.Open(lwd,FileRW.Action.Open,FileRW.Access.Read))!=null)
			{
				fixed(uint* nat=&NAtoms)
					fixed(double* cuth=&CurSliThickness)
					{
						if(fN.Read((byte*)nat,sizeof(int))!=sizeof(int)) return false ;
						if(fN.Read(cuth,1)!=sizeof(double)) return false ;
					}
				if(NAtoms>0)
				{
					x=new double[(int)NAtoms] ;
					y=new double[(int)NAtoms] ;
					z=new double[(int)NAtoms] ;
					DW=new double[(int)NAtoms] ;
					ads=new double[(int)NAtoms] ;
					occ=new double[(int)NAtoms] ;
					type=new int[(int)NAtoms] ;
					type_=new int[(int)NAtoms] ;

					nr=0 ;
					fixed(double* xx=x,yy=y,zz=z,DDW=DW, aads=ads,oocc=occ)
						fixed(int* t=type,t_=type_)
						{
							nr+=fN.Read(xx,NAtoms) ;
							nr+=fN.Read(yy,NAtoms) ;
							nr+=fN.Read(zz,NAtoms) ; // z in Angstrems !!!!
							nr+=fN.Read(DDW,NAtoms) ;
							nr+=fN.Read(aads,NAtoms) ;
							nr+=fN.Read(oocc,NAtoms) ;
							nr+=fN.Read((byte*)t,sizeof(int)*NAtoms) ;
							nr+=fN.Read((byte*)t_,sizeof(int)*NAtoms) ;
						}
					if(nr!=NAtoms*(6*sizeof(double)+2*sizeof(int)))
					{ // add code to find and display the reason
						Console.WriteLine("Only "+nr+" bytes are read from"+lwd);
						fN.Close() ;
						return false ;
					}
				}
			}
			else return false ;

			fN.Close() ;
			return true;
		}

		
		void PG_rs_pullZ()
		{
			int offsx, offsy ;
			uint N2=N*2;
			int i,j,k ;
			double* ddata, bbuf ;
			double* curk ;
			double xx,yy ;
			double dxa=a/N ;
			double dyb=b/M ;
			double aa=dxa*nn,bb=dyb*mm ;
            float progress ;

			for(k=0;k<NAtoms;k++)
			{
				if(PotType==MSProject.PotTypes.PG_curve_DW_pullZ)
				{ 
					xx=x[k]*a ;
					yy=y[k]*b ;
				}
				else
				{ 
					xx=x[k] ;
					yy=y[k] ;
				}

				offsx=(int)(N*xx/a-nn/2) ;
				offsy=(int)(M*yy/b-mm/2) ;
				xx=(xx-a/N*offsx)/aa ;
				yy=(yy-b/M*offsy)/bb ;
				offsx=(int)(offsx*2) ;
				offsy=(int)(offsy+M) ;
				curk=(CurveHolder+type_[k]*CurveLength) ;
							
				for(j=0;j<mm;j++)
				{
					bbuf=buf+j*nn*2 ;
					ddata=data+(int)(((offsy+j)%M)*N2) ;
					if(offsx<0)
					{
						MemMan.CopyData(ddata+N2+offsx,bbuf,(uint)(-offsx)) ;
						MemMan.CopyData(ddata,bbuf-offsx,(uint)(nn*2+offsx)) ;

					}
					else
						if(offsx+nn*2>=N2)
					{
						MemMan.CopyData(ddata+offsx,bbuf,(uint)(N2-offsx)) ;
						MemMan.CopyData(ddata,bbuf+N2-offsx,(uint)(nn*2-(N2-offsx))) ;

					}
					else
						MemMan.CopyData(ddata+offsx,bbuf,nn*2) ;
				}
				if(Math.Abs(z[k])>0.001)
				{
					// Hanning window 
					for(j=0;j<mm;j++)
					{
						bbuf=buf+j*nn*2 ;
						ddata=buf+j*nn*2+nn*2-1 ;
						for(i=0;i<hanwidth*2;i++)
						{
							*(bbuf+i)*=(1-hanning[i/2]) ;
							*(ddata-i)*=(hanning[hanwidth-1-i/2]) ;
						}
					}
					for(j=0;j<hanwidth;j++)
					{
						bbuf=buf+j*nn*2 ;
						ddata=buf+(mm-j-1)*nn*2 ;
						for(i=0;i<nn*2;i++)
						{
							*(bbuf+i)*=(1-hanning[j]) ;
							*(ddata+i)*=(hanning[hanwidth-1-j]) ;
						}
					}
					atSFT_forward.MakeSFT(buf) ;
					ApplyPropagator(aa, bb, gam, z[k], E, X_tilt_crystal, Y_tilt_crystal, nn, mm, buf, &progress) ;
					atSFT_backward.MakeSFT(buf) ;
				}

				PG_rs_curve_DW_cutpot_oneatom(aa,bb,gam, K, atrad,nn,mm, xx, yy,ads[k], occ[k],curk,CurveLength, dela, buf) ;

				if(Math.Abs(z[k])>0.001)
				{
					atSFT_forward.MakeSFT(buf) ;
					ApplyPropagator(aa, bb, gam, -z[k], E,X_tilt_crystal, Y_tilt_crystal, nn, mm, buf, &progress) ;
					atSFT_backward.MakeSFT(buf) ;


				}
					// glue back only the center 
					offsx+=offix ;
					offsy+=offiy ;
					for(j=0;j<mminner;j++)
					{
						bbuf=buf+(j+offiy)*nn*2+offix ;
						ddata=data+(int)(((offsy+j)%M)*N2) ;
						if(offsx<0)
						{
							MemMan.CopyData(bbuf,ddata+N2+offsx,(uint)(-offsx)) ;
							MemMan.CopyData(bbuf-offsx,ddata,(uint)(nninner*2+offsx)) ;

						}
						else
							if(offsx+nninner*2>=N2)
						{
							MemMan.CopyData(bbuf,ddata+offsx,(uint)(N2-offsx)) ;
							MemMan.CopyData(bbuf+N2-offsx,ddata,(uint)(nninner*2-(N2-offsx))) ;

						}
						else
							MemMan.CopyData(bbuf,ddata+offsx,nninner*2) ;
					}
//				else
//				{
//					for(j=0;j<mm;j++)
//					{
//						bbuf=buf+j*nn*2 ;
//						ddata=data+(int)(((offsy+j)%M)*N2) ;
//						if(offsx<0)
//						{
//							MemMan.CopyData(bbuf,ddata+N2+offsx,(uint)(-offsx)) ;
//							MemMan.CopyData(bbuf-offsx,ddata,(uint)(nn*2+offsx)) ;
//
//						}
//						else
//							if(offsx+nn*2>=N2)
//						{
//							MemMan.CopyData(bbuf,ddata+offsx,(uint)(N2-offsx)) ;
//							MemMan.CopyData(bbuf+N2-offsx,ddata,(uint)(nn*2-(N2-offsx))) ;
//
//						}
//						else
//							MemMan.CopyData(bbuf,ddata+offsx,nn*2) ;
//					}
//				}
			}
		}
	}
}
