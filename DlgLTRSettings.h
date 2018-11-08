#pragma once
#include "afxwin.h"


// CDlgLTRSettings dialog

class CDlgLTRSettings : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgLTRSettings)

public:
    CDlgLTRSettings(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgLTRSettings();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_SETTINGS };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonUnregister();
    CButton m_chkLayersOff;
    CButton m_chkLayersFrozen;
    CButton m_chkLayersLocked;
    CButton m_chkLayersFrozenVP;
    CButton m_chkLayersFrozenNewVP;
    afx_msg void OnClickedButtonRegister();
	virtual BOOL OnInitDialog();

private:
	bool m_bAppRegistered;
	void SetLayerStatesSettings(void);
	void GetLayerStateSettings(void);
	void SetDialogText(void);
public:
	CButton m_btnRegister;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClickedButtonEnabled();
	CButton m_btnEnabled;
};
