using System;
using System.IO;
using System.Collections;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe class MSProjectUseOnce: MSProject
	{
		MSUseOnceCalculator Calc ;
		string SliceTmpDirectory ;

		public MSProjectUseOnce()
		{
			// nesessary assignments
			Calc= new MSUseOnceCalculator() ;
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

			// Разрезка на слои
			Console.WriteLine("Slicing scheme is "+SliceType.ToString()) ;
			SliceModel() ;
			Console.WriteLine(NSlices+" slices") ;

			// Сохранение слоев
			if(!SaveSlices())
			{
				Console.WriteLine("Can't save slices, exiting") ;
				ForeWorkResult=false ;
				return;
			}
		
			long allbytes=GC.GetTotalMemory(false);

			Atoms=null ; // free a memory occupied by the model
			GC.Collect() ;
			allbytes=GC.GetTotalMemory(true);
			Console.WriteLine("\n>>>>>Allocated in managed heap after Atoms free and GC3 "+allbytes) ;
			
			
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
			string ostr ;
			int iSTEMline, iSTEMpoint ;
			DateTime start ;
			TimeSpan period ;
			start=DateTime.Now ;
			Console.WriteLine("\n****************** MS with inline potentials started ********************") ;

			if(!PrepareCurves())
			{
				Console.WriteLine("Can't create curves, exiting") ;
				return;
			}

			// Создание планов FFT
			Console.WriteLine("\nCreating FFT plans for "+N+"*"+M+" transformations") ;
			SFT_backward.MakePlan(data,N,M,-1) ;
			SFT_direct.MakePlan(data,N,M,1) ;
			Console.WriteLine("\nInitial wave is "+IniWaveType.ToString()) ;
			Console.WriteLine("Potential type is "+PotType.ToString()) ;
			Calc.a=a ;
			Calc.b=b ;
			Calc.gam=gam ;
			Calc.N=N ;
			Calc.M=M ;
			Calc.E=E ;
			Calc.data=data ;
			Calc.PotType=PotType ;
			Calc.atrad=atrad ;
			Calc.dela=dela ;
			Calc.CurveLength=CurveLength ;
			Calc.CurveHolder=(double*)PotCurve.BaseAddress ;
			Calc.NTypes=NTypes ;
			Calc.NSlices=NSlices ;
			Calc.IsSaveWaves=IsSaveWaves ;
			Calc.IsSaveDiffractions=IsSaveDiffractions ;
			Calc.IsSaveSTEM=(IniWaveType==MSProject.IniWaveTypes.STEMshift) ;
			Calc.SFT_direct=SFT_direct ;
			Calc.SFT_backward=SFT_backward ;
			//			int iE, iBT, iBS, iCA, iDf, iCs, iCT ;  // moved to MyProject class definition
			for(iE=0; iE<Emat.Length; iE++)
			{
				Calc.E=E=Emat[iE] ;
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
						if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.Precalculated)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
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
							if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.Precalculated)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
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
								if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.Precalculated)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
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
									if((IniWaveType==MSProject.IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.Precalculated)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))
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
									for(iCT=0;iCT<Math.Max(CrystalTilt.Length,1);iCT++)
									{
										try
										{
											X_tilt_crystal=Calc.X_tilt_crystal=CrystalTilt[iCT][0]/1000.0 ;
											Y_tilt_crystal=Calc.Y_tilt_crystal=CrystalTilt[iCT][1]/1000.0 ;
										}
										catch
										{
											Calc.X_tilt_crystal=Calc.Y_tilt_crystal=0.0 ;
										}
										for(TVi=0;TVi<TVNumber;TVi++)
										{

											if((IniWaveType==MSProject.IniWaveTypes.STEMshift))
											{
												iSTEMline=(int)Math.Floor((double)iBS/STEM_N) ;
												iSTEMpoint=iBS%STEM_N ;
												if((iSTEMpoint==0) && (iCA==0) && (iCs==0) && (iDf==0) && (iCT==0) && (TVi==0)) ostr="\nLine "+iSTEMline+" ("+STEM_M+")\n#" ;
												else ostr="#" ;
												if((TVNumber>1) && (TVi==TVNumber-1)) ostr+="!" ;

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
											Calc.CurThickness=StartThickness ; // in the case of precalculated wave - thickness of this wave
											if(IsCalcAll) ThWaves=new double[] {c+StartThickness-0.00001} ;
											Calc.ThWaves=ThWaves ;

											// Начало мултислайса
											Calc.RunCalculation() ;

										} // different phonon configurations (TVNumber)
									} // crystal tilt cycle
								} // Df cycle
							} // Cs cycle
						} // Convergence angle cycle
					} // beam shift cycle
				} // beam tilt cycle
			} // E cycle



			if(this.IsContinue)
			{
				System.IO.File.Copy(Calc.LastSavedWave,
					Path.Combine(WorkingDirectoryName,"last.wav"),true) ;
			}
			if(IniWaveType==IniWaveTypes.STEMshift) FlushSTEMfile() ;

			if(SliceTmpDirectory.Length>0) Directory.Delete(SliceTmpDirectory,true) ;
			period=DateTime.Now-start ;
			Console.WriteLine("\n****** MS finished, total time "+period.ToString()+" ********************") ;

		}
		
		bool SaveSlices()
		{
			double[] x,y,z,DW,ads,occ ;
			double CurThickness=0 ;
			int[] type, type_ ;
			int i,j,jj,js ;
			string lwd ;
			uint nas ;
			FileRW fN ;
			lwd=Path.Combine(WorkingDirectoryName,"Slice stack "+" ("+DateTime.Now.ToShortDateString()+" "+DateTime.Now.Hour+"_"+DateTime.Now.Minute+"_" +DateTime.Now.Second+")") ;
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
			SliceTmpDirectory=lwd ;
			Calc.SliceFilename=Path.Combine(lwd,"Slice ");
			bool repeat ;
			for(i=js=0;i<NSlices;i++)
			{
				repeat=false ;
			memalo:

				try
				{
					nas=(uint)NASlices[i] ;
					x=new double[(int)nas] ;
					y=new double[(int)nas] ;
					z=new double[(int)nas] ;
					DW=new double[(int)nas] ;
					ads=new double[(int)nas] ;
					occ=new double[(int)nas] ;
					type=new int[(int)nas] ;
					type_=new int[(int)nas] ;
				}
				catch
				{
					if(repeat)
					{
						Console.WriteLine("Can't allocate memory for buffers");
						return false ;
					}
					repeat=true ;
					GC.Collect() ;
					goto memalo ;
				}
				for(j=js,jj=0;j<js+(int)nas;jj++,j++)
				{
					x[jj]=((AtomRec)Atoms[j]).x ;
					y[jj]=((AtomRec)Atoms[j]).y ;
					z[jj]=(((AtomRec)Atoms[j]).z*c-CurThickness) ;
					DW[jj]=((AtomRec)Atoms[j]).DW ;
					ads[jj]=((AtomRec)Atoms[j]).ads ;
					occ[jj]=((AtomRec)Atoms[j]).occ ;
					type[jj]=((AtomRec)Atoms[j]).type ;
					type_[jj]=((AtomRec)Atoms[j]).type_ ;
				}
				CurThickness+=(double)ThSlices[i] ;
				js+=(int)nas ;
				lwd=Calc.SliceFilename+(i+1).ToString("00000")+".slice_coo" ;
				if((fN=FileRW.Open(lwd,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
				{
					uint nw=0 ;
					bool sff ;
					double cth=(double)ThSlices[i] ;
					unsafe
					{
						nw+=fN.Write((byte*)&nas,sizeof(int)) ;
						nw+=fN.Write(&cth,1) ;
						if(nas>0)
						{
							fixed(double* xx=x,yy=y,zz=z,DDW=DW,aads=ads,oocc=occ)
								fixed(int* t=type,t_=type_)
								{
									nw+=fN.Write(xx,nas) ;
									nw+=fN.Write(yy,nas) ;
									nw+=fN.Write(zz,nas) ;
									nw+=fN.Write(DDW,nas) ;
									nw+=fN.Write(aads,nas) ;
									nw+=fN.Write(oocc,nas) ;
									nw+=fN.Write((byte*)t,sizeof(int)*nas) ;
									nw+=fN.Write((byte*)t_,sizeof(int)*nas) ;
								}
						}
						sff=(nw!=nas*(6*sizeof(double)+2*sizeof(int))+sizeof(double)+sizeof(int)) ;
					}
					fN.Close() ;
					if(sff)
					{ // add code to find and display the reason
						Console.WriteLine("Only "+nw+" bytes are written to "+lwd);
						fN.Close() ;
						return false ;
					}
				}
				else return false ;
				x=y=z=DW=ads=occ=null ;
				type=type_=null ;
				Console.Write("\rSlice "+(i+1)+" is written      ");

			}
			Console.WriteLine("\r"+i+" slices are saved       ") ;
				GC.Collect() ;
			return true;
		}

	}
}
