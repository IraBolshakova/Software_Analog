#pragma once
#include <afxcmn.h>
#include <sql.h>
#include <sqlext.h>

#pragma comment(lib, "odbc32.lib")

class CMFCApplication1Dlg : public CDialogEx
{
public:
    CMFCApplication1Dlg(CWnd* pParent = nullptr);
    virtual ~CMFCApplication1Dlg();

    void CreateAuthControls();
    void ShowMainInterface();
    CString HashPassword(const CString& password);
    void LoadSoftwareTable();
    void LoadUserCollection();
    void OnSearchButton();
    void SearchAnalogsByFeature(SQLHDBC hDbc, long featureId);
    void LoadPaidSoftwareToCombo();
    void OnSaveCollectionToFile();
    void FillCompareCombos();
    void UpdateCompareInfo(CComboBox* pCombo, CComboBox* pOtherCombo, CStatic* pStatic);
    void LoadReviews();


#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    // Реализация
protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLoginButton();
    afx_msg void OnRegisterButton();
    afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCollectionDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCreateCollection();
    afx_msg void OnShowAllButton();
    afx_msg void OnCbnSelchangeComboPaidSoft();
    afx_msg void OnBnClickedFindAnalogs();
    afx_msg void OnDeleteCollection();
    afx_msg void OnReportsClicked();
    afx_msg void OnSelectLeft();
    afx_msg void OnSelectRight();
    afx_msg void OnSendReview();
    afx_msg void OnBnClickedDeleteReview();
    afx_msg void OnBnClickedEditReview();

    DECLARE_MESSAGE_MAP()

private:
    // Динамические элементы (указатели, чтобы управлять созданием)
    CStatic* m_pStaticLogin;
    CStatic* m_pStaticPassword;
    CEdit* m_pEditLogin;
    CEdit* m_pEditPassword;
    CButton* m_pButtonLogin;
    CButton* m_pButtonRegister;

    // Элементы основного интерфейса
    CTabCtrl  m_tabCtrl;
    CListCtrl m_listSoftware;
    CListCtrl  m_listSelections;
    CFont m_fontCustom;
    bool  m_bIsLoggedIn;
    CString m_strCurrentUserId;
    CString m_strCurrentUserRole;
    CButton* m_pButtonReports;

    CButton* m_pButtonCreateCol;

    CStatic* m_pStaticSearch;
    CEdit* m_pEditSearch;
    CButton* m_pButtonSearch;
    CButton* m_pButtonShowAll;
    CButton* m_pButtonSaveToFile;
    CButton* m_pButtonDeleteColl;

    // Элементы для вкладки "Сравнение"
    CStatic* m_pStaticSelectSoft;
    CComboBox* m_pComboPaidSoft;     // Список платных программ
    CStatic* m_pStaticSelectFeatures;
    CCheckListBox* m_pCheckListFeatures; // Функции с галочками
    CButton* m_pButtonFindAnalogs;
    CListCtrl m_listAnalogs;         // Таблица результатов
    CStatic* m_pStaticLabel1;
    CStatic* m_pStaticLabel2;
    CStatic* m_pStaticAnalogsLabel;

    // Левая сторона
    CComboBox* m_pComboLeft;
    CStatic* m_pStaticInfoLeft;
    // Правая сторона
    CComboBox* m_pComboRight;
    CStatic* m_pStaticInfoRight;

    CListCtrl m_listReviews;
    CComboBox* m_pComboReviewSoft; // Выбор ПО для фильтрации отзывов
    CButton* m_pButtonSendReview;  // Кнопка отправки
    CButton* m_pButtonDeleteReview;
    CButton* m_pButtonEditReview;
};
