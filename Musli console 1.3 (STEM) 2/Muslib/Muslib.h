
struct DoyleTurner
{int z; char name[4];
double ab[4][2] ;
};
extern DoyleTurner bank[] ;





double scat_ampl_rec_DT_DW(int i, double s);
extern "C" _declspec(dllexport)double scat_ampl_rs2_DT_DW(int i, double s, double dw) ;
extern "C" _declspec(dllexport)double scat_ampl_rs2_DT_DW0(int i, double s) ;
extern "C" _declspec(dllexport) void CreatePots_rec_DT_DW(ULONG Ntypes,ULONG Npoints,double dds,LONG *types,double *DW,double _Vol, double *sc) ;

