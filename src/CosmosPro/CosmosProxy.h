/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202101250018
 ********************************************************************************
 * Copyright (C) 2002-2021 by Tangram Team.   All Rights Reserved.
 * There are Three Key Features of Webruntime:
 * 1. Built-in Modern Web Browser: Independent Browser Window and Browser Window
 *    as sub windows of other windows are supported in the application process;
 * 2. DOM Plus: DOMPlus is a natural extension of the standard DOM system.
 *    It allows the application system to support a kind of generalized web pages,
 *    which are composed of standard DOM elements and binary components supported
 *    by the application system;
 * 3. JavaScript for Application: Similar to VBA in MS office, JavaScript will
 *    become a built-in programmable language in the application system, so that
 *    the application system can be expanded and developed for the Internet based
 *    on modern javscript/Web technology.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *
 *******************************************************************************/

// CosmosProxy.h : Declaration of the CCosmosProxy

#pragma once

#include "TangramBase.h"       
#include "Browser.h"
#include "resource.h"       // main symbols

using namespace Universe;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Reflection;

class CTangramWPFObjWrapper;

class CMenuHelperWnd :
	public CWindowImpl<CMenuHelperWnd, CWindow>
{
public:
	CMenuHelperWnd(void)
	{
	};

	~CMenuHelperWnd(void) {};
	HWND m_hOwner = nullptr;
	gcroot<ToolStripDropDownMenu^> m_pToolStripDropDownMenu = nullptr;
	BEGIN_MSG_MAP(CHelperWnd)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
	END_MSG_MAP()
	void OnFinalMessage(HWND hWnd);
	LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

class FormInfo
{
public:
	FormInfo();
	~FormInfo();
	map<int, gcroot<ToolStripMenuItem^>> m_mapShortcutItem;
};

class CCosmosProxy : public ICosmosCLRImpl
{
public: 
	CCosmosProxy();
	virtual~CCosmosProxy();

	bool												m_bInitApp = false;
	HWND												m_hCreatingCLRWnd = nullptr;
	gcroot<String^>										m_strExtendableTypes;
	map<CString, gcroot<Assembly^>>						m_mapAssembly;
	map<HWND, gcroot<Form^>>							m_mapDesigningForm;
	map<HWND, gcroot<MenuStrip^>>						m_mapFormMenuStrip;
	map<HWND, FormInfo*>								m_mapFormInfo;
	map<HWND, gcroot<MenuStrip^>>						m_mapFormMenuStrip2;
	map<HWND, gcroot<WebPage^>>							m_mapChromeWebPage;
	map<HWND, GalaxyInfo*>								m_mapGalaxyInfo;
	map<HWND, CTangramWPFObjWrapper*>					m_mapWpfControlWrapper;
	map<HWND, CMenuHelperWnd*>							m_mapMenuHelperWnd;
	map<HWND, CMenuHelperWnd*>							m_mapVisibleMenuHelperWnd;
	map<IBrowser*, gcroot<Browser^>>					m_mapWebBrowser;
	map<HWND, CString>									m_mapEventBindInfo;
	map<CSession*, gcroot<Wormhole^>>					m_mapSession2Wormhole;

	CMenuHelperWnd*										m_pWorkingMenuHelperWnd = nullptr;
	gcroot<Form^>										m_pCurrentPForm;
	gcroot<System::Windows::Application^>				m_pCosmosWpfApp;
	gcroot<PropertyGrid^>								m_pPropertyGrid;
	Object^ _getObject(Object^ key);
	bool _insertObject(Object^ key, Object^ val);
	bool _removeObject(Object^ key);

	template<typename T1, typename T2>
	T2^ _createObject(T1* punk)
	{
		T2^ pRetObj = nullptr;

		if (punk != NULL)
		{
			LONGLONG l = (LONGLONG)punk;
			pRetObj = (T2^)_getObject(l);

			if (pRetObj == nullptr)
			{
				pRetObj = gcnew T2(punk);
				_insertObject(l, pRetObj);
			}
		}
		return pRetObj;
	}
	void ExportCLRObjInfo(CString strPath);
	void ExportAllCLRObjInfo(CString _strPath);
	IDispatch* CreateCLRObj(CString bstrObjID);
	void CosmosAction(BSTR bstrXml, void*);
	void _GetMenuInfo(FormInfo*, ToolStripMenuItem^);
private:
	map<HWND, gcroot<Form^>>				m_mapForm;
	gcroot<Hashtable^>						m_htObjects;
	gcroot<Object^>							m_pCosmosObj;
	gcroot<Assembly^>						m_pSystemAssembly;
	gcroot<EventHandler^>					m_pOnLoad;
	gcroot<EventHandler^>					m_pOnMdiChildActivate;
	gcroot<EventHandler^>					m_pOnCtrlVisible;

	virtual void OnCLRHostExit();
	CWPFObj* CreateWPFControl(IXobj* pXobj, HWND hPWnd, UINT nID);
	HRESULT ActiveCLRMethod(BSTR bstrObjID, BSTR bstrMethod, BSTR bstrParam, BSTR bstrData);
	HRESULT ActiveCLRMethod(IDispatch* pCLRObj, BSTR bstrMethod, BSTR bstrParam, BSTR bstrData);
	HRESULT ProcessCtrlMsg(HWND hCtrl, bool bShiftKey);
	BOOL ProcessFormMsg(HWND hFormWnd, LPMSG lpMsg, int nMouseButton);
	IDispatch* CreateObject(BSTR bstrObjID, HWND hParent, IXobj* pHostNode);
	int IsWinForm(HWND hWnd);
	IDispatch* GetCLRControl(IDispatch* CtrlDisp, BSTR bstrNames);
	BSTR GetCtrlName(IDispatch* pCtrl);
	HWND GetMDIClientHandle(IDispatch* pMDICtrl);
	IDispatch* GetCtrlByName(IDispatch* CtrlDisp, BSTR bstrName, bool bFindInChild);
	HWND GetCtrlHandle(IDispatch* pCtrl);
	BSTR GetCtrlType(IDispatch* pCtrl);
	IDispatch* GetCtrlFromHandle(HWND hWnd);
	HWND IsGalaxy(IDispatch* ctrl);
	void ReleaseCosmosObj(IDispatch*);
	BSTR GetCtrlValueByName(IDispatch* CtrlDisp, BSTR bstrName, bool bFindInChild);
	void SetCtrlValueByName(IDispatch* CtrlDisp, BSTR bstrName, bool bFindInChild, BSTR strVal);
	HRESULT NavigateURL(IXobj* pXobj, CString strURL, IDispatch* dispObjforScript);
	Control^ GetCanSelect(Control^ ctrl, bool direct);
	void SelectObj(IDispatch*);

	HWND GetHwnd(HWND parent, int x, int y, int width, int height);
	void OnDestroyChromeBrowser(IBrowser*);

	void* Extend(CString strKey, CString strData, CString strFeatures);
	bool IsSupportDesigner();
	HICON GetAppIcon(int nIndex);
	void OnWinFormActivate(HWND, int nState);
	void OnWebPageCreated(HWND, CWebPageImpl*, IWebPage*, int nState);
	void HideMenuStripPopup();
	bool PreWindowPosChanging(HWND hWnd, WINDOWPOS* lpwndpos, int nType);
	void ConnectXobjToWebPage(IXobj*, bool);
	void OnCloudMsgReceived(CSession*);

	void WindowCreated(LPCTSTR strClassName, LPCTSTR strName, HWND hPWnd, HWND hWnd);
	void WindowDestroy(HWND hWnd);
	void SelectXobj(IXobj* );
	Object^ InitControl(Form^ pForm, Control^ pCtrl, bool bSave, CTangramXmlParse* pParse);
	Object^ InitXobj(IXobj* pXobj, Control^ pCtrl, bool bSave, CTangramXmlParse* pParse);
	void SetObjectProperty(IDispatch* pObj, BSTR bstrPropertyName, BSTR bstrPropertyValue);
	IDispatch* CreateWinForm(HWND hParent, BSTR strXML);

	void CtrlInit(int nType, Control^ treeview, IGalaxyCluster* pGalaxyCluster);
	System::Void LoadNode(TreeView^ pTreeView, TreeNode^ pXobj, IGalaxyCluster* pGalaxyCluster, CTangramXmlParse* pParse);
	static void OnLoad(Object ^sender, EventArgs ^e);
	static void OnMdiChildActivate(Object ^sender, EventArgs ^e);
	static void OnApplicationExit(Object ^sender, EventArgs ^e);
	static void OnVisibleChanged(Object ^sender, EventArgs ^e);
	static void OnAfterSelect(Object ^sender, TreeViewEventArgs ^e);
	static void OnItemSelectionChanged(Object ^sender, ListViewItemSelectionChangedEventArgs ^e);
	static void OnControlAdded(Object ^sender, ControlEventArgs ^e);
	static void OnControlRemoved(Object ^sender, ControlEventArgs ^e);
	static void OnHandleDestroyed(Object ^sender, EventArgs ^e);
	static void OnClick(Object ^sender, EventArgs ^e);
	static void OnNodeMouseDoubleClick(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e);
	static void OnTextChanged(System::Object^ sender, System::EventArgs^ e);
	static void OnMenuActivate(System::Object^ sender, System::EventArgs^ e);
	static void OnMenuDeactivate(System::Object^ sender, System::EventArgs^ e);
	static void OnLostFocus(System::Object^ sender, System::EventArgs^ e);
	static void OnMenuComplete(System::Object^ sender, System::EventArgs^ e);
};

class CTangramWPFObjWrapper : public CWPFObj
{
	friend CCosmosProxy;
	static HWND FindNextTabStop(HWND wnd, bool forward)
	{
		HWND nextstop = NULL;
		HWND nextwnd = wnd;
		do
		{
			// get the next/previous window in the z-order
			nextwnd = ::GetWindow(nextwnd, forward ? GW_HWNDNEXT : GW_HWNDPREV);

			// if we are at the end of the z-order, start from the top/bottom
			if (nextwnd == NULL)
				nextwnd = ::GetWindow(wnd, forward ? GW_HWNDFIRST : GW_HWNDLAST);

			// if we returned to the same control then we iterated the entire z-order
			if (nextwnd == wnd)
				break;

			// get the window style and check the WS_TABSTOP style
			DWORD style = (DWORD)::GetWindowLongPtr(nextwnd, GWL_STYLE);
			if ((style & WS_TABSTOP) == WS_TABSTOP)
				nextstop = nextwnd;
		} while (nextstop == NULL);

		return nextstop;
	}

	static IntPtr ChildHwndSourceHook(
		IntPtr hwnd,
		int msg,
		IntPtr wParam,
		IntPtr lParam,
		bool% handled)
	{
		if (msg == WM_GETDLGCODE)
		{
			handled = true;
			return System::IntPtr(DLGC_WANTCHARS | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTALLKEYS);
		}
		else if (msg == WM_CHAR)
		{
			if (wParam.ToInt32() == VK_TAB)
			{
				handled = true;
				HWND nextTabStop = FindNextTabStop((HWND)hwnd.ToPointer(),
					(GetKeyState(VK_SHIFT) & 0x8000) != 0x8000);
				if (nextTabStop)
					::SetFocus(nextTabStop);
			}
		}

		return System::IntPtr::Zero;
	}

public:
	CTangramWPFObjWrapper(void)
	{
		m_hwndWPF = nullptr;
	}

	~CTangramWPFObjWrapper(void);

	template <typename T>
	T^ Control()
	{
		System::Windows::UIElement^ obj = m_pUIElement;
		return dynamic_cast<T^>(obj);
	}

	gcroot<System::Windows::FrameworkElement^> m_pUIElement;
	gcroot<System::Windows::Interop::HwndSource^> m_pSource;
	CWPFObj* CreateControl(Type^ type, HWND parent, DWORD style, int x, int y, int width, int height);

	BOOL IsVisible();
	void InvalidateVisual();
	void Focusable(BOOL bFocus);
	void ShowVisual(BOOL bShow);
	//static void OnLoaded(Object ^sender, System::Windows::RoutedEventArgs ^e);
	//static void OnUnloaded(Object ^sender, System::Windows::RoutedEventArgs ^e);
	//static void OnMouseEnter(Object ^sender, System::Windows::Input::MouseEventArgs ^e);
	//static void OnSizeChanged(Object ^sender, System::Windows::SizeChangedEventArgs ^e);
	//static void OnIsVisibleChanged(Object ^sender, System::Windows::DependencyPropertyChangedEventArgs e);
};

extern _ATL_FUNC_INFO Initialize;
extern _ATL_FUNC_INFO Destroy;
extern _ATL_FUNC_INFO TabChange;
extern _ATL_FUNC_INFO IPCMsg2;

class CGalaxyClusterEvent : public IDispEventSimpleImpl</*nID =*/ 1, CGalaxyClusterEvent, &__uuidof(_ICosmosObjEvents)>
{
public:
	CGalaxyClusterEvent();
	virtual ~CGalaxyClusterEvent();

	gcroot<Object^> m_pGalaxyCluster;

	void __stdcall  OnDestroy();
	void __stdcall  OnTabChange(IXobj* sender, int nActivePage,int nOldPage);
	void __stdcall  OnInitialize(IDispatch* pHtmlWnd, BSTR bstrUrl);
	void __stdcall  OnIPCMsg(IGalaxy* sender, BSTR bstrType, BSTR bstrContent, BSTR bstrFeature);
	BEGIN_SINK_MAP(CGalaxyClusterEvent)
		SINK_ENTRY_INFO(/*nID =*/ 1, __uuidof(_ICosmosObjEvents), /*dispid =*/ 0x00000001, OnInitialize, &Initialize)
		SINK_ENTRY_INFO(/*nID =*/ 1, __uuidof(_ICosmosObjEvents), /*dispid =*/ 0x00000006, OnDestroy, &Destroy)
		SINK_ENTRY_INFO(/*nID =*/ 1, __uuidof(_ICosmosObjEvents), /*dispid =*/ 0x00000009, OnTabChange, &TabChange)
		SINK_ENTRY_INFO(/*nID =*/ 1, __uuidof(_ICosmosObjEvents), /*dispid =*/ 0x0000000d, OnIPCMsg, &IPCMsg2)
	END_SINK_MAP()
};

