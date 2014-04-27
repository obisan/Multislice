#if !defined(AFX_LATDLG_H__E6B76FE2_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_)
#define AFX_LATDLG_H__E6B76FE2_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLatDlg dialog
#include "MyStatic.h"
#include "resource.h"

class CLatDlg : public CDialog
{
// Construction
public:
	double  m_dMaxAtomOffs;
	double  m_dMaxCellOffs;
	double	m_dSizeXRe;
	double	m_dSizeYRe;
	double	m_dDimension;
	CLayersArray *m_pLayersArray;
	CLatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLatDlg)
	enum { IDD = IDD_LATDLG_DIALOG };
	CMyStatic	m_MyStatic;
	int		m_nNumLayers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLatDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateAllCtrls();
	inline void MoveChild(DWORD nChildID, CPoint * lpptOffset)
	{
		CRect rcChild;
		CWnd *pWnd=GetDlgItem(nChildID);
		ASSERT(pWnd);
		if (pWnd)
		{
			pWnd->GetWindowRect(rcChild);
			ScreenToClient(rcChild);
			rcChild.OffsetRect(*lpptOffset);
			pWnd->MoveWindow(rcChild,TRUE);
		}
	}
	void CfgUpdate();
	BOOL m_bInsertMode;
	void UpdateSelection(BOOL bParamsToImage);
	BOOL m_bInit;
	int m_nCurSelection;
	int m_nCurLayer;
	BOOL UpdateCell();
	double m_dExtraVisibleArea;
	void ReScale();

	// Generated message map functions
	//{{AFX_MSG(CLatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboAtoms();
	afx_msg void OnChangeEditAtom();
	afx_msg void OnAddAtom();
	afx_msg void OnAddLayer();
	afx_msg void OnSelchangeComboLayers();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelLayer();
	afx_msg void OnChangeL3();
	afx_msg void OnChangeL2();
	afx_msg void OnChangeL4();
	afx_msg void OnChangeLAll();
	afx_msg void OnL5Load();
	afx_msg void OnL5Save();
	//}}AFX_MSG
	afx_msg BOOL OnTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LATDLG_H__E6B76FE2_CAC0_11D4_AA18_00A0C99E2AB9__INCLUDED_)
