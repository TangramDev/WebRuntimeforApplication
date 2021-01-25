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

#include "BrowserWndOpenDisposition.h"

using namespace System;
using namespace System::Windows::Forms;

namespace Universe
{
	ref class Xobj;
	ref class Galaxy;

	public ref class Wormhole
	{
	public:
		CSession*	m_pCosmosSession;
		Wormhole(CSession* m_pCosmosSession);
		Object^ m_pHostObj = nullptr;
		void InsertString(String^ key, String^ value);
		void InsertLong(String^ key, long value);
		void InsertInt64(String^ key, __int64 value);
		void InsertFloat(String^ key, float value);
		String^ GetString(String^ key);
		long GetLong(String^ key);
		float GetFloat(String^ key);
		__int64 GetInt64(String^ key);
		void SendMessage();

		String^ m_strEvents = L"";
		//property String^ EventBindInfo
		//{
		//	String^ get()
		//	{
		//		return m_strEvents->ToLower();
		//	};
		//	void set(String^ strData)
		//	{
		//		m_strEvents = strData->ToLower();
		//	};
		//}

		bool isBindCLRObjToWebPage(Object^ obj, String^ eventName);
		void AddEventInfo(Object^ obj, String^ eventName);
	};

	public ref class Browser
	{
	public:
		IBrowser* m_pWebBrowser;
		Browser(IBrowser* pWebBrowser);
		void AddURLs(String^ urls);
		void OpenURL(String^ url, Disposition disposition, String^ key, String^ xml);
	};

	public ref class WebPage : public IWin32Window
	{
	public:
		HWND			m_hWnd;
		IWebPage*		m_pWebPage;
		CWebPageImpl*	m_pProxy;
		WebPage(IWebPage* m_pWebPage);

		property IntPtr Handle
		{
			virtual IntPtr get();
		}

		property Xobj^ ParentXobj
		{
			Xobj^ get();
		}

		property Galaxy^ HostGalaxy
		{
			Galaxy^ get();
		}

		Form^ CreateForm(String^ strFormKey);
	};
}