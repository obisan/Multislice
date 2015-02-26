using System;
using System.IO;
using System.Collections;
using System.Xml.Serialization ;
using System.Runtime.InteropServices;
using System.Globalization;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe abstract class MSProject
	{

		[DllImport(Class1.LibPath)]
		static public extern void CreatePots_rec_DT_DW(uint Ntypes,uint Npoints,double dds,int *types,double *dw,double vol,double *sc);
		
		[DllImport(Class1.LibPath)]
		static public extern void CreatePots_rs_DT_DW(uint Ntypes,uint Npoints,double dds,int *types,double *dw,double *sc);

		/// <summary>
		/// Members of generic Project
		/// </summary>
		public double a,b,c,gam ; // gam in rad
		public double al,bet,Gam ; // Gam in deg
		public uint NAtoms, NSlices,Nmult,Mmult ;
		public bool IsSaveSlices ;
		public bool IsSaveWaves ;
		public bool IsSaveDiffractions ;
		public ArrayList Atoms;
		public double* data ;
		public uint N, M ;
		public ArrayList LookUp ;  // table of correspondence of atom types to curvetypes
		public ArrayList DWList ;  // list of DW factors for curvetypes
		public ArrayList NASlices ; // number of atoms in each slice
		public ArrayList ThSlices ; // thickness of each slice
		public double[] ThWaves ; // thicknesses to save waves
		public MSProjectInfo.WaveNameInclusions IncludeInWaveNames ;
		public enum PotTypes 
		{
			rs_DT_DW_cut,rs_DT_DW_periodic,rs_curve_DW_cut,rs_curve_DW_periodic,
			rs_DT_TV_cut,rs_DT_TV_periodic,rs_curve_TV_cut,rs_curve_TV_periodic,
			rec_DT_DW, rec_curve_DW,
			PG_curve_DW_cut, PG_curve_TV_cut, PG_DT_DW_cut, PG_DT_TV_cut,
			PG_curve_DW_periodic, PG_DT_DW_periodic,PG_curve_TV_periodic, PG_DT_TV_periodic,
			PG_curve_DW_pullZ, PG_curve_TV_pullZ

		};
		public int TVNumber, TVi ;
		public enum SliceTypes 
		{
			Layers, Fixed
		};
		public enum IniWaveTypes 
		{
			PlaneWave, Converged, Precalculated, STEMshift, STEMtilt
		};
		public PotTypes PotType ;
		public SliceTypes SliceType ;
		public IniWaveTypes IniWaveType ;
		public delegate	string SaveWaveDelegate(double CT);
		public delegate	bool SaveSTEMDelegate(int iWave);
		public string IniWaveFileName ;
		protected double StartThickness ;

		/// <summary>
		/// Parameters of initial wave
		/// </summary>
		protected double X_tilt_crystal, Y_tilt_crystal, E ;
		protected double  X_beamtilt,Y_beamtilt,X_beamshift,Y_beamshift,ConvAngle,ConvAngleInner,Df,Cs ;
		public bool UseAber ;
		protected double[] Emat,Dfmat,Csmat ;
		protected double[][] CrystalTilt,BeamTilt,BeamShift,ConvAngles ;


		/// <summary>
		/// Variables related to potential curve calculation
		/// </summary>
		protected uint CurveLength, NTypes ;
		protected MemMan PotCurve ;
		protected double atrad,dela,ds ;

		/// <summary>
		/// Members for processing
		/// </summary>
		public int NThreads, IThreads ;
		public string WorkingDirectoryName ;
		protected SFT SFT_direct ;
		protected SFT SFT_backward ;
		public bool IsContinue ;
		public bool IsCalcAll ;
		public bool ForeWorkResult ;


		/// <summary>
		/// STEM stuff
		/// </summary>
		protected double[,,,,,,,][] STEMimages ; 
					//[E, BeamTilt, ConvAngle, Df, Cs, CrystTilt, ThWaves, DetectorAngles]
		protected int iE, iBT, iBS, iCA, iDf, iCs, iCT ; // cycle varialbles from StartCalculations
		// эти переменные должны стать членами класса, поскольку нужны для индексации STEMimages
		protected double[][] DetectorAngles ;
		protected int STEM_N, STEM_M ; // size of STEM image


		/// <summary>
		/// Members of undefined ownship: here or in derived classes
		/// </summary>
		public string ModelFileName ;
		double lambda, lambdaDf, lambdaCs ;


		public MSProject()
		{
			// 
			// TODO: Add constructor logic here
			//

			SFT_direct=new SFT() ;
			SFT_backward=new SFT() ;
			PotCurve=new MemMan() ;


		}

		#region Properties of potential
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
					(PotType==PotTypes.PG_curve_DW_periodic)||(PotType==PotTypes.PG_curve_TV_periodic)||
					(PotType==PotTypes.PG_curve_DW_pullZ)||(PotType==PotTypes.PG_curve_TV_pullZ);

			}
		}
		public bool IsPotDW
		{
			get
			{
				return 	(PotType==PotTypes.rec_curve_DW)||(PotType==PotTypes.rs_curve_DW_cut)||(PotType==PotTypes.rs_curve_DW_periodic)||
					(PotType==PotTypes.rec_DT_DW)||(PotType==PotTypes.rs_DT_DW_cut)||(PotType==PotTypes.rs_DT_DW_periodic)||
					(PotType==PotTypes.PG_curve_DW_cut)||(PotType==PotTypes.PG_DT_DW_cut)||
					(PotType==PotTypes.PG_curve_DW_periodic)||(PotType==PotTypes.PG_curve_DW_pullZ) ;

			}
		}
		public bool IsPotInline
		{
			get
			{	
				return (PotType==PotTypes.PG_curve_DW_cut)||(PotType==PotTypes.PG_curve_TV_cut)||
					(PotType==PotTypes.PG_DT_DW_cut)||(PotType==PotTypes.PG_DT_TV_cut)||
					(PotType==PotTypes.PG_curve_DW_periodic)||(PotType==PotTypes.PG_DT_DW_periodic)||
					(PotType==PotTypes.PG_curve_TV_periodic)||(PotType==PotTypes.PG_DT_TV_periodic)||
					(PotType==PotTypes.PG_curve_DW_pullZ)||(PotType==PotTypes.PG_curve_TV_pullZ) ;

			}
		}
		#endregion

		abstract public void StartCalculations() ;
		virtual public void ForeWork()
		{

			ForeWorkResult=true ;
		}

		virtual public uint MemRequested()
		{
			return N*M*2*sizeof(double)+NTypes*CurveLength*sizeof(double) ;
		}
		virtual public uint MemLockRequested()
		{
			return N*M*2*sizeof(double)+NTypes*CurveLength*sizeof(double) ;
		}

		#region Loaders
		protected bool LoadModel(string FName)
		{
			StreamReader tmpFile ;
			bool LoadResult=false ;

			try	{tmpFile=System.IO.File.OpenText(FName);}
			catch(Exception e)
			{
				Console.WriteLine("Unable to open\n"+FName+"\nfor reading:\n"+e.Message) ;
				return false;
			}
			
			if(FName.ToLower().EndsWith(".coo")) LoadResult=LoadCOO(tmpFile) ;
			else if(FName.ToLower().EndsWith(".hin")) Console.WriteLine("HIN is not supported");
			
			tmpFile.Close() ;

			if(LoadResult&&IsPotcurve) 
			{
				LookUp=new ArrayList()   ;
				DWList=new ArrayList()   ;
				foreach(AtomRec at in Atoms)
				{
					if((at.type_=LookUp.IndexOf(at.type))>=0)
						if(at.DW==(double)DWList[at.type_]) {}
						else
						{
							at.type_=LookUp.Add(at.type) ;
							DWList.Add(at.DW) ;
						}
					else
					{
						at.type_=LookUp.Add(at.type) ;
						DWList.Add(at.DW) ;
					}

				}
				if(IsPotRec) 
				{
					CurveLength=0x10000 ;
				}
				else CurveLength=(uint)(atrad/dela) ;
				NTypes=(uint)LookUp.Count ;
			}


			return LoadResult;
		}


		bool LoadCOO(StreamReader inFile)
		{
			string os ;
			string[] osin ;
			int i,j ;
			double[] buff= new double[8];
			ArrayList comm=new ArrayList(),NewAtoms ;
			double a_,b_,c_,al_,bet_,Gam_,gam_ ;
			uint NewNAtoms ;

			try
			{
			repeat_head1 :
				if((os=inFile.ReadLine())==null)
				{
					Console.WriteLine("Error reading .COO header!");
					return false ;
				}
				i=os.IndexOf("//");
				if(i>=0) 
				{
					comm.Add(os.Substring(i)) ;
					os=os.Substring(0,i) ;
				}
				os=os.Trim() ;
				osin=os.Split(' ') ;
				i=0 ;
				foreach(string arg in osin)
				{
					if(i>=7) 
					{
						Console.WriteLine("Header error");
						return false ;
					}
					if(arg!=String.Empty)
						buff[i++]=double.Parse(arg) ;
				} 
				if(i==0) goto repeat_head1 ;
				if(i<7)
				{
					Console.WriteLine("Error reading .COO header!");
					return false ;
				}

				// Header data were read properly
				// Making assignments of parameters 
				a_=buff[0] ; b_=buff[1] ; c_=buff[2] ;
				al_=buff[3] ; bet_=buff[4] ; Gam_=buff[5] ; gam_=Gam_*Math.PI/180.0 ;
				NewNAtoms=(uint)buff[6] ;
				NewAtoms= new ArrayList((int)NewNAtoms) ;

				for(j=0;j<NewNAtoms;j++)
				{
				repeat_scan1 :
					if((os=inFile.ReadLine())==null)
					{
						Console.WriteLine("Error:\n"+j+" atoms read\nrather then"+NewNAtoms) ;
						return false ;
					}
					i=os.IndexOf("//");
					if(i>=0) 
					{
						os=os.Substring(0,i) ;
					}
					os=os.Trim() ;
					osin=os.Split(' ') ;
					i=0 ;
					foreach(string arg in osin)
					{
						if(i>=8) 
						{
							Console.WriteLine("Data format error in line:\n"+os);
							return false ;
						}
						if(arg!=String.Empty)
							buff[i++]=double.Parse(arg) ;
					} 

					if(i==0) goto repeat_scan1 ;
					if(i<4)
					{
						Console.WriteLine("Data format error in line:\n"+os);
						return false ;
					}
					if(i<5) {  buff[4]=0;}
					if(i<6) {  buff[5]=0;}
					if(i<7) {  buff[6]=1;}
	
					NewAtoms.Add(new AtomRec(buff[0]-Math.Floor(buff[0]),buff[1]-Math.Floor(buff[1]),buff[2]-Math.Floor(buff[2]),buff[3],buff[4],buff[5],buff[6]));
				} // end NAtoms cycle	
			}
			catch(Exception e)
			{
				Console.WriteLine("Can't read COO file :\n"+e.Message) ;
				return false ;
			}
			NewAtoms.Sort() ;
			a=a_;b=b_;c=c_;al=al_;bet=bet_;gam=gam_;Gam=Gam_;NAtoms=NewNAtoms;Atoms=NewAtoms ;
			return true;
		}

		#endregion

		#region Slicers
		protected void SliceModel()
		{
			switch(SliceType)
			{
				case SliceTypes.Fixed : SliceFixed() ; break ;
				case SliceTypes.Layers: SliceLayers(); break ;
			}

		}

		void SliceLayers()
		{
			uint islice=0;
			uint i=0 ;
			double CurThickness=0, StartThickness=0 ;
			NASlices=new ArrayList(1000) ;
			ThSlices=new ArrayList(1000) ;
			foreach(AtomRec at in Atoms)
			{//*****************************************************
				if((at.z-CurThickness)*c>0.001) // 0.001A determines the accuracy of slicing, i.e. min thikness of the layers
				{
					// empty first slice is created
					// only if correspondence at upper and lower surfaces is requested (IsCalcAll==true)
					// otherwise this halfslice is appended to the last one
					if((CurThickness==0)&&(i==0)&& (!IsCalcAll))
					{
						StartThickness=CurThickness=at.z ;
					}
					else
					{
						NASlices.Add(i) ;
						i=0 ;
						ThSlices.Add((at.z-CurThickness)*c) ;
						CurThickness=at.z ;
						islice++ ;
					}
				}
				at.sn=(int)islice ;
				i++ ;
			}
			NASlices.Add(i) ;
			ThSlices.Add((StartThickness+1-CurThickness)*c) ;
			NSlices=islice+1 ;
		}


		void SliceFixed()
		{
			int ilow, ihigh, islice ;
			double SliceThickness=(double)1/(double)NSlices ;
			double CurThickness=SliceThickness ;
			NASlices=new ArrayList((int)NSlices) ;
			ThSlices=new ArrayList((int)NSlices) ;
			for(islice=ilow=ihigh=0; islice<NSlices; islice++)
			{
				while((ihigh<NAtoms)&&(((AtomRec)Atoms[ihigh]).z<CurThickness))
				{
					((AtomRec)Atoms[ihigh++]).sn=islice ;

				}

				NASlices.Add((uint)(ihigh-ilow)) ;
				ThSlices.Add(SliceThickness*c) ;
				ilow=ihigh;
				CurThickness+=SliceThickness ;
			}

		}

		#endregion

		#region Initial waves
		protected bool CreateInitialWave()
		{
			bool result ;
			switch(IniWaveType)
			{
				case IniWaveTypes.PlaneWave : result=PlaneWave() ; StartThickness=0 ; break ;
				case IniWaveTypes.Converged : result=ConvergedBeam() ; StartThickness=0 ; break ;
				case IniWaveTypes.STEMshift : result=ConvergedBeam() ; StartThickness=0 ; break ;
				case IniWaveTypes.Precalculated : 
						Parameters par=new Parameters(IniWaveFileName) ;
					if(!par.result)
					{
						result=false ;
						break ;
					}
					if((Math.Abs(par.a-a)>0.00001)||(Math.Abs(par.b-b)>0.00001)
						||(Math.Abs(par.gam-gam)>0.00001)||(N!=par.N)||(M!=par.M)) 
					{
						result=false ;
						Console.WriteLine("Parameters of the preloaded initial wave do not fit to those of the model") ;
						break;
					}
					E=par.E*1000.0 ;
					StartThickness=par.thickness ;
					result=LoadData(IniWaveFileName) ;
					break ;
				default : result=false ; break ;
			}
			return result ;
		}

		bool PlaneWave()
		{
			// beam tilts are in pixels of rec space here
			MemMan.ZeroData(data,N*M*2) ;
			int i,j ;
			// здесь наклоны в пикселях обратного пространства *0.001 (при переводе в радианы)
			i=(int)(((int)X_beamtilt+N)%N) ;
			j=(int)(((int)Y_beamtilt+M)%M) ;
			*(data+(j*N+i)*2)=N*M ;
			SFT_backward.MakeSFT(data) ;
			return true ;
		}
		bool ConvergedBeam()
		{
			double *bli ;
			//			float dpro=(float)(100.0/M) ;
			int i,j ;
			uint ii,jj ;
			double x,y,s,x0,y0,s0,_A,_B,con ;
			_A=1.0/(a*Math.Sin(gam)) ; _B=1.0/(b*Math.Sin(gam));
			lambda=0.387818/Math.Sqrt(E/1000.0*(1.0+0.978459e-3*E/1000)) ; // E in V
			lambdaCs=Math.Pow(lambda,3)*0.5*Cs ;
			lambdaDf=lambda*Df ; // E in V
			x0=Math.Sin(X_beamtilt)/lambda ;  // in rad
			y0=Math.Sin(Y_beamtilt)/lambda ;  // in rad

			double cos_g=Math.Cos(gam) ;
			double smax=ConvAngle*ConvAngle ;
			double smin=ConvAngleInner*ConvAngleInner ;

			bli=data ;
			for(jj=0 ; jj<M ; jj++ )
			{
				if(jj<M/2) j=(int)jj ;
				else j=(int)(jj-M) ;
				y=(double)j*_B ;
				for(ii=0 ; ii<N ; ii++)
				{
					if(ii<N/2) i=(int)ii ;
					else i=(int)(ii-N) ;
					x=(double)i*_A ;
					s=x*x+y*y-2.0*x*y*cos_g  ;
					s0=Math.Pow(x-x0,2)+Math.Pow(y-y0,2)-2*(x-x0)*(y-y0)*cos_g ;
					if((s0>=smin)&&(s0<=smax)) 
					{ 
						con=-2.0*Math.PI*((double)i*X_beamshift+(double)j*Y_beamshift) ;
						if(UseAber)
						{
							con+=Xi(s) ;
						}
						*(bli++)=Math.Cos(con) ;
						*(bli++)=Math.Sin(con) ;
					}
					else
					{
						*(bli++)=0 ;
						*(bli++)=0 ;
					}
				} /* x-cycle */
				//				progress+=dpro ;
			} /* y-cycle */
			SFT_backward.MakeSFT(data) ;
			return true ;
		}

		double Xi( double s /*square of reciprocal vector*/ )
		{
			return Math.PI*(lambdaDf*s + lambdaCs*s*s )  ;
		}
		#endregion

		/// <summary>
		/// Saves reciprocal wave 
		/// </summary>
		/// <param name="CurThickness"></param>
		/// <returns></returns>
		protected string SaveWav(double CurThickness) // Save wave in real space, equivalent to FlushOld()
		{		
			FileRW fN ;
			string lwd ;
			if(!Directory.Exists(WorkingDirectoryName))
				try 
				{
					Directory.CreateDirectory(WorkingDirectoryName) ;
				} 
				catch (Exception e) 
				{
					Console.WriteLine("Can't create working directory:\n"+e.ToString());
					return null;
				} 
			string wavename = "Wave at "+CurThickness.ToString("######0.00")+"A" ;
			if(IncludeInWaveNames.IncludeModelName)
				wavename+="_Model("+Path.GetFileNameWithoutExtension(ModelFileName)+")";
			if(IncludeInWaveNames.IncludePotentialType)
				wavename+="_"+PotType.ToString();
			if(IncludeInWaveNames.IncludeHT)
				wavename+="_E("+E/1000.0+"kV)";
			if(IncludeInWaveNames.IncludeCrystTilts) 
				wavename+="_CrysTilt("+((double)(X_tilt_crystal*1000.0)).ToString("##0.0#")+", "+((double)(Y_tilt_crystal*1000.0)).ToString("##0.0#")+")mrad";
			if(IncludeInWaveNames.IncludeIniWaveType)
				wavename+="_"+IniWaveType.ToString();
			if(IncludeInWaveNames.IncludeBeamTilt)
				if(IniWaveType==IniWaveTypes.Converged)
				wavename+="_BeamTilt("+((double)(X_beamtilt*1000.0)).ToString("##0.0#")+", "+((double)(Y_beamtilt*1000.0)).ToString("##0.0#")+")mrad";
			else
				wavename+="_BeamTilt("+X_beamtilt.ToString("###0")+", "+Y_beamtilt.ToString("###0")+")";
			if(IniWaveType!=IniWaveTypes.PlaneWave)
			{
				if(IncludeInWaveNames.IncludeBeamShift)
					wavename+="_BeamShift("+((double)(X_beamshift)).ToString("0.0###")+", "+((double)(Y_beamshift)).ToString("0.0###")+")";
				if(IncludeInWaveNames.IncludeConvAngle)
					wavename+="_ConvAngle("+ConvAngle.ToString("#0.0#")+","+ConvAngleInner.ToString("#0.0#")+")recA";
				if(UseAber)
				{
					if(IncludeInWaveNames.IncludeDf)
						wavename+="_Df("+Df.ToString("######0.#")+"A)";
					if(IncludeInWaveNames.IncludeCs)
						wavename+="_Cs("+(Cs/1e7).ToString("##0.0#")+"mm)";
				}
			}
			if(IncludeInWaveNames.IncludeTVNumber)
				wavename+="_(TV_"+(TVi+1)+")";
			wavename+=".wav" ;
			lwd=Path.Combine(WorkingDirectoryName,wavename) ;
			if((fN=FileRW.Open(lwd,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
			{
				byte[] sout=new byte[160] ;
				string Sout ;
				uint nw ;
				nw=fN.Write(data,N*M*(uint)2) ;
				if(nw!=N*M*(uint)2*sizeof(double))
				{ // add code to find and display the reason
					Console.WriteLine("Only "+nw+" bytes out of "+N*M*(uint)2*sizeof(double)+" are written to\n"+lwd);
					fN.Close() ;
					return null ;
				}
				Sout=String.Format("___ {0,12:g10} {1,12:g10} {2,12:g10} {3,10:g5} {4,10:g5} {5,10:g8} {6,10:g} {7,00000} {8,00000} {9,10:g} {10,5:n0} ",
					a,b,CurThickness,Math.PI/2.0,Math.PI/2.0,gam,E/1000.0,N,M,CurThickness,0) ;					
				for(int i=0; i<160;i++)
					sout[i]=(i<Sout.Length)?System.Convert.ToByte(Sout[i]):(byte)0;
				fN.Write(sout,160) ;
				fN.Close() ;
				
				return lwd ;
			}
			else 
			{
				return null ;
			}

		}

		public string SaveDiffraction(double CurThickness)
		{		
			FileRW fN ;
			string lwd ;
			MemMan Mbuf=new MemMan() ;
			if(!Directory.Exists(WorkingDirectoryName))
				try 
				{
					Directory.CreateDirectory(WorkingDirectoryName) ;
				} 
				catch (Exception e) 
				{
					Console.WriteLine("Can't create working directory:\n"+e.ToString());
					return null;
				} 
			uint Bufsize=Mbuf.AllocateAtLeast(0x100000,MemRequested()/2) ;
			if(Bufsize==0) return null ;
			uint nlines=Bufsize/(N*sizeof(double)) ;
			int SL, EL=0 ;
			string wavename = "Diffraction at "+CurThickness.ToString("######0.00")+"A" ;
			if(IncludeInWaveNames.IncludeModelName)
				wavename+="_Model("+Path.GetFileNameWithoutExtension(ModelFileName)+")";
			if(IncludeInWaveNames.IncludePotentialType)
				wavename+="_"+PotType.ToString();
			if(IncludeInWaveNames.IncludeHT)
				wavename+="_E("+E/1000.0+"kV)";
			if(IncludeInWaveNames.IncludeCrystTilts) 
				wavename+="_CrysTilt("+((double)(X_tilt_crystal*1000.0)).ToString("##0.0#")+", "+((double)(Y_tilt_crystal*1000.0)).ToString("##0.0#")+")mrad";
			if(IncludeInWaveNames.IncludeIniWaveType)
				wavename+="_"+IniWaveType.ToString();
			if(IncludeInWaveNames.IncludeBeamTilt)
				if(IniWaveType==IniWaveTypes.Converged)
					wavename+="_BeamTilt("+((double)(X_beamtilt*1000.0)).ToString("##0.0#")+", "+((double)(Y_beamtilt*1000.0)).ToString("##0.0#")+")mrad";
				else
					wavename+="_BeamTilt("+X_beamtilt.ToString("###0")+", "+Y_beamtilt.ToString("###0")+")";
			if(IniWaveType!=IniWaveTypes.PlaneWave)
			{
				if(IncludeInWaveNames.IncludeBeamShift)
					wavename+="_BeamShift("+((double)(X_beamshift)).ToString("0.0###")+", "+((double)(Y_beamshift)).ToString("0.0###")+")";
				if(IncludeInWaveNames.IncludeConvAngle)
					wavename+="_ConvAngle("+ConvAngle.ToString("#0.0#")+","+ConvAngleInner.ToString("#0.0#")+")recA";
				if(UseAber)
				{
					if(IncludeInWaveNames.IncludeDf)
						wavename+="_Df("+Df.ToString("######0.#")+"A)";
					if(IncludeInWaveNames.IncludeCs)
						wavename+="_Cs("+(Cs/1e7).ToString("##0.0#")+"mm)";
				}
			}
			if(IncludeInWaveNames.IncludeTVNumber)
				wavename+="_(TV_"+(TVi+1)+")";

			wavename+=".cbed" ;
			lwd=Path.Combine(WorkingDirectoryName,wavename) ;
			if((fN=FileRW.Open(lwd,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
			{
				byte[] sout=new byte[160] ;
				string Sout ;
				uint nw=0 ;
				int il,ir,iL ;
				double* buf, source=data ;
				do
				{
					SL=EL ;
					EL=(int)Math.Min(SL+nlines,M) ;
					for(il=SL,buf=(double*)Mbuf.BaseAddress; il<EL; il++)
					{
						if(il<M/2) iL=(int)(M/2+il) ; else iL=(int)(il-M/2) ;
						for(ir=0; ir<N/2; ir++,buf++)
						{
							*buf=Math.Pow(*(source+(iL*N+ir+N/2)*2),2)+Math.Pow(*(source+(iL*N+ir+N/2)*2+1),2) ;
							*(buf+N/2)=Math.Pow(*(source+(iL*N+ir)*2),2)+Math.Pow(*(source+(iL*N+ir)*2+1),2) ;
						}
						buf+=N/2 ;
					}
					nw+=fN.Write((double*)Mbuf.BaseAddress,(uint)(N*(EL-SL))) ;
				} while (EL<M) ;
				if(nw!=M*N*sizeof(double))
				{ // add code to find and display the reason
					Console.WriteLine("Only "+nw+" bytes out of "+M*N*sizeof(double)+" are written to\n"+lwd);
					fN.Close() ;
					Mbuf.Free() ;
					return null ;
				}
				Sout=String.Format("~~~ {0,12:g10} {1,12:g10} {2,12:g10} {3,10:g5} {4,10:g5} {5,10:g8} {6,10:g} {7,00000} {8,00000} {9,10:g} {10,5:n0} ",
					a,b,CurThickness,Math.PI/2.0,Math.PI/2.0,gam,E/1000.0,N, M,CurThickness,0) ;					
				for(int i=0; i<160;i++)
					sout[i]=(i<Sout.Length)?System.Convert.ToByte(Sout[i]):(byte)0;
				fN.Write(sout,160) ;
				fN.Close() ;
				return lwd ;
			}
			else 
			{
				return null ;
			}

		}

		public string FlushSTEMfile()
		{
			FileRW fN ;
			string lwd="", Sout ;
			long nw ;
			byte[] sout=new byte[160] ;
			if(!Directory.Exists(WorkingDirectoryName))
				try 
				{
					Directory.CreateDirectory(WorkingDirectoryName) ;
				} 
				catch (Exception e) 
				{
					Console.WriteLine("Can't create working directory:\n"+e.ToString());
					return null;
				} 
			string wavename ;
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
											wavename = "STEM image at "+ThWaves[iTW].ToString("######0.00")+"A" ;
											if(IncludeInWaveNames.IncludeModelName)
												wavename+="_Model("+Path.GetFileNameWithoutExtension(ModelFileName)+")";
											if(IncludeInWaveNames.IncludePotentialType)
												wavename+="_"+PotType.ToString();
											if(IncludeInWaveNames.IncludeHT)
												wavename+="_E("+Emat[iE]/1000.0+"kV)";
											if(IncludeInWaveNames.IncludeCrystTilts) 
												wavename+="_CrysTilt("+((double)(CrystalTilt[iCT][0]*1000.0)).ToString("##0.0#")+", "+((double)(CrystalTilt[iCT][1]*1000.0)).ToString("##0.0#")+")mrad";
											if(IncludeInWaveNames.IncludeIniWaveType)
												wavename+="_"+IniWaveType.ToString();
											if(IncludeInWaveNames.IncludeBeamTilt)
												wavename+="_BeamTilt("+((double)(BeamTilt[iBT][0]*1000.0)).ToString("##0.0#")+", "+((double)(BeamTilt[iBT][1]*1000.0)).ToString("##0.0#")+")mrad";
											if(IncludeInWaveNames.IncludeConvAngle)
												wavename+="_ConvAngle("+ConvAngles[iCA][1].ToString("#0.0#")+","+ConvAngles[iCA][0].ToString("#0.0#")+")recA";
											if(UseAber)
											{
												if(IncludeInWaveNames.IncludeDf)
													wavename+="_Df("+Dfmat[iDf].ToString("######0.#")+"A)";
												if(IncludeInWaveNames.IncludeCs)
													wavename+="_Cs("+(Csmat[iCs]/1e7).ToString("##0.0#")+"mm)";
											}
											wavename+="_DetectorAngle("+DetectorAngles[iDA][1].ToString("#0.0#")+","+DetectorAngles[iDA][0].ToString("#0.0#")+")recA";

											wavename+=".stem" ;
											lwd=Path.Combine(WorkingDirectoryName,wavename) ;
											if((fN=FileRW.Open(lwd,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
											{
												fixed(double* buf=STEMimages[iE,iBT,iCA,iDf,iCs,iCT,iTW,iDA])
												{
													nw=fN.Write(buf,(uint)BeamShift.Length) ;
												}
												if(nw!=(long)BeamShift.Length*sizeof(double))
												{ // add code to find and display the reason
													Console.WriteLine("Only "+nw+" bytes out of "+(uint)BeamShift.Length*sizeof(double)+" are written to\n"+lwd);
													continue ;
												}
												Sout=String.Format("~~~ {0,12:g10} {1,12:g10} {2,12:g10} {3,10:g5} {4,10:g5} {5,10:g8} {6,10:g} {7,00000} {8,00000} {9,10:g} {10,5:n0} ",
													BeamShift[BeamShift.Length-1][0]-BeamShift[0][0],BeamShift[BeamShift.Length-1][1]-BeamShift[0][1],
													ThWaves[iTW],Math.PI/2.0,Math.PI/2.0,gam,E/1000.0,STEM_N, STEM_M,ThWaves[iTW],0) ;					
												for(int i=0; i<160;i++)
													sout[i]=(i<Sout.Length)?System.Convert.ToByte(Sout[i]):(byte)0;
												fN.Write(sout,160) ;
												fN.Close() ;
												Console.WriteLine("\nSTEM image was saved to "+lwd);
											}
											else 
											{
												Console.WriteLine("Can't create file "+lwd);
												continue ;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			return lwd ;
		}

		public bool SaveSTEM(int iWave)
		{
			double* bli=data ;
			double _A,_B, x,y,cos_g,s,y2,ycos ;
			long i,j,iDA,ii,jj ;
			double[] smin,smax,intDA ;
			smin=new double[DetectorAngles.Length] ;
			smax=new double[DetectorAngles.Length] ;
			intDA=new double[DetectorAngles.Length] ;

			_A=1.0/(a*Math.Sin(gam)) ; _B=1.0/(b*Math.Sin(gam));
			cos_g=Math.Cos(gam) ;
			for(iDA=0;iDA<DetectorAngles.Length;iDA++)
			{
				smin[iDA]=DetectorAngles[iDA][1]*DetectorAngles[iDA][1] ;
				smax[iDA]=DetectorAngles[iDA][0]*DetectorAngles[iDA][0] ;
				intDA[iDA]=0 ;
			}
			for(jj=0 ; jj<M ; jj++ )
			{
				if(jj<M/2) j=jj ;
				else j=jj-M ;
				y=(double)j*_B ;
				y2=y*y ;
				ycos=-2.0*y*cos_g ;
				for(ii=0 ; ii<N ; ii++)
				{
					if(ii<N/2) i=ii ;
					else i=ii-N ;
					x=(double)i*_A ;
					s=x*(x+ycos)+y2  ;
					for(iDA=0;iDA<DetectorAngles.Length;iDA++)
					{
						if((s<=smax[iDA])&&(s>=smin[iDA])) 
						{
							intDA[iDA]+=*bli*(*bli)+*(bli+1)*(*(bli+1)) ;
						}
					} // DetectorAngles cycle
					bli++ ;
					bli++ ;
				} /* x-cycle */
			} /* y-cycle */
			for(iDA=0;iDA<DetectorAngles.Length;iDA++)
			{
				// summation applied in order to allow for TV summation
				STEMimages[iE,iBT,iCA,iDf,iCs,iCT,iWave,iDA][iBS]+=intDA[iDA] ;
			}


			return true ;
		}

		public bool PrepareCurves() 
		{
			if(!IsPotcurve) return true ;

			

			ds=Math.Min((double)N/(2.0*a),(double)M/(2.0*b))/CurveLength ;
			
			if(!PotCurve.Allocate(NTypes*CurveLength*sizeof(double))) return false ;
			PotCurve.Lock(0,NTypes*CurveLength*sizeof(double)) ;
			
			int[] tl=(int[])LookUp.ToArray(typeof(int)) ;
			double[] dw_=(double[])DWList.ToArray(typeof(double)) ;
			if(!IsPotDW) Array.Clear(dw_,0,dw_.Length) ;
			fixed(int* typelist=&tl[0])
			{
				fixed(double* dw=&dw_[0])
				{
					if(IsPotRec) 
						CreatePots_rec_DT_DW(NTypes,CurveLength,ds,typelist,dw,1.0/(a*b*Math.Sin(gam)),(double*)PotCurve.BaseAddress) ;
					else 
						CreatePots_rs_DT_DW(NTypes,CurveLength,dela,typelist,dw,(double*)PotCurve.BaseAddress) ;
				}
			}

			return true ;
		}

	
		public bool AssignParameters(MSProjectInfo test)
		{
			ModelFileName=test.ModelFileName ;
			WorkingDirectoryName=test.WorkingDirectoryName ;//+" ("+DateTime.Now.ToShortDateString()+" "+DateTime.Now.Hour+"_"+DateTime.Now.Minute+"_" +DateTime.Now.Second+")";
			if(test.IniWaveFileName!="last.wav") IniWaveFileName=test.IniWaveFileName ;
			else IniWaveFileName=Path.Combine(WorkingDirectoryName,"last.wav") ;

			PotType=test.PotType ;
			if((PotType==MSProject.PotTypes.PG_DT_TV_periodic)||
				(PotType==MSProject.PotTypes.PG_DT_TV_cut)||
				(PotType==MSProject.PotTypes.PG_curve_TV_periodic)||
				(PotType==MSProject.PotTypes.PG_curve_TV_cut)||
				(PotType==MSProject.PotTypes.PG_curve_TV_pullZ))
				TVNumber=Math.Max(test.TVNumber,1) ;
				else TVNumber=1 ;
			if(TVNumber==1) test.IncludeInWaveNames.IncludeTVNumber=false ;
				else test.IncludeInWaveNames.IncludeTVNumber=true ;
			TVi=0 ;
			SliceType=test.SliceType;
			NSlices=test.NSlices ;
			IsSaveSlices=test.SaveSlices ;
			atrad=test.atrad ;
			dela=test.dela ;
			N=test.N ;
			M=test.M ;
			Nmult=test.Nmult ;
			Mmult=test.Mmult ;
			IsCalcAll=test.IsCalcAll ;
			IsSaveWaves=test.IsSaveWaves ;
			IsSaveDiffractions=test.IsSaveDiffractions ;
			IncludeInWaveNames=test.IncludeInWaveNames ;
			IniWaveType=test.IniWaveType;
			UseAber=test.UseAber;

			if(!test.IsCalcAll)
				try
				{
					ArrayList tmp=new ArrayList(test.ThWaves) ;
					tmp.Sort() ;
					ThWaves=(double[])tmp.ToArray(typeof(double)) ;
				}
				catch(Exception e)
				{
					Console.WriteLine("Can't parse thickness list:\n"+e) ;
					return false ;
				}

			if(IniWaveType==IniWaveTypes.STEMtilt)
			{
				Console.WriteLine("STEMtilt is not supported yet.\n") ;
				return false ;
			}
			else
			{
				if(test.BeamTilt!=null) BeamTilt=test.BeamTilt ;
				else BeamTilt=new double[][] {new double[] {0.0,0.0}} ;
				if(BeamTilt.Length>1) IncludeInWaveNames.IncludeBeamTilt=true ;
			}

			if(test.CrystalTilt!=null)	CrystalTilt=test.CrystalTilt ;
			else CrystalTilt=new double[][] {new double[] {0.0,0.0}} ;
			if(CrystalTilt.Length>1) IncludeInWaveNames.IncludeCrystTilts=true ;


			if(IniWaveType==IniWaveTypes.STEMshift)
			{
				if(test.BeamShift==null)
				{
					Console.WriteLine("STEM parameters are not defined correctly.\n") ;
					return false ;
				}
				if(test.BeamShift.Length<3)
				{
					Console.WriteLine("STEM parameters are not defined correctly.\n") ;
					return false ;
				}
				if((int)test.BeamShift[0][0]*(int)test.BeamShift[0][1]<4)
				{
					Console.WriteLine("STEM parameters are not defined correctly.\n") ;
					return false ;
				}
				STEM_N=(int)test.BeamShift[0][0] ;
				STEM_M=(int)test.BeamShift[0][1] ;
				int i, j, k ;
				double STEMstepx=(test.BeamShift[2][0]-test.BeamShift[1][0])/STEM_N;
				double STEMstepy=(test.BeamShift[2][1]-test.BeamShift[1][1])/STEM_M;
				BeamShift=new double[((int)test.BeamShift[0][0]*(int)test.BeamShift[0][1])][] ;
				for(k=0,j=0;j<test.BeamShift[0][1];j++)
					for(i=0;i<test.BeamShift[0][0];k++,i++)
					{
						BeamShift[k]=new double[] {test.BeamShift[1][0]+i*STEMstepx,test.BeamShift[1][1]+j*STEMstepy} ;
					}
				if(test.DetectorAngles!=null)	DetectorAngles=test.DetectorAngles ;
				else 
				{
					Console.WriteLine("STEM detector is not defined correctly.\n") ;
					return false ;

				}
				IncludeInWaveNames.IncludeBeamShift=true ;
			
			}
			else
			{
				if(test.BeamShift!=null)		BeamShift=test.BeamShift ;
				else BeamShift=new double[][] {new double[] {0.5,0.5}} ;
				if(BeamShift.Length>1) IncludeInWaveNames.IncludeBeamShift=true ;
			}

			if(test.ConvAngles!=null)	ConvAngles=test.ConvAngles ;
			else ConvAngles=new double[][] {new double[] {0.3,0.0}} ;
			if(ConvAngles.Length>1) IncludeInWaveNames.IncludeConvAngle=true ;

			if(UseAber)Dfmat=test.Df ;
			else Dfmat=new double[] {0} ;
			if((Dfmat.Length>1)&&((IniWaveType==IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))&&UseAber) IncludeInWaveNames.IncludeDf=true ;
			
			if(UseAber)
			{ 
				int i;
				Csmat=(double[])test.Cs.Clone();
				for(i=0;i<Csmat.Length;i++)
				{
					Csmat[i]*=1e7 ;
				}
			}
			else Csmat=new double[] {0} ;
			if((Csmat.Length>1)&&((IniWaveType==IniWaveTypes.Converged)||(IniWaveType==MSProject.IniWaveTypes.STEMshift))&&UseAber) IncludeInWaveNames.IncludeCs=true ;
			
			Emat=test.E;
			if(Emat.Length>1) IncludeInWaveNames.IncludeHT=true ;
			

			NThreads=Math.Max(1,test.NThreads) ;
			IsContinue=test.IsContinue ;
			return true ;
		}

		public class Parameters
		{
			public double a,b,c,al,bet,gam, E, thickness ;
			public uint N,M ;
			public int type ;
			public bool result=false ;
			public Parameters()
			{}
			public Parameters(string FName)
			{
				result=ReadFromFile(FName);
			}
			public bool ReadFromFile(string FName)
			{
				FileStream inFile;     
				byte[]  binaryData;
                CultureInfo culture;
               // string locale = Request.UserLanguages[0];
                culture = new CultureInfo("en-US ");

				try 
				{
					inFile = new FileStream(FName,FileMode.Open,FileAccess.Read);
					binaryData = new byte[160];
					inFile.Seek(-160,SeekOrigin.End) ;
					inFile.Read(binaryData,0,160);
					inFile.Close();
				}
				catch (Exception e) 
				{
					// Error creating stream or reading from it.
					Console.WriteLine("Can't open "+FName+"\n"+e.Message) ;
					return false;
				}

				string Strin ;
				fixed(byte* ib=&binaryData[0])
				{
					Strin=new string((sbyte*)ib) ;
				}
				Strin=Strin.Trim() ;
				string[] osin=Strin.Split(' ') ;
				double[] buff=new double[11] ;
				int i=0 ;
				osin[0]=String.Empty ;
				foreach(string arg in osin)
				{
					if(i>=11) 
					{
						break ;	
					}
					if(arg!=String.Empty)
						buff[i++]=double.Parse(arg,culture.NumberFormat) ;
				} 
				if(i<11) return false ;
				this.a=buff[0] ;
				this.b=buff[1] ;
				this.c=buff[2] ;
				this.al=buff[3] ;
				this.bet=buff[4] ;
				this.gam=buff[5] ;
				this.E=buff[6] ;
				this.N=(uint)buff[7] ;
				this.M=(uint)buff[8] ;
				this.thickness=buff[9] ;
				this.type=(int)buff[10] ;

				return true ;
			}
		}

		public bool LoadData(string FileName)
		{
			try 
			{
				FileRW f=FileRW.Open(FileName,FileRW.Action.Open,FileRW.Access.Read) ;
				uint am=f.Read(data,N*M*2) ;
				f.Close() ;
				return (am==(N*M*2*sizeof(double))) ;
			} 
			catch (Exception e) 
			{
				Console.WriteLine("Can't read Data from:\n"+FileName+"\n"+e.ToString());
				return false;
			} 
		}

	}
}