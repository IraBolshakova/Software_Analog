#pragma once
#include "afxdialogex.h"

class CEditReviewDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CEditReviewDlg)

public:
    CEditReviewDlg(CWnd* pParent = nullptr);   
    virtual ~CEditReviewDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EDIT_REVIEW_DIALOG }; 
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    
    virtual BOOL OnInitDialog(); // Инициализация при открытии
    afx_msg void OnBnClickedOk(); // Обработчик кнопки "Сохранить"

    DECLARE_MESSAGE_MAP()

public:
    long    m_reviewId;
    CString m_strUserId;
    CString m_strSoftName;
    CString m_strReviewText;
    int     m_nRating;

    CComboBox m_comboRating;
    CEdit     m_editReview;

private:
    CFont m_fontCustom;
};