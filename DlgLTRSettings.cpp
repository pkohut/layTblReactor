// DlgLTRSettings.cpp : implementation file
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "StdArx.h"
#include "resource.h"
#include "DlgLTRSettings.h"


extern bool g_bWatchOn;

extern bool g_bLayersOff;
extern bool g_bLayersLocked;
extern bool g_bLayersFrozen;
extern bool g_bLayersFrozenVP;
extern bool g_bLayersFrozenVPNew;

// CDlgLTRSettings dialog

IMPLEMENT_DYNAMIC(CDlgLTRSettings, CDialogEx)

CDlgLTRSettings::CDlgLTRSettings(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_DIALOG_SETTINGS, pParent)
{

}

CDlgLTRSettings::~CDlgLTRSettings()
{
}

void CDlgLTRSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_LAYERS_OFF, m_chkLayersOff);
	DDX_Control(pDX, IDC_CHECK_LAYERS_FROZEN, m_chkLayersFrozen);
	DDX_Control(pDX, IDC_CHECK_LAYERS_LOCKED, m_chkLayersLocked);
	DDX_Control(pDX, IDC_CHECK_LAYERS_FROZEN_VP, m_chkLayersFrozenVP);
	DDX_Control(pDX, IDC_CHECK_LAYERS_FROZEN_NEW_VP, m_chkLayersFrozenNewVP);
	DDX_Control(pDX, IDC_BUTTON_REGISTER, m_btnRegister);
	DDX_Control(pDX, IDC_BUTTON_ENABLED, m_btnEnabled);
}


BEGIN_MESSAGE_MAP(CDlgLTRSettings, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_REGISTER, &CDlgLTRSettings::OnClickedButtonRegister)
	ON_BN_CLICKED(IDOK, &CDlgLTRSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_ENABLED, &CDlgLTRSettings::OnClickedButtonEnabled)
END_MESSAGE_MAP()

void CDlgLTRSettings::SetDialogText(void)
{
	if (g_bWatchOn)
	{
		SetWindowText(_T("Layer Table Reactor (v0.1) - ON"));
	}
	else
	{
		SetWindowText(_T("Layer Table Reactor (v0.1) - OFF"));
	}
}

BOOL CDlgLTRSettings::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AcadAppInfo info;
	AcadApp::ErrorStatus es = getAppInfo(info);
	if (es == AcadApp::eOk)
	{
		m_bAppRegistered = true;
		m_btnRegister.SetWindowText(_T("Unregister"));
	}
	else if (es == AcadApp::eKeyNotFound)
	{
		m_bAppRegistered = false;
		m_btnRegister.SetWindowText(_T("Register"));
	}

	if (g_bWatchOn)
	{
		m_btnEnabled.SetWindowText(_T("Enabled"));
	}
	else
	{
		m_btnEnabled.SetWindowTextW(_T("Disabled"));
	}

	GetLayerStateSettings();
	SetDialogText();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLTRSettings::SetLayerStatesSettings(void)
{
	g_bLayersOff = !!m_chkLayersOff.GetCheck();
	g_bLayersLocked = !!m_chkLayersLocked.GetCheck();
	g_bLayersFrozen = !!m_chkLayersFrozen.GetCheck();
	g_bLayersFrozenVPNew = !!m_chkLayersFrozenNewVP.GetCheck();
	g_bLayersFrozenVP = !!m_chkLayersFrozenVP.GetCheck();
}

void CDlgLTRSettings::GetLayerStateSettings(void)
{
	m_chkLayersOff.SetCheck(g_bLayersOff);
	m_chkLayersLocked.SetCheck(g_bLayersLocked);
	m_chkLayersFrozen.SetCheck(g_bLayersFrozen);
	m_chkLayersFrozenNewVP.SetCheck(g_bLayersFrozenVPNew);
	m_chkLayersFrozenVP.SetCheck(g_bLayersFrozenVP);
}

// CDlgLTRSettings message handlers

void CDlgLTRSettings::OnClickedButtonRegister()
{
	if (!m_bAppRegistered)
	{		
		CString cs;
		cs = _T("Do you want to register this application with\n"
			_T("AutoCAD's demand load feature?"));
		if (MessageBox(cs, _T("Enable demand load feature"), MB_YESNO) == IDYES)
		{
			AcadApp::ErrorStatus es = RegisterArxApp();
			CString csTitle = _T("Operation failed");
			switch (es)
			{
			case AcadApp::eOk:
				csTitle = _T("Operation successful");
				cs = _T("Demand load feature enabled, application will start automatically");
				m_bAppRegistered = true;
				m_btnRegister.SetWindowText(_T("Unregister"));
				break;
			case AcadApp::eKeyNotFound:
				cs = _T("The demand load key for this application was not found");
				break;
			case AcadApp::eRejected:
				cs = _T("The demand load key cannot be added");
				break;
			case AcadApp::eUnknownError:
				cs = _T("Application instance not set, contact developer");
				break;
			case AcadApp::eValueNotFound:
				cs = _T("Module filename not determined, contact developer");
				break;
			default:
				cs = _T("Removal of the demand load key failed. Reason unknown");
				break;
			}
			MessageBox(cs, csTitle, MB_OK);
		}
	}
	else
	{
		CString cs;
		cs = _T("Do you want to unregister this application with\n"
			_T("AutoCAD's demand load feature?  If you select\n")
			_T("yes, the the next time AutoCAD is started this\n")
			_T("application will not auto load.")
		);
		    
		if (MessageBox(cs, _T("Disable demand load feature"), MB_YESNO) == IDYES)
		{
			AcadApp::ErrorStatus es = UnregisterArxApp();
			CString csTitle = _T("Operation failed");
			switch (es)
			{
			case AcadApp::eOk:
				csTitle = _T("Operation successful");
				cs = _T("The demand load key was successfully removed, and the\n"
					_T("application will not auto start."));
				m_bAppRegistered = false;
				m_btnRegister.SetWindowText(_T("Register"));
				break;
			case AcadApp::eKeyNotFound:
				cs = _T("The demand load key for this application was not found");
				break;
			case AcadApp::eRejected:
				cs = _T("The demand load key cannot be removed");
				break;
			default:
				cs = _T("Removal of the demand load key failed. Reason unknown");
				break;
			}
			MessageBox(cs, csTitle, MB_OK);
		}
	}
}


void CDlgLTRSettings::OnBnClickedOk()
{
	SetLayerStatesSettings();
	WriteLayerStateSettings();
	CDialogEx::OnOK();
}



void CDlgLTRSettings::OnClickedButtonEnabled()
{
	if (g_bWatchOn)
	{
		g_bWatchOn = false;
		m_btnEnabled.SetWindowText(_T("Disabled"));
		SetDialogText();
	}
	else
	{
		g_bWatchOn = true;
		m_btnEnabled.SetWindowText(_T("Enabled"));
		SetDialogText();
	}
}
