using System;
using System.IO;
using System.Collections;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	public unsafe abstract class MSCalculator
	{
		public double* data ;
		public double CurThickness ;
		public double E ;
		public uint N, M, NSlices ;
		public double a,b,gam ; // gam in rad
		public MSProject.SaveWaveDelegate SaveWav ;
		public MSProject.SaveWaveDelegate SaveDiffraction ;
		public MSProject.SaveSTEMDelegate SaveSTEM ;
		public double[] ThWaves ; // thicknesses to save waves
		public bool IsSaveWaves ;
		public bool IsSaveDiffractions ;
		public bool IsSaveSTEM ;
		public SFT SFT_direct ;
		public SFT SFT_backward ;
		public string LastSavedWave ;

		public MSCalculator()
		{
			// 
			// TODO: Add constructor logic here
			//
		}
		abstract public bool RunCalculation() ;

	}
}
