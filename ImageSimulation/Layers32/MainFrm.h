// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__FD44D368_82E5_4AD3_A782_38DDA57AD35D__INCLUDED_)
#define AFX_MAINFRM_H__FD44D368_82E5_4AD3_A782_38DDA57AD35D__INCLUDED_

#include "DialogAtoms.h"	// Added by ClassView
#include "ModelFrameLayers.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DialogChess.h"
#include "DialogConstanta.h"
#include "DialogWave.h"
#include "DialogRect.h"
#include "DialogCircle.h"
#include "DialogNoise.h"
#include "DialogKlin.h"
#include "DialogFractal.h"
#include "DialogKvazi.h"
#include "DialogKvazi2.h"
#include "DialogObob.h"
#include "DialogAniz.h"
#include "DialogTurbo.h"
#include "DialogSP2type.h"
#include "DialogPath.h"
#include "DialogEx.h"
#define KOS 0001
#define PAM 0002




class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ProcessingPAM();
	int m_type_fasa;
	int m_type_ampl;
	CModelFrameLayers *m_fasa;
	CModelFrameLayers *m_ampl;
	int m_iDem_fasa;
	int m_iDem_ampl;
	struct common_datas m_cd_fasa;
	struct common_datas m_cd_ampl;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CToolBar    m_wndToolBar;
	CStatusBar  m_wndStatusBar;

protected:  // control bar embedded members


// Generated message map functions
protected:
	CDialogAtoms DialogAtoms;
	double m_napr;
	int m_type_of_system;
	CDialogWave DialogWave;
	CDialogTurbo DialogTurbo;	
	CDialogSP2Type DialogSP2Type;
	CDialogRect DialogRect;
	CDialogObob DialogObob;
	CDialogNoise DialogNoise;
	CDialogKvazi2 DialogKvazi2;
	CDialogKvazi DialogKvazi;
	CDialogKlin DialogKlin;
	CDialogFractal DialogFractal;
	CDialogCircle DialogCircle;
	CDialogChess DialogChess;
	CDialogAniz DialogAniz;
	CDialogPath DialogPath;
	CDialogEx DialogExample;
	void Fft_back(struct Complex *arroy);
	void Fft_forward(struct Complex *arroy);
	void Fft_back(struct Complex *arroy,int dem);
	void Fft_forward(struct Complex *arroy,int dem);
	double m_apertura;
	double m_distance_layer;
	double m_length_wave;
	double DifPhaseModul;
	int PntLayer;
	void Processing();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnProcessing();
	afx_msg void OnConstant();
	afx_msg void OnWave();
	afx_msg void OnChess();
	afx_msg void OnRect();
	afx_msg void OnCircle();
	afx_msg void OnNoise();
	afx_msg void OnKlin();
	afx_msg void OnFractal();
	afx_msg void OnKvazi();
	afx_msg void OnObob();
	afx_msg void OnAnizotrop();
	afx_msg void OnTurbo();
	afx_msg void OnExample();
	afx_msg void OnKvazi2();
	afx_msg void OnSp2type();
	afx_msg void OnStart();
	afx_msg void OnAtoms();
	afx_msg void OnShowWin();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHelpIndex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__FD44D368_82E5_4AD3_A782_38DDA57AD35D__INCLUDED_)
