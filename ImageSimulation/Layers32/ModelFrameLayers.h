// ModelFrameLayers.h: interface for the CModelFrameLayers class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELFRAMELAYERS_H__5605568A_261C_4F07_963C_A2A9E74D049D__INCLUDED_)
#define AFX_MODELFRAMELAYERS_H__5605568A_261C_4F07_963C_A2A9E74D049D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ModelFrame.h"

struct ModelKvPeriod {
	double A1, fA1, gA1,
		a, fa, ga,
		A2, fA2, gA2,
		b, fb, gb,
		c, fc, gc;  // Переменные модели
	int MinContrast;	// Минимальная интенсивность
	int MaxContrast;	// Максимальная интенсивность
};
struct ModelKvPeriod2 {
	double 
		A1,fA1,gA1,
		A2,fA2,gA2,
		A3,fA3,gA3,
		alfa1,falfa1,galfa1,
		a,fa,ga,
		b,fb,gb,
		c,fc,gc,
		alfa2,falfa2,galfa2,
		beta,fbeta,gbeta;  // Переменные модели
	int MinContrast;	// Минимальная интенсивность
	int MaxContrast;	// Максимальная интенсивность
};
struct ModelSp2 {
	double 
		A1,fA1,gA1,
		A2,fA2,gA2,
		A3,fA3,gA3,
		alfa1,falfa1,galfa1,
		alfa2,falfa2,galfa2,
		alfa3,falfa3,galfa3,
		a,fa,ga,
		b,fb,gb,
		c,fc,gc,
		gamma,fgamma,ggamma,
		beta,fbeta,gbeta;  // Переменные модели
	int MinContrast;	// Минимальная интенсивность
	int MaxContrast;	// Максимальная интенсивность
};

struct ModelGalo {
  double A1, fA1, gA1,
			a, fa, ga,
			A2, fA2, gA2,
			al, fal, gal,
			be, fbe, gbe,
			n, fn, gn;  // Переменные модели
  int MinContrast;	// Минимальная интенсивность
  int MaxContrast;	// Максимальная интенсивность
};

struct ModelAniz {
  double a, b, c, d,
			al1, be1,
			al2, be2;   // Переменные модели
  int MinContrast;	// Минимальная интенсивность
  int MaxContrast;	// Максимальная интенсивность
};

struct ModelTurb {
  double A, Afi, Agam,
			B, Bfi, Bgam,
			alfa, afi, agam,
			gamma, gfi, ggam,
			W, Wfi, Wgam;
  int MinContrast;	// Минимальная интенсивность
  int MaxContrast;	// Максимальная интенсивность
};

class CModelFrameLayers : public CModelFrame  
{
public:
	
	void ReModeling(int dem, int type, common_datas cd,int en_sp);
	CModelFrameLayers();
	CModelFrameLayers(int dem,int type,struct common_datas cd,int en_sp);
	virtual ~CModelFrameLayers();
	char NameFile[500];

protected:
	double ft_sp2(double x, double y, ModelSp2 &M);
	double ft_kvazi2(double x, double y, struct ModelKvPeriod2 &M);
	double ft_turb(double x, double y, struct ModelTurb& M);
	double ft_anizotrop(double x, double y, struct ModelAniz& M);
	double ft_galo(double x, double y, struct ModelGalo& M);
	struct ModelGalo m_M_Galo;
	struct ModelAniz m_M_Anith;
	struct ModelTurb m_M_Turb;
	double ft_KvPeriod(double x, double y, struct ModelKvPeriod& M);
	struct ModelKvPeriod M;
	struct ModelKvPeriod2 m_M_Kvazi2;
	struct ModelSp2 m_M_Sp2;
	void Spectr_to_model(struct Complex *arroy,int MI,int MA);
	void Fft_forward(struct Complex *arroy);
	float gauss(double s, double sco);
	int m_enable_spectr;
	int GetModel();
};

#endif // !defined(AFX_MODELFRAMELAYERS_H__5605568A_261C_4F07_963C_A2A9E74D049D__INCLUDED_)
