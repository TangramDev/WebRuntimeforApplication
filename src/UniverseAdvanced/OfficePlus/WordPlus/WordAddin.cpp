/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202101200014
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

#include "../../stdafx.h"
#include "../../UniverseApp.h"
#include "../../XobjHelper.h"
#include "../../Xobj.h"
#include "../../Galaxy.h"
#include "../../TangramHtmlTreeWnd.h"
#include "fm20.h"
#include "WordAddin.h"
#include "WordPlusDoc.h"

namespace OfficePlus
{
	namespace WordPlus
	{
		CWordAddin::CWordAddin() :COfficeAddin()
		{
			m_pActiveWordObject = nullptr;
			m_pWordAppObjEvents = nullptr;
			m_pWordAppObjEvents2 = nullptr;
			m_strDesignerToolBarCaption = _T("Word Document Designer");
		}

		CWordAddin::~CWordAddin()
		{
			if (m_pWordAppObjEvents)
				delete m_pWordAppObjEvents;
			if (m_pWordAppObjEvents2)
				delete m_pWordAppObjEvents2;
		}

		void CWordAddin::AddDocXml(IDispatch* pDocdisp, BSTR bstrXml, BSTR bstrKey)
		{
			CComQIPtr<Word::_Document> pDoc(pDocdisp);
			if (pDoc)
			{
				CComPtr<Office::_CustomXMLParts> pCustomXMLParts;
				pDoc->get_CustomXMLParts(&pCustomXMLParts);
				_AddDocXml(pCustomXMLParts.p, bstrXml, bstrKey);
			}
		}

		CString CWordAddin::GetDocXmlByKey(IDispatch* pDocdisp, BSTR bstrKey)
		{
			CComQIPtr<Word::_Document> pDoc(pDocdisp);
			CString strRet = _T("");
			if (pDoc)
			{
				CComPtr<Office::_CustomXMLParts> pCustomXMLParts;
				pDoc->get_CustomXMLParts(&pCustomXMLParts);
				if (pCustomXMLParts)
				{
					strRet = _GetDocXmlByKey(pCustomXMLParts.p, bstrKey);
				}
			}
			return strRet;
		}

		void CWordAddin::OnOpenDoc(WPARAM wParam)
		{
			CWordDocument* pWordDoc = (CWordDocument*)wParam;
			CWordObject* pWordWnd = pWordDoc->begin()->second;

			auto it = g_pCosmos->m_mapWindowPage.find(pWordWnd->m_hChildClient);
			if (it != g_pCosmos->m_mapWindowPage.end())
				pWordDoc->m_pDocGalaxyCluster = (CGalaxyCluster*)it->second;
			else
			{
				pWordDoc->m_pDocGalaxyCluster = new CComObject<CGalaxyCluster>();
				pWordDoc->m_pDocGalaxyCluster->m_hWnd = pWordWnd->m_hChildClient;
				g_pCosmos->m_mapWindowPage[pWordWnd->m_hChildClient] = pWordWnd->m_pGalaxyCluster;
			}

			if (pWordDoc->m_pDocGalaxyCluster)
			{
				IGalaxy* pGalaxy = NULL;
				pWordDoc->m_pDocGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)pWordWnd->m_hChildClient), CComBSTR(L"Document"), &pGalaxy);
				pWordDoc->m_pGalaxy = (CGalaxy*)pGalaxy;
				if (pWordDoc->m_pGalaxy)
				{
					auto it = pWordDoc->m_mapDocUIInfo.find(_T("documentui"));
					CString strXml = _T("");
					if (it != pWordDoc->m_mapDocUIInfo.end())
						strXml = it->second;
					else
						strXml = _T("<Document><cluster><xobj name=\"Start\" gridtype=\"nucleus\"/></cluster></Document>");
					IXobj* pXobj = nullptr;
					pWordDoc->m_pGalaxy->Observe(CComBSTR(L"Default"), strXml.AllocSysString(), &pXobj);
					CXobj* _pXobj = (CXobj*)pXobj;
					if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr)
					{
						_pXobj->m_pXobjShareData->m_pOfficeObj = pWordDoc->m_pDoc;
						_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
					}

					CString strKey = _T("taskpaneui");
					
					it = pWordDoc->m_mapDocUIInfo.find(strKey);
					strXml = _T("");
					if (it != pWordDoc->m_mapDocUIInfo.end())
						strXml = it->second;

					if (strXml != _T(""))
					{
						CTangramXmlParse m_Parse;
						if (m_Parse.LoadXml(strXml))
						{
							VARIANT vWindow;
							vWindow.vt = VT_DISPATCH;
							vWindow.pdispVal = nullptr;
							Office::_CustomTaskPane* _pCustomTaskPane;
							CString strCap = _T("");
							CTangramXmlParse* pXobjParse = m_Parse.FindItem(TGM_XOBJ);
							strCap = pXobjParse->attr(_T("caption"), _T(""));
							if (strCap == _T(""))
								strCap = pXobjParse->attr(_T("id"), _T(""));;
							CComBSTR bstrCap(strCap);
							HRESULT hr = m_pCTPFactory->CreateCTP(L"Tangram.Ctrl.1", bstrCap, vWindow, &_pCustomTaskPane);
							_pCustomTaskPane->AddRef();
							_pCustomTaskPane->put_Visible(true);
							m_mapTaskPaneMap[(long)m_pActiveWordObject->m_hForm] = _pCustomTaskPane;
							CComPtr<ICosmosCtrl> pCtrlDisp;
							_pCustomTaskPane->get_ContentControl((IDispatch**)&pCtrlDisp);
							if (pCtrlDisp)
							{
								LONGLONG hWnd = 0;
								pCtrlDisp->get_HWND(&hWnd);
								HWND hPWnd = ::GetParent((HWND)hWnd);
								pWordWnd->m_hTaskPane = (HWND)hWnd;
								CWindow m_Wnd;
								m_Wnd.Attach(hPWnd);
								m_Wnd.ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
								m_Wnd.Detach();
								CWordDocument* m_pDoc = pWordWnd->m_pWordPlusDoc;
								if (m_pDoc->m_pTaskPaneGalaxyCluster == nullptr)
								{
									auto it = g_pCosmos->m_mapWindowPage.find(hPWnd);
									if (it != g_pCosmos->m_mapWindowPage.end())
										m_pDoc->m_pTaskPaneGalaxyCluster = (CGalaxyCluster*)it->second;
									else
									{
										m_pDoc->m_pTaskPaneGalaxyCluster = new CComObject<CGalaxyCluster>();
										m_pDoc->m_pTaskPaneGalaxyCluster->m_hWnd = hPWnd;
										g_pCosmos->m_mapWindowPage[hPWnd] = m_pDoc->m_pTaskPaneGalaxyCluster;
									}

									if (m_pDoc->m_pTaskPaneGalaxyCluster)
									{
										IGalaxy* pTaskPaneFrame = nullptr;
										m_pDoc->m_pTaskPaneGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)hWnd), CComBSTR(L"TaskPane"), &pTaskPaneFrame);
										m_pDoc->m_pTaskPaneGalaxy = (CGalaxy*)pTaskPaneFrame;
										if (m_pDoc->m_pTaskPaneGalaxy)
										{
											IXobj* pXobj = nullptr;
											m_pDoc->m_pTaskPaneGalaxy->Observe(CComBSTR("Default"), strXml.AllocSysString(), &pXobj);
											CXobj* _pXobj = (CXobj*)pXobj;
											if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr)
											{
												_pXobj->m_pXobjShareData->m_pOfficeObj = pWordDoc->m_pDoc;
												_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
											}
										}
									}
								}
								else
									m_pDoc->m_pTaskPaneGalaxy->ModifyHost(hWnd);
							}
						}
					}
				}
			}
		}

		void CWordAddin::OnStartup()
		{
			if (m_pApplication == nullptr)
				return;
			int nIndex = 0x00000001;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>();
					pObj->Init(_T("Startup"), nIndex, m_pApplication.p);
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnQuit()
		{
			int nIndex = 0x00000002;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("Quit"), nIndex, m_pApplication.p);
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnDocumentChange()
		{
			int nIndex = 0x00000003;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentChange"), nIndex, m_pApplication.p);
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnDocumentOpen(_Document* Doc) 
		{
			int nIndex = 0x00000004;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentBeforeSave"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnDocumentBeforeClose(_Document* Doc, VARIANT_BOOL* Cancel) 
		{
			int nIndex = 0x00000006;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentBeforeClose"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnDocumentBeforePrint(_Document* Doc, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000007;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentBeforePrint"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnNewDocument(_Document* Doc) 
		{
			int nIndex = 0x00000009;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("NewDocument"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnWindowActivate(_Document* Doc, Word::Window* Wn)
		{
			int nIndex = 0x0000000a;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowActivate"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnWindowDeactivate(_Document* Doc, Word::Window* Wn)
		{
			int nIndex = 0x0000000b;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowDeactivate"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnWindowSelectionChange(Selection* Sel)
		{
			int nIndex = 0x0000000c;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowSelectionChange"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Sel;
					pObj->m_mapDisp[0]->AddRef();
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnWindowBeforeRightClick(Selection* Sel, VARIANT_BOOL* Cancel) 
		{
			int nIndex = 0x0000000d;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowBeforeRightClick"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Sel;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnWindowBeforeDoubleClick(Selection* Sel, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x0000000e;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowBeforeDoubleClick"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Sel;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::OnDocumentBeforeSave(_Document* Doc, VARIANT_BOOL* SaveAsUI, VARIANT_BOOL* Cancel)
		{
			auto it = find(Doc);
			if (it != end())
			{
				CGalaxy* pGalaxy = it->second->m_pGalaxy;
				if (pGalaxy)
					pGalaxy->UpdateXobj();
				pGalaxy = it->second->m_pTaskPaneGalaxy;
				if (pGalaxy)
					pGalaxy->UpdateXobj();
			}
			int nIndex = 0x00000008;
			auto it2 = m_mapObjEventDic.find(m_pApplication.p);
			if (it2 != m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentBeforeSave"), nIndex, m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_BOOL | VT_BYREF;
					var.pboolVal = SaveAsUI;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[1] = var1;
					FireAppEvent(pObj);
				}
			}
		}

		void CWordAddin::WindowDestroy(HWND hWnd)
		{
			::GetClassName(hWnd, m_szBuffer, MAX_PATH);
			CString strClassName = CString(m_szBuffer);
			if (strClassName == _T("ThunderDFrame"))
			{
				auto it = m_mapVBAForm.find(hWnd);
				if (it != m_mapVBAForm.end())
					m_mapVBAForm.erase(it);
			}
			else if (strClassName == _T("_WwB"))
			{
				OnCloseOfficeObj(strClassName, hWnd);
			}
		}

		void CWordAddin::WindowCreated(CString strClassName, LPCTSTR strName, HWND hPWnd, HWND hWnd)
		{
			if (strClassName == _T("_WwB"))
			{
				::PostMessage(m_hHostWnd, WM_OFFICEOBJECTCREATED, (WPARAM)hWnd, 0);
			}
		}

		STDMETHODIMP CWordAddin::StartApplication(BSTR bstrAppID, BSTR bstrXml)
		{
			CString strID = OLE2T(bstrAppID);
			strID.MakeLower();
			if (strID.Find(_T("word.application")) == 0)
				return put_AppKeyValue(CComBSTR(L"doctemplate"), CComVariant(bstrXml));
			return CCosmos::StartApplication(bstrAppID, bstrXml);
		}

		STDMETHODIMP CWordAddin::CosmosCommand(IDispatch* RibbonControl)
		{
			if (m_spRibbonUI)
				m_spRibbonUI->Invalidate();
			CComBSTR bstrTag(L"");
			CComBSTR bstrID(L"");
			CComQIPtr<IRibbonControl> pIRibbonControl(RibbonControl);
			if (pIRibbonControl)
			{
				pIRibbonControl->get_Id(&bstrID);
				pIRibbonControl->get_Tag(&bstrTag);
				CString strTag = OLE2T(bstrTag);
				if (strTag.CompareNoCase(_T("opentangramfile")) == 0)
				{
					CComPtr<ICosmosDoc> pDoc;
					return this->OpenTangramFile(&pDoc);
				}
				int nPos = strTag.Find(_T("@"));
				CString strPath = m_strAppCommonDocPath + strTag;
				if (::PathFileExists(strPath))
				{
					CComPtr<ICosmosDoc> pDoc;
					return this->OpenCosmosDocFile(strPath.AllocSysString(), &pDoc);
				}
			}
			if (m_pActiveWordObject == nullptr)
				return S_OK;
			CString strTag = OLE2T(bstrTag);
			int nPos = strTag.Find(_T("TangramButton.Cmd"));
			strTag.Replace(_T("TangramButton.Cmd."), _T(""));
			int nCmdIndex = _wtoi(strTag);

			CWordObject* pWnd = m_pActiveWordObject;
			CWordDocument* pWordDoc = m_pActiveWordObject->m_pWordPlusDoc;

			switch (nCmdIndex)
			{
			case 100:
			{
				if (pWnd)
				{
					CGalaxy* pGalaxy = pWnd->m_pWordPlusDoc->m_pGalaxy;
					if (pGalaxy == nullptr)
						break;
					if (pWnd->m_bDesignState == false)
					{
						pGalaxy->m_bDesignerState = true;
						pWnd->m_bDesignState = true;
						CreateCommonDesignerToolBar();
						CXobj* pXobj = pGalaxy->m_pWorkXobj;
						if (pXobj->m_strID.CompareNoCase(TGM_NUCLEUS) == 0)
						{
							CString strXml = _T("<documentui><cluster><xobj name=\"Start\" /></cluster></documentui>");
							IXobj* pDesignNode = nullptr;
							pGalaxy->Observe(CComBSTR(L"default-inDesigning"), CComBSTR(strXml), &pDesignNode);
						}

						m_pDesigningFrame = pGalaxy;
						m_pDesigningFrame->UpdateDesignerTreeInfo();
						break;
					}
					else
					{
						pGalaxy->m_bDesignerState = false;
						pWnd->m_bDesignState = false;
					}
				}
			}
			break;
			case 101:
			case 102:
			{
				auto iter = m_mapTaskPaneMap.find((long)m_pActiveWordObject->m_hForm);
				if (iter != m_mapTaskPaneMap.end())
					iter->second->put_Visible(true);
				else
				{
					CString strKey = _T("taskpaneui");
					auto it = pWnd->m_pWordPlusDoc->m_mapDocUIInfo.find(strKey);
					CString strXml = _T("");
					if (it != pWnd->m_pWordPlusDoc->m_mapDocUIInfo.end())
						strXml = it->second;
					else
						strXml = _T("<taskpaneui><cluster><xobj name=\"Start\" /></cluster></taskpaneui>");
					if (strXml != _T(""))
					{
						CTangramXmlParse m_Parse;
						if (m_Parse.LoadXml(strXml))
						{
							VARIANT vWindow;
							vWindow.vt = VT_DISPATCH;
							vWindow.pdispVal = nullptr;
							Office::_CustomTaskPane* _pCustomTaskPane;
							CString strCap = _T("");
							CTangramXmlParse* pXobjParse = m_Parse.FindItem(TGM_XOBJ);
							strCap = pXobjParse->attr(_T("caption"), _T(""));
							if (strCap == _T(""))
								strCap = pXobjParse->attr(_T("id"), _T(""));;
							CComBSTR bstrCap(strCap);
							HRESULT hr = m_pCTPFactory->CreateCTP(L"Tangram.Ctrl.1", bstrCap, vWindow, &_pCustomTaskPane);
							_pCustomTaskPane->AddRef();
							_pCustomTaskPane->put_Visible(true);
							m_mapTaskPaneMap[(long)m_pActiveWordObject->m_hForm] = _pCustomTaskPane;
							CComPtr<ICosmosCtrl> pCtrlDisp;
							_pCustomTaskPane->get_ContentControl((IDispatch**)&pCtrlDisp);
							if (pCtrlDisp)
							{
								LONGLONG hWnd = 0;
								pCtrlDisp->get_HWND(&hWnd);
								HWND hPWnd = ::GetParent((HWND)hWnd);
								pWnd->m_hTaskPane = (HWND)hWnd;
								CWindow m_Wnd;
								m_Wnd.Attach(hPWnd);
								m_Wnd.ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
								m_Wnd.Detach();
								CWordDocument* m_pDoc = pWnd->m_pWordPlusDoc;
								if (m_pDoc->m_pTaskPaneGalaxyCluster == nullptr)
								{
									auto it = m_mapWindowPage.find(hPWnd);
									if (it != m_mapWindowPage.end())
										m_pDoc->m_pTaskPaneGalaxyCluster = (CGalaxyCluster*)it->second;
									else
									{
										m_pDoc->m_pTaskPaneGalaxyCluster = new CComObject<CGalaxyCluster>();
										m_pDoc->m_pTaskPaneGalaxyCluster->m_hWnd = hPWnd;
										m_mapWindowPage[hPWnd] = m_pDoc->m_pTaskPaneGalaxyCluster;
									}

									if (m_pDoc->m_pTaskPaneGalaxyCluster)
									{
										IGalaxy* pTaskPaneFrame = nullptr;
										m_pDoc->m_pTaskPaneGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)hWnd), CComBSTR(L"TaskPane"), &pTaskPaneFrame);
										m_pDoc->m_pTaskPaneGalaxy = (CGalaxy*)pTaskPaneFrame;
										if (m_pDoc->m_pTaskPaneGalaxy)
										{
											IXobj* pXobj = nullptr;
											m_pDoc->m_pTaskPaneGalaxy->Observe(CComBSTR("Default"), strXml.AllocSysString(), &pXobj);
											CXobj* _pXobj = (CXobj*)pXobj;
											if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr)
											{
												_pXobj->m_pXobjShareData->m_pOfficeObj = pWordDoc->m_pDoc;
												_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
											}
										}
									}
								}
								else
									m_pDoc->m_pTaskPaneGalaxy->ModifyHost(hWnd);
							}
						}
					}
				}
				if (nCmdIndex == 102 && pWnd)
				{
					CreateCommonDesignerToolBar();
					CGalaxy* pGalaxy = pWnd->m_pWordPlusDoc->m_pTaskPaneGalaxy;
					if (pWnd->m_bDesignTaskPane == false)
					{
						pGalaxy->m_bDesignerState = true;
						if (m_pDesigningFrame != pGalaxy)
						{
							m_pDesigningFrame = pGalaxy;
							pGalaxy->UpdateDesignerTreeInfo();
						}
					}
					else
					{
						pGalaxy->m_bDesignerState = false;
						if (m_pDesigningFrame == pGalaxy)
						{
							m_pDesigningFrame = nullptr;
							pGalaxy->UpdateDesignerTreeInfo();
						}
					}
				}
			}
			break;
			}

			CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
			pObj->Init(_T("WordPlus"), 0, pWordDoc->m_pDoc);
			pObj->m_mapVar[0] = CComVariant(bstrID);
			pObj->m_mapVar[1] = CComVariant(bstrTag);

			FireAppEvent(pObj);

			return S_OK;
		}

		HRESULT CWordAddin::OnConnection(IDispatch* pHostApp, int ConnectMode)
		{
			COfficeAddin::OnConnection(pHostApp, ConnectMode);
			if (m_strRibbonXml == _T(""))
			{
				CTangramXmlParse m_Parse;
				if (m_Parse.LoadXml(m_strConfigFile) || m_Parse.LoadFile(m_strConfigFile))
				{
					m_strRibbonXml = m_Parse[_T("RibbonUI")][_T("customUI")].xml();
				}
			}

			pHostApp->QueryInterface(__uuidof(IDispatch), (LPVOID*)&m_pApplication);
			BSTR bstrCap = ::SysAllocString(L"");
			m_pApplication->get_Caption(&bstrCap);
			CString strCaption = OLE2T(bstrCap);
			strCaption += _T(" Plus");
			m_pApplication->put_Caption(strCaption.AllocSysString());
			::SysFreeString(bstrCap);
			HRESULT hr = ((CWordAppEvents2*)this)->DispEventAdvise(m_pApplication);
			if (m_pWordAppObjEvents)
			{
				hr = m_pWordAppObjEvents->DispEventAdvise(m_pApplication);
			}
			if (m_pWordAppObjEvents2)
			{
				hr = m_pWordAppObjEvents2->DispEventAdvise(m_pApplication);
			}
			//hr = ((CWordAppEvents3*)this)->DispEventAdvise(m_pApplication);
			//hr = ((CWordAppEvents4*)this)->DispEventAdvise(m_pApplication);
			if (::IsWindow(m_hHostWnd))
			{
				WNDCLASSEX wcex;
				wcex.cbSize = sizeof(WNDCLASSEX);
				wcex.style = CS_HREDRAW | CS_VREDRAW;
				wcex.lpfnWndProc = ::DefWindowProc;
				wcex.cbClsExtra = 0;
				wcex.cbWndExtra = 0;
				wcex.hInstance = theApp.m_hInstance;
				wcex.hIcon = NULL;
				wcex.hCursor = NULL;
				wcex.hbrBackground = NULL;
				wcex.lpszMenuName = NULL;
				wcex.lpszClassName = L"Tangram Word Helper Window";
				wcex.hIconSm = NULL;
				RegisterClassEx(&wcex);
			}
			return S_OK;
		}

		HRESULT CWordAddin::OnDisconnection(int DisConnectMode)
		{
			HRESULT hr = ((CWordAppEvents2*)this)->DispEventUnadvise(m_pApplication);
			if (m_pWordAppObjEvents)
			{
				hr = m_pWordAppObjEvents->DispEventUnadvise(m_pApplication);
			}
			if (m_pWordAppObjEvents2)
			{
				hr = m_pWordAppObjEvents2->DispEventUnadvise(m_pApplication);
			}

			m_pApplication = nullptr;
			return S_OK;
		}

		STDMETHODIMP CWordAddin::GetCustomUI(BSTR RibbonID, BSTR* RibbonXml)
		{
			if (!RibbonXml || m_strRibbonXml == _T(""))
				return E_POINTER;
			*RibbonXml = CComBSTR(m_strRibbonXml);
			return (*RibbonXml ? S_OK : E_OUTOFMEMORY);
		}

		CString CWordAddin::GetFormXml(CString strFormName)
		{
			CWordDocument* pWordDoc = m_pActiveWordObject->m_pWordPlusDoc;
			auto it = pWordDoc->m_mapUserFormScript.find(strFormName);
			if (it != pWordDoc->m_mapUserFormScript.end())
				return it->second;

			return _T("");
		}

		STDMETHODIMP CWordAddin::ExportOfficeObjXml(IDispatch* OfficeObject, BSTR* bstrXml)
		{
			return S_OK;
		}

		void CWordAddin::UpdateOfficeObj(IDispatch* pObj, CString strXml, CString _strName)
		{
			CComQIPtr<MSForm::_UserForm> pForm(pObj);
			if (pForm)
			{
				CComPtr<_Document> pDoc;
				m_pApplication->get_ActiveDocument(&pDoc);
				if (pDoc)
				{
					CComPtr<Office::_CustomXMLParts> pCustomXMLParts;
					pDoc->get_CustomXMLParts(&pCustomXMLParts);
					CTangramXmlParse m_Parse;
					if (m_Parse.LoadXml(_GetDocXmlByKey(pCustomXMLParts.p, CComBSTR(L"tangram"))))
					{
						CString strName = _T("");
						DISPID dispID = 0x80010000;
						DISPPARAMS dispParams = { NULL, NULL, 0, 0 };
						VARIANT result = { 0 };
						EXCEPINFO excepInfo;
						memset(&excepInfo, 0, sizeof excepInfo);
						UINT nArgErr = (UINT)-1;
						HRESULT hr = pObj->Invoke(dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, &result, &excepInfo, &nArgErr);
						if (S_OK == hr && VT_BSTR == result.vt)
						{
							CComBSTR bstrName = result.bstrVal;
							strName = OLE2T(bstrName);
							::VariantClear(&result);
						}

						CTangramXmlParse* pParse = m_Parse.GetChild(_T("userforms"));
						if (pParse)
						{
							CTangramXmlParse* pParse2 = pParse->GetChild(strName);
							CString strXml2 = _T("");
							if (pParse2)
							{
								CTangramXmlParse* pParse3 = pParse2->GetChild(TGM_CLUSTER);
								CString strGUID = GetNewGUID();
								pParse2->RemoveNode(pParse3);
								pParse2->put_text(strGUID);
								strXml2 = m_Parse.xml();
								strXml2.Replace(strGUID, strXml);
							}
							else
							{
								strXml2.Format(_T("<%s>%s</%s>"), strName, strXml, strName);
								CTangramXmlParse m_Parse2;
								m_Parse2.LoadXml(strXml2);
								pParse->AddNode(&m_Parse2, _T(""));
								strXml2 = m_Parse.xml();
							}
							AddDocXml(pDoc, CComBSTR(strXml2), CComBSTR(L"tangram"));
						}
					}
				}
				return;
			}
			CComQIPtr<_Document> pDoc(pObj);
			if (pDoc)
			{
				CComPtr<Office::_CustomXMLParts> pCustomXMLParts;
				pDoc->get_CustomXMLParts(&pCustomXMLParts);
				CString _strXml = _GetDocXmlByKey(pCustomXMLParts.p, CComBSTR(L"tangram"));
				CTangramXmlParse m_Parse;
				if (m_Parse.LoadXml(_strXml))
				{
					CTangramXmlParse* pParse = m_Parse.GetChild(_strName);
					if (pParse)
					{
						CTangramXmlParse* pParse3 = pParse->GetChild(TGM_CLUSTER);
						CString strGUID = GetNewGUID();
						pParse->RemoveNode(pParse3);
						pParse->put_text(strGUID);
						CString str = m_Parse.xml();
						str.Replace(strGUID, strXml);
						AddDocXml(pDoc, CComBSTR(str), CComBSTR(L"tangram"));
					}
				}

				//CComBSTR bstrXml(L"");
				//it1->second->get_Title(&bstrXml);
				//strXml = OLE2T(bstrXml);
				//m_Parse.put_attr(_T(""), strXml);
				//int nValue = 0;
				//it1->second->get_Width(&nValue);
				//m_Parse.put_attr(_T("width"), nValue);
				//it1->second->get_Height(&nValue);
				//m_Parse.put_attr(_T("height"), nValue);
				//Office::MsoCTPDockPosition m_Pos;
				//it1->second->get_DockPosition(&m_Pos);
				//m_Parse.put_attr(_T("dockposition"), m_Pos);
				//Office::MsoCTPDockPositionRestrict m_MsoCTPDockPositionRestrict;
				//it1->second->get_DockPositionRestrict(&m_MsoCTPDockPositionRestrict);
				//m_Parse.put_attr(_T("dockpositionrestrict"), m_MsoCTPDockPositionRestrict);
				//pAddin->AddDocXml(m_pWorkBook, CComBSTR(m_Parse.xml()), CComBSTR(L"tangram"));
			}
		}

		STDMETHODIMP CWordAddin::AttachObjEvent(IDispatch* pDisp, int nEventIndex)
		{
			if (0x000f <= nEventIndex&&nEventIndex <= 0x0019)
			{
				if (m_pWordAppObjEvents == nullptr)
				{
					m_pWordAppObjEvents = new CWordAppObjEvents();
					m_pWordAppObjEvents->DispEventAdvise(m_pApplication);
				}
			}
			else if (nEventIndex > 0x0019)
			{
				if (m_pWordAppObjEvents2 == nullptr)
				{
					m_pWordAppObjEvents2 = new CWordAppObjEvents2();
					m_pWordAppObjEvents2->DispEventAdvise(m_pApplication);
				}
			}
			return CCosmos::AttachObjEvent(pDisp, nEventIndex);
		}

		STDMETHODIMP CWordAddin::put_AppKeyValue(BSTR bstrKey, VARIANT newVal)
		{
			CString strKey = OLE2T(bstrKey);

			if (strKey == _T(""))
				return S_OK;
			strKey.Trim();
			strKey.MakeLower();
			if (strKey == _T("doctemplate"))
			{
				auto it = m_mapValInfo.find(_T("doctemplate"));
				if (it != g_pCosmos->m_mapValInfo.end())
				{
					::VariantClear(&it->second);
					g_pCosmos->m_mapValInfo.erase(it);
				}
				g_pCosmos->m_mapValInfo[strKey] = newVal;
				CComPtr<Documents> pDocsDisp2;
				m_pApplication->get_Documents(&pDocsDisp2);
				if (pDocsDisp2)
				{
					CComPtr<_Document> pDoc;
					CComVariant varVisible(true);
					CComVariant varNew(false);
					CComVariant varTemplate(L"");
					CComVariant varTypr(WdDocumentType::wdTypeDocument);
					//pDocsDisp2->Add(CComVariant(nullptr), &CComVariant(nullptr), &CComVariant(nullptr), &varVisible, &pDoc);d:\\tangram.docx
					return pDocsDisp2->Add(&varTemplate, &varNew, &varTypr, &varVisible, &pDoc);
				}
			}
			return CCosmos::put_AppKeyValue(bstrKey, newVal);
		}

		bool CWordAddin::OnActiveOfficeObj(HWND hWnd)
		{
			auto it = m_mapOfficeObjects2.find(hWnd);
			if (it != m_mapOfficeObjects2.end())
			{
				if (m_pActiveWordObject != it->second)
				{
					m_pActiveWordObject = (CWordObject*)it->second;
				}
				OnDocActivate(m_pActiveWordObject);
				return true;
			}
			return false;
		}

		void CWordAddin::OnDocActivate(CWordObject* pWnd)
		{
			if (m_pActiveWordObject)
			{
				if (m_pActiveWordObject->m_bDesignState)
				{
					CreateCommonDesignerToolBar();
				}
				CWordDocument* pWordPlusDoc = m_pActiveWordObject->m_pWordPlusDoc;
				if (pWordPlusDoc)
				{
					if (pWordPlusDoc->m_pGalaxy)
						pWordPlusDoc->m_pGalaxy->ModifyHost((LONGLONG)m_pActiveWordObject->m_hChildClient);
					if (pWordPlusDoc->m_pTaskPaneGalaxy)
						pWordPlusDoc->m_pTaskPaneGalaxy->ModifyHost(m_pActiveWordObject->m_hTaskPane?(LONGLONG)m_pActiveWordObject->m_hTaskPane:(LONGLONG)m_pActiveWordObject->m_hTaskPaneChildWnd);
				}
			}
		}

		HRESULT CWordAddin::CreateCosmosCtrl(void* pv, REFIID riid, LPVOID* ppv)
		{
			return CWordAppCtrl::_CreatorClass::CreateInstance(pv, riid, ppv);
		}

		void CWordAddin::ConnectOfficeObj(HWND hWnd)
		{
			if (m_pApplication == nullptr)
				return;
			::GetWindowText(hWnd, m_szBuffer, 255);
			CString strCaption = CString(m_szBuffer);
			if (strCaption == _T(""))
				return;

			m_pActiveWordObject = new CComObject<CWordObject>;
			m_pActiveWordObject->m_hClient = ::GetParent(hWnd);
			m_pActiveWordObject->m_hChildClient = hWnd;
			m_pActiveWordObject->m_hForm = ::GetParent(m_pActiveWordObject->m_hClient);
			m_mapOfficeObjects2[m_pActiveWordObject->m_hForm] = m_pActiveWordObject;
			m_mapOfficeObjects[hWnd] = m_pActiveWordObject;

			CWindow m_wnd;
			m_wnd.Attach(m_pActiveWordObject->m_hChildClient);
			m_wnd.ModifyStyle(WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, 0);

			bool bNewWindow = false;
			_Document* pDoc = nullptr;
			m_pApplication->get_ActiveDocument(&pDoc);
			CWordDocument* pWordDoc = nullptr;
			auto it1 = find(pDoc);
			if (it1 != end())
			{
				pWordDoc = it1->second;
				bNewWindow = true;
			}
			else
			{
				pWordDoc = new CWordDocument();
				(*this)[pDoc] = pWordDoc;
				HRESULT hr = pWordDoc->DispEventAdvise(pDoc);
				pWordDoc->m_pDoc = pDoc;
				pWordDoc->m_pDoc->AddRef();
			}
			m_pActiveWordObject->m_pOfficeObj = pDoc;
			m_pActiveWordObject->m_pOfficeObj->AddRef();
			m_pActiveWordObject->m_pWordPlusDoc = pWordDoc;
			(*pWordDoc)[hWnd] = m_pActiveWordObject;
			if (bNewWindow)
				return;

			RECT rc;
			::GetClientRect(m_pActiveWordObject->m_hClient, &rc);
			::SetWindowPos(m_pActiveWordObject->m_hChildClient, HWND_TOP, 0, 0, rc.right, rc.bottom, SWP_NOREDRAW | SWP_NOACTIVATE);
			CComBSTR bstrPath(L"");
			pDoc->get_Path(&bstrPath);
			CString strPath = OLE2T(bstrPath);
			if (strPath == _T(""))
			{
				CString strTemplate = GetDocTemplateXml(_T("Please select Word Document Template:"), m_strExeName, _T(""));

				CTangramXmlParse m_Parse;
				bool bLoad = m_Parse.LoadXml(strTemplate);
				if (bLoad == false)
					bLoad = m_Parse.LoadFile(strTemplate);
				if (bLoad == false)
					return;
				VARIANT_BOOL bRet = false;
				CComBSTR bstrRet(L"");
				CString strNewDocInfo = _T("");
				pWordDoc->m_strTaskPaneTitle = m_Parse.attr(_T("title"), _T("TaskPane"));
				pWordDoc->m_nWidth = m_Parse.attrInt(_T("width"), 200);
				pWordDoc->m_nHeight = m_Parse.attrInt(_T("height"), 300);
				pWordDoc->m_nMsoCTPDockPosition = (MsoCTPDockPosition)m_Parse.attrInt(_T("dockposition"), 4);
				pWordDoc->m_nMsoCTPDockPositionRestrict = (MsoCTPDockPositionRestrict)m_Parse.attrInt(_T("dockpositionrestrict"), 3);
				pWordDoc->m_strDocXml = m_Parse.xml();
			}
			else
			{
				auto it = m_mapTaskPaneMap.find((LONG)m_pActiveWordObject->m_hForm);
				if (it != m_mapTaskPaneMap.end())
				{
					it->second->put_Visible(false);
					m_mapTaskPaneMap.erase(it);
				}
			}
			m_pActiveWordObject->m_pWordPlusDoc->InitDocument();
		}

		void CWordAppObjEvents::OnEPostagePropertyDialog(_Document* Doc)
		{
			int nIndex = 0x0000000f;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("EPostagePropertyDialog"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnEPostageInsert(_Document* Doc)
		{
			int nIndex = 0x00000010;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("EPostageInsert"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeAfterMerge(_Document* Doc, Word::Document* DocResult)
		{
			int nIndex = 0x00000011;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeAfterMerge"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = (_Document*)DocResult;
					pObj->m_mapDisp[1]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeAfterRecordMerge(_Document* Doc)
		{
			int nIndex = 0x00000012;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeAfterRecordMerge"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeBeforeMerge(_Document* Doc, long StartRecord, long EndRecord, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000013;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeWizardStateChange"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_I4;
					var1.lVal = StartRecord;
					pObj->m_mapVar[0] = var1;
					CComVariant var2;
					var2.vt = VT_I4;
					var2.lVal = EndRecord;
					pObj->m_mapVar[1] = var2;
					CComVariant var3;
					var3.vt = VT_BOOL | VT_BYREF;
					var3.pboolVal = Cancel;
					pObj->m_mapVar[2] = var3;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeBeforeRecordMerge(_Document* Doc, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000014;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeBeforeRecordMerge"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var3;
					var3.vt = VT_BOOL | VT_BYREF;
					var3.pboolVal = Cancel;
					pObj->m_mapVar[0] = var3;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeDataSourceLoad(_Document* Doc) 
		{
			int nIndex = 0x00000015;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeDataSourceLoad"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeDataSourceValidate(_Document* Doc, VARIANT_BOOL* Handled) 
		{
			int nIndex = 0x00000016;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeDataSourceValidate"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var3;
					var3.vt = VT_BOOL | VT_BYREF;
					var3.pboolVal = Handled;
					pObj->m_mapVar[0] = var3;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeWizardSendToCustom(_Document* Doc)
		{
			int nIndex = 0x00000017;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeWizardSendToCustom"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnMailMergeWizardStateChange(_Document* Doc, int* FromState, int* ToState, VARIANT_BOOL* Handled) 
		{
			int nIndex = 0x00000018;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeWizardStateChange"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_INT|VT_BYREF;
					var1.pintVal = FromState;
					pObj->m_mapVar[0] = var1;
					CComVariant var2;
					var2.vt = VT_INT|VT_BYREF;
					var2.pintVal = ToState;
					pObj->m_mapVar[1] = var2;
					CComVariant var3;
					var3.vt = VT_BOOL | VT_BYREF;
					var3.pboolVal = Handled;
					pObj->m_mapVar[2] = var3;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents::OnWindowSize(_Document* Doc, Word::Window* Wn)
		{
			int nIndex = 0x00000019;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowSize"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnXMLSelectionChange(Word::Selection* Sel, XMLNode* OldXMLNode, XMLNode* NewXMLNode, long* Reason)
		{
			int nIndex = 0x0000001a;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("XMLSelectionChange"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Sel;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = OldXMLNode;
					pObj->m_mapDisp[1]->AddRef();
					pObj->m_mapDisp[2] = NewXMLNode;
					pObj->m_mapDisp[2]->AddRef();
					CComVariant var1;
					var1.vt = VT_I4|VT_BYREF;
					var1.plVal = Reason;
					pObj->m_mapVar[0] = var1;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnXMLValidationError(XMLNode* XMLNode)
		{
			int nIndex = 0x0000001b;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("XMLValidationError"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = XMLNode;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnDocumentSync(_Document* Doc, MsoSyncEventType SyncEvent) 
		{
			int nIndex = 0x0000001c;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("DocumentSync"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_I4;
					var1.intVal = SyncEvent;
					pObj->m_mapVar[0] = var1;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnMailMergeDataSourceValidate2(_Document* Doc, VARIANT_BOOL* Handled)
		{
			int nIndex = 0x0000001e;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("MailMergeDataSourceValidate2"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = Doc;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Handled;
					pObj->m_mapVar[0] = var1;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowOpen(ProtectedViewWindow* PvWindow) 
		{
			int nIndex = 0x0000001f;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowOpen"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowBeforeEdit(ProtectedViewWindow* PvWindow, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000020;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowBeforeEdit"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowBeforeClose(ProtectedViewWindow* PvWindow, int CloseReason, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000021;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowBeforeClose"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_INT;
					var.intVal = CloseReason;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[1] = var1;
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowSize(ProtectedViewWindow* PvWindow) 
		{
			int nIndex = 0x00000022;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowSize"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowActivate(ProtectedViewWindow* PvWindow) 
		{
			int nIndex = 0x00000023;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowActivate"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		void CWordAppObjEvents2::OnProtectedViewWindowDeactivate(ProtectedViewWindow* PvWindow)
		{
			int nIndex = 0x00000024;
			CWordAddin* pAddin = (CWordAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowDeactivate"), nIndex, pAddin->m_pApplication.p);
					pObj->m_mapDisp[0] = PvWindow;
					pObj->m_mapDisp[0]->AddRef();
					g_pCosmos->FireAppEvent(pObj);
				}
			}
		}

		CWordAppCtrl::CWordAppCtrl()
		{

		}

		void CWordAppCtrl::OnFinalMessage(HWND hWnd)
		{
			__super::OnFinalMessage(hWnd);
		}

		STDMETHODIMP CWordAppCtrl::get_tag(VARIANT* pVal)
		{
			return S_OK;
		}


		STDMETHODIMP CWordAppCtrl::put_tag(VARIANT newVal)
		{
			return S_OK;
		}

		STDMETHODIMP CWordAppCtrl::put_AppCtrl(VARIANT_BOOL newVal)
		{
			g_pCosmos->m_pCosmosAppCtrl = this;

			return S_OK;
		}

		STDMETHODIMP CWordAppCtrl::get_HWND(LONGLONG* pVal)
		{
			*pVal = (LONGLONG)m_hWnd;
			return S_OK;
		}

		STDMETHODIMP CWordAppCtrl::get_Cosmos(ICosmos** pVal)
		{
			*pVal = g_pCosmos;
			return S_OK;
		}
	}
}
