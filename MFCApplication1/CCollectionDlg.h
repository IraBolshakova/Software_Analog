#pragma once
#include "afxcmn.h"

class CCollectionDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CCollectionDlg)

public:
    CCollectionDlg(long id, CWnd* pParent = nullptr);
    virtual ~CCollectionDlg();

    enum { IDD = IDD_COLLECTION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    long m_collectionId;
    CListCtrl m_listItems;
    CFont m_fontTitle;

    CButton* m_pBtnAdd;

    void LoadData();

public:
    afx_msg void OnBnClickedButtonDelete();
    afx_msg void OnAddSoftware();
    long m_id;

    DECLARE_MESSAGE_MAP()

private:
    CFont m_fontCustom;
};