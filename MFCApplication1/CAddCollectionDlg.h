#pragma once
#include "afxcmn.h"
#include "afxdialogex.h"

class CAddCollectionDlg : public CDialogEx
{
public:
    CAddCollectionDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_ADD_COLLECTION };

    CString m_strName;
    CString m_strDesc;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    BOOL OnInitDialog();
    virtual void OnOK(); 

    DECLARE_MESSAGE_MAP()

private:
    CFont m_fontCustom;
};