/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202101240017
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

#pragma once
#include "chromium/WebPage.h"

using namespace Browser;
class CBKWnd;
class CWormhole;

// CCosmosTreeCtrl window
class CCosmosTreeCtrl : public CTreeCtrl
{
	// Construction
public:
	CCosmosTreeCtrl() noexcept;

	// Overrides
protected:

	// Implementation
public:
	virtual ~CCosmosTreeCtrl();
	HTREEITEM FillTreeView(CTangramXmlParse* pParse, CTangramXmlParse* pParseTag, HTREEITEM hParentItem);
	CXobj* m_pHostObj = nullptr;
	CGalaxy* m_pGalaxy = nullptr;
	map<HTREEITEM, CosmosUIItemData*> m_mapTreeItemData;
protected:
	afx_msg LRESULT OnCloudMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXobjCreatedMsg(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMTVStateImageChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTvnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTvnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTvnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};

// CCosmosListCtrl window
class CCosmosListCtrl : public CListCtrl
{
	// Construction
public:
	CCosmosListCtrl() noexcept;

	// Overrides
protected:

	// Implementation
public:
	virtual ~CCosmosListCtrl();
	//HTREEITEM FillTreeView(CTangramXmlParse* pParse, CTangramXmlParse* pParseTag, HTREEITEM hParentItem);
	CXobj* m_pHostObj = nullptr;
	CGalaxy* m_pGalaxy = nullptr;
	map<LVITEM, CosmosUIItemData*> m_mapListItemData;
protected:
	afx_msg LRESULT OnXobjCreatedMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloudMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnLvnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnDeleteallitems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnInsertitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnItemActivate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnLvnLinkClicked(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMHover(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMRDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMReturn(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};

// CCosmosTabCtrl window
class CCosmosTabCtrl : public CTabCtrl
{
	// Construction
public:
	CCosmosTabCtrl() noexcept;

	// Overrides
protected:

	// Implementation
public:
	virtual ~CCosmosTabCtrl();
	//HTREEITEM FillTreeView(CTangramXmlParse* pParse, CTangramXmlParse* pParseTag, HTREEITEM hParentItem);
	CGalaxy* m_pGalaxy = nullptr;
	CXobj* m_pHostObj = nullptr;
	map<TCITEM, CosmosUIItemData*> m_mapTabItemData;
protected:
	afx_msg LRESULT OnXobjCreatedMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCloudMsgReceived(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNMRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTcnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnTcnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
};

class CMDIChildHelperWnd :
	public CWindowImpl<CMDIChildHelperWnd, CWindow>
{
public:
	CMDIChildHelperWnd(void);
	virtual ~CMDIChildHelperWnd(void);
	HWND m_hClient;
	HWND m_hParent;
	CString m_strKey;
	CGalaxy* m_pGalaxy = nullptr;
	CString m_strDocXml = _T("");
	BEGIN_MSG_MAP(CMDIChildHelperWnd)
		MESSAGE_HANDLER(WM_COSMOSMSG, OnCosmosMg)
		MESSAGE_HANDLER(WM_MDIACTIVATE, OnMDIActivate)
		MESSAGE_HANDLER(WM_COSMOSOBSERVED, OnCosmosDocObserved)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanging)
	END_MSG_MAP()

private:
	void OnFinalMessage(HWND hWnd);
	LRESULT OnMDIActivate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnCosmosMg(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnCosmosDocObserved(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

class CMDTFrameHelperWnd :
	public CWindowImpl<CMDTFrameHelperWnd, CWindow>
{
public:
	CMDTFrameHelperWnd(void);
	virtual ~CMDTFrameHelperWnd(void);
	BEGIN_MSG_MAP(CMDTFrameHelperWnd)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
	LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);

private:
	void OnFinalMessage(HWND hWnd);
};

class CCosmosDocTemplate;
class CUniverseMDIMain :
	public CWindowImpl<CUniverseMDIMain, CWindow>
{
public:
	CUniverseMDIMain(void);
	virtual ~CUniverseMDIMain(void);

	HWND									m_hMDIClient;
	CGalaxy*								m_pGalaxy = nullptr;
	CXobj*									m_pClientXobj = nullptr;
	CGalaxyCluster*							m_pGalaxyCluster = nullptr;
	map<HWND, CMDIChildHelperWnd*>			m_mapMDIChildHelperWnd;
	BEGIN_MSG_MAP(CUniverseMDIMain)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()
	
	void OnFinalMessage(HWND hWnd);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
};

class CWinForm :
	public CWindowImpl<CWinForm, CWindow>
{
public:
	CWinForm(void);
	virtual ~CWinForm(void);
	bool									m_bReady = false;
	int										m_nState;
	BOOL									m_bMdiForm;
	BOOL									m_bMainForm = false;
	CString									m_strKey;
	CString									m_strXml;
	CString									m_strPath;
	CString									m_strBKID;
	CString									m_strChildFormPath;
	
	CBKWnd*									m_pBKWnd;
	CXobj*									m_pBindMDIXobj = nullptr;
	CXobj*									m_pWebBindMDIXobj = nullptr;
	CWebPage*								m_pOwnerHtmlWnd;
	CWormhole*								m_pWormhole;
	CMDIChildFormInfo*						m_pChildFormsInfo;

	void SendMessage();

	BEGIN_MSG_MAP(CWinForm)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_COSMOSMSG, OnCosmosMsg)
		MESSAGE_HANDLER(WM_HUBBLE_DATA, OnGetMe)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
		MESSAGE_HANDLER(WM_GETDPISCALEDSIZE, OnGetDPIScaledSize)
		MESSAGE_HANDLER(WM_WINFORMCREATED, OnFormCreated)
		MESSAGE_HANDLER(WM_HUBBLE_GETXML, OnCosmosGetXml)
		MESSAGE_HANDLER(WM_MDICLIENTCREATED, OnMdiClientCreated)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	void OnFinalMessage(HWND hWnd);

private:
	LRESULT OnDpiChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& );
	LRESULT OnGetMe(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnFormCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& );
	LRESULT OnCosmosMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnCosmosGetXml(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnGetDPIScaledSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnMdiClientCreated(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
public:
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

class ATL_NO_VTABLE CGalaxy :
	public CComObjectRootBase,	
	public CWindowImpl<CGalaxy, CWindow>,
	public IDispatchImpl<IGalaxy, &IID_IGalaxy, &LIBID_Universe, 1, 0>
{
public:
	CGalaxy();           
	virtual ~CGalaxy();  

	bool											m_bObserve = false;
	bool											m_bDockPane = false;

	BOOL											m_bTabbedMDIClient;
	BOOL											m_bDetached;
	BOOL											m_bDesignerState;
	BOOL											m_bMDIChild;
	GalaxyType										m_nGalaxyType;

	HWND											m_hPWnd;
	HWND											m_hHostWnd;
	RECT											m_OldRect;
	CString											m_strLastKey;
	CString											m_strAsynKeys;
	CString											m_strGalaxyName;
	CString											m_strCurrentKey;
	CString											m_strCurrentXml;
	CString											m_strHostWebBrowserNodeName = _T("");
	CEclipseWnd*									m_pWorkBenchFrame;
	CCosmosDocTemplate*								m_pCosmosDocTemplate;
	map<IUniverseAppProxy*, CGalaxyProxy*>			m_mapGalaxyProxy;

	IPCMsg*											m_pCurrentIPCMsg;
	CBKWnd*											m_pBKWnd;
	CWebPage*										m_pWebPageWnd;
	CXobj*											m_pHostWebBrowserNode = nullptr;
	CBrowser*										m_pHostWebBrowserWnd = nullptr;
	CGalaxyCluster*									m_pGalaxyCluster;
	CXobj*											m_pParentXobj;
	CXobj*											m_pWorkXobj;
	CXobj*											m_pContainerNode;
	CXobj*											m_pBindingXobj;
	CGalaxy*										m_pSubGalaxy;
	GalaxyInfo*										m_pGalaxyInfo;
	CWormhole*										m_pWormhole = nullptr;
	map<CString, CXobj*>							m_mapXobj;
	map<CString, CXobj*>							m_mapNeedSaveToConfigNode;
	map<CString, VARIANT>							m_mapVal;
	map<HWND, CWPFView*>							m_mapWPFView;
	map<HWND, CWPFView*>							m_mapVisibleWPFView;
	map<CString, CXobj*>							m_mapXobjScript;
	CComObject<CXobjCollection>*					m_pRootNodes;

	void Lock(){}
	void Unlock(){}
	void Destroy();
	void HostPosChanged();
	HWND GetWinForm(HWND hForm);
	void UpdateDesignerTreeInfo();
	void UpdateVisualWPFMap(HWND, BOOL);
	CTangramXmlParse* UpdateXobj();
	BOOL CreateGalaxyCluster();
	CXobj* ObserveXtmlDocument(CTangramXmlParse* pParse, CString strKey);

	STDMETHOD(get_GalaxyXML)(BSTR* pVal);
	STDMETHOD(ModifyHost)(LONGLONG hHostWnd);
	STDMETHOD(Observe)(BSTR bstrKey, BSTR bstrXml, IXobj** ppRetXobj);
	STDMETHOD(GetXml)(BSTR bstrRootName, BSTR* bstrRet);

	BEGIN_COM_MAP(CGalaxy)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IGalaxy)
	END_COM_MAP()

	BEGIN_MSG_MAP(CGalaxy)
		MESSAGE_HANDLER(WM_DPICHANGED, OnDpiChanged)
		MESSAGE_HANDLER(WM_GETDPISCALEDSIZE, OnGetDPIScaledSize)
		MESSAGE_HANDLER(WM_DPICHANGED_BEFOREPARENT, OnBeforeParentDpiChanged)
		MESSAGE_HANDLER(WM_DPICHANGED_AFTERPARENT, OnAfterParentDpiChanged)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_VSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_HUBBLE_DATA, OnGetMe)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
		MESSAGE_HANDLER(WM_COSMOSMSG, OnCosmosMsg)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PARENTNOTIFY, OnParentNotify)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_QUERYAPPPROXY, OnQueryAppProxy)
		MESSAGE_HANDLER(WM_HUBBLE_ACTIVEPAGE, OnTabChanged)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
	END_MSG_MAP()

protected:
	ULONG InternalAddRef(){ return 1; }
	ULONG InternalRelease(){ return 1; }

private:
	LRESULT OnDpiChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnGetDPIScaledSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnBeforeParentDpiChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnAfterParentDpiChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL&);
	LRESULT OnGetMe(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnHScroll(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnNcDestroy(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnTabChanged(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnCosmosMsg(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnShowWindow(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnParentNotify(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnMouseActivate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnQueryAppProxy(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnWindowPosChanging(UINT, WPARAM, LPARAM, BOOL&);

	STDMETHOD(get_Count)(long* pCount);
	STDMETHOD(get_Xobj)(VARIANT vIndex, IXobj **ppXobj);
	STDMETHOD(get__NewEnum)(IUnknown** ppVal);
	STDMETHOD(get_HWND)(LONGLONG* pVal);
	STDMETHOD(get_GalaxyCluster)(IGalaxyCluster** pVal);
	STDMETHOD(get_CurrentNavigateKey)(BSTR* pVal);
	STDMETHOD(get_VisibleXobj)(IXobj** pVal);
	STDMETHOD(get_RootXobjs)(IXobjCollection** pXobjColletion);
	STDMETHOD(get_GalaxyData)(BSTR bstrKey, VARIANT* pVal);
	STDMETHOD(put_GalaxyData)(BSTR bstrKey, VARIANT newVal);
	STDMETHOD(get_DesignerState)(VARIANT_BOOL* pVal);
	STDMETHOD(put_DesignerState)(VARIANT_BOOL newVal);
	STDMETHOD(get_GalaxyType)(GalaxyType* pVal);
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(get_HostBrowser)(IBrowser** ppChromeWebBrowser);
	STDMETHOD(get_HostWebPage)(IWebPage** ppChromeWebPage);

	STDMETHOD(Attach)(void);
	STDMETHOD(Detach)(void);

	void OnFinalMessage(HWND hWnd);
};
