using System;
using System.IO;
using System.Timers ;
using System.Threading;
using System.Diagnostics;
using System.Collections;
using System.Globalization;
using System.Xml.Serialization ;

namespace MusliConsole
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
#if DEBUG
#if  !_WIN32 
        public const string LibPath="..\\..\\..\\Muslib\\x64\\Debug\\Muslib" ;

#else
        public const string LibPath="..\\..\\Muslib\\Debug\\Muslib" ;

#endif
        //public const string FFTWPath = "..\\..\\..\\fftw3";
        public const string FFTWPath = "fftw3";

#else
#if  !_WIN32 
        public const string LibPath="..\\..\\..\\Muslib\\x64\\Release\\Muslib" ;

#else
        public const string LibPath="..\\..\\Muslib\\Release\\Muslib" ;

#endif

//		public const string LibPath="Muslib" ;
		public const string FFTWPath="fftw3" ;
		//public const string FFTWPath=Process.GetCurrentProcess().StartInfo.WorkingDirectory ;
#endif
        static bool IsContinue=false ;
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{

#if DEBUG
			Console.WriteLine("DEBUG build "+MemMan.GetCurrentVersion());
#else
         Console.WriteLine("Release build "+MemMan.GetCurrentVersion());
#endif
#if  !_WIN32 
		Console.WriteLine(" x64 ");

#else
		Console.WriteLine("DEBUG win32 ");
        //Console.WriteLine(Path.GetDirectoryName(Application.ExecutablePath));
       // Console.WriteLine("GetExecutingAssembly=" + Assembly.GetExecutingAssembly().FullName);

        Console.WriteLine("DEBUG win32 ");

#endif
			projectstarter(args);

			if(!IsContinue) Console.Read() ;
		
		}


		private static void projectstarter(string[] args)
		{
			string InfoFile="" ;
			MSProject tmpun ;
			ProjectStarter proj=new ProjectStarter() ;
			if(!proj.result) return ;

			if(args.Length==0)
			{
				Console.Write("Parameters file: ") ;
				InfoFile=Console.ReadLine() ;
			}
			else
			{
				if(args[0].Equals("/?"))
				{
					Console.WriteLine("To create sample XML file - /xml filename");
					Console.WriteLine("XML file enums:");
					Console.WriteLine("\n  Potential types:");
					Console.WriteLine(@"   reuse potential calculation:
      rs_DT_DW_cut
      rs_DT_DW_periodic
      rs_curve_DW_cut
      rs_curve_DW_periodic
      rec_DT_DW
      rec_curve_DW");
					Console.WriteLine(@"   inline potential calculation:
      PG_DT_DW_cut
      PG_DT_DW_periodic
      PG_DT_TV_cut
      PG_DT_TV_periodic
      PG_curve_DW_cut
      PG_curve_DW_periodic
      PG_curve_TV_cut
      PG_curve_TV_periodic");
					Console.WriteLine("\n  Slicing schemes:");
					Console.WriteLine(@"     Layers
     Fixed");
					Console.WriteLine("\n  Initial wave types:");
					Console.WriteLine(@"     UntiltedPlaneWave
     Converged,
     Precalculated") ;


					return ;
				}
				else
					if(args[0].Equals("/xml"))
				{
					createMSProjectInfoXML(args[1]) ;
					return ;
				}
				else
				{
					InfoFile=args[0] ;
					if(args.Length>1)
					{
						Console.SetOut(new StreamWriter(args[1]));
					}
				}
			}
			// read project info from xml file
			// Загрузка параметров
			MSProjectInfo test=new MSProjectInfo() ;
			XmlSerializer mySerializer = new XmlSerializer(typeof(MSProjectInfo));
			try
			{  
				FileStream fs = new FileStream(InfoFile, FileMode.Open);
				test = (MSProjectInfo) mySerializer.Deserialize(fs);
			}
			catch(Exception e)
			{
				Console.WriteLine("Can't read parameters: "+e.Message) ;
				return ;
			}
			IsContinue=test.IsContinue ;
			if(test.IsPotInline)
			{tmpun=new MSProjectUseOnce() ;}
			else
			{tmpun=new MSProjectReuse() ;}

	
			if(!tmpun.AssignParameters(test))
			{
				Console.WriteLine("Abnormal program termination");
				return ;
			}
			proj.tmp=tmpun ;

			if(!proj.StartProc())
				Console.WriteLine("Abnormal program termination");

		}
	
		static void createMSProjectInfoXML(string XMLFileName)
		{
			MSProjectInfo test=new MSProjectInfo() ;
			XmlSerializer mySerializer = new XmlSerializer(typeof(MSProjectInfo));

			try
			{
				if(!XMLFileName.ToLower().EndsWith(".xml")) XMLFileName+=".xml" ;
				XMLFileName=Path.GetFullPath(XMLFileName);
				StreamWriter myWriter = new StreamWriter(XMLFileName);
				mySerializer.Serialize(myWriter, test);
				Console.WriteLine("XML example was created as:\n "+XMLFileName) ;
				return ;
			}
			catch(Exception e)
			{
				Console.WriteLine("Can't create XML file: "+e.Message) ;
				return ;
			}
		}
	}
}
