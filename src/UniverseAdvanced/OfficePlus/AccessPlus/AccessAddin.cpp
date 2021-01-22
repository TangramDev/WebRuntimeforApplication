/********************************************************************************
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

#include "../../stdafx.h"
#include "../../UniverseApp.h"
#include "AccessAddin.h"
//#include "AccessEvents.cpp"

namespace OfficePlus
{
	namespace AccessPlus
	{
		CAccessAddin::CAccessAddin()
		{
		}

		CAccessAddin::~CAccessAddin()
		{
		}

		STDMETHODIMP CAccessAddin::CosmosCommand(IDispatch* RibbonControl)
		{
			if (m_spRibbonUI)
				m_spRibbonUI->Invalidate();
			return S_OK;
		}

		HRESULT CAccessAddin::OnConnection(IDispatch* pHostApp, int ConnectMode)
		{
			COfficeAddin::OnConnection(pHostApp, ConnectMode);
			pHostApp->QueryInterface(__uuidof(IDispatch), (LPVOID*)&m_pApplication);
			int nHandle = 0;
			m_pApplication->hWndAccessApp(&nHandle);
			HWND h = (HWND)nHandle;
			m_bEnableProcessFormTabKey = true;
			return S_OK;
		}

		HRESULT CAccessAddin::OnDisconnection(int DisConnectMode)
		{
			m_pApplication.Detach();
			return S_OK;
		}

		STDMETHODIMP CAccessAddin::GetCustomUI(BSTR RibbonID, BSTR* RibbonXml)
		{
			if (!RibbonXml)
				return E_POINTER;
			*RibbonXml = m_strRibbonXml.AllocSysString();
			return (*RibbonXml ? S_OK : E_OUTOFMEMORY);
		}

		STDMETHODIMP CAccessAddin::GetObjectFromWnd(LONG hWnd, IDispatch** ppObjFromWnd)
		{
			HWND h = (HWND)hWnd;
			auto it = m_mapOfficeObjects.find(h);
			if (it != m_mapOfficeObjects.end())
			{
				*ppObjFromWnd = it->second;
			}
			return S_OK;
		}

		void CAccessAddin::WindowDestroy(HWND hWnd)
		{
			::GetClassName(hWnd, m_szBuffer, MAX_PATH);
			CString strClassName = CString(m_szBuffer);
			if (strClassName == _T("OForm") || strClassName == _T("OFormPopup"))
			{
				OnCloseOfficeObj(strClassName, hWnd);
			}
		}

		void CAccessAddin::WindowCreated(CString strClassName, LPCTSTR strName, HWND hPWnd, HWND hWnd)
		{
			if (strClassName == _T("OForm")|| strClassName == _T("OFormPopup"))
			{
				CAccessForm* pWnd = new CComObject<CAccessForm>;
				pWnd->m_hForm = hWnd;
				m_mapOfficeObjects[hWnd] = pWnd;
				::PostMessage(m_hHostWnd, WM_OFFICEOBJECTCREATED, (WPARAM)hWnd, 0);
				return;
			}
		}

		void CAccessAddin::ConnectOfficeObj(HWND hWnd)
		{
			CComPtr<MSAccess::Forms> m_pForms;
			if (m_pApplication)
			{
				m_pApplication->get_Forms(&m_pForms);
				if (m_pForms)
				{
					long nCount = 0;
					m_pForms->get_Count(&nCount);
					if (nCount)
					{
						for (int i = 0; i < nCount;i++)
						{
							CComPtr<MSAccess::_Form3> pForm;
							m_pForms->get_Item(CComVariant(i), &pForm);
							if (pForm)
							{
								long h = 0;
								pForm->get_Hwnd(&h);
								if (h == (LONGLONG)hWnd)
								{
									auto it = m_mapOfficeObjects.find(hWnd);
									if (it != m_mapOfficeObjects.end())
									{
										it->second->m_pOfficeObj = pForm.p;
										it->second->m_pOfficeObj->AddRef();
										CAccessForm* pAccessForm = (CAccessForm*)it->second;
										HRESULT hr = pAccessForm->DispEventAdvise(it->second->m_pOfficeObj);
									}
									break;
								}
							}
						}
					}
				}
			}
		}

		CAccessForm::CAccessForm(void)
		{

		}

		CAccessForm::~CAccessForm(void) 
		{
		}

		void CAccessForm::OnObjDestory()
		{
			if (m_pOfficeObj != nullptr)
			{
				HRESULT hr = this->DispEventUnadvise(m_pOfficeObj);
			}
		}

		//void CAccessForm::Load()
		//{
		//}

		//void CAccessForm::Current() 
		//{
		//}

		//void CAccessForm::BeforeInsert(short* Cancel) 
		//{
		//}

		//void CAccessForm::AfterInsert()
		//{
		//}

		//void CAccessForm::BeforeUpdate(short* Cancel)
		//{
		//}

		//void CAccessForm::AfterUpdate()
		//{
		//}

		//void CAccessForm::Delete(short* Cancel)
		//{
		//}

		//void CAccessForm::BeforeDelConfirm(short* Cancel, short* Response)
		//{
		//}

		//void CAccessForm::AfterDelConfirm(short* Status) 
		//{
		//}

		//void CAccessForm::Open(short* Cancel)
		//{
		//}
		//
		//void CAccessForm::Resize() 
		//{
		//}
		//
		//void CAccessForm::Unload(short* Cancel)
		//{
		//}
		//
		//void CAccessForm::Close()
		//{
		//}
		//
		//void CAccessForm::Activate()
		//{
		//}
		//
		//void CAccessForm::Deactivate()
		//{
		//}
		//
		//void CAccessForm::GotFocus() {
		//}

		//void CAccessForm::LostFocus() 
		//{
		//}
	}
}

