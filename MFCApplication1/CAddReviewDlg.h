#pragma once
#include "afxdialogex.h"

class CAddReviewDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CAddReviewDlg)

public:
    CAddReviewDlg(CString strUserId, CWnd* pParent = nullptr);
    virtual ~CAddReviewDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADD_REVIEW };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // Поддержка DDX/DDV
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    // Переменные для хранения данных 
    CString m_strCurrentUserId; 
    CString m_strUserId;        
    CString m_strSoftwareName;
    CString m_strReviewText;
    int     m_nRating;

    // Переменные для режима совместимости 
    bool    m_bIsEditMode;
    long    m_reviewId;

private:
    // Элементы управления 
    CComboBox m_comboSoftware;  
    CComboBox m_comboRating;    
    CEdit     m_editReviewText; 

    // Вспомогательный метод для загрузки списка программ
    void LoadSoftwareToCombo();

    CFont m_fontCustom; 

public:
    afx_msg void OnBnClickedOk();
};