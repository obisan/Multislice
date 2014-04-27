// Defocus.h : Declaration of the CDefocus
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "Imvpho.h"
#include "IProcessingimpl.h"

// CDefocus
class ATL_NO_VTABLE CDefocus : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IDefocus, &IID_IDefocus, &LIBID_ImvphoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CDefocus>,
	public IOleControlImpl<CDefocus>,
	public IOleObjectImpl<CDefocus>,
	public IOleInPlaceActiveObjectImpl<CDefocus>,
	public IViewObjectExImpl<CDefocus>,
	public IOleInPlaceObjectWindowlessImpl<CDefocus>,
	public IPersistStorageImpl<CDefocus>,
	public ISpecifyPropertyPagesImpl<CDefocus>,
	public IQuickActivateImpl<CDefocus>,
	public IDataObjectImpl<CDefocus>,
	public IProvideClassInfo2Impl<&CLSID_Defocus, NULL, &LIBID_ImvphoLib>,
	public CComCoClass<CDefocus, &CLSID_Defocus>,
	public CComCompositeControl<CDefocus>,
	public IProcessingImpl<CDefocus>
{
public:

	CDefocus()
	{
		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);

		m_bstrName = "Дефокусировка";
		m_type = 3;
		m_alfa = 1.f;
		m_constant = 1.f;
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_DEFOCUS)

BEGIN_COM_MAP(CDefocus)
	COM_INTERFACE_ENTRY(IDefocus)
	COM_INTERFACE_ENTRY2(IDispatch, IProcessing)
	COM_INTERFACE_ENTRY(IProcessing)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CDefocus)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CDefocus)
	COMMAND_HANDLER(IDC_RADIO1, BN_CLICKED, OnBnClickedRadio1)
	COMMAND_HANDLER(IDC_RADIO2, BN_CLICKED, OnBnClickedRadio2)
	COMMAND_HANDLER(IDC_RADIO3, BN_CLICKED, OnBnClickedRadio3)
	COMMAND_HANDLER(IDC_EDIT1, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT2, EN_KILLFOCUS, OnEnKillfocusEdit1)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	CHAIN_MSG_MAP(CComCompositeControl<CDefocus>)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

BEGIN_SINK_MAP(CDefocus)
	//Make sure the Event Handlers have __stdcall calling convention
END_SINK_MAP()

	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid)
	{
		if (dispid == DISPID_AMBIENT_BACKCOLOR)
		{
			SetBackgroundColorFromAmbient();
			FireViewChange();
		}
		return IOleControlImpl<CDefocus>::OnAmbientPropertyChange(dispid);
	}
// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IDefocus

	enum { IDD = IDD_DEFOCUS };

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public: // IProcessing
	STDMETHOD (Calculate) (long iteration);
protected:
	long	m_type;
	float	m_alfa;
	float	m_constant;
public:
	LRESULT OnBnClickedRadio1 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio2 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio3 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnKillfocusEdit1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

OBJECT_ENTRY_AUTO(__uuidof(Defocus), CDefocus)
