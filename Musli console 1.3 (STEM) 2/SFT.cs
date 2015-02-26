using System;
using System.IO;
using System.Runtime.InteropServices;

namespace MusliConsole
{
	/// <summary>
	/// 
	/// </summary>
	unsafe public class SFT
	{
		[DllImport(Class1.FFTWPath)]
		static extern void* fftw_plan_dft_2d(short nx, short ny,
			double* indata, double* outdata,
			short sign, ushort flags);

		[DllImport(Class1.FFTWPath)]
		static extern void* fftw_plan_dft_1d(short n,
			double* indata, double* outdata,
			short sign, ushort flags);

		[DllImport(Class1.FFTWPath)]
		static extern void* fftw_plan_r2r_1d(short n,
			double* indata, double* outdata,
			ushort kind, ushort flags);

		[DllImport(Class1.FFTWPath)]
		static extern void fftw_execute(void* plan);

		[DllImport(Class1.FFTWPath)]
		static extern void fftw_execute_dft(void* plan, double *indata, double *outdata);
		/*		[DllImport(liblocation)]
				static extern int fftw_init_threads();

				[DllImport(liblocation)]
				static extern void fftw_plan_with_nthreads(short nthreads);
		*/
		[DllImport(Class1.FFTWPath)]
		static extern void fftw_destroy_plan(void* plan);

		[DllImport(Class1.FFTWPath)]
		static extern byte *fftw_export_wisdom_to_string();
		
		[DllImport(Class1.FFTWPath)]
		static extern int fftw_import_wisdom_from_string(byte *input_string);

		[DllImport(Class1.FFTWPath)]
		static extern void fftw_free(void *p);

		static public string wisdom_path ;
		void* plan ;
		uint N,M ;
		int tf ;
		public delegate void SFTdelegate(double *data) ;
		public SFTdelegate MakeSFT ;
		public SFT()
		{
			wisdom_path="FFTW.wisdom" ;
			plan=null ;
			ReadWisdom() ;
		}
		~SFT()
		{
			if(plan!=null) fftw_destroy_plan(plan);
		}

		public bool Ready
		{
			get
			{
				return plan!=null ;
			}
		}
		public static void SaveWisdom()
		{
			byte* wisd=fftw_export_wisdom_to_string();
			uint i ;
			for(i=0;(*(wisd+i))!=0;i++) ;
			try
			{
				FileRW fN ;
				if((fN=FileRW.Open(wisdom_path,FileRW.Action.CreateAlways,FileRW.Access.Write))!=null)
				{
					fN.Write(wisd,i) ;
					fN.Close() ;

				}
				fftw_free(wisd) ;
			}
			catch
			{}
		}
		public static void ReadWisdom()
		{
			if(File.Exists(wisdom_path))
			{
				try
				{
					FileStream fs = new FileStream(wisdom_path,FileMode.Open, FileAccess.Read);
					BinaryReader w = new BinaryReader(fs);
					byte[] wisd=w.ReadBytes((int)fs.Length) ;
					w.Close() ;
					fixed(byte* istr=&wisd[0])
					{
						fftw_import_wisdom_from_string(istr);
					}
				}
				catch(Exception e)
				{
					Console.WriteLine("SFT ----> read wisdom from file - error\n"+e) ;
				}
			}
			else
			{
				Console.WriteLine("SFT ----> read wisdom from file - no such file") ;
			}
		}

		public void MakePlan(double* data, uint x, uint y, short dir)
		{
			if(plan!=null) fftw_destroy_plan(plan);
			if(y==1)plan=fftw_plan_dft_1d((short)x, data, data,(short)-dir, (ushort)Flags.FFTW_PATIENT);
			else plan=fftw_plan_dft_2d((short)y, (short)x, data, data,(short)-dir, (ushort)Flags.FFTW_PATIENT);
			 N=x; M=y; tf=dir ;
			SaveWisdom() ;
			MakeSFT=new SFTdelegate(SFT2DMem) ;
		}
		public void MakePlan_r2r(double* data, uint x)
		{
			if(plan!=null) fftw_destroy_plan(plan);
			plan=fftw_plan_r2r_1d((short)x,data,data,(ushort)Kinds_of_r2r.FFTW_REDFT00,(ushort)Flags.FFTW_PATIENT) ;

			 N=x; M=1; tf=0 ;
			SaveWisdom() ;
			MakeSFT=new SFTdelegate(SFT_r2r) ;
		}

		void SFT2DMem(double* data)
		{
			fftw_execute_dft(plan,data,data);
			if(tf<0)
			{
				double kof;
				double* p;
				long j ;
				kof=(double)N*(double)M ;

				for(p=data,j=0;j<N*M*2 ;p++, j++)  *p/=kof ;
			}
		}

		void SFT_r2r(double* data)
		{
			fftw_execute_dft(plan,data,data);
				double kof;
				double* p;
				long j ;
				kof=(double)N ;

				for(p=data,j=0;j<N ;p++, j++)  *p/=kof ;
		}
		public enum Flags:ushort   
		{
			FFTW_MEASURE=0,
			FFTW_DESTROY_INPUT=((ushort)1) << 0,
			FFTW_UNALIGNED=((ushort)1) << 1,
			FFTW_CONSERVE_MEMORY=((ushort)1) << 2,
			FFTW_EXHAUSTIVE=((ushort)1) << 3, /* NO_EXHAUSTIVE is default */
			FFTW_PRESERVE_INPUT=((ushort)1) << 4, /* cancels FFTW_DESTROY_INPUT */
			FFTW_PATIENT=((ushort)1) << 5, /* IMPATIENT is default */
			FFTW_ESTIMATE=((ushort)1) << 6
		}
		public enum Kinds_of_r2r:ushort   
		{
			FFTW_R2HC=0,
			FFTW_HC2R=1,
			FFTW_DHT=2,
			FFTW_REDFT00=3,
			FFTW_REDFT01=4,
			FFTW_REDFT10=5,
			FFTW_REDFT11=6,
			FFTW_RODFT00=7,
			FFTW_RODFT01=8,
			FFTW_RODFT10=9,
			FFTW_RODFT11=10
		}

	}
}
