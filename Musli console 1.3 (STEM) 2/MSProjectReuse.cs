using System;
using System.IO;
using System.Collections;
using System.Runtime.InteropServices;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe class MSProjectReuse : MSProject
	{
		MSReuseCalculator Calc ;
		ArrayList MMList ;
		double*[] SliList ;
		double*[] PropList ;
		double[] x,y,DW,ads,occ ;
		int[] type, type_ ;
		ArrayList Props ;   // list of different slice thicknesses
		double*[] PropsP ;  // pointers to different propagators


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
		static public extern int CreatePropagator(double a,double b,double gam,double thickness, double E,
			double Xtilt, double Ytilt, 
			uint N,uint M,
			double* gli, float* cpro) ;

		#endregion

		public MSProjectReuse()
		{
			Calc= new MSReuseCalculator() ;
			Calc.SaveWav= new SaveWaveDelegate(this.SaveWav) ;
			Calc.SaveDiffraction= new SaveWaveDelegate(this.SaveDiffraction) ;
			Calc.SaveSTEM= new SaveSTEMDelegate(this.SaveSTEM) ;
		}

		public override void ForeWork()
		{
			// Загрузка и сортировка модели
			if(!LoadModel(ModelFileName))
			{
				Console.WriteLine("Can't load model "+ModelFileName+", exiting") ;
				ForeWorkResult=false ;
				return;
			}
			Console.WriteLine("\nModel "+ModelFileName+" loaded:") ;
			Console.WriteLine("a="+a.ToString("####0.00")+"A  b="+b.ToString("####0.00")+"A  c="+c.ToString("####0.00")+"A  gam="+gam.ToString("####0.00")+"  NAtoms="+NAtoms) ;

			// Создание кривых рассеяния при необходимости (если не надо - возвращается из функции обратно)
			if(!PrepareCurves())
			{
				Console.WriteLine("Can't create curves, exiting") ;
				ForeWorkResult=false ;
				return;
			}

			// Разрезка на слои
			Console.WriteLine("Slicing scheme is "+SliceType.ToString()) ;
			SliceModel() ;
			Console.WriteLine(NSlices+" slices") ;


			//выделение памяти под картинки STEM, если считается STEM
			if(IniWaveType==IniWaveTypes.STEMshift)
			{
				STEMimages=new double[Emat.Length,Math.Max(BeamTilt.Length,1),Math.Max(ConvAngles.Length,1),
					Math.Max(Dfmat.Length,1),Math.Max(Csmat.Length,1),Math.Max(CrystalTilt.Length,1),(IsCalcAll?1:ThWaves.Length),DetectorAngles.Length][] ;
	

				
				for(iE=0;iE<Emat.Length;iE++)
				{
					for(iBT=0;iBT<Math.Max(BeamTilt.Length,1);iBT++)
					{
						for(iCA=0;iCA<Math.Max(ConvAngles.Length,1);iCA++)
						{
							for(iDf=0;iDf<Math.Max(Dfmat.Length,1);iDf++)
							{
								for(iCs=0;iCs<Math.Max(Csmat.Length,1);iCs++)
								{
									for(iCT=0;iCT<Math.Max(CrystalTilt.Length,1);iCT++)
									{
										for(int iTW=0;iTW<(IsCalcAll?1:ThWaves.Length);iTW++)
										{
											for(int iDA=0;iDA<DetectorAngles.Length;iDA++)
											{
												STEMimages[iE,iBT,iCA,iDf,iCs,iCT,iTW,iDA]=new double[BeamShift.Length] ;
											}
										}
									}
								}
							}
						}
					}
				}
				
			}

			ForeWorkResult=true ;
		}

		override public void StartCalculations()
		{
			DateTime start ;
			string ostr ;
			int iSTEMline, iSTEMpoint ;
			TimeSpan period ;
			start=DateTime.Now ;
			Console.WriteLine("\n****************** MS with reused potentials started ********************") ;

			// Создание планов FFT
			Console.WriteLine("\nCreating FFT plans for "+N+"*"+M+" transformations") ;
			SFT_backward.MakePlan(data,N,M,-1) ;
			SFT_direct.MakePlan(data,N,M,1) ;

			Calc.a=a ;
			Calc.b=b ;
			Calc.gam=gam ;
			Calc.N=N ;
			Calc.M=M ;
			Calc.E=E ;
			Calc.data=data ;
			Calc.NSlices=NSlices ;
			Calc.ThSlices=ThSlices ;
			Calc.IsSaveWaves=IsSaveWaves ;
			Calc.IsSaveDiffractions=IsSaveDiffractions ;
			Calc.IsSaveSTEM=(IniWaveType==MSProject.IniWaveTypes.STEMshift) ;
			Calc.SFT_direct=SFT_direct ;
			Calc.SFT_backward=SFT_backward ;

			Console.WriteLine("\nInitial wave is "+IniWaveType.ToString()) ;

//			int iE, iBT, iBS, iCA, iDf, iCs, iCT ; // moved to MyProject class definition
			for(iE=0; iE<Emat.Length; iE++)
			{
				Calc.E=E=Emat[iE] ;
				// Расчет и сохранение слоев
				if(!GeneratePG())
				{
					Console.WriteLine("Can't create slices, exiting") ;
					return;
				}
				for(iCT=0;iCT<Math.Max(CrystalTilt.Length,1);iCT++)
				{
					try
					{
						X_tilt_crystal=CrystalTilt[iCT][0]/1000.0 ;
						Y_tilt_crystal=CrystalTilt[iCT][1]/1000.0 ;
					}
					catch
					{
						Y_tilt_crystal=0.0 ;
					}

					// создание пропагаторов
					GenerateProps() ;
					Calc.SliList=SliList ;
					Calc.PropList=PropList ;


					for(iBT=0;iBT<Math.Max(BeamTilt.Length,1);iBT++)
					{
						X_beamtilt=Y_beamtilt=0 ;
						if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
						{
							try
							{
								X_beamtilt=BeamTilt[iBT][0]/1000.0 ;
								Y_beamtilt=BeamTilt[iBT][1]/1000.0 ;
							}
							catch
							{
							}
						}
						if(IniWaveType==MSProject.IniWaveTypes.PlaneWave)
						{
							try
							{
								X_beamtilt=BeamTilt[iBT][0] ;
								Y_beamtilt=BeamTilt[iBT][1] ;
							}
							catch
							{
							}
						}
						for(iBS=0;iBS<(((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))?Math.Max(BeamShift.Length,1):1);iBS++)
						{
							if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
							{
								try
								{
									X_beamshift=BeamShift[iBS][0] ;
									Y_beamshift=BeamShift[iBS][1] ;
								}
								catch
								{
									X_beamshift=Y_beamshift=0.5 ;
								}
							}
							for(iCA=0;iCA<(((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))?Math.Max(ConvAngles.Length,1):1);iCA++)
							{
								if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
								{
									try
									{
										ConvAngle=ConvAngles[iCA][0] ;
										ConvAngleInner=ConvAngles[iCA][1] ;
									}
									catch
									{
										ConvAngle=0.3 ;
										ConvAngleInner=0 ;
									}
								}
								for(iCs=0;iCs<(((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))?Math.Max(Csmat.Length,1):1);iCs++)
								{
									if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
									{
										try
										{
											Cs=Csmat[iCs] ;
										}
										catch
										{
											Cs=0 ;
										}
									}
									for(iDf=0;iDf<(((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))?Math.Max(Dfmat.Length,1):1);iDf++)
									{
										if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
										{
											try
											{
												Df=Dfmat[iDf] ;
											}
											catch
											{
												Df=0 ;
											}
										}


										if((IniWaveType==MSProject.IniWaveTypes.STEMshift))
										{
											iSTEMline=(int)Math.Floor((double)iBS/STEM_N) ;
											iSTEMpoint=iBS%STEM_N ;
											if((iSTEMpoint==0) && (iCA==0) && (iCs==0) && (iDf==0) && (iCT==0)) ostr="\nLine "+iSTEMline+" ("+STEM_M+")\n#" ;
											else ostr="#" ;
										}
										else
										{
											ostr="\n*** "+(E/1000.0).ToString("###0.0")+"kV" ;
											ostr+=", CT("+X_tilt_crystal.ToString("0.0###")+","+Y_tilt_crystal.ToString("0.0###")+")" ;
											ostr+=", BT("+X_beamtilt.ToString("0.0###")+","+Y_beamtilt.ToString("0.0###")+")" ;
											ostr+="\n*** BS("+X_beamshift.ToString("0.0###")+","+Y_beamshift.ToString("0.0###")+")" ;
											ostr+=", CA("+ConvAngle.ToString("0.0###")+","+ConvAngleInner.ToString("0.0###")+")" ;
											ostr+=", Df"+Df.ToString("###0.0")+"A" ;
											ostr+=", Cs"+(Cs/1e7).ToString("###0.0")+"mm\n" ;

										}
										Console.Write(ostr) ;

										// Создание начальной волны
										if(!CreateInitialWave())
										{
											Console.WriteLine("Can't create initial wave, exiting") ;
											return ;
										}
										Calc.CurThickness=StartThickness ; 
										if(IsCalcAll) this.ThWaves=new double[] {c+StartThickness-0.00001} ;
										Calc.ThWaves=ThWaves ;

										// Начало мултислайса

										Calc.RunCalculation() ;
									} // Df cycle
								} // Cs cycle
							} // Convergence angle cycle
						} // beam shift cycle
					} // beam tilt cycle
				} // crystal tilt cycle
			} // E cycle


			if(IsContinue)
			{
				System.IO.File.Copy(Calc.LastSavedWave,
					Path.Combine(this.WorkingDirectoryName,"last.wav"),true) ;
			}
			if(IniWaveType==IniWaveTypes.STEMshift) FlushSTEMfile() ;

			period=DateTime.Now-start ;
			Console.WriteLine("\n****** MS finished, total time "+period.ToString()+" ********************") ;
		}

	
		bool GeneratePG()
		{
			int i,j,k ;
			MemMan iM ;
			uint NMMs ;
			float progress ;
			int[] PropSliTable=new int[NSlices] ;
			Props=new ArrayList() ;
			SliList=new double*[NSlices] ;
			PropList=new double*[NSlices] ;
			for(i=0;i<NSlices;i++)
			{
				for(j=0;j<Props.Count;j++)
					if(Math.Abs((double)Props[j]-(double)ThSlices[i])<0.000001)
					{
						break ;
					}
				if(j==Props.Count) j=Props.Add(ThSlices[i]) ;
				PropSliTable[i]=j ;
			}

			PropsP=new double*[Props.Count] ;

			MMList=new ArrayList((int)(NSlices+Props.Count)) ;
			for(i=0;i<NSlices+Props.Count;i++)
			{
				iM=new MemMan() ;
				if(iM.Allocate(MemRequested()))
					MMList.Add(iM) ;
				else break ;
			}
			NMMs=(uint)MMList.Count ;
			if(NMMs<NSlices+Props.Count) 
			{
				Console.WriteLine("Can't allocate all data in memory, only "+NMMs+" pages are available, "+(NSlices+Props.Count)+" requested,\nuse inline calculation") ;
				return false ;
			}
			for(i=0;i<NSlices;i++)
			{
				Console.WriteLine("Creating slice number "+i.ToString("0000")+" from "+(uint)NASlices[i]+" atoms using "+PotType.ToString()) ;
				//((MemMan)MMList[i]).Lock(0,MemRequested()) ;
				SliList[i]=(double*)((MemMan)MMList[i]).BaseAddress ;
				if((uint)NASlices[i]>0)
				{
					ExtractSlice(i) ;
					fixed(double* _x=x,_y=y,_dw=DW,_ads=ads,_occ=occ)
					{
						fixed(int* _type_=type_,_type=type)  
						{
							switch(PotType)
							{
								case PotTypes.rs_DT_DW_periodic    : 
									Potential_rs_DT_DW_periodic(a,b,gam,atrad,N,M,(uint)NASlices[i],_x,_y,_dw,_ads,_occ,_type,SliList[i],&progress) ;
									break ;
								case PotTypes.rs_DT_DW_cut         :
									Potential_rs_DT_DW_cutpot(a,b,gam,atrad,N,M,(uint)NASlices[i],_x,_y,_dw,_ads,_occ,_type,SliList[i],&progress) ;
									break ;
								case PotTypes.rs_curve_DW_periodic : 
									Potential_rs_curve_DW_periodic(a,b,gam,atrad,N,M,(uint)NASlices[i],_x,_y,_ads,_occ,_type_,(double*)PotCurve.BaseAddress,CurveLength,dela,SliList[i],&progress) ;
									break ;
								case PotTypes.rs_curve_DW_cut      : 
									Potential_rs_curve_DW_cutpot(a,b,gam,atrad,N,M,(uint)NASlices[i],_x,_y,_ads,_occ,_type_,(double*)PotCurve.BaseAddress,CurveLength,dela,SliList[i],&progress) ;
									break ;
								case PotTypes.rec_DT_DW            : 
									Potential_rec_DT_DW(a,b,gam,Nmult,Mmult,N,M,(uint)NASlices[i],0,M,_x,_y,_dw,_ads,_occ,_type,SliList[i],&progress) ;
									SFT_backward.MakeSFT(SliList[i]) ;
									break ;
								case PotTypes.rec_curve_DW         : 
									Potential_rec_curve_DW(a,b,gam,Nmult,Mmult,N,M,(uint)NASlices[i],0,M,_x,_y,_ads,_occ,_type_,(double*)PotCurve.BaseAddress,CurveLength,ds,SliList[i],&progress) ;
									SFT_backward.MakeSFT(SliList[i]) ;
									break ;
								default :
									Console.WriteLine("Potential " +PotType.ToString()+" is not supported") ;
									return false ;
							}
						}
					}
				}
				if(IsSaveSlices) SaveSlice(i) ;
			}
			if(IsPotRec) {a*=Nmult;b*=Mmult;}

			//Создание PG из слайсов
			double lambda=0.387818/Math.Sqrt(E/1000.0*(1.0+0.978459e-3*E/1000)) ; // E in V
			double K=lambda*Math.Sqrt(1.0+Math.Pow((0.0242621/lambda),2)) ;
			if(IsPotRec) K*=(N*M) ;
			for(i=0;i<NSlices;i++)
			{
				double*	pbuf=SliList[i] ;
				double aR,aI ;
				for(j=0;j<M;j++)
				{

					for(k=0;k<N;k++)
					{
						aR=K**pbuf ;
						aI=Math.Exp(-*(pbuf+1)*K) ;
						*pbuf=Math.Cos(aR)*aI ;
						*(++pbuf)=-Math.Sin(aR)*aI ;
						pbuf++ ;
					}
				} // y-cycle 

			}

			Atoms=null ; // free a memory occupied by the model
			GC.Collect() ;

			for(i=0;i<Props.Count;i++)
			{
				PropsP[i]=(double*)((MemMan)MMList[(int)(i+NSlices)]).BaseAddress ;
			}
			for(i=0;i<NSlices;i++)
				PropList[i]=PropsP[PropSliTable[i]] ;
			
			return true ;
		}

		bool GenerateProps()
		{
			int i;
			float progress ;

			for(i=0;i<Props.Count;i++)
			{
				Console.WriteLine("Creating propagator for "+((double)Props[i]).ToString("##0.000000")+" A") ;
				CreatePropagator(a, b, gam, (double)Props[i], E, X_tilt_crystal, Y_tilt_crystal,  N, M, PropsP[i], &progress) ;
			}
			return true ;
		}

		void ExtractSlice(int i)
		{
			uint nas=(uint)NASlices[i] ;
			x=new double[(int)nas] ;
			y=new double[(int)nas] ;
			DW=new double[(int)nas] ;
			ads=new double[(int)nas] ;
			occ=new double[(int)nas] ;
			type=new int[(int)nas] ;
			type_=new int[(int)nas] ;
			int j=0;
			foreach(AtomRec at in Atoms)
			{
				if(at.sn==i)
				{
					x[j]=at.x ;
					y[j]=at.y ;
					DW[j]=at.DW ;
					ads[j]=at.ads ;
					occ[j]=at.occ ;
					type[j]=at.type ;
					type_[j]=at.type_ ;
					j++ ;
				}
			}

		}
		/// <summary>
		/// Saves slices in real space
		/// </summary>
		/// <returns></returns>
		bool SaveSlice(int iSlice) // save slices in real space
		{
			// a*Nmult, b*Mmult
			FileRW fN ;
			string lwd, SavedFilename ;
			lwd=Path.Combine(WorkingDirectoryName,"Slice stack") ;
			if(!Directory.Exists(lwd))
				try 
				{
					Directory.CreateDirectory(lwd) ;
				} 
				catch (Exception e) 
				{
					Console.WriteLine("Can't create working directory:\n"+e.ToString());
					return false;
				} 
			SavedFilename=Path.Combine(lwd,"Slice_"+iSlice.ToString("0000")+".slice") ;
			if((fN=FileRW.Open(SavedFilename,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
			{
				byte[] sout=new byte[160] ;
				string Sout ;
				uint nw ;
				nw=fN.Write(SliList[iSlice],N*M*(uint)2) ;
				if(nw!=N*M*(uint)2*sizeof(double))
				{ // add code to find and display the reason
					Console.WriteLine("Only "+nw+" bytes out of "+N*M*(uint)2*sizeof(double)+" are written to\n"+SavedFilename);
					fN.Close() ;
					SavedFilename=null ;
					return false ;
				}
				Sout=String.Format("___ {0,12:g10} {1,12:g10} {2,12:g10} {3,10:g5} {4,10:g5} {5,10:g8} {6,10:g} {7,00000} {8,00000} {9,10:g} {10,5:n0} ",
					a*Nmult,b*Mmult,ThSlices[iSlice],Math.PI/2.0,Math.PI/2.0,gam,-1.0,N,M,iSlice,NSlices) ;					
				for(int i=0; i<160;i++)
					sout[i]=(i<Sout.Length)?System.Convert.ToByte(Sout[i]):(byte)0;
				fN.Write(sout,160) ;
				fN.Close() ;
				return true ;
			}
			else 
			{
				SavedFilename=null ;
				return false ;
			}
		}
	}
}
