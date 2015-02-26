#include "stdafx.h"

// функци€ принимает кривые атомных потенциалов в RS и координаты атомов
// потенциал атомов обрезаетс€ краем €чейки
extern "C" _declspec(dllexport) int	CreatePropagator(double a,double b,double gam,double thickness, double E,
													 double Xtilt, double Ytilt, 
													 ULONG N,ULONG M,
													 double* gli, float* cpro) 
		{
			*cpro=0 ;
			double *bli ;
			double x,y,GK,XTG,YTG,cos_g,con,s,smax,y2,ycos, dpro=100.0/(double)M ;
			double _A,_B ;
			long i,j,ii,jj ;
			_A=1.0/(a*sin(gam)) ; _B=1.0/(b*sin(gam));
			cos_g=cos(gam) ;
			double lambda=0.387818/sqrt(E/1000.0*(1.0+0.978459e-3*E/1000)) ; // E in V
			GK=M_PI*lambda*thickness ;
			smax=min((double)N*0.5/a,(double)M*0.5/b) ;
			smax*=smax ;
			XTG=2.0*M_PI*thickness*tan(Xtilt) ;
			YTG=2.0*M_PI*thickness*tan(Ytilt) ;
			bli=gli ;
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
					if(s>=smax) 
					{
						*(bli++)=0 ;
						*(bli++)=0 ;
					}
					else 
					{
						con=GK*s-XTG*x-YTG*y ;
						*(bli++)=cos(con) ;
						*(bli++)=sin(con) ;
					}
				} /* x-cycle */
				*cpro+=dpro ;
			} /* y-cycle */
			*cpro=100 ;
			return 0 ;
		}

extern "C" _declspec(dllexport) int	ApplyPropagator(double a,double b,double gam,double thickness, double E,
													 double Xtilt, double Ytilt, 
													 ULONG N,ULONG M,
													 double* gli, float* cpro) 
		{
			*cpro=0 ;
			double *bli ;
			double x,y,GK,XTG,YTG,cos_g,con,s,smax,y2,ycos, dpro=100.0/(double)M ;
			double _A,_B,wr,wi,rr,ri ;
			long i,j,ii,jj ;
			_A=1.0/(a*sin(gam)) ; _B=1.0/(b*sin(gam));
			cos_g=cos(gam) ;
			double lambda=0.387818/sqrt(E/1000.0*(1.0+0.978459e-3*E/1000)) ; // E in V
			GK=M_PI*lambda*thickness ;
			smax=min((double)N*0.5/a,(double)M*0.5/b) ;
			smax*=smax ;
			XTG=2.0*M_PI*thickness*tan(Xtilt) ;
			YTG=2.0*M_PI*thickness*tan(Ytilt) ;
			bli=gli ;
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
					if(s>=smax) 
					{
						*(bli++)=0 ;
						*(bli++)=0 ;
					}
					else 
					{
						con=GK*s-XTG*x-YTG*y ;
						rr=cos(con) ;
						ri=sin(con) ;
						wr=*bli ;
						wi=*(bli+1) ;
						*(bli++)=wr*rr-wi*ri ;
						*(bli++)=wr*ri+wi*rr ;
					}
				} /* x-cycle */
				*cpro+=dpro ;
			} /* y-cycle */
			*cpro=100 ;
			return 0 ;
		}
