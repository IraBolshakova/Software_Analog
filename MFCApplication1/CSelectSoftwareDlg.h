#pragma once
#include "afxcmn.h"
#include "resource.h"

class CSelectSoftwareDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CSelectSoftwareDlg)

public:
    CSelectSoftwareDlg(CWnd* pParent = nullptr);
    virtual ~CSelectSoftwareDlg();

    enum { IDD = IDD_SELECT_SOFTWARE };

    long m_selectedSoftwareId; 

protected:
    CListCtrl m_listSelect;
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnDblClkList(NMHDR* pNMHDR, LRESULT* pResult);

    DECLARE_MESSAGE_MAP()

private:
    CFont m_fontCustom;
};