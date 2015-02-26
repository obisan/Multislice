using System;
using System.Collections;
using System.Xml.Serialization ;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	[XmlRootAttribute("Calculation_parameters")]
	public class MSProjectInfo
	{
		/// <summary>
		/// COO file with atom coordinates
		/// </summary>
		public string ModelFileName ;

		public MSProject.PotTypes PotType ;
		public int TVNumber ;
		public MSProject.SliceTypes SliceType ;
		public uint NSlices ;
		public bool SaveSlices ;
		public double atrad,dela ;
		public double[][] CrystalTilt ;
		public bool IsCalcAll ;
		public bool IsSaveWaves ;
		public bool IsSaveDiffractions ;
		public WaveNameInclusions IncludeInWaveNames ;
		public double[] ThWaves ; // thicknesses to save waves
		public uint N, M ;
		public uint Nmult,Mmult ;

		public double[] E ;
		public MSProject.IniWaveTypes IniWaveType ;
		public string IniWaveFileName ;
		public double[][] BeamTilt,BeamShift,ConvAngles,DetectorAngles ;
		public double[] Df,Cs ;
		public bool UseAber ;



		public int NThreads ;
		public string WorkingDirectoryName ;
		public bool IsContinue ;

		public MSProjectInfo()
		{
			ModelFileName="any.coo" ;
			WorkingDirectoryName="tmp.dir" ;
			IniWaveFileName="last.wav" ;
			NSlices=1 ;
			BeamShift=new double[][] {new double[] {0.5,0.5}} ;
			BeamTilt=new double[][] {new double[] {0.0,0.0}} ;
			ConvAngles=new double[][] {new double[] {0.3,0.0}} ;
			CrystalTilt=new double[][] {new double[] {0.0,0.0}} ;
			ThWaves= new double[] {0,100,200} ;
			N=M=512 ;
			NThreads=1 ;
			atrad=3 ;
			dela=0.001 ;
			E=new double[] {100000} ;
			Df=new double[] {0} ;
			Cs=new double[] {0} ;
			IniWaveType=MSProject.IniWaveTypes.PlaneWave ;
			PotType=MSProject.PotTypes.PG_curve_DW_periodic ;
			TVNumber=1 ;
			SliceType=MSProject.SliceTypes.Layers ;
			Nmult=Mmult=1 ;
			IsContinue=false ;
			IsCalcAll=false ;
			IsSaveWaves=true ;
		}

		public bool IsPotInline
		{
			get
			{	
				return (PotType==MSProject.PotTypes.PG_curve_DW_cut)||(PotType==MSProject.PotTypes.PG_curve_TV_cut)||
					(PotType==MSProject.PotTypes.PG_DT_DW_cut)||(PotType==MSProject.PotTypes.PG_DT_TV_cut)||
					(PotType==MSProject.PotTypes.PG_curve_DW_periodic)||(PotType==MSProject.PotTypes.PG_DT_DW_periodic)||
					(PotType==MSProject.PotTypes.PG_curve_TV_periodic)||(PotType==MSProject.PotTypes.PG_DT_TV_periodic)||
					(PotType==MSProject.PotTypes.PG_curve_DW_pullZ)||(PotType==MSProject.PotTypes.PG_curve_TV_pullZ) ;

			}
		}



		public struct WaveNameInclusions
		{
			public bool IncludeModelName ;
			public bool IncludePotentialType ;
			public bool IncludeHT ;
			public bool IncludeCrystTilts ;
			public bool IncludeIniWaveType ;
			public bool IncludeBeamTilt ;
			public bool IncludeBeamShift ;
			public bool IncludeConvAngle ;
			public bool IncludeDf ;
			public bool IncludeCs ;
			public bool IncludeTVNumber ;

		}

	}
}
