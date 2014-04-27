// ModelFrameLayers.cpp: implementation of the CModelFrameLayers class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Layers32.h"
#include "ModelFrameLayers.h"
#include "SpectrumFrameLayers.h"
#include "math.h"
#include "dib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModelFrameLayers::CModelFrameLayers()
{
	
}

CModelFrameLayers::CModelFrameLayers(int dem,int type,struct common_datas cd,int en_sp)
{
	m_cd=cd;
	m_iDem=dem;
	m_type=type;
	m_enable_spectr=en_sp;
	GetModel();
}

CModelFrameLayers::~CModelFrameLayers()
{
	
}

int CModelFrameLayers::GetModel()
{
	char str[300];
	
	delete[] m_bArroy;
	m_bArroy=new BYTE[m_iDem*m_iDem];
	int Dem=m_iDem,n2;
	int i,j;int n=0, l;
	double mi = 1e20, ma = -1e20, AA = 1, step;
	static int s=127;
	static double w,wx,wy,wwx,wwy;
	BYTE *Buf;
	Buf =new BYTE[Dem*Dem];
	struct Complex *Spectr;
	Spectr=new struct Complex[1];
	switch(m_type)
	{
	case 1://модель константа
		delete[] m_bArroy;
		m_bArroy=new BYTE[m_iDem*m_iDem];
		for (i=0; i<Dem; i++)
		{
			for(j=0; j<Dem; j++)
			{
				m_bArroy[i*Dem+j] = 255;
			}
		}
		break;
	case 2://модель волна
		delete[] m_bArroy;
		m_bArroy=new BYTE[m_iDem*m_iDem];
		w=2*3.1415926/m_cd.i2; 
		wx=w*cos(m_cd.i3/180.*M_PI);
		wy=w*sin(m_cd.i3/180.*M_PI);
		for(i=0;i<m_iDem;i++)
		{	
			wwx=0;
			for(j=0;j<m_iDem;j++)		 
			{
				m_bArroy[i*m_iDem+j]= min(255,(int)((m_cd.i1*sin(wwx+wwy)+s)));
				wwx+=wx;
			}
			wwy+=wy;
		}
		break;
	case 3://модель шахматы
		delete[] m_bArroy;
		m_bArroy=new BYTE[m_iDem*m_iDem];
		n=0;
		
		static int ton[2];
		ton[0]=m_cd.i2;
		ton[1]=m_cd.i3;
		BYTE *Buffer;
		Buffer =new BYTE[m_iDem*m_iDem];
		for(i=0;i<m_iDem;i++)
		{
			memset(&Buffer[i*m_iDem],0,256);	  
			if (i%m_cd.i1==0) n=1-n;
			l = n;
			for(j=0;j<m_iDem;j+=m_cd.i1)
			{
				memset(&Buffer[(i*m_iDem)+j],ton[n],min(m_cd.i1,m_iDem-j));
				n=1-n;
			}
			n = l;
		}
		
		for(i=0; i<m_iDem; i++)
		{
			for(int j=0; j<m_iDem; j++)
			{
				m_bArroy[(i*m_iDem)+j] = (int)(Buffer[(i*m_iDem)+j]);
			}
		}
		delete Buffer;
		break;
	case 4://модель случайные квадраты
		delete[] m_bArroy;
		m_bArroy=new BYTE[m_iDem*m_iDem];
		
		int i, j, D, Pnt, CF, CR;
		int *LocalBlock;
		BYTE *Local_Buffer;
		HLOCAL hMemLocal;
		float Part;
		Dem=m_iDem;
		
		D   =  m_cd.i1;
		Pnt =  m_cd.i2;
		CF  =  m_cd.i3;
		CR  =  m_cd.i4;
		Part = 1;
		
		Local_Buffer =new BYTE[Dem*Dem];
		
		LocalCompact(Pnt*2*sizeof(int));
		hMemLocal = LocalAlloc(LHND, Pnt*2*sizeof(int));
		if (hMemLocal == NULL)
		{
			AfxMessageBox("Ќе хватает пам€ти дл€ создани€"
				"модели \"—лучайные квадраты\"",
				MB_OK | MB_ICONSTOP,NULL);
			return 0;
		}
		
		LocalBlock = (int *)LocalLock(hMemLocal);
		if (LocalBlock == NULL)
		{
			AfxMessageBox("Ќе хватает пам€ти дл€ создани€"
				"модели \"—лучайные квадраты\"",
				MB_OK | MB_ICONSTOP,NULL);
			LocalFree(hMemLocal);
			return 0;
		}
		
		///	randomize();///////////////////////////////////////////////////////////////
		for (i=0; i<Pnt; i++)
		{
			LocalBlock[i] =(int) (((long)rand()*(Dem - D))>>15);
			LocalBlock[i+Pnt] =(int) (((long)rand()*(Dem - D))>>15);
		}
		
		for (i=0; i<Dem; i++)
		{
			memset(&Local_Buffer[i*Dem], CF, Dem);
			for(j=0; j<Pnt; j++)
			{
				if ((i >= LocalBlock[Pnt+j]) &&
					(i <= LocalBlock[Pnt+j]+D))
					memset(&Local_Buffer[i*Dem+LocalBlock[j]], CR, D);
			}
		}
		for(i=0; i<Dem; i++)
		{
			for(j=0; j<Dem; j++)
			{
				m_bArroy[i*Dem+j] =(BYTE)(((1.-Part)*m_bArroy[i*Dem+j])+( Part*(Local_Buffer[i*Dem+j])));
			}
		}
		delete Local_Buffer;
		LocalUnlock(hMemLocal);
		LocalFree(hMemLocal);
		break;
	case 5://модель случайные круги
		int NumC, InF, InC, FT, Radius;
		int l, x, y;
		UINT Diameter;
		double tmp;
		D   = m_cd.i1;
		NumC = m_cd.i2;
		InF  = m_cd.i3;
		InC  = m_cd.i4;
		FT  = m_cd.i5;
		Part = 1;
		if(FT==0) FT=1; else FT=0;
		Buffer =new BYTE[Dem*Dem];
		LocalCompact(NumC*2*sizeof(int));
		hMemLocal = LocalAlloc(LHND, NumC*2*sizeof(int));
		if (hMemLocal == NULL)
		{
			AfxMessageBox("Ќе хватает пам€ти дл€ создани€"
				"модели \"—лучайные круги\"",
				MB_OK | MB_ICONSTOP);
			return 1;
		}
		
		LocalBlock = (int *)LocalLock(hMemLocal);
		if (LocalBlock == NULL)
		{
			AfxMessageBox("Ќе хватает пам€ти дл€ создани€"
				"модели \"—лучайные круги\"",
				MB_OK | MB_ICONSTOP);
			LocalFree(hMemLocal);
			return 1;
		}
		
		tmp = (double)D*D/4.;
		Diameter = (D+1)*(D+1);
		Radius = D/2;
		///	randomize();////////////////////////////////////////////////////////////////////////////////
		LocalBlock[0] = (int) (((long)rand()*(Dem-D))>>15)+Radius+1;
		LocalBlock[NumC] = (int) (((long)rand()*(Dem-D))>>15)+Radius+1;
		
		x = (int) (((long)rand()*(Dem-D))>>15)+Radius+1;
		y = (int) (((long)rand()*(Dem-D))>>15)+Radius+1;
		
		for(i=0; i<NumC; i++)
		{
			for(j=0; j<i; j++)
			{
				while((long)(x - LocalBlock[j])*(x - LocalBlock[j])+
					(long)(y - LocalBlock[NumC+j])*(y - LocalBlock[NumC+j]) < Diameter)
				{
					x =(int) (((long)rand()*(Dem-D))>>15)+Radius+1;
					y =(int) (((long)rand()*(Dem-D))>>15)+Radius+1;
				}
			}
			LocalBlock[i] = x;
			LocalBlock[NumC+i] = y;
		}
		
		for(i=0; i<Dem; i++)
		{
			memset(&Buffer[i*Dem], InF, Dem);
			for(j=0; j<NumC; j++)
			{
				y = LocalBlock[NumC+j]-i;
				x = (int)sqrt(max((float)0., (float)(tmp-y*y)));
				if ((i>=LocalBlock[NumC+j]-Radius) &&
					(i<=LocalBlock[NumC+j]+Radius))
				{
					if (FT == 1)
					{
						memset(&Buffer[(i*Dem)+LocalBlock[j]-x], InC, x*2+1);
					}
					else
					{
						for(l=-x; l<=x; l++)
						{
							Buffer[(i*Dem)+LocalBlock[j]+l] =(BYTE)(InF+(InC-InF)*min(1., max(0., (tmp-l*l-y*y)/tmp)));
						}
					}
				}
			}
		}
		for (i=0; i<Dem; i++)
		{
			for(j=0; j<Dem; j++)
			{
				m_bArroy[i*Dem+j] = (int)((1.-Part)*m_bArroy[i*Dem+j] + Part*(Buffer[i*Dem+j]));
			}
		}
		delete Buffer;
		LocalUnlock(hMemLocal);
		LocalFree(hMemLocal);
		break;
	case 6://модель шум
		delete[] m_bArroy;
		m_bArroy=new BYTE[m_iDem*m_iDem];
		Dem=m_iDem;
		int d;
		d=m_cd.i2-m_cd.i1;
		//	randomize();///////////////
		for (i=0; i<Dem; i++)
		{
			for(j=0; j<Dem; j++)
			{
				m_bArroy[i*Dem+j] = (int)(m_cd.i1+rand()%d);
			}
		}
		break;
	case 7://модель клин
		n2 = (int)Dem/2;
		double A, B, C;
		A = m_cd.d1;
		B = sin((m_cd.i1)*M_PI/180.);
		C = cos((m_cd.i1)*M_PI/180.);
		
		for (i=0; i<Dem; i++)
		{
			for (j=0; j<Dem; j++)
			{
				m_bArroy[i*Dem+j] = (unsigned char)(A*(127+(B*(i-n2)+C*(j-n2))/(Dem/n2*M_SQRT2)));
			}
		}
		break;
	case 8://модель фрактальное поле
		/////////расчет спектра/////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		
		n2 = m_iDem/2;
		double u, v;
		step = 222./m_iDem;
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = i*i;
			else u = (double)(m_iDem-i)*(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = j*j;
				else v = (double)(m_iDem-j)*(m_iDem-j);
				if (i==0 && j==0) 
				{
					Spectr[i*m_iDem+j].re =1.;
					//TempComp[i][j].re = 1.;
				} else 
				{
					Spectr[i*m_iDem+j].re =
						(float)(1./pow((u+v), (2.*m_cd.d1 + 1.)/2./2.));
				}
				Spectr[i*m_iDem+j].im = (float)0.;
			}
		}
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=50000;
			m_spectr->ScaleView();
			sprintf(str,"—пектр фрактального пол€ [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 9:// вазипериодический спектр
		M.A1=m_cd.d1;
		M.fA1=m_cd.d2;
		M.gA1=m_cd.d3;
		M.a=m_cd.d4;
		M.fa=m_cd.d5;
		M.ga=m_cd.d6;
		M.A2=m_cd.d7;
		M.fA2=m_cd.d8;
		M.gA2=m_cd.d9;
		M.b=m_cd.d10;
		M.fb=m_cd.d11;
		M.gb=m_cd.d12;
		M.c=m_cd.d13;
		M.fc=m_cd.d14;
		M.gc=m_cd.d15;
		M.MinContrast=m_cd.i1;
		M.MaxContrast=m_cd.i2;
		//////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		
		n2 = m_iDem/2;
		step = 222./m_iDem;
		
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re   =(float)ft_KvPeriod(v, u, M);
				Spectr[i*m_iDem+j].im = 0;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=25;
			m_spectr->ScaleView();
			sprintf(str,"ќбобщенный спектр [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 10://обобщенный спектр
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		n2 = m_iDem/2;
		//////////////////////
		m_M_Galo.A1=m_cd.d1;
		m_M_Galo.fA1=m_cd.d2;
		m_M_Galo.gA1=m_cd.d3;
		m_M_Galo.a=m_cd.d4;
		m_M_Galo.fa=m_cd.d5;
		m_M_Galo.ga=m_cd.d6;
		m_M_Galo.A2=m_cd.d7;
		m_M_Galo.fA2=m_cd.d8;
		m_M_Galo.gA2=m_cd.d9;
		m_M_Galo.al=m_cd.d10;
		m_M_Galo.fal=m_cd.d11;
		m_M_Galo.gal=m_cd.d12;
		m_M_Galo.be=m_cd.d13;
		m_M_Galo.fbe=m_cd.d14;
		m_M_Galo.gbe=m_cd.d15;
		m_M_Galo.n=m_cd.d16;
		m_M_Galo.fn=m_cd.d17;
		m_M_Galo.gn=m_cd.d18;
		m_M_Galo.MinContrast=m_cd.i1;
		m_M_Galo.MaxContrast=m_cd.i2;
		//////////////////////
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re = (float)sqrt(ft_galo(v, u,m_M_Galo));
				Spectr[i*m_iDem+j].im = 0;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=25;
			m_spectr->ScaleView();
			sprintf(str," вазипереодический спектр [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 11://анизотропный спектр
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		n2 = m_iDem/2;
		//////////////////////
		m_M_Anith.a=m_cd.d1;
		m_M_Anith.b=m_cd.d2;
		m_M_Anith.c=m_cd.d3;
		m_M_Anith.d=m_cd.d4;
		m_M_Anith.al1=m_cd.d5;
		m_M_Anith.al2=m_cd.d6;
		m_M_Anith.be1=m_cd.d7;
		m_M_Anith.be2=m_cd.d8;
		m_M_Anith.MinContrast=m_cd.i1;
		m_M_Anith.MaxContrast=m_cd.i2;
		//////////////////////
		for(i=0; i<m_iDem; i++)
		{
			if (i<m_iDem/2) u = i;
			else       u = m_iDem-i;
			for(j=0; j<m_iDem; j++)
			{
				if(j<m_iDem/2) v = j;
				else      v = m_iDem-j;
				Spectr[i*m_iDem+j].re  =(float)sqrt(ft_anizotrop(v, u, m_M_Anith));
				Spectr[i*m_iDem+j].im   = 0;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=2500;
			m_spectr->ScaleView();
			sprintf(str,"јнизотропный спектр [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 12://турболентный спектр
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		n2 = m_iDem/2;
		//////////////////////
		m_M_Turb.A=m_cd.d1;
		m_M_Turb.Afi=m_cd.d2;
		m_M_Turb.Agam=m_cd.d3;
		m_M_Turb.alfa=m_cd.d4;
		m_M_Turb.afi=m_cd.d5;
		m_M_Turb.agam=m_cd.d6;
		m_M_Turb.B=m_cd.d7;
		m_M_Turb.Bfi=m_cd.d8;
		m_M_Turb.Bgam=m_cd.d9;
		m_M_Turb.gamma=m_cd.d10;
		m_M_Turb.gfi=m_cd.d11;
		m_M_Turb.ggam=m_cd.d12;
		m_M_Turb.W=m_cd.d13;
		m_M_Turb.Wfi=m_cd.d14;
		m_M_Turb.Wgam=m_cd.d15;
		m_M_Turb.MinContrast=m_cd.i1;
		m_M_Turb.MaxContrast=m_cd.i2;
		//////////////////////
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = (double)j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re = (float)ft_turb(v, u,m_M_Turb);
				Spectr[i*m_iDem+j].im = (float)0.;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=1250;
			m_spectr->ScaleView();
			sprintf(str,"“урбулентный спектр [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 13://квазипереодический - 2
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		n2 = m_iDem/2;
		//////////////////////
		m_M_Kvazi2.A1=m_cd.d1;
		m_M_Kvazi2.fA1=m_cd.d2;
		m_M_Kvazi2.gA1=m_cd.d3;
		
		m_M_Kvazi2.alfa1=m_cd.d4;
		m_M_Kvazi2.falfa1=m_cd.d5;
		m_M_Kvazi2.galfa1=m_cd.d6;
		
		m_M_Kvazi2.A2=m_cd.d7;
		m_M_Kvazi2.fA2=m_cd.d8;
		m_M_Kvazi2.gA2=m_cd.d9;
		
		m_M_Kvazi2.A3=m_cd.d10;
		m_M_Kvazi2.fA3=m_cd.d11;
		m_M_Kvazi2.gA3=m_cd.d12;
		
		m_M_Kvazi2.a=m_cd.d13;
		m_M_Kvazi2.fa=m_cd.d14;
		m_M_Kvazi2.ga=m_cd.d15;
		
		m_M_Kvazi2.b=m_cd.d16;
		m_M_Kvazi2.fb=m_cd.d17;
		m_M_Kvazi2.gb=m_cd.d18;
		
		m_M_Kvazi2.c=m_cd.d19;
		m_M_Kvazi2.fc=m_cd.d20;
		m_M_Kvazi2.gc=m_cd.d21;
		
		m_M_Kvazi2.alfa2=m_cd.d22;
		m_M_Kvazi2.falfa2=m_cd.d23;
		m_M_Kvazi2.galfa2=m_cd.d24;
		
		m_M_Kvazi2.beta=m_cd.d25;
		m_M_Kvazi2.fbeta=m_cd.d26;
		m_M_Kvazi2.gbeta=m_cd.d27;
		//////////////////////
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = (double)j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re = (float)ft_kvazi2(v, u,m_M_Kvazi2);
				Spectr[i*m_iDem+j].im = (float)0.;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=1250;
			m_spectr->ScaleView();
			sprintf(str," вазипереодический спектр 2 [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 14://спектральна€ плотность 2-го типа
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		n2 = m_iDem/2;
		//////////////////////
		m_M_Sp2.A1=m_cd.d1;
		m_M_Sp2.fA1=m_cd.d2;
		m_M_Sp2.gA1=m_cd.d3;
		
		m_M_Sp2.A2=m_cd.d4;
		m_M_Sp2.fA2=m_cd.d5;
		m_M_Sp2.gA2=m_cd.d6;
		
		m_M_Sp2.A3=m_cd.d7;
		m_M_Sp2.fA3=m_cd.d8;
		m_M_Sp2.gA3=m_cd.d9;
		
		m_M_Sp2.alfa1=m_cd.d10;
		m_M_Sp2.falfa1=m_cd.d11;
		m_M_Sp2.galfa1=m_cd.d12;
		
		m_M_Sp2.alfa2=m_cd.d13;
		m_M_Sp2.falfa2=m_cd.d14;
		m_M_Sp2.galfa2=m_cd.d15;
		
		m_M_Sp2.alfa3=m_cd.d16;
		m_M_Sp2.falfa3=m_cd.d17;
		m_M_Sp2.galfa3=m_cd.d18;
		
		m_M_Sp2.a=m_cd.d19;
		m_M_Sp2.fa=m_cd.d20;
		m_M_Sp2.ga=m_cd.d21;
		
		m_M_Sp2.b=m_cd.d22;
		m_M_Sp2.fb=m_cd.d23;
		m_M_Sp2.gb=m_cd.d24;
		
		m_M_Sp2.c=m_cd.d25;
		m_M_Sp2.fc=m_cd.d26;
		m_M_Sp2.gc=m_cd.d27;
		
		m_M_Sp2.beta=m_cd.d28;
		m_M_Sp2.fbeta=m_cd.d29;
		m_M_Sp2.gbeta=m_cd.d30;
		
		m_M_Sp2.gamma=m_cd.d31;
		m_M_Sp2.fgamma=m_cd.d32;
		m_M_Sp2.ggamma=m_cd.d33;
		//////////////////////
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = (double)j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re = (float)ft_sp2(v, u,m_M_Sp2);
				Spectr[i*m_iDem+j].im = (float)0.;
			}
		}
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=1250;
			m_spectr->ScaleView();
			sprintf(str,"—пектральна€ плотность 2-го типа [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	case 15://ћодель по образцу
		//////////////////////
		CDib *bmp_file=new CDib(NameFile);
		delete[] Spectr;
		Spectr=new struct Complex[m_iDem*m_iDem];
		//////////////////////
		//NameFile
		//////////////////////
		/*
			for(int i=0;i<m_iDem*m_iDem;i++)
			{
			m_bArroy[i]=m_Dib->lpBMP[i];
			}
		*/
		for(i=0; i<m_iDem; i++)
		{
			if (i<n2) u = -i;
			else u = (double)(m_iDem-i);
			for(j=0; j<m_iDem; j++)
			{
				if(j<n2) v = (double)j;
				else v = -(double)(m_iDem-j);
				Spectr[i*m_iDem+j].re = bmp_file->lpBMP[i*m_iDem+j];
				Spectr[i*m_iDem+j].im = (float)0.;
			}
		}
		Fft_forward(Spectr);
		//////////////////////
		//отображение спектра
		if(m_enable_spectr==1)
		{
			CSpectrumFrameLayers *m_spectr;
			m_spectr = new CSpectrumFrameLayers(m_iDem,Spectr);
			m_spectr->m_fPos_Const=0.1250;
			m_spectr->ScaleView();
			sprintf(str,"—пектральна€ плотность 2-го типа [%dx%d]",m_iDem,m_iDem);
			if (!m_spectr->Create(NULL,_TEXT(str),0, rectDefault, NULL))
				return 0;
			m_spectr->MoveWindow(rand()%100,80,10000,10000);
		}
		////////расчет модели изображени€/
		Spectr_to_model(Spectr,m_cd.i1,m_cd.i2);
		//////////////////////////////////
		delete[] Spectr;
		Spectr=new struct Complex[1];
		break;
	}
	delete[] Buf;
	delete[] Spectr;
	return 0;
}

void CModelFrameLayers::ReModeling(int dem, int type, common_datas cd,int en_sp)
{
	m_cd=cd;
	m_iDem=dem;
	m_type=type;
	m_enable_spectr=en_sp;
	GetModel();
	this->RedrawWindow();
	
}

float CModelFrameLayers::gauss(double s, double sco)
{
	int i;
	double z = 0;
	for(i=0; i<10; i++) z+=(rand()/(double)RAND_MAX);
	return (float)((z-5.)*1.095444511*sco + s);
}

void CModelFrameLayers::Fft_forward(Complex *arroy)
{
	
	fftw_complex *in=new fftw_complex[m_iDem*m_iDem];
	int dxdy=m_iDem*m_iDem;
	
	for(int i=0;i<dxdy;i++)
	{
		
		in[i].re=arroy[i].re;
		in[i].im=arroy[i].im;
	}
	fftwnd_plan p;
	p = fftw2d_create_plan(m_iDem,m_iDem, FFTW_FORWARD,FFTW_ESTIMATE|FFTW_IN_PLACE);
	fftwnd_one(p,in, NULL);
	fftwnd_destroy_plan(p);
	
	for(int i=0;i<dxdy;i++)
	{
		arroy[i].re=(float)in[i].re;
		arroy[i].im=(float)in[i].im;
	}
	delete[] in;
}

void CModelFrameLayers::Spectr_to_model(Complex *Spectr, int MI, int MA)
{
	double mi = 1e20, ma = -1e20, AA = 1,d;
	int i,j;
	mi = 1e20, ma = -1e20, AA = 1;
	for(i=0; i<=m_iDem/2; i++)
	{
		if (i==0 || i==m_iDem/2)
		{
			Spectr[i*m_iDem+0].re = (float)(gauss(0., Spectr[i*m_iDem+0].re)*AA);
			Spectr[i*m_iDem+(m_iDem/2)].re =(float)(gauss(0., Spectr[i*m_iDem+(m_iDem/2)].re)*AA);
			Spectr[i*m_iDem+0].im = 0.;
			Spectr[i*m_iDem+(m_iDem/2)].im = 0.;
			
			for(j=1; j<m_iDem/2; j++)	
			{
				Spectr[i*m_iDem+j].re = (float)(gauss(0., Spectr[i*m_iDem+j].re)*AA);
				Spectr[i*m_iDem+j].im =(float)(gauss(0., Spectr[i*m_iDem+j].re)*AA);
				cconj(Spectr[i*m_iDem+j], Spectr[i*m_iDem+m_iDem-j]);
			}
		}
		else
		{
			for(j=0; j<m_iDem; j++)
			{
				Spectr[i*m_iDem+j].re = (float)(gauss(0., Spectr[i*m_iDem+j].re)*AA);
				Spectr[i*m_iDem+j].im = (float)(gauss(0., Spectr[i*m_iDem+j].re)*AA);
				cconj(Spectr[i*m_iDem+j],Spectr[((m_iDem-i)%m_iDem)*m_iDem+((m_iDem-j)%m_iDem)]);
			}
		}
		
	}
	Fft_forward(Spectr);
	for(i=0; i<m_iDem; i++) 
	{
		for(j=0; j<m_iDem; j++) 
		{
			d = Spectr[j*m_iDem+i].re;
			if (d>ma) ma=d;
			if (d<mi) mi=d;
		}
	}
	for(i=0; i<m_iDem; i++)
		for(j=0; j<m_iDem; j++)
		{
			d = Spectr[i*m_iDem+j].re;
			m_bArroy[i*m_iDem+j] = (unsigned char)(MI + (d-mi)/(ma-mi)*(MA-MI));
		}
}

double CModelFrameLayers::ft_KvPeriod(double x, double y, ModelKvPeriod &M)
{
	double A1, A2, a, b, c, a2, b2, c2, bmc, bpc, ax;
	double d, x2, x4;
	double f, cs, sn;
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A1 = M.A1/M.gA1/sqrt(cs*cs/(M.gA1*M.gA1)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A2 = M.A2/M.gA2/sqrt(cs*cs/(M.gA2*M.gA2)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fa/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	a = M.a/M.ga/sqrt(cs*cs/(M.ga*M.ga)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fb/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	b = M.b/M.gb/sqrt(cs*cs/(M.gb*M.gb)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fc/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	c = M.c/M.gc/sqrt(cs*cs/(M.gc*M.gc)+sn*sn);
	
	x2 = x*x + y*y;
	a2 = a*a;  b2 = b*b;  c2 = c*c;
	ax = a2 + x2; ax = ax*sqrt(ax);
	bmc = b2 - c2;   bpc = b2 + c2;
	x4 = x2*x2;
	d = sqrt(x4 + 2*bmc*x2 + bpc*bpc);
	d = 100*(A1/ax + A2/(d*sqrt(d + x2 + b2 - c2)));
	return (d);
	
}

double CModelFrameLayers::ft_galo(double x, double y, ModelGalo &M)
{
	double A1, A2, a, al, be, n, a2, bmc, bpc;
	double d, x2;
	double f, cs, sn;
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A1 = M.A1/M.gA1/sqrt(cs*cs/(M.gA1*M.gA1)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A2 = M.A2/M.gA2/sqrt(cs*cs/(M.gA2*M.gA2)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fa/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	a = M.a/M.ga/sqrt(cs*cs/(M.ga*M.ga)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fal/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	al = M.al/M.gal/sqrt(cs*cs/(M.gal*M.gal)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fbe/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	be = M.be/M.gbe/sqrt(cs*cs/(M.gbe*M.gbe)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fn/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	n = M.n/M.gn/sqrt(cs*cs/(M.gn*M.gn)+sn*sn);
	
	x2 = hypot(x, y);
	d = A1*exp(-x2*x2/a/a);
	a2 = al*al;
	bmc = x2 - be;  bmc = bmc*bmc;
	bpc = x2 + be;  bpc = bpc*bpc;
	d = d + A2/pow(bmc + a2, n) + A2/pow(bpc + a2, n) + 1e-10;
	return (d);
	
}

double CModelFrameLayers::ft_anizotrop(double x, double y, ModelAniz &M)
{
	double a, b, c;
	double d, al1, be1, al2, be2;
	double xmb, xpb, ymb, ypb, s;
	
	a = M.a;  b = M.b;  c = M.c;  d = M.d;
	al1 = M.al1;  al2 = M.al2;
	be1 = M.be1;  be2 = M.be2;
	
	xmb = (x - be1);  xmb = xmb*xmb;
	xpb = (x + be1);  xpb = xpb*xpb;
	ymb = (y - be2);  ymb = ymb*ymb;
	ypb = (y + be2);  ypb = ypb*ypb;
	
	s = a*exp(-(x*x/b/b + y*y/c/c)) +
		(d/(xmb + al1*al1)/(ymb + al2*al2)) +
		(d/(xpb + al1*al1)/(ypb + al2*al2));
	
	return (s);
}

double CModelFrameLayers::ft_turb(double x, double y, ModelTurb &M)
{
	double A, B, alfa, gamma, W;
	double f, cs, sn, x2;
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.Afi/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A = M.A/M.Agam/sqrt(cs*cs/(M.Agam*M.Agam)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.Bfi/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	B = M.B/M.Bgam/sqrt(cs*cs/(M.Bgam*M.Bgam)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.afi/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa = M.alfa/M.agam/sqrt(cs*cs/(M.agam*M.agam)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.gfi/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	gamma = M.gamma/M.ggam/sqrt(cs*cs/(M.ggam*M.ggam)+sn*sn);
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.Wfi/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	W = M.W/M.Wgam/sqrt(cs*cs/(M.Wgam*M.Wgam)+sn*sn);
	
	x2 = x*x + y*y;
	
	if (x2 == 0)
		return(100*A*exp(-B*pow(fabs(W),gamma))+1e-10);
	else
		return(100*A/pow(x2,alfa/2./2.)*exp(-B*pow(fabs(sqrt(x2)-W),gamma))+1e-10);
	
}

double CModelFrameLayers::ft_kvazi2(double x, double y, ModelKvPeriod2 &M)
{
	double r,u2,alfa22,beta2;
	double A1, alfa1,A2, A3,a, b, c,alfa2,beta;
	double d;
	double f, cs, sn;
	double t;
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A1 = M.A1/M.gA1/sqrt(cs*cs/(M.gA1*M.gA1)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A2 = M.A2/M.gA2/sqrt(cs*cs/(M.gA2*M.gA2)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA3/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A3 = M.A3/M.gA3/sqrt(cs*cs/(M.gA3*M.gA3)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.falfa1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa1 = M.alfa1/M.galfa1/sqrt(cs*cs/(M.galfa1*M.galfa1)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.falfa2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa2 = M.alfa2/M.galfa2/sqrt(cs*cs/(M.galfa2*M.galfa2)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fa/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	a = M.a/M.ga/sqrt(cs*cs/(M.ga*M.ga)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fb/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	b = M.b/M.gb/sqrt(cs*cs/(M.gb*M.gb)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fc/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	c = M.c/M.gc/sqrt(cs*cs/(M.gc*M.gc)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fbeta/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	beta = M.beta/M.gbeta/sqrt(cs*cs/(M.gbeta*M.gbeta)+sn*sn);
	
	
	//u = sqrt(x*x + y*y);
	u2=x*x + y*y;//u*u;
	alfa22=alfa2*alfa2;
	beta2=beta*beta;
	///
	r=u2*u2+2*(alfa22-beta2)*u2+(alfa22+beta2)*(alfa22+beta2);
	///
	t=pow(r,b);
	t=A2/(pow(r,b));
	t=5;	
	///
	d=A1/(pow((u2+alfa1*alfa1),a))+A2/(pow(r,b))+A3/(pow(r,c));
	return d;
}

double CModelFrameLayers::ft_sp2(double x, double y, ModelSp2 &M)
{
	double u,u2;
	double A1, A2, A3,alfa1,alfa2,alfa3,a, b, c,beta,gamma;
	double d;
	double f, cs, sn;
	
	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A1 = M.A1/M.gA1/sqrt(cs*cs/(M.gA1*M.gA1)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A2 = M.A2/M.gA2/sqrt(cs*cs/(M.gA2*M.gA2)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fA3/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	A3 = M.A3/M.gA3/sqrt(cs*cs/(M.gA3*M.gA3)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.falfa1/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa1 = M.alfa1/M.galfa1/sqrt(cs*cs/(M.galfa1*M.galfa1)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.falfa2/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa2 = M.alfa2/M.galfa2/sqrt(cs*cs/(M.galfa2*M.galfa2)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.falfa3/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	alfa3 = M.alfa3/M.galfa3/sqrt(cs*cs/(M.galfa3*M.galfa3)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fa/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	a = M.a/M.ga/sqrt(cs*cs/(M.ga*M.ga)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fb/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	b = M.b/M.gb/sqrt(cs*cs/(M.gb*M.gb)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fc/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	c = M.c/M.gc/sqrt(cs*cs/(M.gc*M.gc)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fbeta/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	beta = M.beta/M.gbeta/sqrt(cs*cs/(M.gbeta*M.gbeta)+sn*sn);

	if (x==0. && y==0.) f = 0.;
	else f = atan2(y, x)-M.fgamma/180.*M_PI;
	cs = cos(f);
	sn = sin(f);
	gamma = M.gamma/M.ggamma/sqrt(cs*cs/(M.ggamma*M.ggamma)+sn*sn);
	
	
	
	//u = sqrt(x*x + y*y);
	//u2=u*u;
	u2=x*x + y*y;
	///
	d=A1/(pow((u2+(alfa1*alfa1)),a))+A2*pow(u,(2*beta))/(pow((u2+(alfa2*alfa2)),b))+A3*pow(u,(2*gamma))/(pow((u2+(alfa3*alfa3)),c));
	///
	return d;
	
}

