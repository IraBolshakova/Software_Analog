#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "CRegisterDlg.h"
#include "CSoftwareDetailDlg.h"
#include "CCollectionDlg.h"
#include "CAddCollectionDlg.h"
#include "CReportsDlg.h"
#include "CAddReviewDlg.h"
#include "CEditReviewDlg.h"
#include <string>
#include <clocale>
#include <wincrypt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent)
    : CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bIsLoggedIn = false;
    // Инициализируем указатели нулями
    m_pStaticLogin = nullptr; m_pStaticPassword = nullptr;
    m_pEditLogin = nullptr; m_pEditPassword = nullptr;
    m_pButtonLogin = nullptr; m_pButtonRegister = nullptr;
    m_pButtonCreateCol = nullptr; m_pStaticSearch = nullptr;
    m_pEditSearch = nullptr; m_pButtonSearch = nullptr;
    m_pStaticLabel1 = nullptr; m_pStaticLabel2 = nullptr;
    m_pComboPaidSoft = nullptr; m_pCheckListFeatures = nullptr;
    m_pButtonFindAnalogs = nullptr; m_pButtonShowAll = nullptr;
    m_pStaticAnalogsLabel = nullptr; m_pButtonDeleteColl = nullptr;
    m_pButtonSaveToFile = nullptr; m_pButtonReports = nullptr;
    m_pButtonDeleteReview = nullptr; m_pComboLeft = nullptr; m_pComboRight = nullptr;
    m_pStaticInfoLeft = nullptr; m_pStaticInfoRight = nullptr;
    m_pButtonSendReview = nullptr; m_pButtonEditReview = nullptr;
}

CMFCApplication1Dlg::~CMFCApplication1Dlg()
{
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(1003, &CMFCApplication1Dlg::OnLoginButton)
    ON_BN_CLICKED(1004, &CMFCApplication1Dlg::OnRegisterButton)
    ON_NOTIFY(TCN_SELCHANGE, 3001, &CMFCApplication1Dlg::OnTcnSelchangeTab1)
    ON_NOTIFY(NM_DBLCLK, 2001, &CMFCApplication1Dlg::OnItemDoubleClick)
    ON_NOTIFY(NM_DBLCLK, 2002, &CMFCApplication1Dlg::OnCollectionDoubleClick)
    ON_BN_CLICKED(1023, &CMFCApplication1Dlg::OnCreateCollection)
    ON_BN_CLICKED(1024, &CMFCApplication1Dlg::OnSearchButton)
    ON_BN_CLICKED(1026, &CMFCApplication1Dlg::OnShowAllButton)
    ON_CBN_SELCHANGE(4001, &CMFCApplication1Dlg::OnCbnSelchangeComboPaidSoft)
    ON_BN_CLICKED(4003, &CMFCApplication1Dlg::OnBnClickedFindAnalogs)
    ON_BN_CLICKED(1030, &CMFCApplication1Dlg::OnSaveCollectionToFile)
    ON_BN_CLICKED(1042, &CMFCApplication1Dlg::OnDeleteCollection)
    ON_BN_CLICKED(5001, &CMFCApplication1Dlg::OnReportsClicked)
    ON_CBN_SELCHANGE(6001, &CMFCApplication1Dlg::OnSelectLeft)
    ON_CBN_SELCHANGE(6002, &CMFCApplication1Dlg::OnSelectRight)
    ON_BN_CLICKED(8003, &CMFCApplication1Dlg::OnSendReview)
    ON_BN_CLICKED(1005, &CMFCApplication1Dlg::OnBnClickedDeleteReview)
    ON_BN_CLICKED(1007, &CMFCApplication1Dlg::OnBnClickedEditReview)
END_MESSAGE_MAP()

BOOL CMFCApplication1Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    SetWindowPos(nullptr, 0, 0, 1050, 650, SWP_NOMOVE | SWP_NOZORDER);
    CreateAuthControls();

    SetFont(&m_fontCustom);

    CWnd* pChild = GetWindow(GW_CHILD); // Берем первый дочерний элемент
    while (pChild)
    {
        pChild->SetFont(&m_fontCustom); // Устанавливаем ему шрифт
        pChild = pChild->GetWindow(GW_HWNDNEXT); // Переходим к следующему
    }

    return TRUE;
}

void CMFCApplication1Dlg::CreateAuthControls()
{
    if (m_pEditLogin) return;

    m_pStaticLogin = new CStatic();
    m_pStaticLogin->Create(_T("Логин:"), WS_CHILD | WS_VISIBLE, CRect(350, 200, 450, 225), this);
    m_pStaticLogin->SetFont(&m_fontCustom);

    m_pEditLogin = new CEdit();
    m_pEditLogin->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(460, 200, 660, 230), this, 1001);

    m_pStaticPassword = new CStatic();
    m_pStaticPassword->Create(_T("Пароль:"), WS_CHILD | WS_VISIBLE, CRect(350, 250, 450, 275), this);
    m_pStaticPassword->SetFont(&m_fontCustom);

    m_pEditPassword = new CEdit();
    m_pEditPassword->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, CRect(460, 250, 660, 280), this, 1002);

    m_pButtonLogin = new CButton();
    m_pButtonLogin->Create(_T("Войти"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(460, 310, 660, 350), this, 1003);
    m_pButtonLogin->SetFont(&m_fontCustom);

    m_pButtonRegister = new CButton();
    m_pButtonRegister->Create(_T("Регистрация"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(460, 360, 660, 400), this, 1004);
    m_pButtonRegister->SetFont(&m_fontCustom);
}

void CMFCApplication1Dlg::OnLoginButton()
{
    CString login, password;
    m_pEditLogin->GetWindowText(login);
    m_pEditPassword->GetWindowText(password);

    if (login.IsEmpty() || password.IsEmpty()) {
        AfxMessageBox(_T("Введите данные!")); return;
    }

    auto WStringToUtf8 = [](CStringW wstr) -> std::string {
        if (wstr.IsEmpty()) return "";
        int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        std::string str(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], len, NULL, NULL);
        return str;
    };

    CString passHash = HashPassword(password);
    // Конвертируем логин и хеш в UTF-8
    std::string utf8Login = WStringToUtf8(login);
    std::string utf8Hash = WStringToUtf8(passHash);

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;

    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            // Используем W-версию запроса
            SQLWCHAR* sql = L"SELECT O_USER_ID, O_ROLE FROM AUTH_USER(?, ?)";

            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, utf8Login.length(), 0, (SQLPOINTER)utf8Login.c_str(), 0, NULL);
            SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, utf8Hash.length(), 0, (SQLPOINTER)utf8Hash.c_str(), 0, NULL);

            if (SQLExecDirectW(hStmt, sql, SQL_NTS) == SQL_SUCCESS && SQLFetch(hStmt) == SQL_SUCCESS) {
                long userId;
                char rawRole[100] = { 0 }; 

                SQLGetData(hStmt, 1, SQL_C_LONG, &userId, 0, NULL);
                SQLGetData(hStmt, 2, SQL_C_CHAR, rawRole, sizeof(rawRole), NULL);

                auto Utf8ToWString = [](char* buffer) -> std::wstring {
                    if (!buffer || strlen(buffer) == 0) return L"";
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
                    std::wstring wstr(wlen, 0);
                    MultiByteToWideChar(CP_UTF8, 0, buffer, -1, &wstr[0], wlen);
                    return wstr;
                    };

                m_bIsLoggedIn = true;
                m_strCurrentUserId.Format(_T("%ld"), userId);
                m_strCurrentUserRole = Utf8ToWString(rawRole).c_str();

                SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = NULL;
                ShowMainInterface();
            }
            else { AfxMessageBox(_T("Неправильный логин или пароль!")); }
        }
    }
    catch (...) { AfxMessageBox(_T("Критическая ошибка БД")); }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::ShowMainInterface()
{

    // Прячем элементы входа
    if (m_pStaticLogin) m_pStaticLogin->ShowWindow(SW_HIDE);
    if (m_pEditLogin) m_pEditLogin->ShowWindow(SW_HIDE);
    if (m_pStaticPassword) m_pStaticPassword->ShowWindow(SW_HIDE);
    if (m_pEditPassword) m_pEditPassword->ShowWindow(SW_HIDE);
    if (m_pButtonLogin) m_pButtonLogin->ShowWindow(SW_HIDE);
    if (m_pButtonRegister) m_pButtonRegister->ShowWindow(SW_HIDE);
    if (m_pButtonSaveToFile) m_pButtonSaveToFile->ShowWindow(SW_HIDE);

    // Вкладки
    if (!m_tabCtrl.GetSafeHwnd()) {
        m_tabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_BUTTONS | TCS_HOTTRACK, CRect(20, 20, 1050, 60), this, 3001);
        m_tabCtrl.SetFont(&m_fontCustom);
        m_tabCtrl.InsertItem(0, _T("Главная"));
        m_tabCtrl.InsertItem(1, _T("Подборки"));
        m_tabCtrl.InsertItem(2, _T("Поиск по функциям"));
        m_tabCtrl.InsertItem(3, _T("Сравнение ПО"));
        m_tabCtrl.InsertItem(4, _T("Отзывы"));
    }

    // Создаем кнопку, если она еще не создана
    if (m_pButtonCreateCol == nullptr) {
        m_pButtonCreateCol = new CButton();
        // CRect(лево, верх, право, низ)
        m_pButtonCreateCol->Create(_T("Создать"), WS_CHILD | BS_PUSHBUTTON, CRect(800, 200, 980, 240), this, IDC_BTN_CREATE_COLLECTION);
        m_pButtonCreateCol->SetFont(&m_fontCustom);
    }

    if (m_pButtonSaveToFile == nullptr) {
        m_pButtonSaveToFile = new CButton();
        m_pButtonSaveToFile->Create(_T("Сохранить"), WS_CHILD | BS_PUSHBUTTON, CRect(800, 310, 980, 350), this, 1030);
        m_pButtonSaveToFile->SetFont(&m_fontCustom);
    }

    if (m_pButtonDeleteColl == nullptr) {
        m_pButtonDeleteColl = new CButton();
        m_pButtonDeleteColl->Create(_T("Удалить"), WS_CHILD | BS_PUSHBUTTON, CRect(800, 255, 980, 295), this, 1042);
        m_pButtonDeleteColl->SetFont(&m_fontCustom);
    }

    if (m_pStaticSearch == nullptr) {
        m_pStaticSearch = new CStatic();
        m_pStaticSearch->Create(_T("Поиск:"), WS_CHILD | WS_VISIBLE, CRect(20, 65, 80, 85), this);
        m_pStaticSearch->SetFont(&m_fontCustom);

        m_pEditSearch = new CEdit();
        m_pEditSearch->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(90, 62, 400, 88), this, 1025);
        m_pEditSearch->SetFont(&m_fontCustom);

        m_pButtonSearch = new CButton();
        m_pButtonSearch->Create(_T("Найти"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(410, 62, 510, 88), this, 1024);
        m_pButtonSearch->SetFont(&m_fontCustom);

        m_pButtonShowAll = new CButton();
        m_pButtonShowAll->Create(_T("Показать все"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(520, 62, 650, 88), this, 1026);
        m_pButtonShowAll->SetFont(&m_fontCustom);
    }

    if (m_pButtonReports == nullptr) {
        m_pButtonReports = new CButton();
        m_pButtonReports->Create(_T("Отчеты"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            CRect(830, 550, 1000, 590), this, IDC_BUTTON_REPORTS);
        m_pButtonReports->SetFont(&m_fontCustom);
    }

    // Таблица ПО
    if (!m_listSoftware.GetSafeHwnd()) {
        m_listSoftware.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER, CRect(95, 100, 945, 540), this, 2001);
        m_listSoftware.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        m_listSoftware.InsertColumn(0, _T("Название"), LVCFMT_LEFT, 200);
        m_listSoftware.InsertColumn(1, _T("Описание"), LVCFMT_LEFT, 400);
        m_listSoftware.InsertColumn(2, _T("Лицензия"), LVCFMT_LEFT, 150);
        m_listSoftware.InsertColumn(3, _T("Размер, МБ"), LVCFMT_LEFT, 100);
    }

    // Таблица ПОДБОРОК
    if (!m_listSelections.GetSafeHwnd()) {
        m_listSelections.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER, CRect(20, 80, 730, 540), this, 2002);
        m_listSelections.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        m_listSelections.InsertColumn(0, _T("Название подборки"), LVCFMT_LEFT, 300);
        m_listSelections.InsertColumn(1, _T("Описание"), LVCFMT_LEFT, 410);
    }

    // Создаем ComboBox для выбора платного ПО
    if (m_pComboPaidSoft == nullptr) {
        m_pComboPaidSoft = new CComboBox();
        m_pComboPaidSoft->Create(WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(20, 100, 300, 400), this, 4001);
        m_pComboPaidSoft->SetFont(&m_fontCustom);
    }

    // Создаем CheckListBox для функций
    if (m_pCheckListFeatures == nullptr) {
        m_pCheckListFeatures = new CCheckListBox();
        m_pCheckListFeatures->Create(WS_CHILD | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_BORDER, CRect(320, 100, 650, 250), this, 4002);
        m_pCheckListFeatures->SetFont(&m_fontCustom);
        m_pCheckListFeatures->SetCheckStyle(BS_AUTOCHECKBOX);
    }

    // Создаем текстовые метки
    if (m_pStaticLabel1 == nullptr) {
        m_pStaticLabel1 = new CStatic();
        m_pStaticLabel1->Create(_T("Выберите платное ПО:"), WS_CHILD, CRect(20, 75, 300, 95), this);
        m_pStaticLabel1->SetFont(&m_fontCustom);
    }

    if (m_pStaticLabel2 == nullptr) {
        m_pStaticLabel2 = new CStatic();
        m_pStaticLabel2->Create(_T("Доступные функции:"), WS_CHILD, CRect(320, 75, 600, 95), this);
        m_pStaticLabel2->SetFont(&m_fontCustom);
    }

    if (m_pButtonFindAnalogs == nullptr) {
        m_pButtonFindAnalogs = new CButton();
        m_pButtonFindAnalogs->Create(_T("Найти"), WS_CHILD | BS_PUSHBUTTON, CRect(660, 100, 800, 140), this, 4003);
        m_pButtonFindAnalogs->SetFont(&m_fontCustom);
    }

    if (m_pStaticAnalogsLabel == nullptr) {
        m_pStaticAnalogsLabel = new CStatic();
        m_pStaticAnalogsLabel->Create(_T("Аналоги ПО:"), WS_CHILD, CRect(80, 235, 370, 255), this);
        m_pStaticAnalogsLabel->SetFont(&m_fontCustom);
    }

    if (m_pComboLeft == nullptr) {
        // ЛЕВАЯ ЧАСТЬ
        m_pComboLeft = new CComboBox();
        m_pComboLeft->Create(WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(80, 100, 410, 400), this, 6001);
        m_pComboLeft->SetFont(&m_fontCustom);

        m_pStaticInfoLeft = new CStatic();
        m_pStaticInfoLeft->Create(_T("Выберите ПО для сравнения"), WS_CHILD | WS_BORDER | SS_CENTER, CRect(80, 140, 410, 500), this);
        m_pStaticInfoLeft->SetFont(&m_fontCustom);

        // ПРАВАЯ ЧАСТЬ
        m_pComboRight = new CComboBox();
        m_pComboRight->Create(WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL, CRect(630, 100, 960, 400), this, 6002);
        m_pComboRight->SetFont(&m_fontCustom);

        m_pStaticInfoRight = new CStatic();
        m_pStaticInfoRight->Create(_T("Выберите ПО для сравнения"), WS_CHILD | WS_BORDER | SS_CENTER, CRect(630, 140, 960, 500), this);
        m_pStaticInfoRight->SetFont(&m_fontCustom);
    }

    if (!m_listReviews.GetSafeHwnd()) {
        m_listReviews.Create(WS_CHILD | LVS_REPORT | WS_BORDER, CRect(18, 80, 1008, 540), this, 8001);
        m_listReviews.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        m_listReviews.InsertColumn(0, _T("Пользователь"), LVCFMT_LEFT, 130);
        m_listReviews.InsertColumn(1, _T("ПО"), LVCFMT_LEFT, 150);
        m_listReviews.InsertColumn(2, _T("Дата"), LVCFMT_LEFT, 100);
        m_listReviews.InsertColumn(3, _T("Оценка"), LVCFMT_LEFT, 80);
        m_listReviews.InsertColumn(4, _T("Отзыв"), LVCFMT_LEFT, 650);
    }

    if (m_pButtonSendReview == nullptr) {
        m_pButtonSendReview = new CButton();
        m_pButtonSendReview->Create(_T("Оставить отзыв"), WS_CHILD | BS_PUSHBUTTON,
            CRect(402, 550, 600, 590), this, 8003);
        m_pButtonSendReview->SetFont(&m_fontCustom);
    }

    if (m_pButtonDeleteReview == nullptr) {
        m_pButtonDeleteReview = new CButton();
        m_pButtonDeleteReview->Create(_T("Удалить"), WS_CHILD | BS_PUSHBUTTON,
            CRect(610, 550, 800, 590), this, 1005); 
        m_pButtonDeleteReview->SetFont(&m_fontCustom);
    }

    if (m_pButtonEditReview == nullptr) {
        m_pButtonEditReview = new CButton();
        m_pButtonEditReview->Create(_T("Изменить"), WS_CHILD | BS_PUSHBUTTON,
            CRect(810, 550, 1008, 590), this, 1007);
        m_pButtonEditReview->SetFont(&m_fontCustom);
    }

    // Показываем только "Главную" по умолчанию
    m_listSoftware.ShowWindow(SW_SHOW);
    m_listSelections.ShowWindow(SW_HIDE);

    LoadSoftwareTable();
}

void CMFCApplication1Dlg::LoadSoftwareTable()
{
    if (!m_listSoftware.GetSafeHwnd()) return;
    m_listSoftware.DeleteAllItems();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
            if (SQLExecDirectW(hStmt, L"SELECT * FROM GET_SOFTWARE_MAIN_LIST", SQL_NTS) == SQL_SUCCESS) {
                int i = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    long id; WCHAR name[255], desc[512], lic[100], size[50];
                    SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
                    SQLGetData(hStmt, 3, SQL_C_WCHAR, desc, sizeof(desc), NULL);
                    SQLGetData(hStmt, 4, SQL_C_WCHAR, lic, sizeof(lic), NULL);
                    SQLGetData(hStmt, 5, SQL_C_WCHAR, size, sizeof(size), NULL);

                    int idx = m_listSoftware.InsertItem(i, name);
                    m_listSoftware.SetItemText(idx, 1, desc);
                    m_listSoftware.SetItemText(idx, 2, lic);
                    m_listSoftware.SetItemText(idx, 3, size);
                    m_listSoftware.SetItemData(idx, (DWORD_PTR)id);
                    i++;
                }
            }
        }
    }
    catch (...) {}
    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int index = pNMIA->iItem;

    if (index != -1) {
        long id = static_cast<long>(m_listSoftware.GetItemData(index));

        CSoftwareDetailDlg dlg(id, this);
        dlg.DoModal();
    }
    *pResult = 0;
}

CString CMFCApplication1Dlg::HashPassword(const CString& password)
{
    HCRYPTPROV hProv = 0; HCRYPTHASH hHash = 0;
    BYTE hash[32]; DWORD hashLen = 32;
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) return _T("");
    CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
    CStringA passA(password);
    CryptHashData(hHash, (BYTE*)(LPCSTR)passA, passA.GetLength(), 0);
    CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0);
    CString res;
    for (DWORD i = 0; i < hashLen; i++) {
        CString tmp; tmp.Format(_T("%02X"), hash[i]);
        res += tmp;
    }
    CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0);
    return res;
}

void CMFCApplication1Dlg::OnRegisterButton() { CRegisterDlg dlg; dlg.DoModal(); }

void CMFCApplication1Dlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
    int sel = m_tabCtrl.GetCurSel();

    // 1. Прячем ВСЁ по умолчанию
    m_listSoftware.ShowWindow(SW_HIDE);
    m_listSelections.ShowWindow(SW_HIDE);

    if (m_pButtonCreateCol) m_pButtonCreateCol->ShowWindow(SW_HIDE);
    if (m_pButtonSaveToFile) m_pButtonSaveToFile->ShowWindow(SW_HIDE);
    if (m_pButtonDeleteColl) m_pButtonDeleteColl->ShowWindow(SW_HIDE);

    // Прячем элементы поиска (из предыдущих шагов)
    if (m_pStaticSearch) m_pStaticSearch->ShowWindow(SW_HIDE);
    if (m_pEditSearch) m_pEditSearch->ShowWindow(SW_HIDE);
    if (m_pButtonSearch) m_pButtonSearch->ShowWindow(SW_HIDE);
    if (m_pButtonShowAll) m_pButtonShowAll->ShowWindow(SW_HIDE);
    if (m_pButtonReports) m_pButtonReports->ShowWindow(SW_HIDE);

    // Прячем элементы сравнения (новой вкладки)
    int showCompare = (sel == 2) ? SW_SHOW : SW_HIDE;
    if (m_pStaticLabel1) m_pStaticLabel1->ShowWindow(showCompare);
    if (m_pStaticLabel2) m_pStaticLabel2->ShowWindow(showCompare);
    if (m_pComboPaidSoft) m_pComboPaidSoft->ShowWindow(showCompare);
    if (m_pCheckListFeatures) m_pCheckListFeatures->ShowWindow(showCompare);
    if (m_pButtonFindAnalogs) m_pButtonFindAnalogs->ShowWindow(showCompare);
    if (m_pStaticAnalogsLabel) m_pStaticAnalogsLabel->ShowWindow(showCompare);

    int showCompareDetailed = (sel == 3) ? SW_SHOW : SW_HIDE;
    if (m_pComboLeft) m_pComboLeft->ShowWindow(showCompareDetailed);
    if (m_pStaticInfoLeft) m_pStaticInfoLeft->ShowWindow(showCompareDetailed);
    if (m_pComboRight) m_pComboRight->ShowWindow(showCompareDetailed);
    if (m_pStaticInfoRight) m_pStaticInfoRight->ShowWindow(showCompareDetailed);

    int showReviews = (sel == 4) ? SW_SHOW : SW_HIDE;
    m_listReviews.ShowWindow(showReviews);
    if (m_pButtonSendReview) m_pButtonSendReview->ShowWindow(showReviews);
    if (m_pButtonDeleteReview) m_pButtonDeleteReview->ShowWindow(showReviews);
    if (m_pButtonEditReview) m_pButtonEditReview->ShowWindow(showReviews);

    // Показываем нужное в зависимости от вкладки
    if (sel == 0) { // ГЛАВНАЯ
        m_listSoftware.MoveWindow(95, 100, 850, 440);
        m_listSoftware.ShowWindow(SW_SHOW);
        if (m_pStaticSearch) m_pStaticSearch->ShowWindow(SW_SHOW);
        if (m_pEditSearch) m_pEditSearch->ShowWindow(SW_SHOW);
        if (m_pButtonSearch) m_pButtonSearch->ShowWindow(SW_SHOW);
        if (m_pButtonShowAll) m_pButtonShowAll->ShowWindow(SW_SHOW);
        if (m_pButtonReports) m_pButtonReports->ShowWindow(SW_SHOW);
        LoadSoftwareTable();
    }
    else if (sel == 1) { // ПОДБОРКИ
        m_listSelections.ShowWindow(SW_SHOW);
        if (m_pButtonCreateCol) m_pButtonCreateCol->ShowWindow(SW_SHOW);
        if (m_pButtonSaveToFile) m_pButtonSaveToFile->ShowWindow(SW_SHOW);
        if (m_pButtonDeleteColl) m_pButtonDeleteColl->ShowWindow(SW_SHOW);
        LoadUserCollection();
    }
    else if (sel == 2) { 
        if (m_pStaticAnalogsLabel) m_pStaticAnalogsLabel->ShowWindow(SW_SHOW);
        m_listSoftware.MoveWindow(80, 260, 875, 320);
        m_listSoftware.ShowWindow(SW_SHOW);
        LoadPaidSoftwareToCombo();
    }
    else if (sel == 3) { 
        FillCompareCombos();
    }
    else if (sel == 4) {
        LoadReviews();
    }
    else {
        m_listSoftware.MoveWindow(95, 100, 850, 440); // Возвращаем на место для 1-й вкладки
    }

    *pResult = 0;
}

void CMFCApplication1Dlg::OnPaint() {
    if (IsIconic()) { CPaintDC dc(this); SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0); }
    else { CDialogEx::OnPaint(); }
}
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon() { return static_cast<HCURSOR>(m_hIcon); }

void CMFCApplication1Dlg::LoadUserCollection()
{
    m_listSelections.DeleteAllItems();

    if (!m_bIsLoggedIn || m_strCurrentUserId.IsEmpty()) {
        m_listSelections.InsertItem(0, _T("Войдите в систему"));
        return;
    }

    long userId = _ttoi(m_strCurrentUserId);
    if (userId <= 0) {
        m_listSelections.InsertItem(0, _T("Ошибка: неверный ID пользователя"));
        return;
    }

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    bool hasData = false;

    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringA query;
            query.Format("SELECT ID, NAME, DESCRIPTION FROM COLLECTION WHERE USER_ID = %ld", userId);

            if (SQLExecDirectA(hStmt, (SQLCHAR*)(LPCSTR)query, SQL_NTS) == SQL_SUCCESS) {
                int i = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    hasData = true;
                    long collId = 0;
                    char name[256] = { 0 }, desc[512] = { 0 };
                    SQLLEN l1, l2;

                    SQLGetData(hStmt, 1, SQL_C_LONG, &collId, sizeof(collId), NULL);
                    SQLGetData(hStmt, 2, SQL_C_CHAR, name, sizeof(name), &l1);
                    SQLGetData(hStmt, 3, SQL_C_CHAR, desc, sizeof(desc), &l2);

                    int row = m_listSelections.InsertItem(i, l1 != SQL_NULL_DATA ? CString(name) : _T("Без названия"));
                    m_listSelections.SetItemText(row, 1, l2 != SQL_NULL_DATA ? CString(desc) : _T("Нет описания"));
                    m_listSelections.SetItemData(row, (DWORD_PTR)collId); 
                    i++;
                }
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Ошибка при загрузке подборок"));
    }

    if (!hasData) {
        m_listSelections.InsertItem(0, _T("У вас нет подборок"));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::OnCollectionDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int nItem = pNMItemActivate->iItem;

    if (nItem != -1) {
        long collId = (long)m_listSelections.GetItemData(nItem); 

        if (collId > 0) {
            CCollectionDlg dlg(collId, this); // ID передается в конструктор
            dlg.DoModal();
        }
    }
    *pResult = 0;
}

void CMFCApplication1Dlg::OnCreateCollection()
{
    CAddCollectionDlg dlg(this);

    if (dlg.DoModal() == IDOK) {
        dlg.m_strName.Trim();
        dlg.m_strDesc.Trim();

        // 1. ПРОВЕРКА НА КИРИЛЛИЦУ
        bool hasCyrillic = false;
        for (int i = 0; i < dlg.m_strName.GetLength(); ++i) {
            wchar_t c = dlg.m_strName[i];
            if ((c >= 0x0400 && c <= 0x04FF) || c == 0x0500) {
                hasCyrillic = true;
                break;
            }
        }
        if (!hasCyrillic) {
            for (int i = 0; i < dlg.m_strDesc.GetLength(); ++i) {
                wchar_t c = dlg.m_strDesc[i];
                if ((c >= 0x0400 && c <= 0x04FF) || c == 0x0500) {
                    hasCyrillic = true; break;
                }
            }
        }

        if (hasCyrillic) {
            AfxMessageBox(_T("Неподдерживаемый формат!\nПожалуйста, используйте только латиницу и цифры."));
            return;
        }

        if (dlg.m_strName.IsEmpty()) {
            AfxMessageBox(_T("Название подборки не может быть пустым!"));
            return;
        }

        // 2. РАБОТА С БАЗОЙ ДАННЫХ
        SQLHENV hEnv = NULL;
        SQLHDBC hDbc = NULL;
        SQLHSTMT hStmtCheck = NULL;
        SQLHSTMT hStmtInsert = NULL;

        try {
            SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
            SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

            CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

            if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

                long userId = _ttoi(m_strCurrentUserId);
                long count = 0;

                // ПРОВЕРКА НА ДУБЛИКАТ
                SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmtCheck);
                SQLWCHAR* checkSql = L"SELECT COUNT(*) FROM COLLECTION WHERE USER_ID = ? AND NAME = ?";

                SQLBindParameter(hStmtCheck, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userId, 0, NULL);
                SQLBindParameter(hStmtCheck, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, (SQLPOINTER)dlg.m_strName.GetString(), 0, NULL);

                if (SQLExecDirectW(hStmtCheck, checkSql, SQL_NTS) >= 0) {
                    if (SQLFetch(hStmtCheck) == SQL_SUCCESS) {
                        SQLGetData(hStmtCheck, 1, SQL_C_LONG, &count, 0, NULL);
                    }
                }

                SQLFreeHandle(SQL_HANDLE_STMT, hStmtCheck);
                hStmtCheck = NULL;

                if (count > 0) {
                    AfxMessageBox(_T("У вас уже есть подборка с таким названием!"));
                }
                else {
                    // ВСТАВКА НОВОЙ ЗАПИСИ 
                    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmtInsert);

                    SQLWCHAR* insertSql = L"INSERT INTO COLLECTION (USER_ID, NAME, DESCRIPTION) VALUES (?, ?, ?)";

                    SQLBindParameter(hStmtInsert, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userId, 0, NULL);
                    SQLBindParameter(hStmtInsert, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, (SQLPOINTER)dlg.m_strName.GetString(), 0, NULL);
                    SQLBindParameter(hStmtInsert, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 1000, 0, (SQLPOINTER)dlg.m_strDesc.GetString(), 0, NULL);

                    if (SQLExecDirectW(hStmtInsert, insertSql, SQL_NTS) >= 0) {
                        AfxMessageBox(_T("Подборка успешно создана!"));
                        LoadUserCollection(); // Обновляем список в интерфейсе
                    }
                    else {
                        SQLWCHAR sqlState[6], msg[1024];
                        SQLINTEGER nativeError;
                        SQLSMALLINT msgLen;
                        SQLGetDiagRecW(SQL_HANDLE_STMT, hStmtInsert, 1, sqlState, &nativeError, msg, 1024, &msgLen);

                        CString errorInfo;
                        errorInfo.Format(_T("Не удалось сохранить данные.\nТехническая ошибка: %s"), msg);
                        AfxMessageBox(errorInfo);
                    }
                }
            }
        }
        catch (...) {
            AfxMessageBox(_T("Критическая ошибка при соединении с базой данных."));
        }

        if (hStmtCheck) SQLFreeHandle(SQL_HANDLE_STMT, hStmtCheck);
        if (hStmtInsert) SQLFreeHandle(SQL_HANDLE_STMT, hStmtInsert);
        if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
        if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

void CMFCApplication1Dlg::OnDeleteCollection()
{
    int nItem = m_listSelections.GetNextItem(-1, LVNI_SELECTED);
    if (nItem == -1) {
        AfxMessageBox(_T("Пожалуйста, выберите подборку для удаления."));
        return;
    }

    long collId = (long)m_listSelections.GetItemData(nItem);

    if (AfxMessageBox(_T("Удалить выбранную подборку?"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringW query;
            query.Format(L"EXECUTE PROCEDURE DELETE_FULL_COLLECTION(%ld)", collId);

            if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) >= 0) {
                m_listSelections.DeleteItem(nItem);

                AfxMessageBox(_T("Подборка успешно удалена."));
            }
            else {
                SQLWCHAR sqlState[6], msg[1024];
                SQLINTEGER nativeError; SQLSMALLINT msgLen;
                SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 1024, &msgLen);
                AfxMessageBox(CString(L"Ошибка БД: ") + msg);
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Системная ошибка при удалении."));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    LoadUserCollection();
}

void CMFCApplication1Dlg::OnSearchButton()
{
    CString searchText;
    m_pEditSearch->GetWindowText(searchText);

    if (searchText.IsEmpty()) {
        LoadSoftwareTable();
        return;
    }

    m_listSoftware.DeleteAllItems();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            SQLWCHAR* sql = L"SELECT OUT_ID, OUT_NAME, OUT_DESC, OUT_LIC, OUT_SIZE FROM SEARCH_SOFTWARE(?)";

            SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_TCHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)searchText.GetString(), 0, NULL);

            if (SQLExecDirectW(hStmt, sql, SQL_NTS) >= 0) {
                int i = 0;
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    long id, size;
                    WCHAR name[256], desc[1024], lic[50];

                    SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
                    SQLGetData(hStmt, 3, SQL_C_WCHAR, desc, sizeof(desc), NULL);
                    SQLGetData(hStmt, 4, SQL_C_WCHAR, lic, sizeof(lic), NULL);
                    SQLGetData(hStmt, 5, SQL_C_LONG, &size, 0, NULL);

                    CString strSize; strSize.Format(_T("%ld"), size);

                    int idx = m_listSoftware.InsertItem(i, name);
                    m_listSoftware.SetItemText(idx, 1, desc);
                    m_listSoftware.SetItemText(idx, 2, lic);
                    m_listSoftware.SetItemText(idx, 3, strSize);
                    m_listSoftware.SetItemData(idx, (DWORD_PTR)id);
                    i++;
                }
            }
        }
    }
    catch (...) { AfxMessageBox(_T("Ошибка поиска")); }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::OnShowAllButton()
{
    if (m_pEditSearch) {
        m_pEditSearch->SetWindowText(_T(""));
    }
    LoadSoftwareTable();
}

void CMFCApplication1Dlg::OnCbnSelchangeComboPaidSoft()
{
    int sel = m_pComboPaidSoft->GetCurSel();
    if (sel == CB_ERR) return;

    // Получаем ID выбранного ПО, который мы сохранили ранее через SetItemData
    long selectedSoftId = (long)m_pComboPaidSoft->GetItemData(sel);

    m_pCheckListFeatures->ResetContent(); // Очищаем старые функции

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringW query;
            query.Format(L"SELECT F.ID, F.NAME FROM FEATURE F "
                L"JOIN PROVIDES P ON F.ID = P.FEATURE_ID "
                L"WHERE P.SOFTWARE_ID = %ld", selectedSoftId);

            if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    long fId; WCHAR fName[255];
                    SQLGetData(hStmt, 1, SQL_C_LONG, &fId, 0, NULL);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, fName, sizeof(fName), NULL);

                    // Добавляем функцию в список с галочкой
                    int idx = m_pCheckListFeatures->AddString(fName);
                    m_pCheckListFeatures->SetItemData(idx, (DWORD_PTR)fId);

                    // По умолчанию ставим галочку на все функции этой программы
                    m_pCheckListFeatures->SetCheck(idx, 1);
                }
            }
        }
    }
    catch (...) { AfxMessageBox(_T("Ошибка загрузки функций")); }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::OnBnClickedFindAnalogs()
{
    m_listSoftware.DeleteAllItems();
    int count = m_pCheckListFeatures->GetCount();

    bool anyChecked = false;
    for (int i = 0; i < count; i++) {
        if (m_pCheckListFeatures->GetCheck(i)) { anyChecked = true; break; }
    }
    if (!anyChecked) {
        AfxMessageBox(_T("Выберите хотя бы одну функцию!"));
        return;
    }

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");
        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

            for (int i = 0; i < count; i++) {
                if (m_pCheckListFeatures->GetCheck(i)) {
                    long fId = (long)m_pCheckListFeatures->GetItemData(i);
                    // Передаем открытый hDbc в функцию
                    SearchAnalogsByFeature(hDbc, fId);
                }
            }
        }
    }
    catch (...) { AfxMessageBox(_T("Ошибка поиска")); }

    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

void CMFCApplication1Dlg::SearchAnalogsByFeature(SQLHDBC hDbc, long featureId)
{
    SQLHSTMT hStmt = NULL;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    CStringW query;
    query.Format(L"SELECT OUT_ID, OUT_NAME, OUT_DESC, OUT_LIC, OUT_SIZE FROM GET_FREE_ANALOGS(%ld)", featureId);

    if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            long id, size; WCHAR name[255], desc[1024], lic[50];
            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
            SQLGetData(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);
            SQLGetData(hStmt, 3, SQL_C_WCHAR, desc, sizeof(desc), NULL);
            SQLGetData(hStmt, 4, SQL_C_WCHAR, lic, sizeof(lic), NULL);
            SQLGetData(hStmt, 5, SQL_C_LONG, &size, 0, NULL);

            // Проверка на дубликаты 
            bool exists = false;
            for (int j = 0; j < m_listSoftware.GetItemCount(); j++) {
                if ((long)m_listSoftware.GetItemData(j) == id) { exists = true; break; }
            }

            if (!exists) {
                int idx = m_listSoftware.InsertItem(m_listSoftware.GetItemCount(), name);
                m_listSoftware.SetItemText(idx, 1, desc);
                m_listSoftware.SetItemText(idx, 2, lic);
                CString sSize; sSize.Format(_T("%ld"), size);
                m_listSoftware.SetItemText(idx, 3, sSize);
                m_listSoftware.SetItemData(idx, (DWORD_PTR)id);
            }
        }
    }
    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void CMFCApplication1Dlg::LoadPaidSoftwareToCombo()
{
    if (!m_pComboPaidSoft || !m_pComboPaidSoft->GetSafeHwnd()) return;
    m_pComboPaidSoft->ResetContent();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            SQLWCHAR* sql = L"SELECT ID, NAME FROM SOFTWARE "
                L"WHERE UPPER(LICENSE_TYPE) NOT LIKE '%FREE%' "
                L"AND UPPER(LICENSE_TYPE) NOT LIKE '%OPEN%'";

            if (SQLExecDirectW(hStmt, sql, SQL_NTS) == SQL_SUCCESS) {
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    long id;
                    WCHAR name[255];

                    SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), NULL);

                    int idx = m_pComboPaidSoft->AddString(name);

                    m_pComboPaidSoft->SetItemData(idx, (DWORD_PTR)id);
                }
            }
        }
    }
    catch (...) {
        AfxMessageBox(_T("Ошибка при загрузке списка платного ПО"));
    }

    if (hStmt) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    if (m_pComboPaidSoft->GetCount() > 0) {
        m_pComboPaidSoft->SetCurSel(0);
        OnCbnSelchangeComboPaidSoft();
    }
}

void CMFCApplication1Dlg::OnSaveCollectionToFile()
{
    int nItem = m_listSelections.GetNextItem(-1, LVNI_SELECTED);
    if (nItem == -1) {
        AfxMessageBox(_T("Сначала выберите подборку в списке!"));
        return;
    }

    DWORD_PTR dwData = m_listSelections.GetItemData(nItem);
    long collId = static_cast<long>(dwData);
    CString collName = m_listSelections.GetItemText(nItem, 0);

    CFileDialog fileDlg(FALSE, _T("txt"), collName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"), this);

    if (fileDlg.DoModal() != IDOK) return;

    setlocale(LC_ALL, "Russian");

    CStdioFile file;
    if (!file.Open(fileDlg.GetPathName(), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
        AfxMessageBox(_T("Не удалось создать файл!"));
        return;
    }

    // Пишем заголовок поэтапно
    file.WriteString(_T("ОТЧЕТ ПО ПОДБОРКЕ: ") + collName + _T("\n"));
    file.WriteString(_T("------------------------------------------\n"));

    if (collId <= 0) {
        file.WriteString(_T("Внимание: ID подборки не определен (0).\n"));
        file.Close();
        return;
    }

    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;
    bool hasData = false;

    try {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

        CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

        if (SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
            SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

            CStringW query;
            query.Format(L"SELECT S.NAME, S.DESCRIPTION FROM SOFTWARE S "
                L"JOIN COLLECTIONITEM CI ON S.ID = CI.SOFTWARE_ID "
                L"WHERE CI.COLLECTION_ID = %ld", collId);

            if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
                while (SQLFetch(hStmt) == SQL_SUCCESS) {
                    hasData = true;
                    WCHAR szName[256] = { 0 };
                    WCHAR szDesc[1024] = { 0 };

                    SQLGetData(hStmt, 1, SQL_C_WCHAR, szName, sizeof(szName), NULL);
                    SQLGetData(hStmt, 2, SQL_C_WCHAR, szDesc, sizeof(szDesc), NULL);

                    file.WriteString(_T("Программа: ") + CString(szName) + _T("\n"));
                    file.WriteString(_T("Описание: ") + CString(szDesc) + _T("\n\n"));
                }
            }
        }
    }
    catch (...) {
        file.WriteString(_T("Произошла системная ошибка при обращении к БД.\n"));
    }

    if (hStmt != SQL_NULL_HSTMT) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (hDbc != SQL_NULL_HDBC) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    if (hEnv != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    if (!hasData) {
        file.WriteString(_T("В этой подборке пока нет добавленного программного обеспечения.\n"));
    }

    file.Close();
    setlocale(LC_ALL, "C");
    AfxMessageBox(_T("Данные успешно выгружены в файл!"), MB_OK | MB_ICONINFORMATION);
}

void CMFCApplication1Dlg::OnReportsClicked()
{
    CReportsDlg dlg;
    dlg.DoModal();   // Открываем окно отчетов
}

void CMFCApplication1Dlg::FillCompareCombos()
{
    if (!m_pComboLeft || !m_pComboRight) return;

    m_pComboLeft->ResetContent();
    m_pComboRight->ResetContent();

    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;

    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) == SQL_SUCCESS) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    if (SQLExecDirect(hStmt, (SQLWCHAR*)L"SELECT ID, NAME FROM SOFTWARE ORDER BY NAME", SQL_NTS) == SQL_SUCCESS) {
                        while (SQLFetch(hStmt) == SQL_SUCCESS) {
                            long id;
                            WCHAR szName[256] = { 0 };
                            SQLLEN cbName;

                            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                            SQLGetData(hStmt, 2, SQL_C_WCHAR, szName, sizeof(szName), &cbName);

                            int idxL = m_pComboLeft->AddString(szName);
                            m_pComboLeft->SetItemData(idxL, id);

                            int idxR = m_pComboRight->AddString(szName);
                            m_pComboRight->SetItemData(idxR, id);
                        }
                    }
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

void CMFCApplication1Dlg::UpdateCompareInfo(CComboBox* pCombo, CComboBox* pOtherCombo, CStatic* pStatic) {
    int sel = pCombo->GetCurSel();
    if (sel == CB_ERR || !pStatic) return;

    long softId = (long)pCombo->GetItemData(sel);

    if (pOtherCombo) {
        int otherSel = pOtherCombo->GetCurSel();
        if (otherSel != CB_ERR) {
            long otherSoftId = (long)pOtherCombo->GetItemData(otherSel);
            if (softId == otherSoftId) {
                AfxMessageBox(_T("Это ПО уже выбрано для сравнения! Пожалуйста, выберите другое."));
                pCombo->SetCurSel(CB_ERR); // Сбрасываем выбор
                pStatic->SetWindowText(_T("")); // Очищаем текст
                return;
            }
        }
    }

    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) == SQL_SUCCESS) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    CStringW query;
                    query.Format(L"SELECT * FROM GET_SOFTWARE_DETAILS(%ld)", softId);

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
                        if (SQLFetch(hStmt) == SQL_SUCCESS) {
                            WCHAR name[256], desc[1024], lic[100], cat[100], dev[100], web[256], os[100], cpu[100], gpu[100];
                            SQLINTEGER size = 0, ram = 0, disk = 0;

                            SQLGetData(hStmt, 1, SQL_C_WCHAR, name, sizeof(name), NULL);
                            SQLGetData(hStmt, 2, SQL_C_WCHAR, desc, sizeof(desc), NULL);
                            SQLGetData(hStmt, 3, SQL_C_WCHAR, lic, sizeof(lic), NULL);
                            SQLGetData(hStmt, 4, SQL_C_LONG, &size, 0, NULL);
                            SQLGetData(hStmt, 5, SQL_C_WCHAR, cat, sizeof(cat), NULL);
                            SQLGetData(hStmt, 6, SQL_C_WCHAR, dev, sizeof(dev), NULL);
                            SQLGetData(hStmt, 7, SQL_C_WCHAR, web, sizeof(web), NULL);
                            SQLGetData(hStmt, 8, SQL_C_WCHAR, os, sizeof(os), NULL);
                            SQLGetData(hStmt, 9, SQL_C_LONG, &ram, 0, NULL);
                            SQLGetData(hStmt, 10, SQL_C_WCHAR, cpu, sizeof(cpu), NULL);
                            SQLGetData(hStmt, 11, SQL_C_WCHAR, gpu, sizeof(gpu), NULL);
                            SQLGetData(hStmt, 12, SQL_C_LONG, &disk, 0, NULL);

                            CString fullInfo;
                            fullInfo.Format(_T("=== ОБЩАЯ ИНФОРМАЦИЯ ===\r\n")
                                _T("Название: %s\r\n")
                                _T("Категория: %s\r\n")
                                _T("Разработчик: %s\r\n")
                                _T("Лицензия: %s\r\n")
                                _T("Объём: %d МБ\r\n")
                                _T("Сайт: %s\r\n\r\n")
                                _T("=== СИСТЕМНЫЕ ТРЕБОВАНИЯ ===\r\n")
                                _T("ОС: %s\r\n")
                                _T("ОЗУ: %d МБ\r\n")
                                _T("Процессор: %s\r\n")
                                _T("Видеокарта: %s\r\n")
                                _T("Диск: %d ГБ\r\n\r\n")
                                _T("=== ОПИСАНИЕ ===\r\n%s"),
                                name, cat, dev, lic, size, web,
                                os, ram, cpu, gpu, disk, desc);

                            pStatic->SetWindowText(fullInfo);
                        }
                    }
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

void CMFCApplication1Dlg::OnSelectLeft() {
    UpdateCompareInfo(m_pComboLeft, m_pComboRight, m_pStaticInfoLeft);
}

void CMFCApplication1Dlg::OnSelectRight() {
    UpdateCompareInfo(m_pComboRight, m_pComboLeft, m_pStaticInfoRight);
}

void CMFCApplication1Dlg::LoadReviews()
{
    if (!m_listReviews.GetSafeHwnd()) return;
    m_listReviews.DeleteAllItems();

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    const wchar_t* sql = L"SELECT R.ID, U.NAME, S.NAME, "
                        L"CAST(EXTRACT(DAY FROM R.\"date\") AS VARCHAR(2)) || '.' || "
                        L"CAST(EXTRACT(MONTH FROM R.\"date\") AS VARCHAR(2)) || '.' || "
                        L"CAST(EXTRACT(YEAR FROM R.\"date\") AS VARCHAR(4)), "
                        L"R.RATING, R.TEXT "
                        L"FROM REVIEW R "
                        L"JOIN \"User\" U ON R.USER_ID = U.ID "
                        L"JOIN SOFTWARE S ON R.SOFTWARE_ID = S.ID "
                        L"ORDER BY S.NAME ASC";

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)sql, SQL_NTS) == SQL_SUCCESS) {
                        int i = 0;
                        auto Utf8ToWString = [](char* buffer) -> std::wstring {
                            if (!buffer || strlen(buffer) == 0) return L"";
                            int wlen = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
                            std::wstring wstr(wlen, 0);
                            MultiByteToWideChar(CP_UTF8, 0, buffer, -1, &wstr[0], wlen);
                            return wstr;
                            };

                        while (SQLFetch(hStmt) == SQL_SUCCESS) {
                            long reviewId = 0; // Переменная для ID
                            char rawUser[256] = { 0 }, rawSoft[256] = { 0 }, rawDate[128] = { 0 }, rawText[2048] = { 0 };
                            int rating = 0;
                            SQLLEN cbId, cbText;

                            SQLGetData(hStmt, 1, SQL_C_LONG, &reviewId, 0, &cbId);
                            SQLGetData(hStmt, 2, SQL_C_CHAR, rawUser, sizeof(rawUser), NULL);
                            SQLGetData(hStmt, 3, SQL_C_CHAR, rawSoft, sizeof(rawSoft), NULL);
                            SQLGetData(hStmt, 4, SQL_C_CHAR, rawDate, sizeof(rawDate), NULL);
                            SQLGetData(hStmt, 5, SQL_C_LONG, &rating, 0, NULL);
                            SQLGetData(hStmt, 6, SQL_C_CHAR, rawText, sizeof(rawText), &cbText);

                            int idx = m_listReviews.InsertItem(i, Utf8ToWString(rawUser).c_str());

                            m_listReviews.SetItemData(idx, (DWORD_PTR)reviewId);

                            m_listReviews.SetItemText(idx, 1, Utf8ToWString(rawSoft).c_str());
                            m_listReviews.SetItemText(idx, 2, Utf8ToWString(rawDate).c_str());

                            CString strRating;
                            strRating.Format(_T("%d"), rating);
                            m_listReviews.SetItemText(idx, 3, strRating);
                            m_listReviews.SetItemText(idx, 4, (cbText == SQL_NULL_DATA) ? _T("") : Utf8ToWString(rawText).c_str());

                            i++;
                        }
                    }
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}

void CMFCApplication1Dlg::OnSendReview()
{
    if (m_strCurrentUserId.IsEmpty()) {
        AfxMessageBox(_T("Пожалуйста, авторизуйтесь для написания отзыва."));
        return;
    }

    CAddReviewDlg dlg(m_strCurrentUserId, this);
    if (dlg.DoModal() == IDOK) {
        LoadReviews(); // Обновляем таблицу отзывов, чтобы увидеть новый
    }
}

void CMFCApplication1Dlg::OnBnClickedDeleteReview()
{
    // 1. Проверка выбора в списке
    int nItem = m_listReviews.GetNextItem(-1, LVNI_SELECTED);
    if (nItem == -1) {
        AfxMessageBox(_T("Выберите отзыв для удаления!"));
        return;
    }

    // 2. Получение ID отзыва
    long reviewId = (long)m_listReviews.GetItemData(nItem);
    if (reviewId == 0) {
        AfxMessageBox(_T("Ошибка: ID отзыва не найден."));
        return;
    }

    // Подтверждение удаления
    if (AfxMessageBox(_T("Вы уверены, что хотите удалить этот отзыв?"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
        return;
    }

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    bool success = false;
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    // 3. ПРЯМОЙ ЗАПРОС вместо процедуры для корректной работы SQLRowCount
                    // Мы удаляем только если совпадает ID отзыва И ID текущего пользователя
                    CStringW query;
                    query.Format(L"DELETE FROM REVIEW WHERE ID = %ld AND USER_ID = '%s'",
                        reviewId, (const wchar_t*)m_strCurrentUserId);

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {

                        // Теперь SQLRowCount точно вернет 1, если вы автор, и 0, если нет
                        SQLLEN rowsAffected = 0;
                        SQLRowCount(hStmt, &rowsAffected);

                        if (rowsAffected > 0) {
                            SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_COMMIT);
                            success = true;
                        }
                        else {
                            // Если удалено 0 строк, значит USER_ID в базе другой
                            SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_ROLLBACK);
                            success = false;
                        }
                    }
                    else {
                        // Ошибка синтаксиса или связи
                        SQLWCHAR sqlState[6], msg[512];
                        SQLINTEGER nativeError; SQLSMALLINT msgLen;
                        SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 512, &msgLen);
                        AfxMessageBox(CString(L"Ошибка SQL: ") + msg);
                    }

                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }

    // 4. Итоговый результат
    if (success) {
        LoadReviews(); // Обновляем список на экране
        AfxMessageBox(_T("Отзыв успешно удален!"));
    }
    else {
        AfxMessageBox(_T("Ошибка: Вы не являетесь автором этого отзыва или запись уже удалена."));
    }
}

void CMFCApplication1Dlg::OnBnClickedEditReview()
{
    int nItem = m_listReviews.GetNextItem(-1, LVNI_SELECTED);
    if (nItem == -1) {
        AfxMessageBox(_T("Пожалуйста, сначала выберите отзыв для редактирования!"));
        return;
    }

    long reviewId = (long)m_listReviews.GetItemData(nItem);

    SQLHENV hEnv = NULL; SQLHDBC hDbc = NULL; SQLHSTMT hStmt = NULL;
    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    bool isAuthor = false;

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    // Проверка авторства. ВАЖНО: Проверьте имя таблицы (REVIEW или REVIEWS)
                    // Если USER_ID числовой, убираем кавычки вокруг %s
                    CStringW checkQuery;
                    checkQuery.Format(L"SELECT COUNT(*) FROM REVIEW WHERE ID = %ld AND USER_ID = %d",
                        reviewId, _ttoi(m_strCurrentUserId));

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)checkQuery.GetString(), SQL_NTS) == SQL_SUCCESS) {
                        if (SQLFetch(hStmt) == SQL_SUCCESS) {
                            long count = 0;
                            SQLGetData(hStmt, 1, SQL_C_LONG, &count, 0, NULL);
                            if (count > 0) isAuthor = true;
                        }
                    }
                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }

    if (!isAuthor) {
        AfxMessageBox(_T("Ошибка: Вы не можете редактировать чужой отзыв!"));
        return;
    }

    // Если проверка пройдена — открываем диалог
    CEditReviewDlg dlg(this);
    dlg.m_reviewId = reviewId;
    dlg.m_strUserId = m_strCurrentUserId;
    dlg.m_strSoftName = m_listReviews.GetItemText(nItem, 1);
    dlg.m_nRating = _ttoi(m_listReviews.GetItemText(nItem, 3));
    dlg.m_strReviewText = m_listReviews.GetItemText(nItem, 4);

    if (dlg.DoModal() == IDOK) {
        LoadReviews();
    }
}