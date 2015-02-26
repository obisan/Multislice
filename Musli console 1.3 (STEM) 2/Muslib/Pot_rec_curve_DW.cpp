#include "stdafx.h"



// ����������� ������������������ Npoints � Ntypes - ����������� ����� � Potential

extern "C" _declspec(dllexport) int Potential_rec_curve_DW(double a,double b,double gam,ULONG Nmult, ULONG Mmult,
											   ULONG N,ULONG M,ULONG Na,ULONG SL, ULONG EL,
											   double* xx,double* yy, double* ads,double* occ,LONG* type,
											   double *scatentryes,ULONG Npoints, double dds,
											   double* gli, float* cpro)
{// ����� type - ������ ������� ������, �� ������� ������ ���������!!
    double cos_gam=-cos(gam) ;
    double  _A=1./(a*sin(gam));
    double  _B=1./(b*sin(gam));
    long jj,ii,k ;
    long j,i ;
    ULONG jjj;
    double rem, *centr, *curk ;
    double *bli, *blii,*vli, *vlii,x,y,s,ss,res,res1,rr,ri ;
    double mpro=200./(double)M ;
    double _Vol=1./(a*b*sin(gam)) ; 
    bool isupin, islowin ;
    double y_pow2,y_cosgam, cosj, sinj,cosi,sini,cosicosj, sinisinj,cosisinj, cosjsini, con;
    double ds=1.0/dds ;

	if(SL==0) 
	{
		vli=gli ; 
		for(k=0;k<Na;k++)
		{
			curk=(scatentryes+*(type+k)*Npoints) ;
			res=*curk**(occ+k) ;
			*vli+=res ;       // 0,0 reflection
		    *(vli+1)+=*(ads+k)*res ;
		    
			for(i=1;(ii=i*Nmult)<N/2;i++)
			{
                x=(double)i*_A ;   
                s=x*0.5*ds ;
                jjj=(ULONG)floor(s) ;
		        if(jjj>=Npoints-1) continue ;
		        rem=s-jjj ;
			    centr=curk+jjj ;
			    res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);

                con=-2.*M_PI**(xx+k)*(double)i ; //!!!!!!!!!!!! -
                cosi=cos(con)*res ;
                sini=sin(con)*res ;
	            bli=vli+ii*2 ;
	            blii=vli+(N-ii)*2 ;

			    rr=cosi ;     // i,0 reflections
			    ri=sini ;  
				*bli+=rr-*(ads+k)*ri ;
				*(bli+1)+=ri+*(ads+k)*rr ;
				*blii+=rr+*(ads+k)*ri ;
				*(blii+1)+=-ri+*(ads+k)*rr ;
            }
          }
          *cpro+=mpro ;
       }



    for(j=1;(jj=j*Mmult)<M/2; j++)
    {
	    isupin=((jj>=SL)&&(jj<EL)) ;
	    islowin=(M-jj>=SL)&& (M-jj<EL) ;
	    if((!isupin)&&(!islowin)) continue ;
    	
            y=(double)j*_B ;     
	        y_pow2=y*y;
	        y_cosgam=y*cos_gam*2. ;

		    vli=gli+(jj-SL)*N*2 ;
		    vlii=gli+(M-jj-SL)*N*2 ;
		    for(k=0;k<Na;k++)
		    {
			    curk=(scatentryes+*(type+k)*Npoints) ;
                con=-2.*M_PI**(yy+k)*(double)j ; //!!!!!!!!!! -
                cosj=cos(con) ;
                sinj=sin(con) ;
                s=y*ds*0.5 ;
		        jjj=(ULONG)floor(s) ;
		        if(jjj>=Npoints-1) continue ;
		        rem=s-jjj ;
			    centr=curk+jjj ;
			    res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);
			    rr=cosj*res ;    // 0,j reflections
			    ri=sinj*res ;  
                //****************************************
                if(isupin)
                    {
				        *vli+=rr-*(ads+k)*ri ;
				        *(vli+1)+=ri+*(ads+k)*rr ;
			        }
                    if(islowin)
                    {
				        *vlii+=rr+*(ads+k)*ri ;
				        *(vlii+1)+=-ri+*(ads+k)*rr ;
                    }
			    for(i=1;(ii=i*Nmult)<N/2;i++)
			    {
                    x=(double)i*_A ;  
                     
                    ss=(x*x+y_pow2+x*y_cosgam)*0.25 ; // for x*y>0
	                s=sqrt(ss)*ds ;
		            jjj=(ULONG)floor(s) ;
		            if(jjj>=Npoints-1) continue ;
		            rem=s-jjj ;
			        centr=curk+jjj ;
			        res=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);
    			    
                    ss-=x*y_cosgam*0.5 ;  // for x*y<0
	                s=sqrt(ss)*ds ;
		            jjj=(ULONG)floor(s) ;
		            if(jjj>=Npoints-1) continue ;
		            rem=s-jjj ;
			        centr=curk+jjj ;
			        res1=(*centr+(*(centr+1)-*centr)*rem)**(occ+k);
    	            
                    con=-2.*M_PI**(xx+k)*(double)i ; // !!!!!!!!! -
                    cosicosj=(cosi=cos(con))*cosj ;
                    sinisinj=(sini=sin(con))*sinj ;
                    cosisinj=cosi*sinj ;
                    cosjsini=cosj*sini ;

                if(isupin)                          // i,j reflections
                    {
	                    bli=vli+ii*2 ;
				        blii=vli+(N-ii)*2 ;

			            rr=(cosicosj-sinisinj)*res ;
			            ri=(cosisinj+cosjsini)*res ;  
				        *bli+=rr-*(ads+k)*ri ;
				        *(bli+1)+=ri+*(ads+k)*rr ;
			            rr=(cosicosj+sinisinj)*res1 ;
			            ri=(cosisinj-cosjsini)*res1 ;  
				        *blii+=rr-*(ads+k)*ri ;
				        *(blii+1)+=ri+*(ads+k)*rr ;
                    }
                    if(islowin)
                    {
	                    bli=vlii+ii*2 ;
				        blii=vlii+(N-ii)*2 ;

			            rr=(cosicosj+sinisinj)*res1 ;
			            ri=(-cosisinj+cosjsini)*res1 ;  
				        *bli+=rr-*(ads+k)*ri ;
				        *(bli+1)+=ri+*(ads+k)*rr ;
			            rr=(cosicosj-sinisinj)*res ;
			            ri=(-cosisinj-cosjsini)*res ;  
			            *blii+=rr-*(ads+k)*ri ;
				        *(blii+1)+=ri+*(ads+k)*rr ;
                    }
			    }
		    }
           *cpro+=mpro ;
   	
    }

return 0;
}
