/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202101250018           *
 ********************************************************************************
 * Copyright (C) 2002-2021 by Tangram Team.   All Rights Reserved.
 *
 * Web Runtime is a new, subversive and Internet Oriented Development Technology
 * for Windows Desktop Software. Its core idea is that, relying on the open-source
 * project of Chromium, developers are allowed to use their familiar software
 * development technology to realize the Main Process of their application system
 * as the Browser Process of Modern Web Browser. Because the Main Process is realized
 * by the developers themselves, the application system developed in this way is
 * completely different from modern web browser. This kind of application will
 * integrate the infrastructure of modern web browser as a part of the system model,
 * the application system contains not only the browser window inherent in the browser,
 * but also other desktop elements oriented to the application, such as WinForms, Java
 * and other mature software elements.
 *
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
 *
 * Built-in Full-Featured Modern Web Browser Subsystem
 *		Since the main process of the application system is essentially a Browser
 * Process, the application system can naturally create a new browser window. The
 * difference with standard browsers is that we allow the browser window to be a child
 * window of other windows. in sub-window model, we see a brand-new scene: a multi-tab
 * browser sub-window, we think this structure is more suitable for organizing specific
 * applications. Of course, the application system fully supports the creation of an
 * independent browser window as a part of the complete application system, and the browser
 * window serves as a natural extension of the ecological chain to ensure the richness
 * and diversity of the application system content.
 * DOM Plus:
 *		Because a large number of mature binary components that conform to industry
 * standards have formed a wealth of software asset accumulation, web runtime allows
 * developers to regard these existing accumulation as new DOM elements and then become
 * a part of web pages, this starting point eventually forms "DOM Plus". DOM plus is a
 * natural extension of standard DOM for desktop applications, similar to the extension
 * from "real number system" to "complex number system" in mathematical theory. Undoubtedly,
 * for the development of mathematics, from "real number system" to "complex number system"
 * is a huge leap, which can be said to be a decisive change in the development of modern
 * mathematics. We hope that the evolution from "DOM" to "DOM plus" will have a similar
 * effect, which is bound to be a huge change from desktop software to Internet desktop software.
 *		Different from earlier IE, based on the sandbox mechanism of Modern Web Browser,
 * we let these binary elements run in the Browser Process, completely isolated from the
 * Browser's Renderer Process. This makes full use of the security mechanisms of modern
 * browsers and also takes into account the accumulation of mature software component
 * technologies. Based on such considerations, existing development technologies such as
 * .NET, COM, Java, etc. can be fully applied to Internet software development.
 *
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *******************************************************************************/

#pragma once

namespace Browser {
	using namespace ATL;
	class CWebPage;

	class ATL_NO_VTABLE CBrowser :
		public CBrowserImpl, 
		public CWindowImpl<CBrowser, CWindow>,
		public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<IBrowser, &_uuidof(IBrowser), &LIBID_Universe, 1, 0>
	{
		friend class CXobj;
	public:
		CBrowser();
		~CBrowser() override;

		bool		m_bTabChange;
		int			m_heightfix;
		float		m_fdevice_scale_factor;
		HWND		m_hDrawWnd;
		HWND		m_hOldTab;
		CString		m_strCurKey;
		CXobj*		m_pParentXobj;
		CWebPage*	m_pVisibleWebWnd;
		LRESULT		BrowserLayout();

		STDMETHOD(get_RemoteXobj)(IXobj** pVal);
		STDMETHOD(put_RemoteXobj)(IXobj* newVal);
		STDMETHOD(AddURLs)(BSTR bstrURLs);
		STDMETHOD(OpenURL)(BSTR bstrURL, BrowserWndOpenDisposition nDisposition, BSTR bstrKey, BSTR bstrXml);
		BEGIN_MSG_MAP(CBrowser)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
			MESSAGE_HANDLER(WM_COSMOSMSG, OnCosmosMsg)
			MESSAGE_HANDLER(WM_TABCHANGE, OnChromeTabChange)
			MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
			MESSAGE_HANDLER(WM_BROWSERLAYOUT, OnBrowserLayout)
			MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
			MESSAGE_HANDLER(WM_DEVICESCALEFACTORCHANGED, OnDeviceScaleFactorChanged)
		END_MSG_MAP()

		BEGIN_COM_MAP(CBrowser)
			COM_INTERFACE_ENTRY(IBrowser)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()
	protected:
		ULONG InternalAddRef() { return 1; }
		ULONG InternalRelease() { return 1; }
	private:
		void UpdateContentRect(HWND hContentWnd, RECT& rc, int nTopFix) override;
		void ActiveChromeTab(HWND hActive, HWND hOldWnd) override;

		void OnFinalMessage(HWND hWnd) override;
		LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnCosmosMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnBrowserLayout(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnChromeTabChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnDeviceScaleFactorChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	};
};