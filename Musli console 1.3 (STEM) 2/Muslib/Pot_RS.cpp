#include "stdafx.h"

// функция принимает кривые атомных потенциалов в RS и координаты атомов
// потенциал атомов обрезается краем ячейки
extern "C" _declspec(dllexport) int Potential_rs_curve_DW_cutpot(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ,LONG* type,
											   double *scatentryes,ULONG Npoints, double dds,
											   double* gli, float* cpro)
{// здесь type - номера атомных кривых, не атомные номера элементов!!
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,jb,je,ib,ie,jjj ;
	double* curk, *cgli, *centr ;
	double mpro=100.0/(double)Na ;
	double x,y,yyy, yycos,x0,y0,drax,dray,dxa,dyb, ar2, rem, res ;

	drax=atrad*sin_gam/a ;
	dray=atrad*sin_gam/b ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			x0=*(xx+k)*a ;
			y0=*(yy+k)*b ;
			curk=(scatentryes+*(type+k)*Npoints) ;
		    ib=max((long)((*(xx+k)-drax)*N),0) ;
			ie=min((long)((*(xx+k)+drax)*N),N) ;
			jb=max((long)((*(yy+k)-dray)*M),0) ;
			je=min((long)((*(yy+k)+dray)*M),M) ;

			for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
			{
				yycos=2*y*cos_gam ;
				yyy=y*y;
				for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
				{
					ar2=sqrt(x*(x+yycos)+yyy)/dds ;
					jjj=(ULONG)floor(ar2) ;
					if(jjj>=Npoints-1) continue ;
					rem=ar2-jjj ;
					centr=curk+jjj ;
					res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);

					*cgli+=res ;
					*(cgli+1)+=res**(ads+k) ;
				}
			}
          *cpro+=mpro ;
         }



return 0 ;
}

extern "C" _declspec(dllexport) int Potential_rs_curve_TV_cutpot(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ, LONG* type,
											   double *scatentryes,ULONG Npoints, double dds,
											   double* gli, float* cpro)
{
	// здесь type - номера атомных кривых, не атомные номера элементов!!
	// координаты уже пересчитаны в ангстремы и потрясены
	// dw и occ не имеют смысла, потому и не считаются
   double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,jb,je,ib,ie,jjj ;
	double* curk, *cgli, *centr ;
	double mpro=100.0/(double)Na ;
	double x,y,yyy, yycos,x0,y0,drax,dray,dxa,dyb, ar2, rem, res ;

	drax=atrad*sin_gam ;
	dray=atrad*sin_gam ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			x0=*(xx+k) ;
			y0=*(yy+k) ;
			curk=(scatentryes+*(type+k)*Npoints) ;
		    ib=max((long)((*(xx+k)-drax)*N)/a,0) ;
			ie=min((long)((*(xx+k)+drax)*N)/a,N) ;
			jb=max((long)((*(yy+k)-dray)*M)/b,0) ;
			je=min((long)((*(yy+k)+dray)*M)/b,M) ;

			for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
			{
				yycos=2*y*cos_gam ;
				yyy=y*y;
				for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
				{
					ar2=sqrt(x*(x+yycos)+yyy)/dds ;
					jjj=(ULONG)floor(ar2) ;
					if(jjj>=Npoints-1) continue ;
					rem=ar2-jjj ;
					centr=curk+jjj ;
					res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);

					*cgli+=res ;
					*(cgli+1)+=res**(ads+k) ;
				}
			}
          *cpro+=mpro ;
         }



return 0 ;
}
extern "C" _declspec(dllexport) int Potential_rs_DT_DW_cutpot(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double *dw,double* ads, double* occ,LONG* type,
											   double* gli, float* cpro)
{// здесь type - атомные номера элементов
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,jb,je,ib,ie ;
	double *cgli ;
	double mpro=100.0/(double)Na ;
	double x,y,yyy,yycos,x0,y0,drax,dray,dxa,dyb, ar2, res ;

	drax=atrad*sin_gam/a ;
	dray=atrad*sin_gam/b ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			x0=*(xx+k)*a ;
			y0=*(yy+k)*b ;
		    ib=max((long)((*(xx+k)-drax)*N),0) ;
			ie=min((long)((*(xx+k)+drax)*N),N) ;
			jb=max((long)((*(yy+k)-dray)*M),0) ;
			je=min((long)((*(yy+k)+dray)*M),M) ;

			for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
			{
				yycos=2*y*cos_gam ;
				yyy=y*y;
				for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
				{
					ar2=x*(x+yycos)+yyy ;
					res=scat_ampl_rs2_DT_DW(*(type+k),ar2,*(dw+k))**(occ+k);

					*cgli+=res ;
					*(cgli+1)+=res**(ads+k) ;
				}
			}
          *cpro+=mpro ;
         }



return 0 ;
}

extern "C" _declspec(dllexport) int Potential_rs_DT_TV_cutpot(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ,LONG* type,
											   double* gli, float* cpro)
{
	// здесь type - атомные номера элементов
	// координаты уже пересчитаны в ангстремы и потрясены
	// dw и occ не имеют смысла, потому и не считаются

    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,jb,je,ib,ie ;
	double *cgli ;
	double mpro=100.0/(double)Na ;
	double x,y,yyy,yycos,x0,y0,drax,dray,dxa,dyb, ar2, res ;

	drax=atrad*sin_gam ;
	dray=atrad*sin_gam ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			x0=*(xx+k) ;
			y0=*(yy+k) ;
		    ib=max((long)((*(xx+k)-drax)*N/a),0) ;
			ie=min((long)((*(xx+k)+drax)*N/a),N) ;
			jb=max((long)((*(yy+k)-dray)*M/b),0) ;
			je=min((long)((*(yy+k)+dray)*M/b),M) ;

			for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
			{
				yycos=2*y*cos_gam ;
				yyy=y*y;
				for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
				{
					ar2=x*(x+yycos)+yyy ;
					res=scat_ampl_rs2_DT_DW0(*(type+k),ar2)**(occ+k);

					*cgli+=res ;
					*(cgli+1)+=res**(ads+k) ;
				}
			}
          *cpro+=mpro ;
         }



return 0 ;
}

// функция принимает кривые атомных потенциалов в RS и координаты атомов
// потенциал атомов периодически продолжается с другого края ячейки
extern "C" _declspec(dllexport) int Potential_rs_curve_DW_periodic(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ,LONG* type,
											   double *scatentryes,ULONG Npoints, double dds,
											   double* gli, float* cpro)
{// здесь type - номера атомных кривых, не атомные номера элементов!!
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,l, jb,je,ib,ie,jjj ;
	long jbb, jee, ibb, iee ;
	double* curk, *cgli, *centr ;
	double mpro=100.0/(double)Na ;
	double x,y,yycos,yyy, x0,y0,drax,dray,dxa,dyb, ar2, rem, res ;

	drax=atrad*sin_gam/a ;
	dray=atrad*sin_gam/b ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			curk=(scatentryes+*(type+k)*Npoints) ;
			ibb=(long)((*(xx+k)-drax)*N);
			iee=(long)((*(xx+k)+drax)*N);
			jbb=(long)((*(yy+k)-dray)*M);
			jee=(long)((*(yy+k)+dray)*M);

			for(l=0;l<9;l++)
			{
				switch(l)
				{
				case 0: // рисуем потенциал в том квадранте где и атом
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b ;
					break ;
				case 1: // рисуем верхний край снизу
					if(jbb>=0) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b+b ;
					break ;
				case 2: // рисуем нижний край сверху
					if(jee<M) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b-b ;
					break ;
				case 3:// рисуем левый край справа
					if(ibb>=0) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b ;
					break ;
				case 4:// рисуем правый край слева
					if(iee<N) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b ;
					break ;
				case 5:// рисуем левый верхний угол справа внизу
					if((ibb>=0)||(jbb>=0)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b+b ;
					break ;
					break ;
				case 6:// рисуем правый нижний угол слева вверху
					if((iee<N)||(jee<M)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b-b ;
					break ;
				case 7:// рисуем правый верхний угол слева внизу
					if((iee<N)||(jbb>=0)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b+b ;
					break ;
				case 8:// рисуем левый нижний угол справа вверху
					if((ibb>=0)||(jee<M)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b-b ;
					break ;

				}
				for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
				{
					yycos=2*y*cos_gam ;
					yyy=y*y;
					for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
					{
						ar2=sqrt(x*(x+yycos)+yyy)/dds ;
						jjj=(ULONG)floor(ar2) ;
						if(jjj>=Npoints-1) continue ;
						rem=ar2-jjj ;
						centr=curk+jjj ;
						res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);

						*cgli+=res ;
						*(cgli+1)+=res**(ads+k) ;
					}
				}
			}



          *cpro+=mpro ;
		}


*cpro=100.0 ;
return 0 ;
}
extern "C" _declspec(dllexport) int Potential_rs_DT_DW_periodic(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double *dw,double* ads, double* occ,LONG* type,
											   double* gli, float* cpro)
{// здесь type - номера атомных кривых, не атомные номера элементов!!
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,l, jb,je,ib,ie ;
	long jbb, jee, ibb, iee ;
	double *cgli ;
	double mpro=100.0/(double)Na ;
	double x,y,yycos,yyy,x0,y0,drax,dray,dxa,dyb, ar2, res ;

	drax=atrad*sin_gam/a ;
	dray=atrad*sin_gam/b ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			ibb=(long)((*(xx+k)-drax)*N);
			iee=(long)((*(xx+k)+drax)*N);
			jbb=(long)((*(yy+k)-dray)*M);
			jee=(long)((*(yy+k)+dray)*M);

			for(l=0;l<9;l++)
			{
				switch(l)
				{
				case 0: // рисуем потенциал в том квадранте где и атом
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b ;
					break ;
				case 1: // рисуем верхний край снизу
					if(jbb>=0) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b+b ;
					break ;
				case 2: // рисуем нижний край сверху
					if(jee<M) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a ;
					y0=*(yy+k)*b-b ;
					break ;
				case 3:// рисуем левый край справа
					if(ibb>=0) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b ;
					break ;
				case 4:// рисуем правый край слева
					if(iee<N) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b ;
					break ;
				case 5:// рисуем левый верхний угол справа внизу
					if((ibb>=0)||(jbb>=0)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b+b ;
					break ;
					break ;
				case 6:// рисуем правый нижний угол слева вверху
					if((iee<N)||(jee<M)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b-b ;
					break ;
				case 7:// рисуем правый верхний угол слева внизу
					if((iee<N)||(jbb>=0)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)*a-a ;
					y0=*(yy+k)*b+b ;
					break ;
				case 8:// рисуем левый нижний угол справа вверху
					if((ibb>=0)||(jee<M)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)*a+a ;
					y0=*(yy+k)*b-b ;
					break ;

				}
				for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
				{
					yycos=2*y*cos_gam ;
					yyy=y*y;
					for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
					{
						ar2=x*(x+yycos)+yyy ;
						res=scat_ampl_rs2_DT_DW(*(type+k),ar2,*(dw+k))**(occ+k);

						*cgli+=res ;
						*(cgli+1)+=res**(ads+k) ;
					}
				}
			}



          *cpro+=mpro ;
		}


*cpro=100.0 ;
return 0 ;
}
// функция принимает кривые атомных потенциалов в RS и координаты атомов
// потенциал атомов периодически продолжается с другого края ячейки
// замороженные фононы - соординаты уже в ангстемах, уже потрясены, occ=1 и кривые с DW=0
extern "C" _declspec(dllexport) int Potential_rs_curve_TV_periodic(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ, LONG* type,
											   double *scatentryes,ULONG Npoints, double dds,
											   double* gli, float* cpro)
{// здесь type - номера атомных кривых, не атомные номера элементов!!
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,l, jb,je,ib,ie,jjj ;
	long jbb, jee, ibb, iee ;
	double* curk, *cgli, *centr ;
	double mpro=100.0/(double)Na ;
	double x,y,yycos,yyy, x0,y0,drax,dray,dxa,dyb, ar2, rem, res ;

	drax=atrad*sin_gam ;
	dray=atrad*sin_gam ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			curk=(scatentryes+*(type+k)*Npoints) ;
			ibb=(long)((*(xx+k)-drax)/a*N);
			iee=(long)((*(xx+k)+drax)/a*N);
			jbb=(long)((*(yy+k)-dray)/b*M);
			jee=(long)((*(yy+k)+dray)/b*M);

			for(l=0;l<9;l++)
			{
				switch(l)
				{
				case 0: // рисуем потенциал в том квадранте где и атом
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k) ;
					y0=*(yy+k) ;
					break ;
				case 1: // рисуем верхний край снизу
					if(jbb>=0) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k) ;
					y0=*(yy+k)+b ;
					break ;
				case 2: // рисуем нижний край сверху
					if(jee<M) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k) ;
					y0=*(yy+k)-b ;
					break ;
				case 3:// рисуем левый край справа
					if(ibb>=0) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)+a ;
					y0=*(yy+k) ;
					break ;
				case 4:// рисуем правый край слева
					if(iee<N) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)-a ;
					y0=*(yy+k) ;
					break ;
				case 5:// рисуем левый верхний угол справа внизу
					if((ibb>=0)||(jbb>=0)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)+a ;
					y0=*(yy+k)+b ;
					break ;
					break ;
				case 6:// рисуем правый нижний угол слева вверху
					if((iee<N)||(jee<M)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)-a ;
					y0=*(yy+k)-b ;
					break ;
				case 7:// рисуем правый верхний угол слева внизу
					if((iee<N)||(jbb>=0)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)-a ;
					y0=*(yy+k)+b ;
					break ;
				case 8:// рисуем левый нижний угол справа вверху
					if((ibb>=0)||(jee<M)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)+a ;
					y0=*(yy+k)-b ;
					break ;

				}
				for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
				{
					yycos=2*y*cos_gam ;
					yyy=y*y;
					for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
					{
						ar2=sqrt(x*(x+yycos)+yyy)/dds ;
						jjj=(ULONG)floor(ar2) ;
						if(jjj>=Npoints-1) continue ;
						rem=ar2-jjj ;
						centr=curk+jjj ;
						res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);

						*cgli+=res ;
						*(cgli+1)+=res**(ads+k) ;
					}
				}
			}



          *cpro+=mpro ;
		}


*cpro=100.0 ;
return 0 ;
}
extern "C" _declspec(dllexport) int Potential_rs_DT_TV_periodic(double a,double b,double gam, double atrad,
											   ULONG N,ULONG M,ULONG Na,
											   double* xx,double* yy, double* ads, double* occ,LONG* type,
											   double* gli, float* cpro)
{// здесь type - номера атомных кривых, не атомные номера элементов!!
    double sin_gam=sin(gam) ;
	double cos_gam=cos(gam) ;
	ULONG  k,i,j,l, jb,je,ib,ie ;
	long jbb, jee, ibb, iee ;
	double *cgli ;
	double mpro=100.0/(double)Na ;
	double x,y,yycos,yyy,x0,y0,drax,dray,dxa,dyb, ar2, res ;

	drax=atrad*sin_gam ;
	dray=atrad*sin_gam ;
	dxa=a/(double)N ;
	dyb=b/(double)M ;
	for(k=0;k<Na;k++)
		{
			ibb=(long)((*(xx+k)-drax)/a*N);
			iee=(long)((*(xx+k)+drax)/a*N);
			jbb=(long)((*(yy+k)-dray)/b*M);
			jee=(long)((*(yy+k)+dray)/b*M);

			for(l=0;l<9;l++)
			{
				switch(l)
				{
				case 0: // рисуем потенциал в том квадранте где и атом
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k) ;
					y0=*(yy+k) ;
					break ;
				case 1: // рисуем верхний край снизу
					if(jbb>=0) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k) ;
					y0=*(yy+k)+b ;
					break ;
				case 2: // рисуем нижний край сверху
					if(jee<M) continue ;
					ib=max(ibb,0) ;
					ie=min(iee,N) ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k) ;
					y0=*(yy+k)-b ;
					break ;
				case 3:// рисуем левый край справа
					if(ibb>=0) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)+a ;
					y0=*(yy+k) ;
					break ;
				case 4:// рисуем правый край слева
					if(iee<N) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=max(jbb,0) ;
					je=min(jee,M) ;
					x0=*(xx+k)-a ;
					y0=*(yy+k) ;
					break ;
				case 5:// рисуем левый верхний угол справа внизу
					if((ibb>=0)||(jbb>=0)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)+a ;
					y0=*(yy+k)+b ;
					break ;
					break ;
				case 6:// рисуем правый нижний угол слева вверху
					if((iee<N)||(jee<M)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)-a ;
					y0=*(yy+k)-b ;
					break ;
				case 7:// рисуем правый верхний угол слева внизу
					if((iee<N)||(jbb>=0)) continue ;
					ib=0 ;
					ie=iee-N+1 ;
					jb=jbb+M ;
					je=M ;
					x0=*(xx+k)-a ;
					y0=*(yy+k)+b ;
					break ;
				case 8:// рисуем левый нижний угол справа вверху
					if((ibb>=0)||(jee<M)) continue ;
					ib=ibb+N ;
					ie=N ;
					jb=0 ;
					je=jee-M+1 ;
					x0=*(xx+k)+a ;
					y0=*(yy+k)-b ;
					break ;

				}
				for(j=jb,y=jb*dyb-y0 ; j<je;j++,y+=dyb)
				{
					yycos=2*y*cos_gam ;
					yyy=y*y;
					for(i=ib,x=ib*dxa-x0,cgli=gli+j*N*(ULONG)2+ib*2 ; i<ie ;i++, x+=dxa, cgli+=2)
					{
						ar2=x*(x+yycos)+yyy ;
						res=scat_ampl_rs2_DT_DW0(*(type+k),ar2)**(occ+k);

						*cgli+=res ;
						*(cgli+1)+=res**(ads+k) ;
					}
				}
			}



          *cpro+=mpro ;
		}


*cpro=100.0 ;
return 0 ;
}
