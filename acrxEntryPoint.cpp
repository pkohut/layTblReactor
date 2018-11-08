// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "StdArx.h"
#include "resource.h"
#include "DlgLTRSettings.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("rpk")


extern bool g_bWatchOn;

extern bool g_bLayersOff;
extern bool g_bLayersLocked;
extern bool g_bLayersFrozen;
extern bool g_bLayersFrozenVP;
extern bool g_bLayersFrozenVPNew;

// const CString csAppRegistryPath(_T("SOFTWARE\\PK_TOOLS\\"));
const CString csAppName(_T("layTblReactor"));

#ifdef BRX_APP  // BRX builds
const CString csDllName(_T("rpkBRX18.brx"));
#else           // ARX builds
const CString csDllName(_T("rpkARX22.arx"));
#endif // BRX_APP



AcadApp::ErrorStatus getAppInfo(AcadAppInfo & info)
{
	info.setAppName(csAppName);
	return info.readFromRegistry();
}

AcadApp::ErrorStatus UnregisterArxApp()
{
	AcadAppInfo info;
	info.setAppName(csAppName);
	return info.delFromRegistry();
}

AcadApp::ErrorStatus UnregisterArxApp(AcadAppInfo & info)
{
	info.setAppName(csAppName);
	return info.delFromRegistry();
}

AcadApp::ErrorStatus RegisterArxApp(void)
{
	if (!_hdllInstance)
		return AcadApp::eUnknownError;

	//AcadApp::ErrorStatus es = acrxRegisterApp((AcadApp::LoadReasons) (AcadApp::kOnAutoCADStartup),
	//	pcszAppRegistryPath);


	AcadAppInfo info;
	info.setAppName(csAppName);
	HMODULE hModule = GetModuleHandle(csDllName);
	TCHAR szModulePath[MAX_PATH];
	DWORD dwPathLength = GetModuleFileName(hModule, szModulePath, MAX_PATH);
	if (!dwPathLength)
		return AcadApp::eValueNotFound;


	info.setModuleName(szModulePath);
	info.setLoadReason(
		AcadApp::LoadReasons(AcadApp::kOnAutoCADStartup)
	);
#ifdef BRX_APP  // BRX builds
	info.setManaged(Adesk::kFalse);
	return info.writeToRegistry(false, true);
#else           // ARX builds
	info.setManaged(AcadAppInfo::kUnManaged);
	return info.writeToRegistry();
#endif // BRX_APP

}

bool RetrieveLayerStateSettings(void)
{
	CString csFlag;
	CString csEntryName = csAppName + _T("-flags");
	TCHAR * psBuffer = csFlag.GetBuffer(20);
	int nRet =  acedGetEnv(csEntryName, psBuffer, 19);
	csFlag.ReleaseBuffer();

	if (RTNORM == nRet)
	{
		DWORD dwFlag = _tcstoul(csFlag, NULL, 10);
		g_bLayersFrozen = !!(dwFlag & (1 << 0));
		g_bLayersOff = !!(dwFlag & (1 << 1));
		g_bLayersLocked = !!(dwFlag & (1 << 2));
		g_bLayersFrozenVP = !!(dwFlag & (1 << 3));
		g_bLayersFrozenVPNew = !!(dwFlag & (1 << 4));

		return true;
	}
	return false;
}

bool WriteLayerStateSettings(void)
{
	DWORD dwFlag = (DWORD)g_bLayersFrozen	// 1
		| g_bLayersOff << 1			// 2
		| g_bLayersLocked << 2			// 4
		| g_bLayersFrozenVP << 3		// 8
		| g_bLayersFrozenVPNew << 4; 	// 16
	
	CString csFlag;
	csFlag.Format(_T("%u"), dwFlag);
	CString csEntryName = csAppName + _T("-flags");
	if (RTNORM == acedSetEnv(csEntryName, csFlag))
	{
		return true;
	}
	return false;
}

//	TCHAR szBuffer[1024];
//	g_bLayersFrozen = false;
//	g_bLayersOff = true;
//	g_bLayersLocked = false;
//	g_bLayersFrozenVP = true;
//	g_bLayersFrozenVPNew = true;
//
//	DWORD dwFlag = (DWORD) g_bLayersFrozen
//		|  g_bLayersOff << 1
//		|  g_bLayersLocked << 2
//		|  g_bLayersFrozenVP << 3
//		|  g_bLayersFrozenVPNew << 4;
//
//
//
//
//	CString csFlag;
//	csFlag.Format(_T("%u"), dwFlag);
//
//	CString csEntryName = csAppName + _T("-flags");
//	int val = acedSetEnv(csEntryName, csFlag);
//
//	TCHAR * psBuffer = csFlag.GetBuffer(20);
//	val = acedGetEnv(csEntryName, psBuffer, 19);
//	csFlag.ReleaseBuffer();
//
//	AcadAppInfo info1;
//	info1.setAppName(csAppName);
//	AcadApp::ErrorStatus es = info1.readFromRegistry();
//	if (es == AcadApp::eOk)
//	{
//		const TCHAR * pAppName = info1.appName();
//		int x = 0;
//		x++;
//	}
//	return false;
//}

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CBRX18App : public AcRxArxApp {

public:
	CBRX18App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kLoadDwgMsg(void *pkt)
	{
		watchDb();
		return AcRx::kRetOK;
	}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		acrxDynamicLinker->unlockApplication(pkt);
		acrxDynamicLinker->registerAppMDIAware(pkt);

		//RegisterArxApp();

		if (!RetrieveLayerStateSettings())
		{
			g_bLayersFrozen = false;
			g_bLayersOff = false;
			g_bLayersLocked = false;
			g_bLayersFrozenVP = true;
			g_bLayersFrozenVPNew = true;
		}

		//WriteLayerStateSettings();

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// removeReactors();
		clearReactors();
		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CBRX18App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	static void rpkGroupLayTblReactorOn() {
		g_bWatchOn = true;
	}

	static void rpkGroupLayTblReactorOff() {
		g_bWatchOn = false;
	}

	static void rpkGroupDDLayTblReactor() {
		CAcModuleResourceOverride * myResources = new CAcModuleResourceOverride;
		CDlgLTRSettings dlg;
		dlg.DoModal();
		//if (dlg.DoModal() == IDOK)
		//{
		//	int i = 0;
		//}
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CBRX18App)



//ACED_ARXCOMMAND_ENTRY_AUTO(CBRX18App, rpkGroup, LayTblReactorOn, _LayTblReactorOn, ACRX_CMD_MODAL, NULL)
//ACED_ARXCOMMAND_ENTRY_AUTO(CBRX18App, rpkGroup, LayTblReactorOff, _LayTblReactorOff, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CBRX18App, rpkGroup, DDLayTblReactor, DDLayTblReactor, ACRX_CMD_MODAL, NULL)

