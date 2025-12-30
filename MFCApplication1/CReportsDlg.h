#pragma once
#include <afxcmn.h>

class CReportsDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CReportsDlg)

public:
    CReportsDlg(CWnd* pParent = nullptr);
    virtual ~CReportsDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REPORTS_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    CFont m_fontCustom;
    CImageList m_imgList;

    DECLARE_MESSAGE_MAP()

public:
    CListCtrl m_listReports;
    CComboBox m_comboTypes;
    afx_msg void OnCbnSelchangeComboReportType();
    void ExecuteReport(CString sql);
};