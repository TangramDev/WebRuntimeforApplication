﻿/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202101200014           *
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
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *******************************************************************************/

#include "../stdafx.h"
#include "../UniverseApp.h"
#include "../Xobj.h"
#include "../XobjHelper.h"
#include "../Galaxy.h"
#include "WebPage.h"
#include "Browser.h"

namespace Browser {
	CBrowser::CBrowser() {
		m_hDrawWnd = 0;
		m_heightfix = 0;
		m_hOldTab = NULL;
		m_bTabChange = false;
		m_pRemoteXobj = nullptr;
		m_pBrowser = nullptr;
		m_pParentXobj = nullptr;
		m_fdevice_scale_factor = 1.0f;
		m_strCurKey = _T("");
		m_pVisibleWebWnd = nullptr;
		m_pOmniboxViewViews = nullptr;
		if (g_pCosmos->m_pCreatingOmniboxViewViews) {
			m_pOmniboxViewViews = g_pCosmos->m_pCreatingOmniboxViewViews;
			g_pCosmos->m_pCreatingOmniboxViewViews = nullptr;
		}
	}

	CBrowser::~CBrowser() {}

	void CBrowser::ActiveChromeTab(HWND hActive, HWND hOldWnd)
	{
		m_bTabChange = true;
		if (g_pCosmos->m_bChromeNeedClosed == false && m_pBrowser)
		{
			if (::IsWindow(hOldWnd))
			{
				m_hOldTab = hOldWnd;
			}
			if (m_pParentXobj)
			{
				HWND h = m_pParentXobj->m_pXobjShareData->m_pGalaxy->m_hWnd;
				auto it = g_pCosmos->m_mapHtmlWnd.find(hActive);
				if (it != g_pCosmos->m_mapHtmlWnd.end())
				{
					CWebPage* pPage = (CWebPage*)it->second;
					if (pPage->m_pGalaxy&&pPage->m_pCosmosFrameWndInfo)
					{
						CString strKey = pPage->m_pGalaxy->m_strCurrentKey;
						for (auto it : pPage->m_pCosmosFrameWndInfo->m_mapAuxiliaryGalaxys)
						{
							if (it.second != pPage->m_pGalaxy)
							{
								IXobj* pObj = nullptr;
								pPage->Observe(CComBSTR(strKey), CComBSTR(""), &pObj);
							}
						}
					}
				}
			}
		}
	}

	LRESULT CBrowser::OnChromeTabChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		g_pCosmos->m_pActiveHtmlWnd = m_pVisibleWebWnd;
		if (m_pVisibleWebWnd && g_pCosmos->m_pActiveHtmlWnd->m_pChromeRenderFrameHost)
		{
			g_pCosmos->m_pGalaxy = nullptr;
			g_pCosmos->m_bWinFormActived = false;
		}
		m_pBrowser->LayoutBrowser();
		return lRes;
	}

	void CBrowser::UpdateContentRect(HWND hWnd, RECT& rc, int nTopFix) {
		if (hWnd == 0 || ::IsWindowVisible(m_hWnd) == false || g_pCosmos->m_bChromeNeedClosed == TRUE || g_pCosmos->m_bOMNIBOXPOPUPVISIBLE) {
			return;
		}
		if (m_hOldTab)
		{
			RECT rc;
			::GetWindowRect(m_hOldTab, &rc);
			ScreenToClient(&rc);
			::SetWindowPos(m_hOldTab, HWND_BOTTOM, rc.left, rc.top, 1, 1, /*SWP_NOREDRAW |*/ SWP_NOACTIVATE);
			m_hOldTab = NULL;
		}

		BrowserLayout();
		if (m_bTabChange == true || ::IsWindowVisible(hWnd) == FALSE ||
			(m_pVisibleWebWnd && m_pVisibleWebWnd->m_hWnd != hWnd))
		{
			auto it = g_pCosmos->m_mapHtmlWnd.find(hWnd);
			if (it != g_pCosmos->m_mapHtmlWnd.end())
			{
				m_pVisibleWebWnd = (CWebPage*)it->second;
				if (m_pVisibleWebWnd->m_pChromeRenderFrameHost)
					m_pVisibleWebWnd->m_pChromeRenderFrameHost->ShowWebPage(true);
				if (m_bTabChange)
					::PostMessage(m_hWnd, WM_COSMOSMSG, 20200205, 1);
				return;
			}
		}

		if (m_pVisibleWebWnd)
		{
			if (m_pVisibleWebWnd->m_hExtendWnd == nullptr)
			{
				m_pVisibleWebWnd->m_hExtendWnd = ::CreateWindowEx(NULL, _T("Chrome Extended Window Class"), L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, m_hWnd, NULL, theApp.m_hInstance, NULL);
				m_pVisibleWebWnd->m_hChildWnd = ::CreateWindowEx(NULL, _T("Chrome Extended Window Class"), L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, m_pVisibleWebWnd->m_hExtendWnd, (HMENU)2, theApp.m_hInstance, NULL);

				::SetWindowLongPtr(m_pVisibleWebWnd->m_hExtendWnd, GWLP_USERDATA, (LONG_PTR)m_pVisibleWebWnd->m_hChildWnd);
				::SetWindowLongPtr(m_pVisibleWebWnd->m_hChildWnd, GWLP_USERDATA, (LONG_PTR)m_pVisibleWebWnd);
			}
			HWND hExtendWnd = m_pVisibleWebWnd->m_hExtendWnd;
			if (::IsChild(hWnd, hExtendWnd))
				::SetParent(hExtendWnd, m_hWnd);

			::SetWindowPos(hExtendWnd, m_hDrawWnd,
				rc.left,
				nTopFix * m_fdevice_scale_factor,
				rc.right * m_fdevice_scale_factor,
				(rc.bottom - rc.top) * m_fdevice_scale_factor,
				SWP_SHOWWINDOW /*| SWP_NOREDRAW*/ | SWP_NOACTIVATE);
			HWND hWebHostWnd = m_pVisibleWebWnd->m_hWebHostWnd;
			if (hWebHostWnd == NULL)
				hWebHostWnd = m_pVisibleWebWnd->m_hChildWnd;
			if (::IsWindowVisible(hWebHostWnd) == false) {
				m_pVisibleWebWnd->m_bWebContentVisible = false;
				rc.right = rc.left + 1;
				rc.bottom = rc.top + 1;
			}
			else {
				RECT rc2;
				::GetWindowRect(hWebHostWnd, &rc2);
				if (::ScreenToClient(hExtendWnd, (LPPOINT)&rc2))
				{
					::ScreenToClient(hExtendWnd, ((LPPOINT)&rc2) + 1);
					RECT rect;
					::GetClientRect(hExtendWnd, &rect);

					rc.left += rc2.left / m_fdevice_scale_factor;
					rc.right -= (rect.right - rc2.right) / m_fdevice_scale_factor;
					rc.top += (rc2.top - rect.top) / m_fdevice_scale_factor;
					rc.bottom -= (rect.bottom - rc2.bottom) / m_fdevice_scale_factor;
					m_pVisibleWebWnd->m_bWebContentVisible = true;
					if (rc.right <= rc.left || rc.bottom <= rc.top) {
						m_pVisibleWebWnd->m_bWebContentVisible = false;
						rc.right = rc.left + 1;
						rc.bottom = rc.top + 1;
					}
				}
			}
		}
	};

	LRESULT CBrowser::BrowserLayout() {
		if (m_pVisibleWebWnd == nullptr || m_bTabChange ||
			!::IsWindowVisible(m_hWnd) ||
			g_pCosmos->m_bChromeNeedClosed == TRUE)
			return 0;
		if (!::IsWindow(m_hWnd))
			return 0;
		RECT rcBrowser;
		GetClientRect(&rcBrowser);
		if (m_pVisibleWebWnd->m_pGalaxy == nullptr) {
			if (rcBrowser.right * rcBrowser.left)
				::SetWindowPos(m_pVisibleWebWnd->m_hExtendWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_SHOWWINDOW);
			::SetWindowRgn(m_hDrawWnd, NULL, true);
			return 0;
		}

		if (!::IsWindowVisible(m_pVisibleWebWnd->m_hWnd))
		{
			if (m_pVisibleWebWnd->m_hExtendWnd)
				::SetParent(m_pVisibleWebWnd->m_hExtendWnd, m_pVisibleWebWnd->m_hWnd);
			auto it = g_pCosmos->m_mapHtmlWnd.find(m_pBrowser->GetActiveWebContentWnd());
			if (it != g_pCosmos->m_mapHtmlWnd.end())
			{
				m_pVisibleWebWnd = (CWebPage*)it->second;
				if (m_pVisibleWebWnd->m_hExtendWnd)
					::SetParent(m_pVisibleWebWnd->m_hExtendWnd, m_hWnd);
			}
		}

		if (::GetParent(m_pVisibleWebWnd->m_hExtendWnd) != m_hWnd) {
			::SetParent(m_pVisibleWebWnd->m_hExtendWnd, m_hWnd);
		}

		RECT rcWebPage;
		RECT rcExtendWnd;
		::GetWindowRect(m_pVisibleWebWnd->m_hExtendWnd, &rcExtendWnd);
		::ScreenToClient(m_hWnd, (LPPOINT)&rcExtendWnd);
		::ScreenToClient(m_hWnd, ((LPPOINT)&rcExtendWnd) + 1);
		HWND _hWebPage = m_pVisibleWebWnd->m_hWnd;
		if (m_pVisibleWebWnd->m_pDevToolWnd)
		{
			if (::GetParent(m_pVisibleWebWnd->m_hWnd) == ::GetParent(m_pVisibleWebWnd->m_pDevToolWnd->m_hWnd))
			{
				_hWebPage = m_pVisibleWebWnd->m_pDevToolWnd->m_hWnd;
				::ShowWindow(_hWebPage, SW_SHOW);
			}
		}
		::GetWindowRect(_hWebPage, &rcWebPage);
		::ScreenToClient(m_hWnd, (LPPOINT)&rcWebPage);
		::ScreenToClient(m_hWnd, ((LPPOINT)&rcWebPage) + 1);
		//浏览器窗口区域：
		HRGN hGPUWndRgn = ::CreateRectRgn(rcBrowser.left, rcBrowser.top, rcBrowser.right, rcBrowser.bottom);
		//浏览器页面扩展窗口区域：
		HRGN hWebExtendWndRgn = ::CreateRectRgn(rcExtendWnd.left, rcExtendWnd.top, rcExtendWnd.right, rcExtendWnd.bottom);
		//浏览器页面窗口区域：
		HRGN hWebPage = ::CreateRectRgn(rcWebPage.left, rcWebPage.top, rcWebPage.right, rcWebPage.bottom);
		::CombineRgn(hWebExtendWndRgn, hWebExtendWndRgn, hWebPage, RGN_DIFF);
		::CombineRgn(hGPUWndRgn, hGPUWndRgn, hWebExtendWndRgn, RGN_DIFF);
		::DeleteObject(hWebPage);
		::DeleteObject(hWebExtendWndRgn);
		::SetWindowRgn(m_hDrawWnd, hGPUWndRgn, false);

		return 0;
	}

	LRESULT CBrowser::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		if (LOWORD(wParam) != WA_INACTIVE) {
			if (m_pBrowser) {
				g_pCosmos->m_pActiveBrowser = m_pBrowser;
				g_pCosmos->m_pActiveBrowser->m_pProxy = this;
				//m_pBrowser->LayoutBrowser();
				//BrowserLayout();
				//::InvalidateRect(m_hWnd, nullptr, true);
			}
		}
		else
		{
			if (g_pCosmos->m_pCLRProxy)
				g_pCosmos->m_pCLRProxy->HideMenuStripPopup();
		}
		::PostMessage(m_hWnd, WM_BROWSERLAYOUT, 0, 4);
		return lRes;
	}

	LRESULT CBrowser::OnDeviceScaleFactorChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		m_fdevice_scale_factor = (float)lParam / 100;
		return lRes;
	}

	LRESULT CBrowser::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		if (g_pCosmos->m_pCLRProxy)
			g_pCosmos->m_pCLRProxy->HideMenuStripPopup();
		if (m_pVisibleWebWnd)
		{
			g_pCosmos->m_pActiveHtmlWnd = m_pVisibleWebWnd;
			if (g_pCosmos->m_pActiveHtmlWnd && g_pCosmos->m_pActiveHtmlWnd->m_pChromeRenderFrameHost)
			{
				g_pCosmos->m_pGalaxy = nullptr;
				g_pCosmos->m_bWinFormActived = false;
			}
		}
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CBrowser::OnCosmosMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		HWND hWnd = (HWND)lParam;
		switch (wParam) {
		case 0: {
			g_pCosmos->m_pHtmlWndCreated = new CComObject<CWebPage>;
			g_pCosmos->m_pHtmlWndCreated->SubclassWindow(hWnd);
			if (g_pCosmos->m_pCLRProxy)
				g_pCosmos->m_pCLRProxy->OnWebPageCreated(hWnd, (CWebPageImpl*)g_pCosmos->m_pHtmlWndCreated, (IWebPage*)g_pCosmos->m_pHtmlWndCreated, 0);
			HWND hPWnd = ::GetParent(m_hWnd);
			if (g_pCosmos->m_bCreatingDevTool == false)
			{
				g_pCosmos->m_pHtmlWndCreated->m_bDevToolWnd = false;
				g_pCosmos->m_mapHtmlWnd[hWnd] = g_pCosmos->m_pHtmlWndCreated;
				if (m_pBrowser&&hWnd == m_pBrowser->GetActiveWebContentWnd())
					m_pVisibleWebWnd = g_pCosmos->m_pHtmlWndCreated;
#ifdef WIN32	
				if (::IsWindow(hPWnd))
				{
					DWORD dwID = 0;
					::GetWindowThreadProcessId(hPWnd, &dwID);
					if (dwID != ::GetCurrentProcessId())
					{
						auto it = g_pCosmos->m_mapRemoteTangramApp.find(dwID);
						if (it != g_pCosmos->m_mapRemoteTangramApp.end())
						{
							g_pCosmos->m_pHtmlWndCreated->m_pRemoteCosmos = it->second;
						}
					}
				}
#endif
			}
			else
			{
				g_pCosmos->m_bCreatingDevTool = false;
				g_pCosmos->m_pHtmlWndCreated->m_bDevToolWnd = true;
				if (m_pVisibleWebWnd) {
					m_pVisibleWebWnd->m_pDevToolWnd = g_pCosmos->m_pHtmlWndCreated;
					g_pCosmos->m_pHtmlWndCreated->m_pWebWnd = m_pVisibleWebWnd;
				}
			}
			if (hPWnd)
			{
				g_pCosmos->m_pActiveHtmlWnd = m_pVisibleWebWnd;
				g_pCosmos->m_pGalaxy = nullptr;
				g_pCosmos->m_bWinFormActived = false;
				::PostMessage(hWnd, WM_COSMOSMSG, 20190331, 1);
			}
		} break;
		case 1:
		{
			if (lParam == 20200115)
			{
				::InvalidateRect(m_hWnd, nullptr, true);
				m_pBrowser->LayoutBrowser();
				::PostMessage(m_hWnd, WM_BROWSERLAYOUT, 0, 2);
				::InvalidateRect(m_hWnd, nullptr, true);
			}
		}
		break;
		case 20201101:
		{
			m_hDrawWnd = (HWND)lParam;
			::PostMessage(m_hWnd, WM_BROWSERLAYOUT, 0, 4);
			return 0;
		}
		break;
		case 20190527:
		{
			return (LRESULT)((IBrowser*)this);
		}
		break;
		case 20200205:
		{
			if (lParam == 1)
			{
				m_bTabChange = false;
			}
		}
		return 1;
		break;
		case 20200214:
		{
			auto t = create_task([this]()
				{
					m_heightfix = 0;
					SleepEx(500, true);
					try
					{
						if (m_pVisibleWebWnd)
						{
							::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW);
							BrowserLayout();
							::SetFocus(m_pVisibleWebWnd->m_hWnd);
							::SetForegroundWindow(m_pVisibleWebWnd->m_hWnd);
						}
					}
					catch (...)
					{
						ATLASSERT(false);
						return 0;
					}
					return 1;
				});
		}
		return 1;
		break;
		case 20200128:
		{
			if (m_pVisibleWebWnd)
			{
				CGalaxy* pGalaxy = m_pVisibleWebWnd->m_pGalaxy;
				if (pGalaxy)
				{
					CXobj* pXobj = pGalaxy->m_pWorkXobj;
					CXobjHelper* pWnd = (CXobjHelper*)(pXobj->m_pHostWnd);
					return (LRESULT)(pWnd->m_hWnd);
				}
			}
		}
		break;
		case 1992:
		{
			return (LRESULT)m_pBrowser->GetBrowser();
		}
		break;
		case 20190629:
		{
			switch (lParam)
			{
			case 1:
				return m_pVisibleWebWnd && ::IsWindow(m_pVisibleWebWnd->m_hExtendWnd);
				break;
			}
		}
		break;
		}
		return lRes;
	}

	LRESULT CBrowser::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		if (g_pCosmos->m_pCLRProxy)
		{
			IBrowser* pIBrowser = nullptr;
			QueryInterface(__uuidof(IBrowser), (void**)&pIBrowser);
			g_pCosmos->m_pCLRProxy->OnDestroyChromeBrowser(pIBrowser);
		}

		m_pVisibleWebWnd = nullptr;
		auto it = g_pCosmos->m_mapBrowserWnd.find(m_hWnd);
		if (it != g_pCosmos->m_mapBrowserWnd.end()) {
			g_pCosmos->m_mapBrowserWnd.erase(it);
		}

		if ((g_pCosmos->m_hMainWnd == g_pCosmos->m_hHostWnd && g_pCosmos->m_mapBrowserWnd.size() == 1) ||
			g_pCosmos->m_hHostBrowserWnd == m_hWnd)
		{
			if (g_pCosmos->m_hHostBrowserWnd == m_hWnd)
			{
				g_pCosmos->m_bChromeNeedClosed = true;
				for (auto it : g_pCosmos->m_mapBrowserWnd)
				{
					if (((CBrowser*)it.second)->m_hWnd != m_hWnd)
						((CBrowser*)it.second)->PostMessageW(WM_CLOSE, 0, 0);
				}
				g_pCosmos->m_mapBrowserWnd.erase(g_pCosmos->m_mapBrowserWnd.begin(), g_pCosmos->m_mapBrowserWnd.end());
			}
		}

		if ((g_pCosmos->m_hMainWnd == NULL && g_pCosmos->m_mapBrowserWnd.size() == 0) ||
			g_pCosmos->m_hHostBrowserWnd == m_hWnd) {
			if (g_pCosmos->m_hHostBrowserWnd == m_hWnd)
				g_pCosmos->m_hHostBrowserWnd = NULL;
			if (g_pCosmos->m_pCLRProxy)
			{
				if (g_pCosmos->m_pCosmosAppProxy)
					g_pCosmos->m_pCosmosAppProxy->OnCosmosClose();
			}

			if (g_pCosmos->m_hCBTHook) {
				UnhookWindowsHookEx(g_pCosmos->m_hCBTHook);
				g_pCosmos->m_hCBTHook = nullptr;
			}
		}

		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);

		if (g_pCosmos->m_hMainWnd == NULL &&
			g_pCosmos->m_bChromeNeedClosed == false &&
			g_pCosmos->m_mapBrowserWnd.size() == 1 &&
			::IsWindow(g_pCosmos->m_hHostBrowserWnd) &&
			g_pCosmos->m_nAppID == 10000 &&
			g_pCosmos->m_bEclipse == false)
		{
			::SendMessageW(g_pCosmos->m_hHostBrowserWnd, WM_CLOSE, 0, 0);
		}
		return lRes;
	}

	LRESULT CBrowser::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&) {
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		WINDOWPOS* lpwndpos = (WINDOWPOS*)lParam;
		if (g_pCosmos->m_pCLRProxy)
		{
			g_pCosmos->m_pCLRProxy->PreWindowPosChanging(m_hWnd, lpwndpos, 0);
		}
		HWND hPWnd = ::GetParent(m_hWnd);
		if (::IsWindow(hPWnd))
		{
			WINDOWPOS* lpwndpos = (WINDOWPOS*)lParam;
			RECT rc;
			::GetClientRect(hPWnd, &rc);
			lpwndpos->x = -12;
			lpwndpos->y = -6 - m_heightfix;
			lpwndpos->cx = rc.right + 24;
			lpwndpos->cy = rc.bottom + 18 + 3 + m_heightfix;
		}
		else if (g_pCosmos->m_bOMNIBOXPOPUPVISIBLE)
			::SendMessage(m_hWnd, WM_BROWSERLAYOUT, 0, 2);
		return lRes;
	}

	LRESULT CBrowser::OnBrowserLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		DefWindowProc(uMsg, wParam, lParam);
		if (g_pCosmos->m_bChromeNeedClosed == false && m_pVisibleWebWnd)
		{
			switch (lParam)
			{
			case 1:
			{
				if (m_pBrowser)
				{
					m_pBrowser->LayoutBrowser();
					BrowserLayout();
				}
			}
			break;
			case 2:
			case 3:
			case 4:
			{
				if (lParam == 4)
				{
					if (g_pCosmos->m_bOMNIBOXPOPUPVISIBLE)
					{
						BrowserLayout();
					}
					m_pBrowser->LayoutBrowser();
				}
				else if (m_pBrowser)
				{
					if (!::IsWindowVisible(m_hWnd))
						::ShowWindow(m_hWnd, SW_SHOW);
					if (g_pCosmos->m_bOMNIBOXPOPUPVISIBLE)
					{
						BrowserLayout();
						m_pBrowser->LayoutBrowser();
					}
					m_bTabChange = false;
				}
			}
			break;
			}
		}
		return 0;
	}

	void CBrowser::OnFinalMessage(HWND hWnd) {
		CWindowImpl::OnFinalMessage(hWnd);
		delete this;
	}

	STDMETHODIMP CBrowser::get_RemoteXobj(IXobj** pVal)
	{
		if (m_pRemoteXobj != nullptr)
		{
			*pVal = m_pRemoteXobj;
		}
		return S_OK;
	}

	STDMETHODIMP CBrowser::put_RemoteXobj(IXobj* newVal)
	{
		CComQIPtr<IXobj>pXobj(newVal);
		if (pXobj)
			m_pRemoteXobj = pXobj.Detach();
		//if (m_pRemoteNode == nullptr)
		//{
		//	IStream* pStream = 0;
		//	HRESULT hr = ::CoMarshalInterThreadInterfaceInStream(IID_IXobj, (IXobj*)newVal.pdispVal, &pStream);
		//	if (hr == S_OK)
		//	{
		//		IDispatch* pTarget = nullptr;
		//		hr = ::CoGetInterfaceAndReleaseStream(pStream, IID_IDispatch, (LPVOID*)&pTarget);
		//		if (hr == S_OK && pTarget)
		//			hr = pTarget->QueryInterface(IID_IXobj, (void**)m_pRemoteNode);
		//	}
		//}
		return S_OK;
	}

	STDMETHODIMP CBrowser::AddURLs(BSTR bstrURLs)
	{
		if (m_pVisibleWebWnd)
		{
			CString strDisposition = _T("");
			strDisposition.Format(_T("%d"), NEW_BACKGROUND_TAB);
			if (m_pVisibleWebWnd->m_pChromeRenderFrameHost)
			{
				IPCMsg msg;
				msg.m_strId = L"ADD_URL";
				msg.m_strParam1 = OLE2T(bstrURLs);
				msg.m_strParam2 = strDisposition;
				m_pVisibleWebWnd->m_pChromeRenderFrameHost->SendCosmosMessage(&msg);
			}
		}
		return S_OK;
	}

	STDMETHODIMP CBrowser::OpenURL(BSTR bstrURL, BrowserWndOpenDisposition nDisposition, BSTR bstrKey, BSTR bstrXml)
	{
		if (m_pVisibleWebWnd)
		{
			CString strDisposition = _T("");
			strDisposition.Format(_T("%d"), nDisposition);
			if (m_pVisibleWebWnd->m_pChromeRenderFrameHost)
			{
				IPCMsg msg;
				msg.m_strId = L"OPEN_URL";
				msg.m_strParam1 = OLE2T(bstrURL);
				msg.m_strParam2 = strDisposition;
				m_pVisibleWebWnd->m_pChromeRenderFrameHost->SendCosmosMessage(&msg);
			}
		}
		return S_OK;
	}
}  // namespace Browser
