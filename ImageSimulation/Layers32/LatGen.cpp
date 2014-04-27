// LatGen.cpp: implementation of the CLatGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "LatGen.h"
#include "resource.h"
#include "LatDlg.h"
#include "MyImage.h"
#include "blt.h"
#include "fparams.h"
#include "latprogr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLatGenerator::CLatGenerator() {
	//LatDlg
	m_bGenerate=FALSE;
	m_dSizeXRe = 5.0;
	m_dSizeYRe = 5.0;
	m_dDimension = 10;
	m_dMaxAtomOffs = 0.0;
	m_dMaxCellOffs = 0.0;
	m_nNumLayers = 5;
	CAtomParams *AtomParams;
	
	CAtomsArray *AtomsArray;
	FILE *in = fopen("atoms.dat","r");
	if (in == NULL) {
		AtomsArray = new CAtomsArray();
		AtomsArray->m_sName = "Слой 1";
		AtomParams=new CAtomParams();
		AtomParams->SetXPos(0);
		AtomParams->SetYPos(0);
		AtomParams->SetZ(14);
		AtomsArray->Add(AtomParams);
		m_LayersArray.Add(AtomsArray);
	} else {
		int nLayers = 0;
		int nAtoms;
		double dZ;
		fscanf(in, "%lf\n", &m_dSizeXRe);
		fscanf(in, "%lf\n", &m_dSizeYRe);
		fscanf(in, "%lf\n", &m_dDimension);
		fscanf(in, "%lf\n", &m_dMaxAtomOffs);
		fscanf(in, "%lf\n", &m_dMaxCellOffs);
		fscanf(in, "%d\n", &m_nNumLayers);
		fscanf(in, "%d", &nLayers);
		for(size_t i = 0; i < nLayers; i++) {
			nAtoms = 0;
			fscanf(in, "%d", &nAtoms);
			fscanf(in, "%lf", &dZ);
			AtomsArray = new CAtomsArray();
			AtomsArray->m_sName.Format("Слой %d", i + 1);
			AtomsArray->m_dZ = dZ;
			for(size_t j = 0; j < nAtoms; j++) {
				int nZ = 14;
				double dX = 0, dY = 0;
				fscanf(in, "%d", &nZ);
				fscanf(in, "%lf", &dX);
				fscanf(in, "%lf", &dY);
				AtomParams = new CAtomParams();
				AtomParams->SetXPos(dX);
				AtomParams->SetYPos(dY);
				AtomParams->SetZ(nZ);
				AtomsArray->Add(AtomParams);
			}
			m_LayersArray.Add(AtomsArray);
		}
		fclose(in);
	}
}

CLatGenerator::~CLatGenerator() {
	FILE *out = fopen("atoms.dat","wb");
	fprintf(out,"%g\n", m_dSizeXRe);
	fprintf(out,"%g\n", m_dSizeYRe);
	fprintf(out,"%g\n", m_dDimension);
	fprintf(out,"%g\n", m_dMaxAtomOffs);
	fprintf(out,"%g\n", m_dMaxCellOffs);
	fprintf(out,"%d\n", m_nNumLayers);
	fprintf(out,"%d\n", m_LayersArray.GetSize());
	for(size_t i = 0; i < m_LayersArray.GetSize(); i++) {
		CAtomsArray *AtomsArray = m_LayersArray[i];
		fprintf(out, "%d %lf\n", AtomsArray->GetSize(), AtomsArray->m_dZ);
		for(size_t j = 0; j < AtomsArray->GetSize(); j++) {
			CAtomParams *AtomParams = AtomsArray->GetAt(j);
			fprintf(out, "%d %lf %lf\n", AtomParams->GetZ(), AtomParams->GetXPos(), AtomParams->GetYPos());
			delete AtomParams;
		}
		delete AtomsArray;
	}
	fclose(out);
}

BOOL CLatGenerator::ShowDialog() {
	CLatDlg LatDlg;
	LatDlg.m_pLayersArray = &m_LayersArray;
	LatDlg.m_dSizeXRe = m_dSizeXRe;
	LatDlg.m_dSizeYRe = m_dSizeYRe;
	LatDlg.m_dDimension = m_dDimension;
	LatDlg.m_dMaxAtomOffs = m_dMaxAtomOffs;
	LatDlg.m_dMaxCellOffs = m_dMaxCellOffs;
	LatDlg.m_nNumLayers = m_nNumLayers;

	if (LatDlg.DoModal() == IDOK) {
		m_bGenerate = TRUE;
		m_dSizeXRe = LatDlg.m_dSizeXRe;
		m_dSizeYRe = LatDlg.m_dSizeYRe;
		m_dDimension = LatDlg.m_dDimension;
		m_dMaxAtomOffs = LatDlg.m_dMaxAtomOffs;
		m_dMaxCellOffs = LatDlg.m_dMaxCellOffs;
		m_nNumLayers = LatDlg.m_nNumLayers;
		
		CAtomParams *pAtomParams;
		CAtomsArray *pAtomsArray;
		pAtomsArray = m_LayersArray[0];
		pAtomParams = pAtomsArray->GetAt(0);
		pAtomParams->m_dDimension = m_dDimension;
		for(size_t i = 0; i < m_LayersArray.GetSize(); i++) {
			pAtomsArray = m_LayersArray[i];
			for(size_t j = 0; j < pAtomsArray->GetSize(); j++) {
				pAtomParams = (CAtomParams *) pAtomsArray->GetAt(j);
				pAtomParams->Render();
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CLatGenerator::RenderLayer(SRenderParam *RenderParam) {
	int nWidth = RenderParam->m_nWidth;
	int nHeight = RenderParam->m_nHeight;
	ASSERT(nWidth == nHeight);
	ASSERT(nWidth%2 == 0);
	ASSERT(nHeight%2 == 0);
	double re;
	if (!m_bGenerate)
		return FALSE;
	if (nWidth != nHeight)
		return FALSE;
	CLatProgr LatProgressDlg;
	LatProgressDlg.SetRange(0, m_nNumLayers * m_LayersArray.GetSize());
	LatProgressDlg.SetPos(1);
	LatProgressDlg.SetPos(0);
	double dLambda = RenderParam->m_dLambda;
	double dSigma = RenderParam->m_dSigma;

	double dKmul2 = 2.0 * (2.0 * M_PI) / dLambda;
	memset(RenderParam->m_pfi_complex, 0, sizeof(fi_complex) * nWidth * nHeight);
	double dSizeXRe = m_dSizeXRe;
	double dSizeYRe = m_dSizeYRe;
	double dDimension = m_dDimension;
	
	int nCellWidth = (int)(dSizeXRe * dDimension);
	int nCellHeight = (int)(dSizeYRe * dDimension);
	if (nCellWidth < 1)
		nCellWidth = 1;
	if (nCellHeight < 1)
		nCellHeight = 1;
	CMyImage<double> *DestIm = new CMyImage<double>(nWidth, nHeight);
	srand((unsigned) time(NULL));
	double dMaxCellOffs = m_dMaxCellOffs;
	double dMaxAtomOffs = m_dMaxAtomOffs;
	
	fftw_complex *pfftw_in = new fftw_complex[nWidth * nHeight];
	fftwnd_plan fftw_plan_forward = fftw2d_create_plan(nHeight,nWidth, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
	fftwnd_plan fftw_plan_backward = fftw2d_create_plan(nHeight,nWidth, FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_IN_PLACE);
	
	for(size_t i = 0; i < nWidth * nHeight; i++) {
		pfftw_in[i].re = 1.0;
		pfftw_in[i].im = 0.0;
	}

	int dx = nWidth;
	int dy = nHeight;
				
	int dx2 = dx / 2;
	int dy2 = dy / 2;
	double u1, u2;
	double Z = 0;   // расстояние до след. слоя
	double dImgSize = (double) nWidth / dDimension; // размер всего изображения в ангстремах
	RenderParam->m_dWidthAng = (double) nWidth / dDimension;
	double Hkoef;
	double freq_re;
	double freq_im;


	for(int nNumLayers = 0; nNumLayers < m_nNumLayers; nNumLayers++) {
		CAtomParams *pAtomParams;	
		CAtomsArray *pAtomsArray;
		
		for(int nCurLayer = 0; nCurLayer < m_LayersArray.GetSize(); nCurLayer++) {
			LatProgressDlg.SetPos(nNumLayers * m_LayersArray.GetSize() + nCurLayer);
			pAtomsArray = m_LayersArray[nCurLayer];
			for(int nCurAtom = 0; nCurAtom < pAtomsArray->GetSize(); nCurAtom++) {
				pAtomParams = (CAtomParams *) pAtomsArray->GetAt(nCurAtom);
				if (fabs(pAtomParams->GetXPos()) != dSizeXRe / 2 && fabs(pAtomParams->GetYPos()) != dSizeYRe / 2)
					continue;
				if (pAtomParams->GetXPos() == dSizeXRe / 2 || pAtomParams->GetYPos() == -dSizeYRe / 2)
					continue;
				CAtomParams *pAtomParams1;
				for(int k = 0; k < pAtomsArray->GetSize(); k++) {
					pAtomParams1 = (CAtomParams *) pAtomsArray->GetAt(k);
					if (pAtomParams1->GetXPos() == pAtomParams->GetXPos() && fabs(pAtomParams1->GetYPos()) == dSizeYRe / 2) {
						if (pAtomParams1->GetYPos() == -pAtomParams->GetYPos())
							pAtomParams1->m_bitTweenX = 1;
					}
					if (pAtomParams1->GetYPos() == pAtomParams->GetYPos() && fabs(pAtomParams1->GetXPos()) == dSizeXRe / 2) {
						if (pAtomParams1->GetXPos() == -pAtomParams->GetXPos())
							pAtomParams1->m_bitTweenY = 1;
					}
					if (pAtomParams->GetXPos() == -dSizeXRe / 2 && pAtomParams->GetYPos() == dSizeYRe / 2) {
						if (pAtomParams1->GetXPos() == dSizeXRe / 2 && pAtomParams1->GetYPos() == -dSizeYRe / 2) {
							pAtomParams1->m_bitTweenX = 1;
							pAtomParams1->m_bitTweenY = 1;
						}
					}
				}
			}
						
			memset(DestIm->m_lpImage, 0, sizeof(double) * nWidth * nHeight);
			for(int y = -nCellHeight; y < (nHeight + nCellHeight); y += nCellHeight) {
				for(int x = -nCellWidth; x < (nWidth + nCellWidth); x += nCellWidth) {
					double dRandCellXOffs;
					double dRandCellYOffs;
					////////////////////////////////////////////////////////////////////////////////////////
					/// Добавляем рандомное смещение ячейки ////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////////
					for(int nCount = 0; nCount < 100; nCount++) {
						dRandCellXOffs = ((double)rand() * dMaxCellOffs) / (double) RAND_MAX;
						dRandCellYOffs = ((double)rand() * dMaxCellOffs) / (double) RAND_MAX;
						if ((dMaxCellOffs * dMaxCellOffs) >= (dRandCellXOffs * dRandCellXOffs + dRandCellYOffs * dRandCellYOffs))
							break;
					}
					////////////////////////////////////////////////////////////////////////////////////////
					/// Добавляем рандомное смещение атома /////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////////
					for(int j = 0; j < pAtomsArray->GetSize(); j++) {
						double dRandAtomXOffs;
						double dRandAtomYOffs;
						for(int nCount = 0; nCount < 100; nCount++) {
							dRandAtomXOffs = ((double) rand() * dMaxAtomOffs) / (double) RAND_MAX;
							dRandAtomYOffs = ((double) rand() * dMaxAtomOffs) / (double) RAND_MAX;
							if ((dMaxAtomOffs * dMaxAtomOffs) >= (dRandAtomXOffs * dRandAtomXOffs + dRandAtomYOffs * dRandAtomYOffs))
								break;
						}
						
						pAtomParams = (CAtomParams *) pAtomsArray->GetAt(j);
						if (pAtomParams->m_bitTweenX || pAtomParams->m_bitTweenY)
							continue;
						SumBlt(DestIm,
							x + (int)((pAtomParams->GetXPos() - ATOM_IMAGE_SIZE / 2 + dRandAtomXOffs) * dDimension + nCellWidth / 2),
							y + (int)((- pAtomParams->GetYPos() - ATOM_IMAGE_SIZE / 2 + dRandAtomYOffs) * dDimension + nCellHeight / 2),
							pAtomParams->m_Image);
					} //break;
				} //break;
			}
						
			double fi_re;
			double fi_im;
			for(size_t i = 0; i < nWidth * nHeight; i++) {
				fi_re = cos(dSigma * DestIm->m_lpImage[i] / 1000.0);
				fi_im = sin(dSigma * DestIm->m_lpImage[i] / 1000.0);
				re = pfftw_in[i].re * fi_re - pfftw_in[i].im * fi_im;
				pfftw_in[i].im = pfftw_in[i].re * fi_im + pfftw_in[i].im * fi_re;
				pfftw_in[i].re = re;
			}
			
			fftwnd_one(fftw_plan_forward, pfftw_in, NULL);
			for(size_t i = 0; i < nWidth * nHeight; i++) {
				pfftw_in[i].re /= (double) nWidth;
				pfftw_in[i].im /= (double) nWidth;
			}
			
			for(int i = 0; i < dy2; i++) {
				int idx = i * dx;
				size_t pi = 0;
				for(int j = 0; j < dx2; j++) {
					///////////////////////////////////////////////////////////////////////////////////
					// 4 to 2
					u1 = (i) / dImgSize;
					u2 = (j) / dImgSize;
					Hkoef = -(Z / dKmul2) * (u1 * u1 + u2 * u2);
					//pfftw_freq[idx+j].re=cos(Hkoef);
					//pfftw_freq[idx+j].im=sin(Hkoef);
					freq_re = cos(Hkoef);
					freq_im = sin(Hkoef);
					
					
					pi = idx + j;
					re = pfftw_in[pi].re * freq_re - pfftw_in[pi].im * freq_im;
					pfftw_in[pi].im = pfftw_in[pi].re * freq_im + pfftw_in[pi].im * freq_re;
					pfftw_in[pi].re = re;
					
					///////////////////////////////////////////////////////////////////////////////////
					// 2 to 4
					u1 = (i - dy2) / dImgSize;
					u2 = (j - dx2) / dImgSize;
					Hkoef = -(Z / dKmul2) * (u1 * u1 + u2 * u2);
					//pfftw_freq[(i+dy2)*dx+dx2+j].re=cos(Hkoef);
					//pfftw_freq[(i+dy2)*dx+dx2+j].im=sin(Hkoef);
					freq_re = cos(Hkoef);
					freq_im = sin(Hkoef);
					
					pi = (i + dy2) * dx + dx2 + j;
					re = pfftw_in[pi].re * freq_re - pfftw_in[pi].im * freq_im;
					pfftw_in[pi].im = pfftw_in[pi].re * freq_im + pfftw_in[pi].im * freq_re;
					pfftw_in[pi].re = re;
					
					///////////////////////////////////////////////////////////////////////////////////
					// 1 to 3
					u1 = (i - dy2) / dImgSize;
					u2 = (j) / dImgSize;
					Hkoef = -(Z / dKmul2) * (u1 * u1 + u2 * u2);
					//pfftw_freq[(i+dy2)*dx+j].re=cos(Hkoef);
					//pfftw_freq[(i+dy2)*dx+j].im=sin(Hkoef);
					freq_re = cos(Hkoef);
					freq_im = sin(Hkoef);
					
					pi = (i + dy2) * dx + j;
					re = pfftw_in[pi].re * freq_re - pfftw_in[pi].im * freq_im;
					pfftw_in[pi].im = pfftw_in[pi].re * freq_im + pfftw_in[pi].im * freq_re;
					pfftw_in[pi].re = re;
					
					///////////////////////////////////////////////////////////////////////////////////
					// 3 to 1
					u1 = (i) / dImgSize;
					u2 = (j - dx2) / dImgSize;
					Hkoef = -(Z / dKmul2) * (u1 * u1 + u2 * u2);
					//pfftw_freq[idx+dx2+j].re=cos(Hkoef);
					//pfftw_freq[idx+dx2+j].im=sin(Hkoef);
					freq_re = cos(Hkoef);
					freq_im = sin(Hkoef);
					
					pi = idx + dx2 + j;
					re = pfftw_in[pi].re * freq_re - pfftw_in[pi].im * freq_im;
					pfftw_in[pi].im = pfftw_in[pi].re * freq_im + pfftw_in[pi].im * freq_re;
					pfftw_in[pi].re = re;
				}
			}
			
			fftwnd_one(fftw_plan_backward, pfftw_in, NULL);
			for(size_t i = 0; i < nWidth * nHeight; i++) {
				pfftw_in[i].re /= (double) nWidth;
				pfftw_in[i].im /= (double) nWidth;
			}
			
			Z = m_LayersArray.GetAt(nCurLayer)->m_dZ;
			//break;
		}
	}
	fftwnd_destroy_plan(fftw_plan_forward);
	fftwnd_destroy_plan(fftw_plan_backward);
	
	
	for(size_t i = 0; i < nWidth * nHeight; i++) {
		RenderParam->m_pfi_complex[i].re = (float) pfftw_in[i].re;
		RenderParam->m_pfi_complex[i].im = (float) pfftw_in[i].im;
	}
	
	delete[] pfftw_in;
	delete DestIm;
	return TRUE;
}

