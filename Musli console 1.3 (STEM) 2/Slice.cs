using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.IO;

namespace MusliDebug
{
	/// <summary>
	/// 
	/// </summary>
	unsafe public class Slice
	{
		public MemMan MMM ;
		public uint CurveLength ;
		public uint NTypes ;
		public int[] type_ ;
		public double ds ;
		public TimeSpan period ;
		public uint N,M;
		public uint NAtoms ;
		public double SliThickness ;
		public double a,b,gam ;
		public double[] x,y,DW,ads,occ ;
		public int[] type ;
		public double* sli ;
		public float progress ;
		public enum PotTypes {rs_DT_DW_cut,rs_DT_DW_periodic,rs_curve_DW_cut,rs_curve_DW_periodic,
							  rs_DT_TV_cut,rs_DT_TV_periodic,rs_curve_TV_cut,rs_curve_TV_periodic,
							  rec_DT_DW, rec_curve_DW,
							  PG_curve_DW_cut, PG_curve_TV_cut, PG_DT_DW_cut, PG_DT_TV_cut,
							  PG_curve_DW_periodic, PG_DT_DW_periodic,PG_curve_TV_periodic, PG_DT_TV_periodic,
							  Create_Propagator, Apply_Propagator};
		public PotTypes PotType ;
		public double  dela,atrad ;
		SFT locSFT ;
		public uint Nmult, Mmult ;

		#region Lib imports
		
		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_curve_DW_cutpot(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;
	
		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_curve_TV_cutpot(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;
	
		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_curve_DW_periodic(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_curve_TV_periodic(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_DT_DW_cutpot(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* dw, double* ads, double* occ,int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_DT_TV_cutpot(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_DT_DW_periodic(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* dw, double* ads, double* occ,int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rs_DT_TV_periodic(double a,double b,double gam, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rec_DT_DW(double a,double b,double gam,uint Nmult, uint Mmult,
			uint N,uint M,uint Na,uint SL,uint EL,
			double* xx,double* yy,double* DW, double* ads, double* occ,int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int Potential_rec_curve_DW(double a,double b,double gam,uint Nmult, uint Mmult,
			uint N,uint M,uint Na,uint SL, uint EL,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double ds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern void CreatePots_rec_DT_DW(uint Ntypes,uint Npoints,double dds,int *types,double *dw,double vol,double *sc);
		
		[DllImport(Class1.LibPath)]
		static public extern void CreatePots_rs_DT_DW(uint Ntypes,uint Npoints,double dds,int *types,double *dw,double *sc);

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_DW_cutpot(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, double* occ,int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_curve_TV_cutpot(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
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
			double* xx,double* yy, double* ads, int* type,
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
			double* xx,double* yy, double* ads, int* type,
			double *scatentryes,uint Npoints, double dds,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int PG_rs_DT_TV_periodic(double a,double b,double gam, double E, double atrad,
			uint N,uint M,uint Na,
			double* xx,double* yy, double* ads, int* type,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int CreatePropagator(double a,double b,double gam,double thickness, double E,
			double Xtilt, double Ytilt, 
			uint N,uint M,
			double* gli, float* cpro) ;

		[DllImport(Class1.LibPath)]
		static public extern int ApplyPropagator(double a,double b,double gam,double thickness, double E,
			double Xtilt, double Ytilt, 
			uint N,uint M,
			double* gli, float* cpro) ;


		#endregion


		public Slice()
		{
			// 
			// TODO: Add constructor logic here
			//
			NAtoms=4 ;
			a=20 ;
			b=20 ;
			SliThickness=0.3 ;
			N=512 ;
			M=512 ;
			gam=(90.0/180.0)*Math.PI ;
			x=new double[] {0.25,0.25,0.5,0.5} ;
			y=new double[] {0.25,0.75,0.25,0.75} ;
			DW=new double[] {0.5,0.5,0.0,0.0}  ;
			occ=new double[] {1.0,1.0,1.0,1.0}  ;
			ads=new double[] {0.0,0.0,0.0,0.0} ;
			type=new int[] {92,6,14,6} ;
			type_=new int[type.Length] ;
			atrad=8 ;  // range of RS curve - atomic radius
			dela=0.001 ; // step of RS curve in A
			Nmult=1 ;
			Mmult=1 ;
			locSFT=new SFT() ;

		}
		public bool IsPotRec
		{
			get
			{
				return (PotType==PotTypes.rec_curve_DW)||(PotType==PotTypes.rec_DT_DW) ;
			}
		}
		public bool IsPotcurve
		{
			get
			{
				return 	(PotType==PotTypes.rec_curve_DW)||(PotType==PotTypes.rs_curve_DW_cut)||(PotType==PotTypes.rs_curve_DW_periodic)||
						(PotType==PotTypes.rs_curve_TV_cut)||(PotType==PotTypes.rs_curve_TV_periodic)||
						(PotType==PotTypes.PG_curve_DW_cut)||(PotType==PotTypes.PG_curve_TV_cut)||
						(PotType==PotTypes.PG_curve_DW_periodic)||(PotType==PotTypes.PG_curve_TV_periodic);

			}
		}
		public bool IsPotDW
		{
			get
			{
				return 	(PotType==PotTypes.rec_curve_DW)||(PotType==PotTypes.rs_curve_DW_cut)||(PotType==PotTypes.rs_curve_DW_periodic)||
						(PotType==PotTypes.rec_DT_DW)||(PotType==PotTypes.rs_DT_DW_cut)||(PotType==PotTypes.rs_DT_DW_periodic)||
						(PotType==PotTypes.PG_curve_DW_cut)||(PotType==PotTypes.PG_DT_DW_cut)||
						(PotType==PotTypes.PG_curve_DW_periodic) ;

			}
		}

		public void StartCalculations() 
		{
			progress=0 ;
			int i ;
			MusliRandom vibr ;
			DateTime start=DateTime.Now ;
			if(IsPotRec) locSFT.MakePlan(sli,N,M,-1);
			fixed(double* _x=x)
			{
				fixed(double* _y=y)
				{
					fixed(double* _dw=DW)
					{
						fixed(double* _ads=ads)
						{
							fixed(double* _occ=occ)
							{
								fixed(int* _type_=type_)  
								{
									fixed(int* _type=type) 
									{
										fixed(float* _pro=&progress)
										{
											switch(PotType)
											{
												case PotTypes.rs_DT_DW_periodic    : 
													Potential_rs_DT_DW_periodic(a,b,gam,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,sli,_pro) ;
													break ;
												case PotTypes.PG_DT_DW_periodic    : 
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													PG_rs_DT_DW_periodic(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,sli,_pro) ;
													break ;
												case PotTypes.rs_DT_TV_periodic    : 
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													Potential_rs_DT_TV_periodic(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_type,sli,_pro) ;
													break ;
												case PotTypes.PG_DT_TV_periodic    : 
													vibr=new MusliRandom(0) ;
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													PG_rs_DT_TV_periodic(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_type,sli,_pro) ;
													break ;
												case PotTypes.rs_DT_DW_cut         :
													Potential_rs_DT_DW_cutpot(a,b,gam,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,sli,_pro) ;
													break ;
												case PotTypes.PG_DT_DW_cut         :
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													PG_rs_DT_DW_cutpot(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_dw,_ads,_occ,_type,sli,_pro) ;
													break ;
												case PotTypes.rs_DT_TV_cut         :
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													Potential_rs_DT_TV_cutpot(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_type,sli,_pro) ;
													break ;
												case PotTypes.PG_DT_TV_cut         :
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													PG_rs_DT_TV_cutpot(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_type,sli,_pro) ;
													break ;
												case PotTypes.rs_curve_DW_periodic : 
													Potential_rs_curve_DW_periodic(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.PG_curve_DW_periodic : 
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													PG_rs_curve_DW_periodic(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.rs_curve_TV_periodic : 
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													Potential_rs_curve_TV_periodic(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.PG_curve_TV_periodic : 
													vibr=new MusliRandom(0) ;
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													PG_rs_curve_TV_periodic(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.rs_curve_DW_cut      : 
													Potential_rs_curve_DW_cutpot(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.PG_curve_DW_cut      : 
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													PG_rs_curve_DW_cutpot(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_occ,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.PG_curve_TV_cut      : 
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													PG_rs_curve_TV_cutpot(a,b,gam,100000.0,atrad,N,M,NAtoms,_x,_y,_ads,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.rs_curve_TV_cut      : 
													vibr=new MusliRandom(0) ;
													for(i=0;i<NAtoms;i++)
													{
														_x[i]*=a;_y[i]*=b ;
														vibr.StDevX=vibr.StDevY=Math.Sqrt(*(_dw+i)*0.5)/(2.0*Math.PI) ;
														vibr.Vobble(ref _x[i],ref _y[i]) ;
														if(_x[i]<0) _x[i]+=a ;
														if(_x[i]>=a) _x[i]-=a ;
														if(_y[i]<0) _y[i]+=b ;
														if(_y[i]>=b) _y[i]-=b ;

													}
													Potential_rs_curve_TV_cutpot(a,b,gam,atrad,N,M,NAtoms,_x,_y,_ads,_type_,(double*)MMM.LockAddress,CurveLength,dela,sli,_pro) ;
													break ;
												case PotTypes.rec_DT_DW            : 
													Potential_rec_DT_DW(a,b,gam,Nmult,Mmult,N,M,NAtoms,0,M,_x,_y,_dw,_ads,_occ,_type,sli,_pro) ;
													break ;
												case PotTypes.rec_curve_DW         : 
													Potential_rec_curve_DW(a,b,gam,Nmult,Mmult,N,M,NAtoms,0,M,_x,_y,_ads,_occ,_type_,(double*)MMM.LockAddress,CurveLength,ds,sli,_pro) ;
													break ;
												case PotTypes.Create_Propagator         : 
													CreatePropagator(a, b, gam, SliThickness, 100000.0, 0.0,100.0/60.0*Math.PI/180.0,  N, M, sli, _pro) ;
													break ;
												case PotTypes.Apply_Propagator         : 
													for(i=0;i<N*M*2;i+=2) *(sli+i)=1 ;
													ApplyPropagator(a, b, gam, SliThickness, 100000.0, 0.0,100.0/60.0*Math.PI/180.0,  N, M, sli, _pro) ;
													break ;

												default :
													Console.WriteLine("Potential " +PotType.ToString()+" is not supported") ;
													break ;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			// this should be done before exit
			//	IsFinished=true ;
			progress=100 ;
			//	RiseEvent_ProcFinished() ;
			period=DateTime.Now-start ;
			if(IsPotRec) unsafe {locSFT.MakeSFT(sli) ; }
		}

		public bool PrepareCurves() 
		{
			if(!IsPotcurve) return true ;
			ArrayList LookUp=new ArrayList()   ;
			ArrayList DWList=new ArrayList()   ;

			uint i ;
			double dze ;
			for( i=0;i<NAtoms ; i++)
			{
				if((type_[i]=LookUp.IndexOf(type[i]))>=0)
					if(DW[i]==(double)DWList[type_[i]]) {}
					else
					{
						type_[i]=LookUp.Add(type[i]) ;
						DWList.Add(DW[i]) ;
					}
				else
				{
					type_[i]=LookUp.Add(type[i]) ;
					DWList.Add(DW[i]) ;
				}

			}
			NTypes=(uint)LookUp.Count ;
			MMM=new MemMan() ;
			MemMan.Compact() ;
			
			if(IsPotRec) 
			{
				CurveLength=0x10000 ;
			}
			else CurveLength=(uint)(atrad/dela) ;

			dze=(double)N/(2.0*(a*Math.Sin(gam))) ;  // gamma ¬ –¿ƒ»¿Õ¿’!!!
			dze=Math.Min(dze,(double)M/(2.0*(b*Math.Sin(gam)))) ;
			
			if(!MMM.Allocate(NTypes*CurveLength*sizeof(double))) return false ;
			if(!MMM.Lock(0,NTypes*CurveLength*sizeof(double))) { MMM.Free() ; return false ;}
			
			int[] tl=(int[])LookUp.ToArray(typeof(int)) ;
			double[] dw_=(double[])DWList.ToArray(typeof(double)) ;
			if(!IsPotDW) Array.Clear(dw_,0,dw_.Length) ;
			fixed(int* typelist=&tl[0])
			{
				fixed(double* dw=&dw_[0])
				{
					if(IsPotRec) 
						CreatePots_rec_DT_DW(NTypes,CurveLength,ds=dze/CurveLength,typelist,dw,1.0/(a*b*Math.Sin(gam)),(double*)MMM.LockAddress) ;
					else 
						CreatePots_rs_DT_DW(NTypes,CurveLength,dela,typelist,dw,(double*)MMM.LockAddress) ;
				}
			}

			return true ;
		}
		public bool FlushOld()
		{
			FileRW fN ;
			if((fN=FileRW.Open("d:\\"+PotType.ToString()+".sli",FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
			{
				byte[] sout=new byte[160] ;
				string Sout ;
				uint nw ;
				nw=fN.Write(sli,N*M*(uint)2) ;
				Sout=String.Format("___ {0,10:g6} {1,10:g6} {2,10:g6} {3,10:g5} {4,10:g5} {5,10:g8} {6,10:g} {7,00000} {8,00000} {9,5:n0} {10,5:n0} ",
					a*Nmult,b*Mmult,SliThickness,Math.PI/2.0,Math.PI/2.0,gam,-1.0,N,M,1,1) ;					
				for(int i=0; i<160;i++)
					sout[i]=(i<Sout.Length)?System.Convert.ToByte(Sout[i]):(byte)0;
				nw+=fN.Write(sout,160) ;
				fN.Close() ;
					Console.WriteLine(nw+" bytes are written");
				return true ;
			}
			else 
			{
				return false ;
			}

		}
	}
}
