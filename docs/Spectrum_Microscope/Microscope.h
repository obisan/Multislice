// Microscope.h : Declaration of the CMicroscope

#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "Imvpho.h"
#include "IProcessingimpl.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CMicroscope

class ATL_NO_VTABLE CMicroscope :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMicroscope, &CLSID_Microscope>,
	public IDispatchImpl<IMicroscope, &IID_IMicroscope, &LIBID_ImvphoLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,

	public IPersistStreamInitImpl<CMicroscope>,
	public IOleControlImpl<CMicroscope>,
	public IOleObjectImpl<CMicroscope>,
	public IOleInPlaceActiveObjectImpl<CMicroscope>,
	public IViewObjectExImpl<CMicroscope>,
	public IOleInPlaceObjectWindowlessImpl<CMicroscope>,
	public IPersistStorageImpl<CMicroscope>,
	public ISpecifyPropertyPagesImpl<CMicroscope>,
	public IQuickActivateImpl<CMicroscope>,
	public IDataObjectImpl<CMicroscope>,
	public IProvideClassInfo2Impl<&CLSID_Microscope, NULL, &LIBID_ImvphoLib>,

	public CComCompositeControl<CMicroscope>,
	public IProcessingImpl<CMicroscope>

{
public:
	CMicroscope()
	{
		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);

		m_bstrName = "Микроскоп";
		m_type = 3;
		m_n = 1;
		m_size = 512;
		m_Cs = 1.3;
		m_Tmin = 0;
		m_Tmax = 0;
		m_D = 100;
		m_V = 200;
		m_deltaF = -41;
		m_defoc_type = 1;
		m_alfa = 1;
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)
DECLARE_REGISTRY_RESOURCEID(IDR_MICROSCOPE)


BEGIN_COM_MAP(CMicroscope)
	COM_INTERFACE_ENTRY(IMicroscope)
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

BEGIN_PROP_MAP(CMicroscope)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_MSG_MAP(CMicroscope)
	COMMAND_HANDLER(IDC_RADIO1, BN_CLICKED, OnBnClickedRadio1)
	COMMAND_HANDLER(IDC_RADIO2, BN_CLICKED, OnBnClickedRadio2)
	COMMAND_HANDLER(IDC_RADIO3, BN_CLICKED, OnBnClickedRadio3)
	COMMAND_HANDLER(IDC_EDIT1, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT2, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT3, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT4, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT5, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT6, EN_KILLFOCUS, OnEnKillfocusEdit1)
	COMMAND_HANDLER(IDC_EDIT7, EN_KILLFOCUS, OnEnKillfocusEdit1)
	
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDC_RADIO4, BN_CLICKED, OnBnClickedRadio4)
	COMMAND_HANDLER(IDC_RADIO5, BN_CLICKED, OnBnClickedRadio5)
	CHAIN_MSG_MAP(CComCompositeControl<CMicroscope>)
END_MSG_MAP()

BEGIN_SINK_MAP(CMicroscope)
	//Make sure the Event Handlers have __stdcall calling convention
END_SINK_MAP()

STDMETHOD(OnAmbientPropertyChange)(DISPID dispid)
	{
		if (dispid == DISPID_AMBIENT_BACKCOLOR)
		{
			SetBackgroundColorFromAmbient();
			FireViewChange();
		}
		return IOleControlImpl<CMicroscope>::OnAmbientPropertyChange(dispid);
	}
// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	enum { IDD = IDD_MICROSCOPE };

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
protected:
	long	m_type, m_size, m_n;
	BOOL	m_defoc_type;
	float	m_Cs, m_Tmin, m_Tmax, m_V, m_deltaF, m_D, m_alfa;
	
public: // IProcessing
	STDMETHOD (Calculate) (long iteration);
public: 
	LRESULT OnBnClickedRadio1 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio2 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRadio3 (WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnKillfocusEdit1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	LRESULT OnBnClickedRadio4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	LRESULT OnBnClickedRadio5(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(Microscope), CMicroscope)
