#include "pch.h"
#include "MFCApplication1.h"
#include "CAddReviewDlg.h"
#include "afxdialogex.h"
#include <sql.h>
#include <sqlext.h>

IMPLEMENT_DYNAMIC(CAddReviewDlg, CDialogEx)

CAddReviewDlg::CAddReviewDlg(CString strUserId, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ADD_REVIEW, pParent)
{
    m_strCurrentUserId = strUserId; 

    m_strSoftwareName = _T("");
    m_strReviewText = _T("");
    m_nRating = 5;

    m_bIsEditMode = false;
    m_reviewId = 0;
}

CAddReviewDlg::~CAddReviewDlg()
{
}

void CAddReviewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_REVIEWS_SOFT, m_comboSoftware);
    DDX_Control(pDX, IDC_COMBO_REVIEWS_RATING, m_comboRating);
    DDX_Control(pDX, IDC_EDIT_TEXT_REVIEW, m_editReviewText);
}

BEGIN_MESSAGE_MAP(CAddReviewDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CAddReviewDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CAddReviewDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_fontCustom.DeleteObject();
    m_fontCustom.CreateFont(22, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Segoe UI"));

    m_comboRating.ResetContent();

    int idx;
    idx = m_comboRating.AddString(_T("5 - Отлично"));
    m_comboRating.SetItemData(idx, 5);

    idx = m_comboRating.AddString(_T("4 - Хорошо"));
    m_comboRating.SetItemData(idx, 4);

    idx = m_comboRating.AddString(_T("3 - Нормально"));
    m_comboRating.SetItemData(idx, 3);

    idx = m_comboRating.AddString(_T("2 - Плохо"));
    m_comboRating.SetItemData(idx, 2);

    idx = m_comboRating.AddString(_T("1 - Ужасно"));
    m_comboRating.SetItemData(idx, 1);

    m_comboRating.SetCurSel(0); 
    LoadSoftwareToCombo();
    SetFont(&m_fontCustom);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        pChild->SendMessage(WM_SETFONT, (WPARAM)m_fontCustom.GetSafeHandle(), MAKELPARAM(TRUE, 0));
        pChild = pChild->GetWindow(GW_HWNDNEXT);
    }

    return TRUE;
}

void CAddReviewDlg::LoadSoftwareToCombo()
{
    SQLHENV hEnv; SQLHDBC hDbc; SQLHSTMT hStmt;
    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {
                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {

                    const wchar_t* sql = L"SELECT ID, NAME FROM SOFTWARE ORDER BY NAME";

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)sql, SQL_NTS) == SQL_SUCCESS) {
                        while (SQLFetch(hStmt) == SQL_SUCCESS) {
                            long id;
                            WCHAR szName[100];
                            SQLGetData(hStmt, 1, SQL_C_LONG, &id, 0, NULL);
                            SQLGetData(hStmt, 2, SQL_C_WCHAR, szName, sizeof(szName), NULL);

                            int idx = m_comboSoftware.AddString(szName);
                            m_comboSoftware.SetItemData(idx, (DWORD_PTR)id); 
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

void CAddReviewDlg::OnBnClickedOk()
{
    CString text;
    m_editReviewText.GetWindowText(text);

    text.Replace(_T("'"), _T("''"));

    int selIdx = m_comboRating.GetCurSel();
    if (selIdx == CB_ERR) {
        AfxMessageBox(_T("Пожалуйста, выберите оценку."));
        return;
    }
    int rating = (int)m_comboRating.GetItemData(selIdx);

    int softIdx = m_comboSoftware.GetCurSel();
    if (softIdx == CB_ERR) {
        AfxMessageBox(_T("Пожалуйста, выберите программу из списка."));
        return;
    }
    long softId = (long)m_comboSoftware.GetItemData(softIdx);

    SQLHENV hEnv = SQL_NULL_HENV;
    SQLHDBC hDbc = SQL_NULL_HDBC;
    SQLHSTMT hStmt = SQL_NULL_HSTMT;

    CString connStr = _T("DRIVER={Firebird/InterBase(r) driver};DBNAME=C:\\MyDatabases\\TEST2.FDB;UID=SYSDBA;PWD=masterkey;CHARSET=UTF8;");

    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_SUCCESS) {
        SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

        if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) == SQL_SUCCESS) {
            if (SQLDriverConnect(hDbc, NULL, (SQLWCHAR*)connStr.GetString(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT) >= 0) {

                if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) == SQL_SUCCESS) {
                    
                    CStringW query;
                    query.Format(L"INSERT INTO REVIEW (TEXT, \"date\", RATING, USER_ID, SOFTWARE_ID) "
                        L"VALUES ('%s', CURRENT_DATE, %d, %s, %ld)",
                        (const wchar_t*)text,
                        rating,
                        (const wchar_t*)m_strCurrentUserId,
                        softId);

                    if (SQLExecDirectW(hStmt, (SQLWCHAR*)query.GetString(), SQL_NTS) == SQL_SUCCESS) {
                        SQLEndTran(SQL_HANDLE_DBC, hDbc, SQL_COMMIT); 
                        AfxMessageBox(_T("Отзыв успешно сохранен!"));
                        CDialogEx::OnOK(); 
                    }
                    else {
                        // Вывод реальной ошибки базы данных
                        SQLWCHAR sqlState[6], msg[512];
                        SQLINTEGER nativeError;
                        SQLSMALLINT msgLen;
                        SQLGetDiagRecW(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, 512, &msgLen);

                        CString errorReport;
                        errorReport.Format(_T("Ошибка БД: %s\nКод: %d"), (CString)msg, nativeError);
                        AfxMessageBox(errorReport);
                    }

                    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
                }
                SQLDisconnect(hDbc);
            }
            else {
                AfxMessageBox(_T("Не удалось подключиться к базе данных!"));
            }
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        }
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    }
}